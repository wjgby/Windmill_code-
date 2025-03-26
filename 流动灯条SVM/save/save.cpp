#include "save.h"
#include <thread>
using namespace cv;
using namespace std;


void save::son()
{
    double start_t; start_t = getTickCount();


    while (1) {
        if(c<100)
        {
            waitKey(80);
            cout << c << endl;
            key = 0;
        }
        else
        {
            waitKey(100000);
            key = 0;
        }
        c++;
    }
}

void timerCallback(int &key)
{
    // 定时触发的事件
    std::cout << "定时事件触发了！" << "key= " << key << std::endl;
    if(key==1){exit(0);}
}
void save::father()
{
//    waitKey(3000);
    // 设置定时器的时间间隔（以毫秒为单位）
    int interval = 5000; // 1秒
    // 循环触发定时事件
    while (true)
    {
        // 调用定时回调函数
        timerCallback(key);
        key=1;
        // 等待一段时间后再次触发定时事件
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));

    }

}


