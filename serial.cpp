#include "serial.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cstddef>   // 用于size_t
#include <cstdint>   // 用于uint8_t等类型
#include <ctime>    // 如果需要 time.h 相关功能
// #include <boost/hana/fwd/integral_constant.hpp>

/****************************************
 * @funcName SerialInit
 * @brief    打开串口权限
 * @para     无
 * @return   无
 * @date     2021.4.27
 * @author   wjl
 *****************************************/

bool UsbSerial::SerialInit()
{
  std::cout << "+--------------------------------------" << std::endl;
  std::cout << "|            Usb Serial Send Data     |" << std::endl;
  std::cout << "+--------------------------------------" << std::endl;
  // fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY);
  fd = open("/dev/ttyACM0", O_RDWR);
  if (-1 == fd) {
    std::cout << "Error open Serial" << std::endl;
    return false;
  } else {
    std::cout << "Open Serial Successfully" << std::endl;
  }
  /*---------- Setting the Attributes of --------- */

  struct termios SerialPortSettings; /* Create the structure                          */

  tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

  cfsetispeed(&SerialPortSettings, B115200); /* Set Read  Speed as 9600                       */
  cfsetospeed(&SerialPortSettings, B115200); /* Set Write Speed as 9600                       */

  SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
  SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
  SerialPortSettings.c_cflag &= ~CSIZE;  /* Clears the mask for setting the data size             */
  SerialPortSettings.c_cflag |= CS8;     /* Set the data bits = 8                                 */

  SerialPortSettings.c_cflag &= ~CRTSCTS; /* No Hardware flow Control                         */
  SerialPortSettings.c_cflag |=
    CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

  SerialPortSettings.c_iflag &=
    ~(IXON | IXOFF | IXANY); /* Disable XON/XOFF flow control both i/p and o/p */
  SerialPortSettings.c_iflag &=
    ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */

  SerialPortSettings.c_oflag &= ~OPOST; /*No Output Processing*/

  SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  if (
    (tcsetattr(fd, TCSANOW, &SerialPortSettings)) !=
    0) /* Set the attributes to the termios structure*/
  {
    std::cout << "\n  ERROR ! in Setting attributes" << std::endl;
  } else {
    std::cout << "\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity = none" << std::endl;
    std::cout << "Serial Start Work " << std::endl;
  }
  tcflush(fd, TCIFLUSH);
  std::cout << "+--------------------------------------" << std::endl;
  /*------------------------------- Write data to serial port -----------------------------*/
  return true;
}

ReceivePacket fromVector(const std::vector<uint8_t> & data)
{
  ReceivePacket packet;
  std::copy(data.begin(), data.end(), reinterpret_cast<uint8_t *>(&packet));
  return packet;
}

std::vector<uint8_t> toVector(const SendPacket & data)
{
  std::vector<uint8_t> packet(sizeof(SendPacket));
  std::copy(
    reinterpret_cast<const uint8_t *>(&data),
    reinterpret_cast<const uint8_t *>(&data) + sizeof(SendPacket), packet.begin());
  return packet;
}

/****************************************
 * @funcName send
 * @brief    发送函数
 * @para     无
 * @return   无
 * @date     2022.4.2
 * @author   cjh
 *****************************************/
void UsbSerial::send(SendPacket & packet)
{
  Append_CRC16_Check_Sum(reinterpret_cast<uint8_t *>(&packet), sizeof(packet));
  std::vector<uint8_t> data;
  data = toVector(packet);
  unsigned char Tdata[send_num];  //transfrom data
  std::copy(data.begin(), data.end(), Tdata);
  write(fd, Tdata, send_num);
  //   for (int i = 0; i < send_num; i++) {
  //     printf("%x  ", Tdata[i]);
  //   }
  //   std::cout << std::endl;
}

void UsbSerial::closePort() { close(fd); }

/****************************************
 * @funcName 强制拉回
 * @brief    拉回处理函数
 * @para     无
 * @return   无
 * @date     2023.x.x
 * @author   zyk
 *****************************************/
ReceivePacket UsbSerial::SerialRecData()
{
  int bytes_read = 0;
  size_t bytes;
  char * name = ttyname(fd);
  if ((name = nullptr)) {
    printf("tty is null\n");
  }
  if (name != nullptr) {
    printf("device:%s\n", name);
  }
  ioctl(fd, FIONREAD, &bytes);
  unsigned char rdata[receive_num];
  bytes = read(fd, rdata, receive_num);
  ReceivePacket packet;
  // ——————————————————————————————————————————————————————————————————
  if (rdata[0] != 0x5A) {
    packet.header = 123;
    return packet;
  }
  //   for (size_t i = 0; i < 28; i++)
  // {
  //   std::cout<<(int)rdata[i];
  // }
  // std::cout<<std::endl;
  //   for (uint i = 0; i < many; i++) {
  //     uint j = i + 1;
  //     if (j == many) {
  //       j = j - many;
  //     }
  //     if (rdata[i] == 0xA5 && rdata[j] == 9) {
  //       uint l = i;
  //       for (uint k = 0; k < many; k++) {
  //         if (l == many) {
  //           l = l - many;
  //         }
  //         data[k] = rdata[l];
  //         l++;
  //       }
  //     }
  //   }
  // ——————————————————————————————————————————————————————————————————
  std::vector<uint8_t> data;
  for (size_t i = 0; i < sizeof(rdata) / sizeof(rdata[0]); i++) {
    data.emplace_back(rdata[i]);
  }
  packet = fromVector(data);
  return packet;
  // for (int i = 0; i < receive_num; i++) {
  //   std::cout << data[i];
  // }
  // std::cout << std::endl;
}
