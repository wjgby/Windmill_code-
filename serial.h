#ifndef SERIAL_H
#define SERIAL_H

#include <cstddef>  // 添加此行，用于size_t定义
#include <cstdint> 
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "CRC.hpp"
#include <ctime>    // 如果需要 time.h 相关功能

struct ReceivePacket
{
  uint8_t header = 0x5A;
  uint8_t detect_color : 1;  // 0-red 1-blue
  bool reset_tracker : 1;
  uint8_t target : 2;
  uint8_t reserved : 4;
  float roll;
  float pitch;
  float yaw;
  float aim_x;
  float aim_y;
  float aim_z;
  uint16_t checksum = 0;
} __attribute__((packed));

struct SendPacket
{
  uint8_t header = 0xA5;
  bool tracking : 1;
  uint8_t id : 3;          // 0-outpost 6-guard 7-base
  uint8_t armors_num : 3;  // 2-balance 3-outpost 4-normal
  uint8_t reserved : 1;
  float x = 0;
  float y = 0;
  float z = 0;
  float yaw = 0;
  float vx = 0;
  float vy = 0;
  float vz = 0;
  float v_yaw = 0;
  float r1 = 0;
  float r2 = 0;
  float dz = 0;
  uint16_t checksum = 0;
} __attribute__((packed));

class UsbSerial
{
public:
  bool SerialInit();
  ReceivePacket SerialRecData();
  void closePort();
  void send(SendPacket & packet);

private:
  int fd;
  int receive_num = 28;
  int send_num = 48;
};
#endif  // SERIAL_H
