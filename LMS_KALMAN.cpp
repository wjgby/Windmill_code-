#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <ceres/ceres.h>

using namespace std;
using namespace cv;
using namespace Eigen;

// ---------- 1️⃣ 目标函数：sin(𝜔t) + b ----------
struct SinusoidalResidual {
    SinusoidalResidual(double t, double spd) : t_(t), spd_(spd) {}

    template <typename T>
    bool operator()(const T* const params, T* residual) const {
        T a = params[0];
        T omega = params[1];
        T b = params[2];

        residual[0] = a * sin(omega * T(t_)) + b - T(spd_);
        return true;
    }

private:
    double t_, spd_; // t_：时间点； spd_：观测值
};

// ---------- 2️⃣ 最小二乘拟合函数 ----------
void fit_sinusoidal(const vector<double>& t, const vector<double>& spd, double& a, double& omega, double& b) {
    ceres::Problem problem;// ceres::Problem 对象，表示待求解的优化问题。
    double params[3] = {1.0, 1.9, 2.0};  // 初始值 a, omega, b

    for (size_t i = 0; i < t.size(); ++i) {
        problem.AddResidualBlock(
            new ceres::AutoDiffCostFunction<SinusoidalResidual, 1, 3>(new SinusoidalResidual(t[i], spd[i])),
            nullptr, params);
    }


    // 配置求解器的参数（如线性求解器类型和是否输出进度信息）。
    // 调用 ceres::Solve 函数，求解非线性最小二乘问题。
    // 将求解结果存储在 summary 中。
    ceres::Solver::Options options;// options：求解器的配置选项。
    options.linear_solver_type = ceres::DENSE_QR;// DENSE_QR：线性求解器用于求解优化问题中的线性方程组
    options.minimizer_progress_to_stdout = false;// 控制是否将优化过程的进度信息输出到标准输出（stdout）。



    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    a = params[0];
    omega = params[1];
    b = params[2];
}

// ---------- 3️⃣ 卡尔曼滤波器 ----------
class KalmanPredictor {
public:
    KalmanPredictor() {
        KF.init(2, 1, 0);
        
        KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);// 系统的状态转移模型
        KF.measurementMatrix = (Mat_<float>(1, 2) << 1, 0);// 从状态到测量值的映射关系
        
        setIdentity(KF.processNoiseCov, Scalar::all(1e-4));// 过程噪声协方差
        setIdentity(KF.measurementNoiseCov, Scalar::all(1e-2));// 测量噪声协方差
        setIdentity(KF.errorCovPost, Scalar::all(0.1));// 误差协方差

        //statePost 存储了根据当前观测值修正后的状态估计值
        KF.statePost.at<float>(0) = 2.0;  // 角速度初值
        KF.statePost.at<float>(1) = 0.0;  // 角加速度初值
    }

    double predict(double measured_spd) {
        Mat prediction = KF.predict();
        Mat measurement(1, 1, CV_32F);
        measurement.at<float>(0) = measured_spd;

        Mat estimated = KF.correct(measurement);
        return estimated.at<float>(0);
    }

private:
    KalmanFilter KF;
};

// ---------- 4️⃣ 处理时间误差 ----------
double correct_time_error(double observed_spd, double predicted_spd, double omega, double a, double t) {
    double delta_spd = observed_spd - predicted_spd;
    double estimated_dt = delta_spd / (omega * a * cos(omega * t));
    return estimated_dt;// estimated_dt：估计的时间差
}
// observed_spd：观测到的速度值（实际测量值）。
// predicted_spd：模型预测的速度值。

// ---------- 5️⃣ 主程序 ----------
int main() {
    vector<double> t_samples;
    vector<double> spd_samples;
    
    KalmanPredictor KF_predictor;
    
    double a = 1.0, omega = 1.9, b = 1.0;  // 初始化
    double t = 0.0;
    double dt = 0.01;  // 采样间隔

    for (int i = 0; i < 1000; ++i) {
        double true_spd = a * sin(omega * t) + b;
        double observed_spd = true_spd + ((rand() % 100 - 50) / 500.0);  // 添加噪声

        // 采样
        t_samples.push_back(t);
        spd_samples.push_back(observed_spd);
        
        // 滑动窗口处理
        if (t_samples.size() > 50) {
            t_samples.erase(t_samples.begin());
            spd_samples.erase(spd_samples.begin());
        }

        // 每 50 个样本进行一次拟合
        if (t_samples.size() == 50) {
            fit_sinusoidal(t_samples, spd_samples, a, omega, b);
        }

        // 卡尔曼滤波预测
        double predicted_spd = KF_predictor.predict(observed_spd);

        // 误差补偿
        double estimated_dt = correct_time_error(observed_spd, predicted_spd, omega, a, t);
        double corrected_spd = a * sin(omega * (t + estimated_dt)) + b;

        // 输出
        cout << "t: " << t << " | True: " << true_spd << " | Observed: " << observed_spd
             << " | Predicted: " << predicted_spd << " | Corrected: " << corrected_spd << endl;

        t += dt;
    }

    return 0;
}

