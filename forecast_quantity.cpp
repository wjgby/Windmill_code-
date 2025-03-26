#include <cmath>
#include <opencv2/opencv.hpp>

// 定义旋转点控制器类
class RotatingPointController {
private:
  cv::Point2f center;  // 旋转中心
  double radius;       // 圆环半径
  double currentAngle; // 当前角度（弧度）
  double arcOffset;    // 弧长偏移量
  double angleOffset;  // 角度差（由弧长计算）

public:
  RotatingPointController(cv::Point2f _center, double _radius)
      : center(_center), radius(_radius), currentAngle(0), arcOffset(0),
        angleOffset(0) {}

  // 设置弧长偏移量（自动计算角度差）
  void setArcOffset(double arc) {
    arcOffset = arc;
    angleOffset = arcOffset / radius; // 弧长转角度差
  }

  // 设置角度差（直接指定）
  void setAngleOffset(double angle) {
    angleOffset = angle;
    arcOffset = angleOffset * radius; // 角度差转弧长
  }

  // 更新当前角度（例如随时间变化或外部控制）
  void updateAngle(double angle) { currentAngle = angle; }

  // 获取原始点位置
  cv::Point2f getOriginalPoint() const {
    return cv::Point2f(center.x + radius * std::cos(currentAngle),
                       center.y + radius * std::sin(currentAngle));
  }

  // 获取偏移点位置
  cv::Point2f getOffsetPoint() const {
    return cv::Point2f(center.x + radius * std::cos(currentAngle + angleOffset),
                       center.y +
                           radius * std::sin(currentAngle + angleOffset));
  }

  // 获取当前弧长偏移和角度差
  double getArcOffset() const { return arcOffset; }
  double getAngleOffset() const { return angleOffset; }
};

// 全局变量（用于OpenCV回调）
RotatingPointController *g_controller = nullptr;
cv::Mat g_image;
int g_sliderArcOffset = 0; // 滑动条值（弧长偏移）

// OpenCV滑动条回调函数
void onTrackbar(int, void *) {
  // 设置弧长偏移量（滑动条范围为0~200）
  g_controller->setArcOffset(g_sliderArcOffset);

  // 清空画布
  g_image.setTo(cv::Scalar(0, 0, 0));

  // 绘制圆环
  cv::circle(g_image, g_controller->center, g_controller->radius,
             cv::Scalar(100, 100, 100), 1);

  // 获取点位置
  cv::Point2f original = g_controller->getOriginalPoint();
  cv::Point2f offset = g_controller->getOffsetPoint();

  // 绘制旋转中心
  cv::circle(g_image, g_controller->center, 5, cv::Scalar(0, 255, 0), -1);

  // 绘制原始点（红色）
  cv::circle(g_image, original, 8, cv::Scalar(0, 0, 255), -1);

  // 绘制偏移点（蓝色）
  cv::circle(g_image, offset, 8, cv::Scalar(255, 0, 0), -1);

  // 显示信息
  std::string text =
      "Arc Offset: " + std::to_string(g_controller->getArcOffset()) +
      "  Angle Offset: " + std::to_string(g_controller->getAngleOffset());
  cv::putText(g_image, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7,
              cv::Scalar(255, 255, 255), 2);

  // 更新显示
  cv::imshow("Dynamic Offset", g_image);
}

int main() {
  // 初始化控制器（中心在(400,300)，半径100）
  RotatingPointController controller(cv::Point2f(400, 300), 100);
  g_controller = &controller;

  // 创建OpenCV窗口和画布
  cv::namedWindow("Dynamic Offset");
  g_image = cv::Mat(600, 800, CV_8UC3, cv::Scalar(0, 0, 0));

  // 创建滑动条（弧长偏移范围0~200）
  cv::createTrackbar("Arc Offset", "Dynamic Offset", &g_sliderArcOffset, 200,
                     onTrackbar);

  // 初始更新
  onTrackbar(0, nullptr);

  // 主循环：模拟角度变化（例如随时间旋转）
  double angle = 0.0;
  while (true) {
    // 更新角度（例如按时间递增）
    angle += 0.02;
    controller.updateAngle(angle);

    // 触发重绘
    onTrackbar(0, nullptr);

    // 退出条件
    if (cv::waitKey(30) == 27) { // 按ESC退出
      break;
    }
  }

  return 0;
}

#include <cmath>
#include <opencv2/opencv.hpp>

// 计算向量绕圆心旋转后的坐标（不改变半径）
cv::Point2f rotateVector(const cv::Point2f &vec, double angle) {
  float cosA = std::cos(angle);
  float sinA = std::sin(angle);
  return cv::Point2f(vec.x * cosA - vec.y * sinA, vec.x * sinA + vec.y * cosA);
}

class CircleTracker {
private:
  cv::Point2f center; // 圆心坐标
  double radius;      // 圆半径
  double arcOffset;   // 弧长偏移量

public:
  CircleTracker(cv::Point2f c, double r) : center(c), radius(r), arcOffset(0) {}

  // 设置弧长偏移量
  void setArcOffset(double s) { arcOffset = s; }

  // 根据原始点位置计算偏移点
  cv::Point2f getOffsetPoint(const cv::Point2f &originalPoint) const {
    // 计算原始点相对于圆心的向量
    cv::Point2f vec = originalPoint - center;

    // 计算旋转角度（弧度）
    double deltaTheta = arcOffset / radius;

    // 旋转向量
    cv::Point2f rotatedVec = rotateVector(vec, deltaTheta);

    // 计算偏移点绝对坐标
    return center + rotatedVec;
  }

  // 获取当前弧长和对应角度差
  double getCurrentArc() const { return arcOffset; }
  double getCurrentAngle() const { return arcOffset / radius; }
};

// 全局变量和回调函数
CircleTracker *g_tracker = nullptr;
cv::Mat g_canvas;
int g_sliderValue = 0;

void onTrackbar(int, void *) {
  g_tracker->setArcOffset(g_sliderValue); // 滑动条范围0~200

  // 清空画布
  g_canvas.setTo(cv::Scalar(60, 60, 60));

  // 绘制圆环
  cv::circle(g_canvas, g_tracker->center, g_tracker->radius,
             cv::Scalar(200, 200, 200), 2);

  // 假设原始点由外部逻辑更新（此处模拟圆周运动）
  static double angle = 0;
  angle += 0.03;
  cv::Point2f originalPoint(
      g_tracker->center.x + g_tracker->radius * std::cos(angle),
      g_tracker->center.y + g_tracker->radius * std::sin(angle));

  // 计算偏移点
  cv::Point2f offsetPoint = g_tracker->getOffsetPoint(originalPoint);

  // 绘制连接线
  cv::line(g_canvas, originalPoint, offsetPoint, cv::Scalar(100, 255, 100), 2);

  // 绘制关键点
  cv::circle(g_canvas, g_tracker->center, 5, cv::Scalar(0, 255, 0), -1); // 圆心
  cv::circle(g_canvas, originalPoint, 8, cv::Scalar(0, 0, 255), -1); // 原始点
  cv::circle(g_canvas, offsetPoint, 8, cv::Scalar(255, 0, 0), -1); // 偏移点

  // 显示参数
  std::string info = "Arc: " + std::to_string(g_tracker->getCurrentArc()) +
                     "  Angle: " + std::to_string(g_tracker->getCurrentAngle());
  cv::putText(g_canvas, info, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.8,
              cv::Scalar(255, 255, 0), 2);

  cv::imshow("Dynamic Offset", g_canvas);
}

int main() {
  // 初始化追踪器（圆心在(400,300)，半径150）
  CircleTracker tracker(cv::Point2f(400, 300), 150);
  g_tracker = &tracker;

  // 创建窗口和画布
  cv::namedWindow("Dynamic Offset");
  g_canvas = cv::Mat(600, 800, CV_8UC3, cv::Scalar(60, 60, 60));

  // 创建滑动条
  cv::createTrackbar("Arc Offset", "Dynamic Offset", &g_sliderValue, 200,
                     onTrackbar);

  // 主循环
  while (true) {
    onTrackbar(0, nullptr); // 强制重绘
    if (cv::waitKey(30) == 27)
      break; // ESC退出
  }

  return 0;
}