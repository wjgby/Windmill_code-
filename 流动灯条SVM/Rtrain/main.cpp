// #include<opencv2/opencv.hpp>
// #include<iostream>

// using namespace std;
// using namespace cv;


// int main()
// {
//     VideoCapture cap("/home/newmaker11/下载/Video_20250214201105409.mp4");//导入视频
//     Mat result;
//     Mat R;
//     int min=200;
//     namedWindow("Control", WINDOW_AUTOSIZE);
//     createTrackbar("min", "Control", &min, 255);
// //------------------------------------------------------------------------------
//     char ad[128] = { 0 };
//     int i =0;
// //-------------------------------------------------------------------------------
//     Mat image;


//     while (1) {
//         double start_time; start_time = getCPUTickCount();
//     //    image = camera.GetFrame();
//         cap >> image;
//         if (image.empty()) {
//             std::cerr << "Error: Image is empty!" << std::endl;
//             return -1;
//         }
//         cvtColor(image,result,COLOR_BGR2GRAY);//以下为作二值化处理
//         threshold(~result,result,min,255,THRESH_BINARY_INV /*| THRESH_OTSU*/);

//         RotatedRect light_ellipse;
//         vector<vector<Point>> lightContours;
//         Mat contourImg;
//         result.copyTo(contourImg);
//         findContours(contourImg,lightContours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);//找到轮廓,耗时

//         for (uint i = 0;i < lightContours.size();i++)
//         {
//             if(lightContours[i].size()<6)continue;

//             //椭
//             light_ellipse = fitEllipse(lightContours[i]);
//             if(light_ellipse.size.area() <300)continue;
//             cout << i << "   " << light_ellipse.size.area() << endl;
// //            ellipse(image,light_ellipse,Scalar(50,250,50),2,8);

//         }
//         Point2f Vertices[4];
//         Vertices[0] = Point2f(light_ellipse.center.x-1.5*light_ellipse.size.width, light_ellipse.center.y-1.5*light_ellipse.size.height);
//         Vertices[1] = Point2f(light_ellipse.center.x+1.5*light_ellipse.size.width, light_ellipse.center.y-1.5*light_ellipse.size.height);
//         Vertices[2] = Point2f(light_ellipse.center.x+1.5*light_ellipse.size.width, light_ellipse.center.y+1.5*light_ellipse.size.height);
//         Vertices[3] = Point2f(light_ellipse.center.x-1.5*light_ellipse.size.width, light_ellipse.center.y+1.5*light_ellipse.size.height);
//         Mat rotation;
//         Point2f dst_points[4];
//         dst_points[0] = Point2f(0.0, 0.0);
//         dst_points[1] = Point2f(20.0, 0.0);
//         dst_points[2] = Point2f(0.0, 100.0);
//         dst_points[3] = Point2f(20.0, 100.0);
//         rotation = getPerspectiveTransform(Vertices,dst_points);//透视方式（最好写透视变换矩阵，才不显的low）
//         warpPerspective(result,R,rotation,Size(50,50));


// //--------------------------------训练模型获取样本用-----------------------------------------
// //    resize(image,image,Size(20,20));测试图片

//         sprintf(ad, "/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/L_photos/%d.jpg",i);
//         imwrite(ad,R);
//         i++;
// //--------------------------------------------------------------------------
//             imshow("R",R);
//             imshow("image",image);
//             imshow("result",result);
//             if(waitKey(10)==' ')//设置合适
//                 break;
//         }
//     return 0;
// }


#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    // 打开视频文件
    VideoCapture cap("/home/newmaker11/下载/Video_20250217204616946.mp4");
    if (!cap.isOpened()) {
        cerr << "Error: 无法打开视频文件!" << endl;
        return -1;
    }
    
    // 获取视频的帧率
    double fps = cap.get(CAP_PROP_FPS);
    // 总共需要处理6秒的视频，计算需要处理的帧数
    int total_frames = 6 * fps;
    // 设置每隔多少帧保存一张图片，确保总共保存500张图片
    int sample_interval = max(1, total_frames / 500);

    // 用于处理图像的Mat对象
    Mat result;
    int min = 200;  // 设置二值化的阈值
    namedWindow("Control", WINDOW_AUTOSIZE);
    createTrackbar("min", "Control", nullptr, 255, nullptr);
    setTrackbarPos("min", "Control", min);

    char ad[128] = { 0 };  // 文件名
    int i = 0;  // 保存图像的计数
    int frame_count = 0;  // 当前帧的计数
    Mat image;  // 存储当前帧的图像

    // 定义膨胀的结构元素
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));  // 3x3 矩形结构元素

    // 处理每一帧图像
    while (frame_count < total_frames && i < 500) {
        cap >> image;  // 从视频中获取一帧图像
        if (image.empty()) {
            cerr << "Error: 图像为空!" << endl;
            break;
        }

        // 每隔sample_interval帧处理一次
        if (frame_count % sample_interval == 0) {
            try {
                // 转为灰度图像
                cvtColor(image, result, COLOR_BGR2GRAY);
                // 进行二值化处理，反转颜色，背景为黑色，目标为白色
                threshold(~result, result, min, 255, THRESH_BINARY_INV);

                // 对二值图像进行膨胀操作
                Mat dilated_image;
                dilate(result, dilated_image, kernel, Point(-1, -1), 0);  // 膨胀操作

                // 设置图像保存路径，并保存膨胀后的图像
                sprintf(ad, "/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/Liu/%d.jpg", i);
                imwrite(ad, dilated_image);  // 保存膨胀后的图像
                i++;

                // 显示图像
                imshow("Dilated Image", dilated_image);
                imshow("Original Image", image);
                imshow("Processed Image", result);
            } catch (const cv::Exception& e) {
                cerr << "OpenCV错误: " << e.what() << endl;
            }
        }

        // 更新帧计数
        frame_count++;
        char key = waitKey(1);  // 每帧等待1毫秒
        if (key == ' ' || key == 27) break;  // 按空格或Esc键退出
    }

    // 输出处理结果
    cout << "处理完成，共保存 " << i << " 张膨胀后的图片" << endl;
    return 0;
}


