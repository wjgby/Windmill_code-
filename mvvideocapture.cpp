#include "mvvideocapture.h"

MVVideoCapture::MVVideoCapture()
: iCameraCounts(0), iStatus(-1), hCamera(-1), channel(3),
  pbyBuffer(nullptr), g_pRgbBuffer(nullptr),
  iDisplayFrames(true), sFileName("Camera0.Config"),
  isInitialized(false)
{
  InitializeCamera();
}

MVVideoCapture::~MVVideoCapture() {
  ReleaseCamera();
}

bool MVVideoCapture::InitializeCamera() {
  // Get camera counts
  iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);
  if (iStatus != CAMERA_STATUS_SUCCESS) {
    printf("Camera enumeration failed! Error code: %d\n", iStatus);
    return false;
  }
  if (iCameraCounts == 0) {
    printf("No camera detected! Please check:\n");
    printf("1. Camera power and USB connection\n");
    printf("2. Camera driver installation\n");
    printf("3. USB permission (try running with sudo)\n");
    return false;
  }
  printf("Detected %d cameras. First camera info:\n", iCameraCounts);
  printf("Model: %s\n", tCameraEnumList.acProductName);
  printf("Serial: %s\n", tCameraEnumList.acSn);

  // Initialize camera with detailed error handling
  int retryCount = 0;
  const int maxRetries = 3;
  
  while (retryCount < maxRetries) {
    iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);
    if (iStatus == CAMERA_STATUS_SUCCESS) {
      break;
    }
    
    printf("Camera init failed, retry %d/3, error: %d\n", retryCount + 1, iStatus);
    printf("Possible causes:\n");
    printf("1. Camera not properly connected\n");
    printf("2. Incorrect camera parameters\n");
    printf("3. Driver or SDK issue\n");
    
    // Try resetting USB connection
    if (iStatus == -18) {
      printf("Attempting USB reset...\n");
      CameraUnInit(hCamera);
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    retryCount++;
  }

  if (iStatus != CAMERA_STATUS_SUCCESS) {
    printf("Failed to initialize camera after %d retries, error code: %d\n", maxRetries, iStatus);
    isInitialized.store(false);
    return false;
  }

  // Get camera capabilities
  iStatus = CameraGetCapability(hCamera, &tCapability);
  if (iStatus != CAMERA_STATUS_SUCCESS) {
    printf("Failed to get camera capabilities, error: %d\n", iStatus);
    isInitialized.store(false);
    return false;
  }

  // Allocate RGB buffer
  g_pRgbBuffer = (unsigned char*)malloc(
    tCapability.sResolutionRange.iHeightMax *
    tCapability.sResolutionRange.iHeightMax * 
    tCapability.sResolutionRange.iWidthMax * 3);

  if (!g_pRgbBuffer) {
    printf("Failed to allocate RGB buffer\n");
    isInitialized.store(false);
    return false;
  }

  // Set output format based on sensor type
  if (tCapability.sIspCapacity.bMonoSensor) {
    channel = 1;
    CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
  } else {
    channel = 3;
    CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
  }

  // Start camera
  iStatus = CameraPlay(hCamera);
  if (iStatus != CAMERA_STATUS_SUCCESS) {
    printf("Failed to start camera, error: %d\n", iStatus);
    isInitialized.store(false);
    return false;
  }

  isInitialized = true;
  return true;
}

void MVVideoCapture::ReleaseCamera() {
  if (isInitialized.load()) {
    iDisplayFrames.store(false);
    {
      std::lock_guard<std::mutex> lock(frameMutex);
      if (hCamera >= 0) {
        CameraUnInit(hCamera);
      }
      if (g_pRgbBuffer) {
        free(g_pRgbBuffer);
        g_pRgbBuffer = nullptr;
      }
    }
    isInitialized.store(false);
    frameCondition.notify_all();
  }
}

bool MVVideoCapture::IsCameraConnected() const {
  return isInitialized.load();
}

int MVVideoCapture::GetFrame(img_get & imget) {
  if (!isInitialized.load()) {
    printf("Camera not initialized\n");
    return -1;
  }

  // Check camera connection status
  if (!IsCameraConnected()) {
    printf("Camera disconnected, attempting to reconnect...\n");
    if (!InitializeCamera()) {
      printf("Failed to reconnect camera\n");
      return -2;
    }
  }

  while (iDisplayFrames.load()) {
    int ret = CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000);
    if (ret == CAMERA_STATUS_SUCCESS) {
      CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);

      cv::Mat matImage(
        cv::Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
        sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
        g_pRgbBuffer,
        cv::Mat::AUTO_STEP);

      if (matImage.empty()) {
        std::cout << "Image empty!" << std::endl;
        continue;
      }

      matImage.copyTo(imget.frame);
      imget.flag.emplace_back(1);
      imget.cond.notify_one();

      CameraReleaseImageBuffer(hCamera, pbyBuffer);
    } else {
      printf("Failed to get image buffer, error: %d\n", ret);
      if (ret == -5) {  // CAMERA_STATUS_DISCONNECTED
        isInitialized.store(false);
        printf("Camera disconnected, attempting to reconnect...\n");
        if (!InitializeCamera()) {
          printf("Failed to reconnect camera\n");
          return -3;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
  }

  return 0;
}