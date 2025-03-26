#include "predict.h"
unsigned char* g_pRgbBuffer;    
int main(int argc, char **argv) 
{
    int                     iCameraCounts = 1;
    int                     iStatus = -1;
    tSdkCameraDevInfo       tCameraEnumList;
    int                     hCamera;
    tSdkCameraCapbility     tCapability;      
    tSdkFrameHead           sFrameInfo;
    BYTE*                   pbyBuffer;
    //int                     iDisplayFrames = 10000;
    Mat                     matimage;
    int                     channel = 3;
    CameraSdkInit(0);
    iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);
    printf("state = %d\n", iStatus);
    printf("count = %d\n", iCameraCounts);
    if (iCameraCounts == 0) 
    {
        return -1;
    }
    iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);
    printf("state = %d\n", iStatus);
    if (iStatus != CAMERA_STATUS_SUCCESS) 
    {
        return -1;
    }
    CameraGetCapability(hCamera, &tCapability);
    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3);
    CameraPlay(hCamera);
    if (tCapability.sIspCapacity.bMonoSensor) 
    {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } 
    else 
    {
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
    }

    namedWindow("预测效果", WINDOW_AUTOSIZE);
    
    Mat frame, gray, twovalue, frame_inrange, frame_inrange_dilate, img1, img2, canny;
    vector<Point2f> centers, R_list, L_list;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Point2f center;
    Point2f vertices[4];
    Mat cam_matrix = (Mat_<double>(3, 3) << 2034.4, -6.2, 663.4457, 0, 2058.11, 672.665, 0, 0, 1);
    Mat distortion_coeff = (Mat_<double>(5, 1) << -0.08441, 0.7558, -0.00717, 0.0049, 0);
    Mat r, rot, trans;
    vector<Point2f> twoDim;
    vector<Point3f> SHOOT;

    SHOOT.push_back(cv::Point3f(-2, 2, 0));
    SHOOT.push_back(cv::Point3f(2, 2, 0));
    SHOOT.push_back(cv::Point3f(2, -2, 0));
    SHOOT.push_back(cv::Point3f(-2, -2, 0));

    Point2d pre(0, 0); //紫色
    Point2d F(0, 0);   //黄色 根据平均计算的扇页中心特征点
    Point2d R(0, 0);   //青色
    double theta = 0;
    double last_theta1 = 0;
    double last_theta2 = 0;
    double last_theta3 = 0;
    double deta_theta = 0;
    double deta_theta_fitter = 0;
    double pre_theta = 0;
    double time = 0, start_time = 0, end_time = 0;
    bool predictJudgement = TRUE;// 默认当前为小符模式
    double radius = 0;
    string fpsString;
    char string[100];
    // int small_s;              // 小符弧长预测量
    Point2f small_forecast;            // 小符预测点


    // MVVideoCapture mv_camera;

    create_Trackbar(R_R_B, R_R_G, R_R_R, B_R_B, B_R_G, B_R_R, R_R_BB, R_R_GG, R_R_RR, B_R_BB, B_R_GG, B_R_RR, min_F_Area, max_F_Area, min_R_Ratio, max_R_Ratio, min_L_Ratio, max_L_Ratio, min_RL_Ratio, max_RL_Ratio, min_R_Area, max_R_Area, min_L_Area, max_L_Area, min_RL_Distance_Ratio, max_RL_Distance_Ratio, min_L_to_F_to_R_distance_ratio, max_L_to_F_to_R_distance_ratio, max_F_Radius, gamma_value, exposure_value, gain_value, red_blue_threshold, small_s);
    
    MatrixXd xx(2, 1);
    MatrixXd PP(2, 2);
    MatrixXd FF(2, 2); 
    MatrixXd HH(1, 2);
    MatrixXd QQ(2, 2);
    MatrixXd RR(1, 1);
    initializeKalmanFilter(xx, PP, FF, HH, QQ, RR);

    // Ptr<ml::SVM> svm = ml::SVM::load("/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/R_svm.xml");
    UsbSerial usb_serial; 
    usb_serial.SerialInit();  // 打开串口

    ///////////////////////////////////////////////////
    // createCameraTrackbars();
    ///////////////////////////////////////////////////

    while (true) 
    {
        if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
		{
            start_time = cv::getTickCount();
            
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);


            Mat frame(
					  cv::Size(sFrameInfo.iWidth,sFrameInfo.iHeight), 
					  sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
					  g_pRgbBuffer
					 );
            

            Mat frame1_gamma_corrected;
            double gamma = gamma_value / 100.0; // 将滑动条值转换为实际的 gamma 值
            Mat lut(1, 256, CV_8UC1);
            for (int i = 0; i < 256; i++) 
            {
                lut.at<uchar>(i) = pow(i / 255.0, gamma) * 255.0;
            }
            LUT(frame, lut, frame1_gamma_corrected);

            // 修改曝光值的处理
            double exposure_scale = exposure_value / 100.0;  // 将曝光值映射到[0,1]范围
            double gain_scale = gain_value / 100.0;         // 增益值映射到合适范围
            // frame1_gamma_corrected.convertTo(frame1_gamma_corrected, -1, gain_value / 100.0, exposure_scale * 255 - 255);
            // 使用乘法而不是加法来调节亮度
            frame1_gamma_corrected.convertTo(frame1_gamma_corrected, -1, gain_scale * exposure_scale, 0);  

            // 确保图像在有效范围内
            frame1_gamma_corrected = max(frame1_gamma_corrected, 0);
            frame1_gamma_corrected = min(frame1_gamma_corrected, 255);


            // 创建一个显示用的副本
            Mat display_frame = frame1_gamma_corrected.clone();


            // 处理帧
            processFrame(frame1_gamma_corrected, display_frame, centers, R_list, L_list, R, small_s);//占用大概10～15ms
            // putText(display_frame, fpsString, Point(50, 50), 2, 1, Scalar(0, 0, 255)); //20ms以下


            // getRwarp(frame1_gamma_corrected, vertices);

            // R标svm模板
            // R_Match(R_warp);

            // R标模板    
            // getRwarp(frame1_gamma_corrected, vertices);





            // // 计算中心
            // F = calculateCenter(centers);
            // cout << "F: " << F << endl;




            // // 检查F点是否有效
            // if (isfinite(F.x) && isfinite(F.y) && (F.x != 0 || F.y != 0)) 
            // {
            //     // 计算角度
            //     calculateTheta(theta, last_theta1, last_theta2, last_theta3, deta_theta, radius, R, F);
                
            //     // 更新 Kalman Filter
            //     pre_theta = updateKalman(predictJudgement, pre_theta, theta, deta_theta, xx, FF, PP, HH, RR, QQ);
            // } 
            // else 
            // {
            //     cout << "F点无效,跳过角度计算" << endl;
            // }




            // // 计算角度
            // calculateTheta(theta, last_theta1, last_theta2, last_theta3, deta_theta, radius, R, F);
            
            // // cout << "000000000" << endl;

            // // 更新 Kalman Filter
            // pre_theta = updateKalman(predictJudgement, pre_theta, theta, deta_theta, xx, FF, PP, HH, RR, QQ);


            // 计算预测
            // pre = calculatePrediction(R, radius, pre_theta);
            // cout << "pre: " << pre << endl;


            // 绘制预测
            // drawPredictions(R, F, pre, display_frame);//占用大概9ms

            
            // 计算并发送角度
            end_time = getTickCount();
            sendAngles(display_frame, SHOOT, twoDim, g_small_forecast, usb_serial);

            time = (end_time - start_time) / getTickFrequency() * 1000;
            fpsString = "delay: " + to_string(time) + " ms";
            putText(display_frame, fpsString, Point(50, 50), 2, 1, Scalar(0, 0, 255)); //25ms以下

                    

            // 在图像上显示当前参数值
            // string params_text = format("Exp:%d Gain:%d R:%d G:%d B:%d Sat:%d Gamma:%d", exposure_time, analog_gain, r_gain, g_gain, b_gain, saturation, gamma);
            // putText(display_frame, params_text, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1);    


            
            imshow("预测效果", display_frame);
            
            waitKey(1);
            if (waitKey(30) >= 0)
                break;
            CameraReleaseImageBuffer(hCamera,pbyBuffer);
		}
    }
    CameraUnInit(hCamera);
    free(g_pRgbBuffer);
    destroyAllWindows();
    return 0;
}