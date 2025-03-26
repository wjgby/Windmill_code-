#ifndef MVVIDEOCAPTURE_H
#define MVVIDEOCAPTURE_H

#include <mutex>
#include <string>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include <thread>

#include "Info.h"
#include "cameraapi.h"

class MVVideoCapture
{
public:
  MVVideoCapture();
  ~MVVideoCapture();
  
  int GetFrame(img_get & imget);
  bool IsCameraConnected() const;

private:
  bool InitializeCamera();
  void ReleaseCamera();
  
  int iCameraCounts;
  int iStatus;
  int hCamera;
  int channel;
  tSdkCameraDevInfo tCameraEnumList;
  tSdkCameraCapbility tCapability;
  tSdkFrameHead sFrameInfo;
  BYTE * pbyBuffer;
  unsigned char * g_pRgbBuffer;
  std::atomic<bool> iDisplayFrames;
  std::string sFileName;
  std::atomic<bool> isInitialized;
  std::mutex frameMutex;
  std::condition_variable frameCondition;
};

#endif  // MVVIDEOCAPTURE_H