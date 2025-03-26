#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "mvvideocapture.h"
#include "serial.h"
#include "cameraapi.h"
#include "Info.h"
#include "CRC.hpp"
#include "CameraApi.h" //相机SDK的API头文件
#include "opencv2/core/core.hpp"
#include <stdio.h>
#include <ceres/ceres.h>
#include <ceres/rotation.h>
// #include <cstddef>   // 用于size_t
// #include <cstdint>   // 用于uint8_t等类型
// #include <ctime>    // 如果需要 time.h 相关功能

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace ceres;

extern int red_blue_threshold;
extern int R_R_B;
extern int R_R_G;
extern int R_R_R;
extern int B_R_B;
extern int B_R_G;
extern int B_R_R;
extern int R_R_BB;
extern int R_R_GG;
extern int R_R_RR;
extern int B_R_BB;
extern int B_R_GG;
extern int B_R_RR;

extern int R_FU_B;
extern int R_FU_G;
extern int R_FU_R;
extern int B_FU_B;
extern int B_FU_G;
extern int B_FU_R;
extern int R_FU_BB;
extern int R_FU_GG;
extern int R_FU_RR;
extern int B_FU_BB;
extern int B_FU_GG;
extern int B_FU_RR;

extern int min_F_Area;
extern int max_F_Area;

extern int min_R_Ratio;
extern int max_R_Ratio;

extern int min_L_Ratio;
extern int max_L_Ratio;

extern int min_RL_Ratio;// 目的是和R标的框选联系起来
extern int max_RL_Ratio;

extern int min_R_Area;
extern int max_R_Area;

extern int min_L_Area;
extern int max_L_Area;

extern int min_RL_Distance_Ratio;
extern int max_RL_Distance_Ratio; 

extern int min_L_to_F_to_R_distance_ratio;
extern int max_L_to_F_to_R_distance_ratio;

extern int max_F_Radius;

extern int gamma_value;
extern int exposure_value;
extern int gain_value;


////////////////////////////////以下是对标段瞄的相机参数的声明过程//////////////////////////////////////

// 添加新的全局变量
// extern int exposure_time;  // 曝光时间(微秒)
// extern int analog_gain;     // 模拟增益
// extern int r_gain;         // R通道增益
// extern int g_gain;         // G通道增益 
// extern int b_gain;         // B通道增益
// extern int saturation;     // 饱和度
// extern int gamma;          // gamma值

/////////////////////////////////////////////////////////////////////////////////////////////////




extern int small_s;
extern Point2f g_small_forecast;  // 声明全局预测点

extern Ptr<ml::SVM> svm;


// 声明函数
// bool isColorInRange(const Vec3b &color, const Scalar &lowerBound, const Scalar &upperBound);
void initializeKalmanFilter(MatrixXd &xx, MatrixXd &PP, MatrixXd &FF, MatrixXd &HH, MatrixXd &QQ, MatrixXd &RR);
void create_Trackbar(int &R_R_B, int &R_R_G, int &R_R_R, int &B_R_B, int &B_R_G, int &B_R_R, int &R_R_BB, int &R_R_GG, int &R_R_RR, int &B_R_BB, int &B_R_GG, int &B_R_RR, int &min_F_Area, int &max_F_Area, int &min_R_Ratio, int &max_R_Ratio, int &min_L_Ratio, int &max_L_Ratio, int &min_RL_Ratio, int &max_RL_Ratio, int &min_R_Area, int &max_R_Area, int &min_L_Area, int &max_L_Area, int &min_RL_Distance_Ratioint, int &max_RL_Distance_Ratioint, int &min_L_to_F_to_R_distance_ratio, int &max_L_to_F_to_R_distance_ratio, int &max_F_Radius, int &gamma_value, int &exposure_value, int &gain_value, int &red_blue_threshold, int &small_s);
Point2d processFrame(Mat &frame, Mat &display_frame, vector<Point2f> &centers, vector<Point2f> &R_list, vector<Point2f> &L_list, Point2d &R, int &small_s);
double calculateAngle(Point2f &A, Point2f &B, Point2f &C);
Ptr<ml::SVM> get_R_SVMModel();
Ptr<ml::SVM> get_L_SVMModel();
bool R_Match(Mat &R_warp);
Mat getRwarp(Mat &frame, Point2f vertices[4]);
Mat getLwarp(Mat &frame, Mat &display_frame, Mat &L_img, Point2f vertices[4]);
Point2f calculateCenter(const vector<Point2f> &centers);
void calculateTheta(double &theta, double &last_theta1, double &last_theta2, double &last_theta3, double &deta_theta, double &radius, Point2d R, Point2d F);
double updateKalman(bool predictJudgement, double &pre_theta, double theta, double deta_theta, MatrixXd &xx, MatrixXd &FF, MatrixXd &PP, MatrixXd &HH, MatrixXd &RR, MatrixXd &QQ);
Point2d calculatePrediction(Point2d R, double radius, double pre_theta);
void drawPredictions(Point2d R, Point2d F, Point2d pre, Mat &frame1);
void sendAngles(Mat &display_frame, vector<Point3f> &SHOOT, vector<Point2f> &twoDim, Point2f &small_forecast, UsbSerial usb_serial);
// template <typename T>
// bool SinusoidalResidualFunction(const T *const params, T *residual, const T t, const T spd);
// void fit_sinusoidal(const vector<double> &t, const vector<double> &spd, double &a, double &omega, double &b);


///////////////////////////////////////////////////////////////////
// void onTrackbar(int, void *);
// void createCameraTrackbars();
///////////////////////////////////////////////////////////////////