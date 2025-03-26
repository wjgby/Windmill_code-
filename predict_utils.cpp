#include "predict.h"

// bool isColorInRange(const Vec3b &color, const Scalar &lowerBound, const Scalar &upperBound)
// {
//     return (color[0] >= lowerBound[0] && color[0] <= upperBound[0]) && 

//            (color[1] >= lowerBound[1] && color[1] <= upperBound[1]) && 

//            (color[2] >= lowerBound[2] && color[2] <= upperBound[2]); 
// }

// 初始化变量
int R_R_B = 0;            // 红0(HOME)       蓝(HOME)
int R_R_G = 0;            // 红0(HOME)       蓝(HOME)
int R_R_R = 70;           // 红130(HOME五次膨胀仍有R标svm)      蓝(HOME)
int B_R_B = 0;
int B_R_G = 0;
int B_R_R = 0;
int R_R_BB = 255;         // 红255(HOME)     蓝(HOME)
int R_R_GG = 255;         // 红255(HOME)     蓝(HOME)
int R_R_RR = 255;         // 红255(HOME)     蓝(HOME)
int B_R_BB = 255;
int B_R_GG = 255;
int B_R_RR = 255;

// int R_FU_B = 30;
// int R_FU_G = 190;
// int R_FU_R = 180;
// int B_FU_B = 180;
// int B_FU_G = 190;
// int B_FU_R = 30;
// int R_FU_BB = 255;
// int R_FU_GG = 255;
// int R_FU_RR = 255;
// int B_FU_BB = 255;
// int B_FU_GG = 255;
// int B_FU_RR = 255;

int min_F_Area = 2370;                            // 最小扇页面积，一定不能为0,因为会导致后续扇页轮廓点集小于5个，导致程序异常终止
int max_F_Area = 10000;                           // 最大扇页面积

int min_R_Ratio = 0;                              // 一开始的minRatio和maxRatio和prescreenRatio有关
int max_R_Ratio = 7;

int min_L_Ratio = 3;
int max_L_Ratio = 5;

int min_RL_Ratio = 3;
int max_RL_Ratio = 8;

int min_R_Area = 200;
int max_R_Area = 570;  

int min_L_Area = 800;
int max_L_Area = 1500;

int min_RL_Distance_Ratio = 0;                      // 这个值如果不是0则无法框选灯条
int max_RL_Distance_Ratio = 70;

int min_L_to_F_to_R_distance_ratio = 0;             // 灯条中心到R标中心 与 灯条中心到扇页中心最小比F4
int max_L_to_F_to_R_distance_ratio = 2;             // 灯条中心到R标中心 与 灯条中心到扇页中心最小比F4

int max_F_Radius = 1000;                            // 最大扇页面积 魔法数字 大于66

int gamma_value = 180;                              // 初始值为 100，表示 gamma = 1.0
int exposure_value = 0;                             // 曝光
int gain_value = 170;                               // 增益
int red_blue_threshold = 200;                       // 红蓝通道阈值差

int small_s = 120;                                  // 小符弧长预测量



////////////////////////////////以下是对标段瞄的相机参数的设置过程//////////////////////////////////////
                                                                                        
// 添加新的全局变量
// int exposure_time = 5000;  // 曝光时间(微秒)
// int analog_gain = 100;     // 模拟增益
// int r_gain = 100;         // R通道增益
// int g_gain = 100;         // G通道增益 
// int b_gain = 100;         // B通道增益
// int saturation = 100;     // 饱和度
// int gamma = 100;          // gamma值
// int                     hCamera;

//////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////以下是processFrame函数体的声明及初始化过程//////////////////////////////////////

Mat gray_local, twovalue_local, frame1_inrange_local, frame1_inrange_dilate_local, img1_local, img2_local, canny_local;
Mat R_warp;
vector<vector<Point>> contours_local;
vector<Vec4i> hierarchy_local;
Point2f center_local;
Point2f R_vertices[4];
Point2f L_vertices[4];
Point2f L_middle; // 画出灯条的中心点
static Point2f g_L_middle;
static bool g_L_middle_valid = false;



double R_to_L_distance = 0.0;      // R标到灯条的距离                                        
double L_to_F_distance = 0.0;                                                              
                                                                                            
float R_radius;                    // R标的半径                                                   
float F_radius;                    // 扇页的半径                                                  
                                                                                            
double R_to_Fan_distance;          // R标到扇页的距离(加上灯条筛选之后可能需要删除)                    
double prescreen_ratio;            // R标中心到扇页中心的距离与扇页直径的比值(加上灯条筛选之后可能需要删除)
                                                                                            
bool R_flag = false;                                                                     
bool L_flag = false;

bool predictJudgement = true;      // 大小符预测标志位
Point2f small_forecast(0.0, 0.0);            // 小符预测点
double small_angle;                // 小符预测角度量

bool CommunicaionFlag;      // 通信标志位

Point2f big_forecast;              // 大符预测点
Point2f g_small_forecast;          // 声明全局预测点


///////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                                  


// 回调函数
// void onTrackbar(int, void*)
// {
//     // 更新相机参数
//     if(hCamera)
//     {
//         // 设置曝光时间
//         CameraSetExposureTime(hCamera, exposure_time);
        
//         // 设置模拟增益
//         CameraSetAnalogGain(hCamera, analog_gain);
        
//         // 设置RGB增益
//         CameraSetGain(hCamera, r_gain, g_gain, b_gain);
        
//         // 设置饱和度
//         CameraSetSaturation(hCamera, saturation);
        
//         // 设置Gamma
//         CameraSetGamma(hCamera, gamma);
//     }
// }




// void createCameraTrackbars()
// {
//     namedWindow("Camera Parameters", WINDOW_AUTOSIZE);
    
//     // 获取相机参数范围
//     tSdkCameraCapbility tCapability;
//     CameraGetCapability(hCamera, &tCapability);
    
//     // 曝光时间
//     double exposure_line_time;
//     CameraGetExposureLineTime(hCamera, &exposure_line_time);
//     int min_exposure = tCapability.sExposeDesc.uiExposeTimeMin * exposure_line_time;
//     int max_exposure = tCapability.sExposeDesc.uiExposeTimeMax * exposure_line_time;
//     createTrackbar("Exposure Time", "Camera Parameters", &exposure_time, 
//                    max_exposure, onTrackbar);
                   
//     // 模拟增益
//     createTrackbar("Analog Gain", "Camera Parameters", &analog_gain,
//                    tCapability.sExposeDesc.uiAnalogGainMax, onTrackbar);
                   
//     // RGB增益
//     createTrackbar("R Gain", "Camera Parameters", &r_gain,
//                    tCapability.sRgbGainRange.iRGainMax, onTrackbar);
//     createTrackbar("G Gain", "Camera Parameters", &g_gain, 
//                    tCapability.sRgbGainRange.iGGainMax, onTrackbar);
//     createTrackbar("B Gain", "Camera Parameters", &b_gain,
//                    tCapability.sRgbGainRange.iBGainMax, onTrackbar);
                   
//     // 饱和度
//     createTrackbar("Saturation", "Camera Parameters", &saturation,
//                    tCapability.sSaturationRange.iMax, onTrackbar);
                   
//     // Gamma值
//     createTrackbar("Gamma", "Camera Parameters", &gamma,
//                    tCapability.sGammaRange.iMax, onTrackbar);
// }




void initializeKalmanFilter(MatrixXd &xx, MatrixXd &PP, MatrixXd &FF, MatrixXd &HH, MatrixXd &QQ, MatrixXd &RR)
{
    double dt = 1.0 / 60.0;
    double noise_variance = 0.4;
    PP = MatrixXd::Identity(2, 2);                 // P矩阵是状态估计协方差，其初值会影响迭代的收敛速度，但一般设定为单位阵让它自己收敛即可
    QQ = MatrixXd::Identity(2, 2) * 0.0025;        // Q矩阵是一个6x6的矩阵，那么应当有六个值需要调
    RR = MatrixXd::Identity(1, 1) * noise_variance;// R矩阵在该模型中为测量噪声矩阵，大小为3x3,一般只需要设置其对角元素，即认为测量量之前无相关性

    xx.resize(2, 1);
    xx << 0, 0;
    FF.resize(2, 2);
    FF << 1, dt, 0, 1;
    HH.resize(1, 2);
    HH << 1, 0;
}

void callback(int val,void* userdata)
{
    int* ptr = static_cast<int*>(userdata);
    *ptr = val;
}

void create_Trackbar(int &R_R_B, int &R_R_G, int &R_R_R, 
                    int &B_R_B, int &B_R_G, int &B_R_R, 
                    int &R_R_BB, int &R_R_GG, int &R_R_RR, 
                    int &B_R_BB, int &B_R_GG,int &B_R_RR, 
                    int &min_F_Area, int &max_F_Area, int &min_R_Ratio, int &max_R_Ratio,
                    int &min_L_Ratio, int &max_L_Ratio, int &min_RL_Ratio, int &max_RL_Ratio,
                    int &min_R_Area, int &max_R_Area, int &min_L_Area, 
                    int &max_L_Area, int &min_RL_Distance_Ratio, int &max_RL_Distance_Ratio, 
                    int &min_L_to_F_to_R_distance_ratio, int &max_L_to_F_to_R_distance_ratio, int &max_F_Radius,
                    int &gamma_value, int &exposure_value, int &gain_value, int &red_blue_threshold,
                    int &small_s)
{

    namedWindow("颜色通道", WINDOW_AUTOSIZE);
    resizeWindow("颜色通道", 200, 150);
    namedWindow("R标与灯条", WINDOW_AUTOSIZE);
    resizeWindow("R标与灯条", 200, 150);
    namedWindow("扇页", WINDOW_AUTOSIZE);
    resizeWindow("扇页", 200, 150);


    createTrackbar("蓝色通道下限阈值", "颜色通道", &R_R_B, 255, callback, (void *)&R_R_B);
    createTrackbar("绿色通道下限阈值", "颜色通道", &R_R_G, 255, callback, (void*)&R_R_G);
    createTrackbar("红色通道下限阈值", "颜色通道", &R_R_R, 255, callback, (void*)&R_R_R);
    // createTrackbar("B_R_B", "Trackbar_Color", &B_R_B, 255, callback, (void*)&B_R_B);
    // createTrackbar("B_R_G", "Trackbar_Color", &B_R_G, 255, callback, (void*)&B_R_G);
    // createTrackbar("B_R_R", "Trackbar_Color", &B_R_R, 255, callback, (void*)&B_R_R);
    createTrackbar("蓝色通道上限阈值", "颜色通道", &R_R_BB, 255, callback, (void*)&R_R_BB);
    createTrackbar("绿色通道上限阈值", "颜色通道", &R_R_GG, 255, callback, (void*)&R_R_GG);
    createTrackbar("红色通道上限阈值", "颜色通道", &R_R_RR, 255, callback, (void*)&R_R_RR);
    // 添加 gamma 滑动条
    createTrackbar("Gamma", "颜色通道", &gamma_value, 300, callback, (void *)&gamma_value);
    createTrackbar("曝光", "颜色通道", &exposure_value, 200, callback, (void *)&exposure_value);
    createTrackbar("增益", "颜色通道", &gain_value, 1000, callback, (void *)&gain_value);
    //添加 红蓝识别 增强术
    createTrackbar("红蓝差值阈值", "颜色通道", &red_blue_threshold, 255, callback, (void *)&red_blue_threshold);

    
    // createTrackbar("B_R_BB", "Trackbar_Color", &B_R_BB, 255, callback, (void*)&B_R_BB);
    // createTrackbar("B_R_GG", "Trackbar_Color", &B_R_GG, 255, callback, (void*)&B_R_GG);
    // createTrackbar("B_R_RR", "Trackbar_Color", &B_R_RR, 255, callback, (void*)&B_R_RR);
    
    // createTrackbar("R_FU_B", "Control", &R_FU_B, 255, callback, (void*)&R_FU_B);
    // createTrackbar("R_FU_G", "Control", &R_FU_G, 255, callback, (void*)&R_FU_G);
    // createTrackbar("R_FU_R", "Control", &R_FU_R, 255, callback, (void*)&R_FU_R);
    // createTrackbar("B_FU_B", "Control", &B_FU_B, 255, callback, (void*)&B_FU_B);
    // createTrackbar("B_FU_G", "Control", &B_FU_G, 255, callback, (void*)&B_FU_G);
    // createTrackbar("B_FU_R", "Control", &B_FU_R, 255, callback, (void*)&B_FU_R);
    // createTrackbar("R_FU_BB", "Control", &R_FU_BB, 255, callback, (void*)&R_FU_BB);
    // createTrackbar("R_FU_GG", "Control", &R_FU_GG, 255, callback, (void*)&R_FU_GG);
    // createTrackbar("R_FU_RR", "Control", &R_FU_RR, 255, callback, (void*)&R_FU_RR);
    // createTrackbar("B_FU_BB", "Control", &B_FU_BB, 255, callback, (void*)&B_FU_BB);
    // createTrackbar("B_FU_GG", "Control", &B_FU_GG, 255, callback, (void*)&B_FU_GG);
    // createTrackbar("B_FU_RR", "Control", &B_FU_RR, 255, callback, (void*)&B_FU_RR);
    
    createTrackbar("最小R面积R1", "R标与灯条", &min_R_Area, 10000, callback, (void*)&min_R_Area);
    createTrackbar("最大R面积R1", "R标与灯条", &max_R_Area, 10000, callback, (void*)&max_R_Area);

    createTrackbar("灯长宽最小比L1", "R标与灯条", &min_L_Ratio, 100, callback, (void*)&min_L_Ratio);
    createTrackbar("灯长宽最大比L1", "R标与灯条", &max_L_Ratio, 100, callback, (void*)&max_L_Ratio);
    
    createTrackbar("灯长边和R半径最小比L2", "R标与灯条", &min_RL_Ratio, 100, callback, (void*)&min_RL_Ratio);
    createTrackbar("灯长边和R半径最大比L2", "R标与灯条", &max_RL_Ratio, 100, callback, (void*)&max_RL_Ratio);
    
    createTrackbar("灯心距R心与R半径最小比L3", "R标与灯条", &min_RL_Distance_Ratio, 100, callback, (void *)&min_RL_Distance_Ratio);
    createTrackbar("灯心距R心与R半径最大比L3", "R标与灯条", &max_RL_Distance_Ratio, 100, callback, (void *)&max_RL_Distance_Ratio);
    
    createTrackbar("最小灯面积L4", "R标与灯条", &min_L_Area, 10000, callback, (void *)&min_L_Area);
    createTrackbar("最大灯面积L4", "R标与灯条", &max_L_Area, 10000, callback, (void *)&max_L_Area);
    
    /* 这里的面积不能调成0,因为会使后续判断点集数量小于5个点，导致程序终止 */
    createTrackbar("最小扇面积F1", "扇页", &min_F_Area, 15000, callback, (void*)&min_F_Area);
    createTrackbar("最大扇面积F1", "扇页", &max_F_Area, 15000, callback, (void*)&max_F_Area);
    
    createTrackbar("R到扇距离与扇直径最小比F2", "扇页", &min_R_Ratio, 100, callback, (void*)&min_R_Ratio);
    createTrackbar("R到扇距离与扇直径最大比F2", "扇页", &max_R_Ratio, 100, callback, (void*)&max_R_Ratio);
    
    createTrackbar("最大扇半径F3", "扇页", &max_F_Radius, 1500, callback, (void *)&max_F_Radius);

    createTrackbar("灯心到R心与扇心到R心最小比F4", "扇页", &min_L_to_F_to_R_distance_ratio, 100, callback, (void *)&min_L_to_F_to_R_distance_ratio);
    createTrackbar("灯心到R心与扇心到R心最大比F4", "扇页", &max_L_to_F_to_R_distance_ratio, 100, callback, (void *)&max_L_to_F_to_R_distance_ratio);

    createTrackbar("小符弧长预测量", "扇页", &small_s, 1500, callback, (void *)&small_s);

}

Point2d processFrame(Mat &frame1, Mat &display_frame, vector<Point2f> &centers, vector<Point2f> &R_list, vector<Point2f> &L_list, Point2d &R, int &small_s)
{
    bool isClockwise = true;           // 旋转方向标志位（true是逆时针，false是顺时针）
    CommunicaionFlag = false;
    small_forecast = Point2f(0, 0);

    if (frame1.empty()) 
    {
        cerr << "相机输入图像为空!" << endl;
        return Point2d(); // 退出函数，避免后续错误
    }

    /////////////////////////////////////////////////////////////////////////
    // // 应用 gamma 校正                                                    //
    // Mat frame1_gamma_corrected;                                          //
    // double gamma = gamma_value / 100.0; // 将滑动条值转换为实际的 gamma 值   //
    // Mat lut(1, 256, CV_8UC1);                                            //
    // for (int i = 0; i < 256; i++) {                                      //
    //     lut.at<uchar>(i) = pow(i / 255.0, gamma) * 255.0;                //
    // }                                                                    //
    // LUT(frame1, lut, frame1_gamma_corrected);                            //
    // imshow("Gamma矫正图像", frame1_gamma_corrected);                      //
    /////////////////////////////////////////////////////////////////////////

    // double start_time = cv::getTickCount();


    // 分离颜色通道
    vector<Mat> channels;
    split(frame1, channels);
     
    // 获取蓝色和红色通道
    Mat blue = channels[0];
    Mat red = channels[2];
     
    // 计算红蓝通道差值
    Mat diff;
    absdiff(red, blue, diff);
     
    // 创建掩码以识别显著的红蓝差异
    Mat mask;
    threshold(diff, mask, red_blue_threshold, 255, THRESH_BINARY); // 50是阈值，可以调整
     
    // 对原始图像应用掩码
    Mat filtered;

    frame1.copyTo(filtered, mask);

     
    
    
    ///////////////////////////////////
    // 显示处理后的图像用于调试          //
    // imshow("红蓝通道差值", diff);   //  
    // imshow("掩码", mask);          //
    // imshow("过滤后图像", filtered); //
    ///////////////////////////////////


    

    cvtColor(filtered, gray_local, COLOR_BGR2GRAY);
    

    threshold(~gray_local, twovalue_local, 200, 255, THRESH_BINARY_INV);
    

    inRange(filtered, Scalar(R_R_B, R_R_G, R_R_R), Scalar(R_R_BB, R_R_GG, R_R_RR), frame1_inrange_local);
    
   
    Mat struct1 = getStructuringElement(1, Size(3, 3));
    
    
    dilate(frame1_inrange_local, frame1_inrange_dilate_local, struct1, Point(-1, -1), 3);
    
   
    imshow("膨胀图像", frame1_inrange_dilate_local);
    

    contours_local.clear();

    findContours(frame1_inrange_dilate_local, contours_local, hierarchy_local, 0, 2, Point()); 
    

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // double R_to_L_distance = 0.0;      // R标到灯条的距离                                         //
    // double L_to_F_distance = 0.0;                                                               //
    //                                                                                             //
    // float R_radius;              // R标的半径                                                     //
    // float F_radius;              // 扇页的半径                                                    //
    //                                                                                             //
    // double R_to_Fan_distance;    // R标到扇页的距离(加上灯条筛选之后可能需要删除)                      //
    // double prescreen_ratio;      // R标中心到扇页中心的距离与扇页直径的比值(加上灯条筛选之后可能需要删除)  //
    //                                                                                             //
    // bool R_flag = false;                                                                        //
    // bool L_flag = false;                                                                        //
    //                                                                                             //
       R_list.clear();              //看来只有图像处理就做到了8ms以下                                   //
    // centers.clear();                                                                            //
    ///////////////////////////////////////////////////////////////////////////////////////////////


    // double end_time = cv::getTickCount();
    // double processing_time = (end_time - start_time) / cv::getTickFrequency() * 1000; // 转换为毫秒
    // cout << "当前帧处理时间: " << processing_time << " ms" << endl;

    if (!contours_local.empty())                                                  
    {
        // 处理 R 标
        for (int n = 0; n < contours_local.size(); n++)
        {
            if (contourArea(contours_local[n]) >= min_R_Area && contourArea(contours_local[n]) <= max_R_Area && contours_local[n].size() > 6) //1
            {
                RotatedRect R_rrect = fitEllipse(contours_local[n]);
                if (R_rrect.size.width / R_rrect.size.height < 1.5 && R_rrect.size.width / R_rrect.size.height > 0.5)
                {
                    // 原来都是0.8而不是0.9
                    R_vertices[0].x = R_rrect.center.x - 0.8 * R_rrect.size.width;    R_vertices[0].y = R_rrect.center.y - 0.8 * R_rrect.size.width;                                                           // 矩形的左上角顶点
                    R_vertices[1].x = R_rrect.center.x + 0.8 * R_rrect.size.width;    R_vertices[1].y = R_rrect.center.y - 0.8 * R_rrect.size.width;// 矩形的右上角顶点
                    R_vertices[2].x = R_rrect.center.x + 0.8 * R_rrect.size.width;    R_vertices[2].y = R_rrect.center.y + 0.8 * R_rrect.size.width;// 矩形的右下角顶点
                    R_vertices[3].x = R_rrect.center.x - 0.8 * R_rrect.size.width;    R_vertices[3].y = R_rrect.center.y + 0.8 * R_rrect.size.width;// 矩形的左下角顶点
                    Mat R_warped_image = getRwarp(frame1_inrange_dilate_local, R_vertices); // 调用 getRwarp 并存储返回值

                    

                    if(R_warped_image.size().width > 0 && R_warped_image.size().height > 0)
                    {
                        if (R_Match(R_warped_image))
                        {
                            R_list.emplace_back(R_rrect.center);
                            minEnclosingCircle(contours_local[n], center_local, R_radius);
                            ellipse(display_frame, R_rrect, Scalar(0, 255, 0), 2, 8);              // 绿色椭圆画出包围的R标
                            // string fpsString;
                            // putText(display_frame, fpsString, Point(50, 50), 2, 1, Scalar(0, 0, 255));
                            // imshow("预测效果", display_frame);

                                                      
                            if (!R_list.empty())
                            {
                                circle(display_frame, R_list[0], 5, Scalar(0, 255, 0), FILLED);    // 圈出R_list第一个的中心点
                                string area_text = "R Area: " + to_string(int(contourArea(contours_local[n]))); 
                                putText(display_frame, area_text, Point(50, 70), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
                                R_flag = true;
                            }
                            else
                            {
                                //cout<<"R_list为空"<<endl;
                                continue;                            
                            }
                        }
                        else 
                        {
                            //cout<<"R_Match失败"<<endl;
                            continue;
                        }
                    }
                    else
                    {
                        //cout<<"R_warped_image为空"<<endl;
                        continue;
                    }
                }
                else
                {
                    //cout << "不符合R标长宽比条件" << endl;
                    continue;
                }
            }
            else
            {
                //cout << "不符合R标面积条件" << endl;
                continue;
            }
        }

        if (R_list.empty()) 
        {
            // cerr << "R_list is empty after processing R 标!" << endl;
            return Point2d(); // 退出函数，避免后续错误
        }


        //处理流动灯条
        for(int n = 0; n < contours_local.size(); n++)
        {
            L_list.clear();
            if (R_flag)
            {
                Mat L_img;
                RotatedRect L_rrect = minAreaRect(contours_local[n]);
                Point2f L_vertices[4];
                L_rrect.points(L_vertices);                                                         // 获取四个角点
                
                // 可以正常画出四个角点
                float L_distance1 = norm(L_vertices[0] - L_vertices[1]);                            // 灯条的四边长度
                float L_distance2 = norm(L_vertices[1] - L_vertices[2]);                            
                float L_distance3 = norm(L_vertices[2] - L_vertices[3]);
                float L_distance4 = norm(L_vertices[3] - L_vertices[0]);
                float L_min_distance = min({L_distance1, L_distance2, L_distance3, L_distance4});   // 相当于筛选灯条长边
                float L_max_distance = max({L_distance1, L_distance2, L_distance3, L_distance4});   // 相当于筛选灯条短边

                float L_ratio = L_max_distance / L_min_distance;                                    // 灯条的长边与短边的比值(筛选条件之一)

                Point2f L_middle((L_vertices[0].x + L_vertices[2].x) / 2, (L_vertices[0].y + L_vertices[2].y) / 2); // 预想画出灯条的中心点
                float RL_Ratio = L_max_distance / R_radius;                                         // 灯条的长边与R标的半径的比值(筛选条件之一)
                double R_to_L_distance = sqrt((L_middle.x - R_list[0].x)                            // R标到灯条的距离
                                                * (L_middle.x - R_list[0].x) 
                                                + (L_middle.y - R_list[0].y) 
                                                * (L_middle.y - R_list[0].y)
                                             );
                

                float RL_Distance_Ratio = R_to_L_distance / R_radius;                               // 灯条中心点到R标距离 与 R标的半径的比值(筛选条件之一)，小于70

                double L_to_F_distance = sqrt((center_local.x - L_middle.x)                         // 灯条中心点到扇页中心点的距离
                                                * (center_local.x - L_middle.x)
                                                + (center_local.y - L_middle.y)
                                                * (center_local.y - L_middle.y)
                                             );



                /* 相当于以下三层条件是筛选所需的目标去除噪点，最后两层条件是筛选筛选已击打和未击打灯条 */
                if(L_ratio >= min_L_Ratio && L_ratio <= max_L_Ratio)// 灯条长宽比筛选1
                {
                    if(RL_Ratio >= min_RL_Ratio && RL_Ratio <= max_RL_Ratio)// 灯条长边和R标半径比筛选2
                    {
                        if (RL_Distance_Ratio >= min_RL_Distance_Ratio && RL_Distance_Ratio <= max_RL_Distance_Ratio) // 判断灯条中心点距离R标中心的筛选3
                        {
                            if (contourArea(contours_local[n]) >= min_L_Area && contourArea(contours_local[n]) <= max_L_Area) // 通过面积筛选已击打灯条与未击打灯条4
                            {
                                // line(display_frame, L_vertices[1], L_vertices[2], Scalar(200, 23, 255), 2);
                                // line(display_frame, L_vertices[3], L_vertices[0], Scalar(200, 23, 255), 2);
                                
                                // double dis = sqrt((ArmorCenter_list[i].x-R_center.x)*(ArmorCenter_list[i].x-R_center.x)+(ArmorCenter_list[i].y-R_center.y)*(ArmorCenter_list[i].y-R_center.y));
                                // double cos = (ArmorCenter_list[i].x-R_center.x)/dis;
                                // double sin = (ArmorCenter_list[i].y-R_center.y)/dis;
                                // L_vertices[0].x = R_center.x - 0.025*dis*sin + 0.3*(ArmorCenter_list[i].x-R_center.x);              L_vertices[0].y = R_center.y + 0.025*dis*cos + 0.3*(ArmorCenter_list[i].y-R_center.y);
                                // L_vertices[1].x = R_center.x + 0.025*dis*sin + 0.3*(ArmorCenter_list[i].x-R_center.x);              L_vertices[1].y = R_center.y - 0.025*dis*cos + 0.3*(ArmorCenter_list[i].y-R_center.y);
                                // L_vertices[2].x = ArmorCenter_list[i].x - 0.025*dis*sin - 0.3*(ArmorCenter_list[i].x-R_center.x);   L_vertices[2].y = ArmorCenter_list[i].y + 0.025*dis*cos - 0.3*(ArmorCenter_list[i].y-R_center.y);
                                // L_vertices[3].x = ArmorCenter_list[i].x + 0.025*dis*sin - 0.3*(ArmorCenter_list[i].x-R_center.x);   L_vertices[3].y = ArmorCenter_list[i].y - 0.025*dis*cos - 0.3*(ArmorCenter_list[i].y-R_center.y);
                                // getLwarp(src_img, final_img, L_vertices);//裁减流动灯条区域

                                Mat L_warped_image = getLwarp(frame1_inrange_dilate_local, display_frame, frame1_inrange_dilate_local, L_vertices); // 调用 getLwarp 并存储返回值
                                if(L_warped_image.size().width > 0 && L_warped_image.size().height > 0)
                                {
                                    // if(get_L_SVMModel())// 设置流动灯条的svm
                                    // {
                                    L_list.emplace_back(L_middle);    
                                        
                                    circle(display_frame, L_middle, 3, Scalar(0, 255, 0), -1);// 画出灯条的中心点

                                    g_L_middle = L_middle;    // 使用g_前缀表示全局变量
                                    g_L_middle_valid = true;
                                    string area_text = "L Area: " + to_string(int(contourArea(contours_local[n]))); 
                                    putText(display_frame, area_text, Point(50, 90), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
                                    L_flag = true;
                                    // }
                                }
                                else
                                {
                                    //cout << "L_warped_image为空" << endl;
                                    continue;
                                }
                                // return Point2d(); // Return a default value if no contours are found
                            }
                            else
                            {
                                //cout << "请调节流动灯条区域的大小以实现框选" << endl;
                                continue;
                            }
                        }
                        else
                        {
                            //cout << "灯条中心点与R标中心点的距离不符合" << endl;
                            continue;
                        }
                    }
                    else
                    {
                        //cout << "流动灯条长度和R标半径比率不符合" << endl;
                        continue;
                    }
                }
                else
                {
                    //cout << "流动灯条的长宽比不符合" << endl;
                    continue;
                }
            }
            else
            {
                //cout << "未检测到流动灯条" << endl;
                continue;
            }
        }

        // 处理扇页
        for (int n = 0; n < contours_local.size(); n++)
        {
            if (centers.size() > 10)
            {
                centers.clear();
            }
            if(L_flag && g_L_middle_valid)            
            {    
                Point2f L_middle((L_vertices[0].x + L_vertices[2].x) / 2, (L_vertices[0].y + L_vertices[2].y) / 2); // 预想画出灯条的中心点
                // cout << "L_middle: " << L_middle << endl;
                R_to_L_distance = sqrt((L_middle.x - R_list[0].x) * (L_middle.x - R_list[0].x) + (L_middle.y - R_list[0].y) * (L_middle.y - R_list[0].y));
                // cout << "R标中心到灯条中心的距离为 " << R_to_L_distance << endl;
                L_to_F_distance = sqrt((center_local.x - L_middle.x) * (center_local.x - L_middle.x) + (center_local.y - L_middle.y) * (center_local.y - L_middle.y));
                // cout << "灯条中心到扇页中心的距离为 " << L_to_F_distance << endl;                
                vector<Point> approx;
                approxPolyDP(contours_local[n], approx, arcLength(contours_local[n], true) * 0.04, true);
                

                /* 这里的面积不能调成0,因为会使后续判断点集数量小于5个点，导致程序终止 */
                if (approx.size() >= 1 && contourArea(contours_local[n]) >= min_F_Area && contourArea(contours_local[n]) <= max_F_Area)//1
                {
                    // cout << "L_flag: " << L_flag << endl;
                    Mat mask = Mat::zeros(frame1_inrange_dilate_local.size(), CV_8UC1); 
                    frame1_inrange_dilate_local.copyTo(mask, frame1_inrange_dilate_local >= 255);
                    drawContours(mask, contours_local, (int)n, Scalar(255, 255, 255), FILLED); 
                    bool isValidColor = true; // 目前没有用到(可删)
                    if (isValidColor)
                    {
                        minEnclosingCircle(contours_local[n], center_local, F_radius);
                        // cout<<"其实已经检查到扇页了"<<endl;
                        RotatedRect Fan_rrect = fitEllipse(contours_local[n]);
                        if (!R_list.empty())
                        {
                            // cout << "uuuuuuuu" << endl;
                            R_to_Fan_distance = sqrt((center_local.x - R_list[0].x) * (center_local.x - R_list[0].x) + (center_local.y - R_list[0].y) * (center_local.y - R_list[0].y));
                            // cout << " R标中心点到扇页中心点的距离 " << R_to_Fan_distance << endl;
                            prescreen_ratio = R_to_Fan_distance / 2 / F_radius;
                            //cout << "R标中心到扇页中心的距离与扇页直径的比值" << prescreen_ratio << endl;
                            if (prescreen_ratio >= min_R_Ratio && prescreen_ratio <= max_R_Ratio) // 2
                            {
                                if (F_radius <= max_F_Radius)//3
                                {
                                
                                    if (R_to_L_distance <= 1e-6) // 使用一个很小的阈值代替严格的0
                                    {
                                        cout << "R_to_L_distance接近0" << endl;
                                        continue;
                                    }                                   
                                    // 添加安全检查
                                    if (isfinite(L_to_F_distance) && isfinite(R_to_L_distance)) 
                                    {
                                        // cout << "R_to_L_distance: " << R_to_L_distance << endl;
                                        // cout << "L_to_F_distance: " << L_to_F_distance << endl;
                                        float L_to_F_to_R_distance_ratio = L_to_F_distance / R_to_L_distance; // 灯条中心到R标中心 与 扇页中心到R标中心比值
                                        // cout << "灯条中心到R标中心 与 灯条中心到扇页中心比值" << L_to_F_to_R_distance_ratio << endl; // 大致是1左右
                                        // 添加比值的合理性检查
                                        if (isfinite(L_to_F_to_R_distance_ratio))
                                        {
                                            // cout << "55555555555555" << endl;

                                            // cout << "L_to_F_distance: " << L_to_F_distance << endl;
                                            // cout << "R_to_L_distance: " << R_to_L_distance << endl;
                                            // cout << "比值: " << L_to_F_to_R_distance_ratio << endl;

                                            if (L_to_F_to_R_distance_ratio >= min_L_to_F_to_R_distance_ratio &&
                                                L_to_F_to_R_distance_ratio <= max_L_to_F_to_R_distance_ratio)
                                            {
                                                double angle = calculateAngle(R_list[0], g_L_middle, center_local);
                                                if (angle == -1) 
                                                {
                                                    // cout << "angle" << angle << endl;
                                                    // cout << "三个中心点不在一条直线上" << endl;
                                                    continue;  // 直接返回，不进行后续框选
                                                }
                                                else
                                                {
                                                    //画出整个扇页中心点的圆环形运动轨迹
                                                    circle(display_frame, R_list[0], R_to_Fan_distance, Scalar(0, 100, 0), 2, LINE_AA);
                                                    cout<<"圆环的周长是： "<< 2 * 3.1415926 * R_to_Fan_distance << endl;
                                                    
                                                    // cout << "完成" << endl;
                                                    ellipse(display_frame, Fan_rrect, Scalar(255, 255, 255), 2, LINE_AA); 
                                                    circle(display_frame, center_local, 3, Scalar(255, 255, 255), FILLED);
                                                    // cout << "——————————————————————————————" << endl;


                                                    //画出大符和小符的预测点（true默认小符）
                                                    if(predictJudgement)
                                                    {
                                                        Point2f vec = center_local - R_list[0];
                                                        // small_forecast.x = R_list[0].x + R_to_Fan_distance * cos(small_s / R_to_Fan_distance);
                                                        small_forecast.x = R_list[0].x + vec.x * cos((isClockwise ? -1 : 1) * small_s / R_to_Fan_distance) - vec.y * sin((isClockwise ? -1 : 1) * small_s / R_to_Fan_distance);
                                                        small_forecast.y = R_list[0].y + vec.x * sin((isClockwise ? -1 : 1) * small_s / R_to_Fan_distance) + vec.y * cos((isClockwise ? -1 : 1) * small_s / R_to_Fan_distance);


                                                        // 更新全局预测点
                                                        g_small_forecast = small_forecast;
                                                            
                                                        // 添加调试输出
                                                        cout << "预测点已更新: " << small_forecast << endl;


                                                        // vec.x *cosA - vec.y *sinA;
                                                        // vec.x *sinA + vec.y *cosA;

                                                        // small_forecast.y = R_list[0].y + R_to_Fan_distance * sin(small_s / R_to_Fan_distance);

                                                        circle(display_frame, small_forecast, 5, Scalar(255, 255, 0), FILLED);
                                                        
                                                        string area_text = "F Area: " + to_string(int(contourArea(contours_local[n]))); 
                                                        putText(display_frame, area_text, Point(50, 110), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
                                                        CommunicaionFlag = true;
                                                    }
                                                    else
                                                    {
                                                        circle(display_frame, big_forecast, 3, Scalar(255, 0, 255), FILLED);
                                                    }
                                                    

                                                    // 添加调试输出
                                                    // cout << "Fan_rrect center: " << Fan_rrect.center << endl;
                                                    // cout << "Fan_rrect size: " << Fan_rrect.size << endl;
                                                    // cout << "center_local: " << center_local << endl;
                                                    
                                                    // cout << "扇页中心点坐标(原图): " << center_local << endl;

                                                    // cout << "已开启打符模式" << endl; 
                                                    centers.push_back(center_local);
                                                    if (!centers.empty()) 
                                                    {
                                                        R = centers[0]; // 或者使用 calculateCenter(centers) 得到平均中心
                                                        return R;
                                                    }
                                                    // cout<<"哈哈哈"<<centers.size()<<endl;
                                                    // R = centers[0];
                                                    // return R; // 扇页中心参考点
                                                    // 根据需要返回 R 或者其他值
                                                    
                                                    // 可打印 centers.size() 提示累计的点数
                                                    // cout << "累计的 centers 数量: " << centers.size() << endl;
                                                }
                                            }
                                            else
                                            {
                                                cout << "灯条中心到R标中心 与 扇页中心到R标中心比值不符" << endl;
                                                continue;
                                            }
                                        }
                                        else 
                                        {
                                            cout << "比值计算无效" << endl;
                                            return Point2d();
                                            continue;
                                        }
                                    }
                                    else 
                                    {
                                        cout << "距离计算无效" << endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    cout << "请上调maxRadius" << endl;
                                    continue;
                                }
                            }
                            else
                            {
                                // cout << "不符合R标到扇页的预筛选比率" << endl;
                                continue;
                            }
                        }
                        else
                        {
                            cout << "R_list为空无法计算R标到扇页中心的距离" << endl;
                            continue;
                        }
                    }
                }
            }
            else
            {
                //cout << "未检测到扇页" << endl;
                continue;
            }
        }
    }

    // else
    // {
    //     cout << "整个画面均未找到R标轮廓" << endl;
    //     return Point2d();
    // }

    // 循环结束后，再对 centers 进行进一步处理
    // if (!centers.empty())
    // {
        
    //}
    
    else
    {
         cout << "未检测到有效的 centers" << endl;
         return Point2d();
    }
    // cout<<"small_forecast.x: "<<small_forecast.x<<"small_forecast.y: "<<small_forecast.y<<endl;

}


Mat getLwarp(Mat &frame, Mat &display_frame, Mat &L_img, Point2f vertices[4])
{
    Mat rotation, L_warped_image;
    Point2f dst_points[4];
    dst_points[3] = Point2f(20.0, 100.0);
    dst_points[2] = Point2f(0.0, 100.0);
    dst_points[1] = Point2f(20.0, 0.0);
    dst_points[0] = Point2f(0.0, 0.0);
    rotation = getPerspectiveTransform(vertices, dst_points);
    warpPerspective(L_img, L_warped_image, rotation, Size(20, 100), INTER_LINEAR,BORDER_CONSTANT);
    morphologyEx(L_warped_image, L_warped_image, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(15, 15)));//开闭运算
    
    // imshow("L_warp",L_warped_image);
    
    circle(display_frame, vertices[0], 1, Scalar(0, 255, 0), 2, 8);      // line(display_frame, vertices[0], vertices[1], Scalar(200, 23, 255), 2);
    circle(display_frame, vertices[1], 1, Scalar(0, 255, 0), 2, 8);      line(display_frame, vertices[1], vertices[3], Scalar(200, 23, 255), 2);
    circle(display_frame, vertices[2], 1, Scalar(0, 255, 0), 2, 8);      line(display_frame, vertices[2], vertices[0], Scalar(200, 23, 255), 2);
    circle(display_frame, vertices[3], 1, Scalar(0, 255, 0), 2, 8);      // line(display_frame, vertices[3], vertices[2], Scalar(200, 23, 255), 2);
    return (L_warped_image); 
}



double calculateAngle(Point2f &A, Point2f &B, Point2f &C)// 默认第一个点为R标中心点，第二个为灯条中心点，第三个点为扇页中心点
{
    bool ANGLE = false;

    Point2f AB = B - A;  // 向量 AB
    Point2f BC = C - B;  // 向量 BC

    double angle1 = atan2(AB.y, AB.x) * 180.0 / CV_PI;
    double angle2 = atan2(BC.y, BC.x) * 180.0 / CV_PI;

    double angleDiff = angle2 - angle1;  // 计算角度差

    if (angleDiff > 180) angleDiff -= 360;  // 限制在 [-180, 180]
    if (angleDiff < -180) angleDiff += 360;
    // cout << "fabs(angleDiff): " << fabs(angleDiff) << endl;

    if (fabs(angleDiff) <= 2.0) 
    {
        // cout << "fabs(angleDiff): " << fabs(angleDiff) << endl;
        // cout << "L_middle: " << B << endl;
        return fabs(angleDiff);                                 // 如果在范围内，返回角度差的绝对值
    }
    else 
    {
        //cout << "角度超出±2.0度范围: " << angleDiff << "度" << endl;
        return -1; // 返回-1表示角度超出范围
    }
}


Mat getRwarp(Mat &frame, Point2f vertices[4])//R标框选
{
    Mat rotation, R_warped_image;
    Point2f dst_points[4];
    dst_points[0] = Point2f(0.0, 0.0);
    dst_points[1] = Point2f(50.0, 0.0);
    dst_points[2] = Point2f(50.0, 50.0);
    dst_points[3] = Point2f(0.0, 50.0);
    rotation = getPerspectiveTransform(vertices, dst_points);
    warpPerspective(frame, R_warped_image, rotation, Size(50, 50), INTER_LINEAR, BORDER_CONSTANT);
    return (R_warped_image);  
}

// 声明静态指针，只在首次调用时加载
Ptr<ml::SVM> get_R_SVMModel()
{
    static Ptr<ml::SVM> svm = ml::SVM::load("/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/R_svm(2025_2_23).xml");
    
    if (svm.empty()) {
        cerr << "Failed to load R标SVM model!" << endl;
    }
    //cout << "999999999999" << endl;
    return svm;
}


Ptr<ml::SVM> get_L_SVMModel()
{
    static Ptr<ml::SVM> svm = ml::SVM::load("");
    
    if (svm.empty()) {
        cerr << "Failed to load 流动灯条SVM model!" << endl;
    }
    //cout << "6666666666666" << endl;
    return svm;
}


bool R_Match(Mat &R_warp)
{
    // Mat frame;
    // Point2f vertices[4];
    // R_warp = getRwarp(frame, vertices);
    if (R_warp.empty())
    {
        cerr << "输入的R_warp图像为空!" << endl;
        return false;
    }
    Ptr<ml::SVM> svm = get_R_SVMModel();
    if (svm.empty())
    {
        cerr << "Failed to load R标SVM model!" << endl;
        return false;
    }
    resize(R_warp, R_warp, Size(50, 50));
    // cvtColor(R_warp, R_warp, COLOR_BGR2GRAY);//以下为作二值化处理
    // threshold(~R_warp, R_warp, 200, 255, THRESH_BINARY_INV);
    imshow("R_warp",R_warp);
    // // 添加调试输出，检查预处理后的图像
    // cout << "Preprocessed R_warp: " << R_warp << endl;
    Mat p = R_warp.reshape(1, 1);
    p.convertTo(p, CV_32FC1);
    int var_count = svm->getVarCount();
    // // 添加调试输出，检查转换后的输入数据
    // cout << "Input data for SVM prediction: " << p << endl;
    if (p.cols != var_count) 
    {
        cerr << "R标的Feature count mismatch: expected " << var_count << ", got " << p.cols << endl;
        return false;
    }
    // cout << "Input data for SVM prediction: " << p << endl;
    int response = (int)svm->predict(p);
    // cout << "response: " << response << endl;
    if(response == 0)
        {return false;}
    else
        {return true;}
}


bool L_Match(Mat &L_warp)
{
    // Mat frame;
    // Point2f vertices[4];
    // R_warp = getRwarp(frame, vertices);

    if (L_warp.empty())
    {
        cerr << "Input L_warp is empty!" << endl;
        return false;
    }

    Ptr<ml::SVM> svm = get_L_SVMModel();
    if (svm.empty())
    {
        cerr << "Failed to load 流动灯条SVM model!" << endl;
        return false;
    }

    resize(L_warp, L_warp, Size(50, 50));
    // cvtColor(R_warp, R_warp, COLOR_BGR2GRAY);//以下为作二值化处理
    // threshold(~R_warp, R_warp, 200, 255, THRESH_BINARY_INV);
    
    // imshow("L_warp",L_warp);
    
    // // 添加调试输出，检查预处理后的图像
    // cout << "Preprocessed R_warp: " << L_warp << endl;

    Mat p = L_warp.reshape(1, 1);
    p.convertTo(p, CV_32FC1);
    int var_count = svm->getVarCount();
    // // 添加调试输出，检查转换后的输入数据
    // cout << "Input data for SVM prediction: " << p << endl;
    if (p.cols != var_count) 
    {
        cerr << "流动灯条的Feature count mismatch: expected " << var_count << ", got " << p.cols << endl;
        return false;
    }
    // cout << "Input data for SVM prediction: " << p << endl;
    int response = (int)svm->predict(p);
    // cout << "response: " << response << endl;
    if(response == 0)
        {return false;}
    else
        {return true;}
}


Point2f calculateCenter(const vector<Point2f> &centers)  //计算扇页的F点
{
    // 添加输入验证
    if (centers.empty()) 
    {
        cout << "centers为空" << endl;
        return Point2f(0, 0);
    }

    if (centers.size() >= 5)
    {
        // 添加详细的坐标输出
        // cout << "使用四点平均计算:" << endl;
        // cout << "Point1: " << centers[1] << endl;
        // cout << "Point2: " << centers[2] << endl;
        // cout << "Point3: " << centers[3] << endl;
        // cout << "Point4: " << centers[4] << endl;
        
        // cout << "大于5个轮廓点的计算结果: " << ((centers[4] + centers[3] + centers[2] + centers[1]) / 4) << endl;
        // cout << "888888888888" << endl;
        return (centers[4] + centers[3] + centers[2] + centers[1]) / 4;
    }
    // else
    // {
    //     Point2f sum(0, 0);
    //     for (const auto &center : centers)
    //     {
    //         sum += center;
    //     }
    //     float avgX = sum.x / static_cast<float>(centers.size());
    //     float avgY = sum.y / static_cast<float>(centers.size());
    //     cout << "小于5个轮廓点的计算结果: " << Point2f(avgX, avgY) << endl;
    //     return Point2f(avgX, avgY);
    // }
}


// centers.push_back(center); // 将圆心添加到数组中
// }
// }

// // 打印 centers 数组的内容以进行调试
// // cout << "centers 数组大小: " << centers.size() << endl;
// for (const auto &center : centers)
// {
// // cout << "center: " << center << endl;
// }

// if (!centers.empty()) // 这用于更新一系列值，为了记录某个状态或计算的中间结果。
// {
// last_theta3 = last_theta2;
// last_theta2 = last_theta1;
// last_theta1 = theta;
// R = centers[0]; // 参考点（Reference point）在这里，R 被赋予了一个具体的数值，这可能是因为在特定的应用场景中，观测噪声被认为是一个固定的、已知的值
// // cout << "R:::::::::::::" << R << endl;
// if (centers.size() >= 5)
// {
//     F= (centers[4] + centers[3] + centers[2] + centers[1]) / 4; // 特征点（Feature point)
// } 
// else
// {
//     Point2f sum(0, 0);
//     for (const auto &center : centers)
//     {
//         sum += center;
//     }
//     float avgX = sum.x / static_cast<float>(centers.size());
//     float avgY = sum.y / static_cast<float>(centers.size());
//     F = Point2f(avgX, avgY);
// }
// }
// else
// {
// continue;
// }

// // 打印 R 和 F 的值以进行调试
// // cout << "F: " << F << endl;

// double radius = sqrt((F.x - R.x) * (F.x - R.x) + (R.y - F.y) * (R.y - F.y));
// // if (radius == 0)
// // {
// //     cout << "半径为 0,无法计算角度" << endl;
// //     continue;
// // }
// // cout << "dx: " << F.x - R.x << ", dy: " << R.y - F.y << ", radius: " << radius << endl;
// if (fabs((F.x - R.x) / radius) > 1.0 || fabs((F.y - R.y) / radius) > 1.0)
// {
// cout << "输入值超出 acos 和 asin 的范围" << endl;
// continue;
// }
// double theta_x = acos((F.x - R.x) / radius);
// double theta_y = asin((F.y - R.y) / radius);
// // cout << "theta_x: " << theta_x << ", theta_y: " << theta_y << endl;
// /*
// 角度范围和周期性
// acos 和 asin 的返回值范围：
// acos 函数返回的角度范围是 [0, π] 弧度。
// asin 函数返回的角度范围是 [-π/2, π/2] 弧度。
// 确定完整角度：
// 当 theta_y > 0 时，说明点 F 在参考点 R 的上方或右上方。此时，theta_x 只能表示 x 轴正方向到点 F 的角度，但我们需要一个完整的角度表示。
// 通过 theta = 2π - theta_x，可以将角度调整到 [π, 2π] 的范围，从而覆盖完整的圆周角度。
// 避免角度不连续：
// 如果不引入 2π，当 theta_y 从正值变为负值时，角度会突然从接近 π 跳变到接近 0，这会导致角度变化量 deta_theta 突然变得非常大，影响后续的预测和滤波效果。
// 引入 2π 后，角度变化更加平滑，避免了这种不连续性。
// */
// if (theta_y > 0)
// {
// theta = 2 * 3.1415926535 - theta_x;
// }
// else
// {
// theta = theta_y;
// }
// deta_theta = theta - last_theta3;
// if (abs(deta_theta) > 0.23) // 0.23 弧度，大约等于 13.18 度。
// {
// // deta_theta = 0;
// deta_theta_fitter = 0.4 * deta_theta + (1 - 0.4) * deta_theta_fitter; // low一阶滤波
// deta_theta = deta_theta_fitter;
// }
// //----------------------------------------------------------------------------------------------
// // 当predictJudgement==0时，为小符预测
// //----------------------------------------------------------------------------------------------
// if (predictJudgement == 0)
// {
// pre_theta = theta + 3.1415926535 / 3 * 0.45 * abs(deta_theta) / (deta_theta); // 小符预测量
// }
// //----------------------------------------------------------------------------------------------
// // 当predictJudgement==1时，为大符预测
// //----------------------------------------------------------------------------------------------
// else
// {
// xx.setZero(); //????????????????????
// xx = FF * xx;
// PP = FF * PP * FF.transpose() + QQ;
// MatrixXd S = HH * PP * HH.transpose() + RR; // 更新
// MatrixXd K = PP * HH.transpose() * S.inverse();
// MatrixXd y = MatrixXd(1, 1);
// y << deta_theta - (HH * xx)(0, 0); // 这段代码计算观测残差 y，即观测值 deta_theta 减去预测值 (HH * xx)(0, 0)。
// xx += K * y;
// PP = (MatrixXd::Identity(2, 2) - K * HH) * PP;
// pre_theta = theta + 0.1 * (xx(0, 0)); // 大符预测量
// }
// pre = Point(R.x + radius * cos(pre_theta), R.y - radius * sin(pre_theta)); // 预测的点集，这里使用的是二维平面上的极坐标到直角坐标的转换公式
// // cout<<"pre_theta:"<<pre_theta<<endl;
// circle(frame, F, 3, Scalar(0, 255, 255), 5);   // 黄色
// circle(frame, R, 3, Scalar(255, 255, 0), 2);   // 青色
// circle(frame, pre, 4, Scalar(255, 0, 255), 8); // 紫色


void calculateTheta(double &theta, double &last_theta1, double &last_theta2, double &last_theta3, 
                    double &deta_theta, double &radius, Point2d R, Point2d F)
{
    // 添加详细的调试输出
    // cout << "Input parameters:" << endl;
    // cout << "R: " << R << endl;
    // cout << "F: " << F << endl;
    
    // 检查R和F是否为同一点
    // if (R == F) 
    // {
    //     cout << "错误:R点和F点坐标相同" << endl;
    //     radius = 0;
    //     return;
    // }
    
    // 添加参数验证
    // if (R.x == 0 && R.y == 0) 
    // {
    //     cout << "R点坐标无效" << endl;
    //     return;
    // }
    
    // if (F.x == 0 && F.y == 0) 
    // {
    //     cout << "F点坐标无效" << endl;
    //     return;
    // }
    last_theta3 = last_theta2;
    last_theta2 = last_theta1;
    last_theta1 = theta;
    radius = sqrt((F.x - R.x) * (F.x - R.x) + (R.y - F.y) * (R.y - F.y));
    
    // cout << "radius: " << radius << endl;

    if (fabs((F.x - R.x) / radius) > 1.0 || fabs((F.y - R.y) / radius) > 1.0)
    {
        cout << "输入值超出 acos 和 asin 的范围" << endl;
    }
    double theta_x = acos((F.x - R.x) / radius);
    double theta_y = asin((F.y - R.y) / radius);
    if (theta_y > 0)// 表示在圆的上半部分
    {
        theta = 2 * 3.1415926535 - theta_x;
    }
    else            // 表示在圆的下半部分
    {
        theta = theta_x;
    }
    deta_theta = theta - last_theta3;
    if (abs(deta_theta) > 0.23)
    {
        deta_theta = 0;
    }
}

double updateKalman(bool predictJudgement, double &pre_theta, double theta, double deta_theta, 
                  MatrixXd &xx, MatrixXd &FF, MatrixXd &PP, MatrixXd &HH, MatrixXd &RR, MatrixXd &QQ)
{       
    // cout << "QQ: " << QQ.rows() << "x" << QQ.cols() << endl;
    // cout << "RR: " << RR.rows() << "x" << RR.cols() << endl;
    // 添加参数验证
    if (!isfinite(theta) || !isfinite(deta_theta)) 
    {
        // cout << "updateKalman 输入参数无效:" << endl;
        // cout << "theta: " << theta << endl;
        // cout << "deta_theta: " << deta_theta << endl;
        return 0.0;  // 返回安全值
    }

    if (predictJudgement == TRUE)// 小符预测量
    {
        pre_theta = theta + 3.1415926535 / 3 * 0.45 * abs(deta_theta) / (deta_theta);
    }
    else
    {
        // cout << "QQ: " << QQ.rows() << "x" << QQ.cols() << endl;
        // cout << "RR: " << RR.rows() << "x" << RR.cols() << endl;

        xx.setZero(); 
        xx = FF * xx;
        //xx = FF * xx.prev;
        PP = FF * PP * FF.transpose() + QQ;
        MatrixXd S = HH * PP * HH.transpose() + RR; 
        MatrixXd K = PP * HH.transpose() * S.inverse();
        MatrixXd y = MatrixXd(1, 1);
        y << deta_theta - (HH * xx)(0, 0);
        xx += K * y;
        PP = (MatrixXd::Identity(2, 2) - K * HH) * PP;
        pre_theta = theta + 6 * (xx(0, 0)); // 大符预测量
    }
    return pre_theta;
}

Point2d calculatePrediction(Point2d R, double radius, double pre_theta)
{
    // 添加参数有效性检查
    if (!isfinite(pre_theta) || !isfinite(R.x) || !isfinite(R.y) || radius <= 0) // 检查一个数值是否为有限数
    {
        // cout << "Invalid parameters in calculatePrediction:" << endl;
        // cout << "pre_theta: " << pre_theta << endl;
        // cout << "R: " << R << endl;
        // cout << "radius: " << radius << endl;
        return Point2d(0, 0);  // 返回安全值
    }

    // 计算预测点
    Point2d pre;
    pre.x = R.x + radius * cos(pre_theta);

    // cout << "pre.x" << pre.x << endl;

    pre.y = R.y + radius * sin(pre_theta); // OpenCV坐标系y轴向下，需要加负号?????

    // cout << "pre.y" << pre.y << endl;

    // 检查计算结果
    if (!isfinite(pre.x) || !isfinite(pre.y)) 
    {
        cout << "预测点计算结果无效" << endl;
        return Point2d(0, 0);
    }
    return pre;
}

void drawPredictions(Point2d R, Point2d F, Point2d pre, Mat &frame1)
{
    vector<Point2f> centers;
    vector<Point2f> R_list;
    vector<Point2f> L_list;
    Mat display_frame = frame1.clone();

    R = processFrame(frame1, display_frame, centers, R_list, L_list, R, small_s);//延迟大
    // circle(frame1, F, 3, Scalar(0, 255, 255), 5);   //黄色
    // circle(frame1, R, 3, Scalar(255, 255, 0), 2);   //青色

    circle(frame1, pre, 4, Scalar(255, 0, 255), 8); // 紫色
}


void sendAngles(Mat &display_frame, vector<Point3f> &SHOOT, vector<Point2f> &twoDim, Point2f &small_forecast, UsbSerial usb_serial)
{
    static double yaw = 0, pitch = 0;
    // CommunicaionFlag = false;
    bool success = false;

    // cout << "small_forecast.x: " << small_forecast.x << " small_forecast.y: " << small_forecast.y << endl;
    
    if (fabs(small_forecast.x) < 1e-3 && fabs(small_forecast.y) < 1e-3) 
    {
        // cout << "预测点消失, small_forecast 清零！" << endl;
        yaw = 0;
        pitch = 0;
        CommunicaionFlag = false;
    }
    
    // 判断预测点是否有效
    bool valid_forecast = fabs(small_forecast.x) > 1e-3 || fabs(small_forecast.y) > 1e-3;
    
    if (!valid_forecast)
    {
        yaw = 0;
        pitch = 0;
        CommunicaionFlag = false;
    }
    else
    {
        Mat cam_matrix = (Mat_<double>(3, 3) << 2034.4, -6.2, 663.4457, 0, 2058.11, 672.665, 0, 0, 1);
        Mat distortion_coeff = (Mat_<double>(5, 1) << -0.08441, 0.7558, -0.00717, 0.0049, 0);

        SHOOT.clear();
        SHOOT.push_back(cv::Point3f(-2, 2, 0));
        SHOOT.push_back(cv::Point3f(2, 2, 0));
        SHOOT.push_back(cv::Point3f(2, -2, 0));
        SHOOT.push_back(cv::Point3f(-2, -2, 0));

        twoDim.clear();
        twoDim.emplace_back(Point2f(small_forecast.x - 100, small_forecast.y - 100));
        twoDim.emplace_back(Point2f(small_forecast.x + 100, small_forecast.y - 100));
        twoDim.emplace_back(Point2f(small_forecast.x + 100, small_forecast.y + 100));
        twoDim.emplace_back(Point2f(small_forecast.x - 100, small_forecast.y + 100));
        
        // cout << "solvePnP 之前, valid_forecast = " << valid_forecast << endl;

        Mat r, trans;
        if(CommunicaionFlag)
        {
            solvePnP(SHOOT, twoDim, cam_matrix, distortion_coeff, r, trans);
            success = true;
        }
        // cout << "0000000000000000000000000000000000000000000" << endl;

        if (success)
        {
            double qqx = trans.at<double>(0, 0);
            double qqy = trans.at<double>(1, 0);
            double qqz = trans.at<double>(2, 0);
            if (qqz != 0)
            {
                double qtan_pitch = qqy / sqrt(qqx * qqx + qqz * qqz);
                double qtan_yaw = qqx / qqz;
                double new_yaw = atan(qtan_yaw) * 180 / CV_PI + 1.5;
                double new_pitch = -atan(qtan_pitch) * 180 / CV_PI - 2;

                // double new_yaw = atan(qtan_yaw);
                // double new_pitch = -atan(qtan_pitch);

                if (!isnan(new_yaw) && !isnan(new_pitch))
                {
                    yaw = new_yaw;
                    pitch = new_pitch;
                    CommunicaionFlag = true;
                }
            }
        }
        else
        {
            yaw = 0;
            pitch = 0;
            CommunicaionFlag = false;
        }
    }

    // 确保通信失败时，yaw和 pitch 归零
    if (!CommunicaionFlag)
    {
        yaw = 0;
        pitch = 0;
    }

    // 显示角度值
    string area_text = "yaw: " + to_string(yaw) + " pitch: " + to_string(pitch);
    Scalar color = CommunicaionFlag ? Scalar(0, 255, 0) : Scalar(0, 0, 255);
    putText(display_frame, area_text, Point(50, 150), FONT_HERSHEY_SIMPLEX, 0.5, color, 1);

    // 调试输出
    cout << "通信状态: " << (CommunicaionFlag ? "正常" : "异常") << endl;
    cout << "yaw值: " << yaw << "   " << "pitch值: " << pitch << endl;

    // 发送数据
    SendPacket packet;
    packet.r1 = yaw;
    packet.r2 = pitch;
    usb_serial.send(packet);
}


// // 最小二乘法拟合正弦曲线
// // CostFunctor：你的 代价函数，它是一个类或结构体，需要实现 operator()
// // ResidualDim（1）：残差的维度，即 residual 向量的大小
// // ParamDim1, ParamDim2, ...（3）：待优化参数的维度（params 数组有 3 个参数）
// void fit_sinusoidal(const vector<double>& t, const vector<double>& spd, double& a, double& omega, double& b) 
// {
//     ceres::Problem problem;// 创建优化问题
//     double params[3] = {1.0, 1.9, 1.0};  // 初始值 a, omega, b

//     for (size_t i = 0; i < t.size(); ++i) {
//         problem.AddResidualBlock(
//             new ceres::AutoDiffCostFunction<SinusoidalResidual, 1, 3>(new SinusoidalResidual(t[i], spd[i])),
//             nullptr, params);
//     }

//     ceres::Solver::Options options;
//     options.linear_solver_type = ceres::DENSE_QR;
//     options.minimizer_progress_to_stdout = false;

//     ceres::Solver::Summary summary;
//     ceres::Solve(options, &problem, &summary);

//     a = params[0];
//     omega = params[1];
//     b = params[2];
// }



// // 卡尔曼滤波器相关变量
// KalmanFilter KF(2, 1, 0);  // 状态维度为2，测量维度为1
// Mat statePost = Mat::zeros(2, 1, CV_32F);  // 初始状态
// Mat processNoiseCov = Mat::eye(2, 2, CV_32F) * 1e-4;  // 过程噪声协方差
// Mat measurementNoiseCov = Mat::eye(1, 1, CV_32F) * 1e-2;  // 测量噪声协方差
// Mat errorCovPost = Mat::eye(2, 2, CV_32F) * 0.1;  // 后验估计误差协方差

// // 初始化卡尔曼滤波器
// void initializeKalmanFilter() 
// {
//     KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);  // 状态转移矩阵
//     KF.measurementMatrix = (Mat_<float>(1, 2) << 1, 0);  // 测量矩阵

//     setIdentity(KF.processNoiseCov, Scalar::all(1e-4));  // 过程噪声协方差
//     setIdentity(KF.measurementNoiseCov, Scalar::all(1e-2));  // 测量噪声协方差
//     setIdentity(KF.errorCovPost, Scalar::all(0.1));  // 后验估计误差协方差

//     KF.statePost.at<float>(0) = 2.0;  // 初始角速度
//     KF.statePost.at<float>(1) = 0.0;  // 初始角加速度
// }

// // 卡尔曼滤波器预测函数
// double kalmanPredict(double measured_spd) 
// {
//     // 预测
//     Mat prediction = KF.predict();

//     // 测量值
//     Mat measurement(1, 1, CV_32F);
//     measurement.at<float>(0) = measured_spd;

//     // 校正（更新）
//     Mat estimated = KF.correct(measurement);

//     // 返回估计的角速度
//     return estimated.at<float>(0);
// }