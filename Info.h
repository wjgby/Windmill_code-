#ifndef INFO_H
#define INFO_H
// #include "PCH.h"
#include <string.h>

#include <condition_variable>
#include <iostream>
#define small_x 135.0 / 2.0 / 1000
#define small_y 55.0 / 2.0 / 1000
#define big_x 225 / 2.0 / 1000
#define big_y 55 / 2.0 / 1000
enum EnemyColor  //目标单位的颜色，图像处理用
{
  RED = 0,
  BLUE
};
enum AttackPattern  //目标类型
{
  ARMOR = 0,
  FU
};
enum ArmorSize  //装甲板大小
{
  SMALL_ARMOR = 0,
  BIG_ARMOR,
  INVALID
};
enum FindResult { COUPLIE_LIGHT_FOUND = 0, NO_COUPLIE_LIGHT_FOUND, ARMOR_FOUND, NO_ARMOR_FOUND };
enum RobotType {
  NO = 0,
  HERO,
  TRUCK,
  INFANTRY_3,
  INFANTRY_4,
  INFANTRY_5,
  SENTRY,
  HOME = 8

};

class img_get
{
public:
  cv::Mat frame;
  cv::Mat frames;
  int flages;
  int flages1;
  std::vector<int> flags;
  std::vector<int> flag;
  std::mutex mx;
  std::mutex mxs;
  std::condition_variable cond;
  std::condition_variable conds;
};

struct OBB
{
  float cx;
  float cy;
  float longside;
  float shortside;
  float theta_pred;
  float max_class_score;
  int class_idx;
};

struct BoxInfo
{
  int id;               //������id
  float confidence;     //������Ŷ�
  cv::RotatedRect box;  //���ο�
  cv::Point2f vertice[5];//您可以存储 5 个二维点。
};
inline std::string debug_yaml = "/home/newmaker11/国赛能量机关总文件夹//国赛能量机关/config/debug.yaml";
#endif  // INFO_H
