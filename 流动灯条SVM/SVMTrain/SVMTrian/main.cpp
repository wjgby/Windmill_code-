///////////////////////////////////////////////////////////////分割图像////////////////////////////////////////////////////////////////////
//#include <opencv2/opencv.hpp>
//#include <iostream>

//using namespace std;
//using namespace cv;

//int main()
//{
//    char ad[128] = { 0 };
//    int  filename = 0, filenum = 0;
//    Mat img = imread("/home/forest/图片/digits.png");
//    Mat gray;
//    cvtColor(img, gray, COLOR_BGR2GRAY);
//    int b = 20;
//    int m = gray.rows / b;   //原图为1000*2000
//    int n = gray.cols / b;   //裁剪为5000个20*20的小图块
//    for (int i = 0; i < m; i++)
//    {
//        int offsetRow = i*b;  //行上的偏移量
//        if (i % 5 == 0 && i != 0)
//        {
//            filename++;
//            filenum = 0;
//        }
//        for (int j = 0; j < n; j++)
//        {
//            int offsetCol = j*b; //列上的偏移量
//            sprintf(ad, "/home/forest/图片/num_picture %d /num %d .jpg",filename,filenum++);
//            //截取20*20的小块
//            Mat tmp;
//            gray(Range(offsetRow, offsetRow + b), Range(offsetCol, offsetCol + b)).copyTo(tmp);
//            imwrite(ad, tmp);
//        }
//    }
//    return 0;
//}
/////////////////////////////////////////////////////////////训练模型/////////////////////////////////////////////////////////////////
/*
#include <stdio.h>
#include <time.h>
#include <opencv2/opencv.hpp>
//#include <io.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/ml/ml.hpp>
#include <cstring>
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::ml;

void getFiles(string path, vector<string>& files);

//void get_L(Mat& trainingImages, vector<int>& trainingLabels);
//void get_L_null(Mat& trainingImages, vector<int>& trainingLabels);

void get_R(Mat& trainingImages, vector<int>& trainingLabels);
void get_hero(Mat& trainingImages, vector<int>& trainingLabels);
void get_home(Mat& trainingImages, vector<int>& trainingLabels);
void get_qianshao(Mat& trainingImages, vector<int>& trainingLabels);
void get_sentry(Mat& trainingImages, vector<int>& trainingLabels);
void get_5(Mat& trainingImages, vector<int>& trainingLabels);
void get_4(Mat& trainingImages, vector<int>& trainingLabels);
void get_3(Mat& trainingImages, vector<int>& trainingLabels);
void get_2(Mat& trainingImages, vector<int>& trainingLabels);
void get_null(Mat& trainingImages, vector<int>& trainingLabels);

int main()
{
    //获取训练数据
    Mat classes;
    Mat trainingData;
    Mat trainingImages;
    vector<int> trainingLabels;
//________________________________________________________
//    get_L(trainingImages,trainingLabels);
//    get_L_null(trainingImages,trainingLabels);
//________________________________________________________
    get_R(trainingImages,trainingLabels);
//    get_hero(trainingImages,trainingLabels);
//    get_home(trainingImages,trainingLabels);
//    get_qianshao(trainingImages,trainingLabels);
//    get_sentry(trainingImages,trainingLabels);
//    get_5(trainingImages, trainingLabels);
//    get_4(trainingImages, trainingLabels);
//    get_3(trainingImages, trainingLabels);
//    get_2(trainingImages, trainingLabels);
    get_null(trainingImages, trainingLabels);
    Mat(trainingImages).copyTo(trainingData);
    trainingData.convertTo(trainingData, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);
    //配置SVM训练器参数
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setDegree(0);
    svm->setGamma(1);
    svm->setCoef0(0);
    svm->setC(1);
    svm->setNu(0);
    svm->setP(0);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.01));
    //训练
    svm->train(trainingData, ROW_SAMPLE, classes );
    //保存模型
    svm->save("/home/forest/最新XML/R_svm.xml");

    cout << endl;
    cout << "训练好了！！！！！" << endl;
    cout << "存放的目录是/home/forest/最新XML/R_svm.xml" << endl;

    getchar();
    return 0;
}
void get_R(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/newR";
    cout << "获取/home/forest/图片/Data/train/newR" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(50,50));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(22);
    }
}


void get_hero(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/hero";
    cout << "获取/home/forest/图片/Data/train/hero" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(20,20));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(1);
    }
}
void get_home(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/home";
    cout << "获取/home/forest/图片/Data/train/home" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(50,50));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(8);
    }
}
void get_qianshao(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/qianshao";
    cout << "获取/home/forest/图片/Data/train/qianshao" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(50,50));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(7);
    }
}
void get_sentry(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/sentry";
    cout << "获取/home/forest/图片/Data/train/sentry" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(20,20));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(6);
    }
}
void get_5(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/5";
    cout << "/home/forest/图片/Data/train/5" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(20,20));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(5);
    }
}
void get_4(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/4";
    cout << "获取/home/forest/图片/Data/train/4" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        resize(SrcImage,SrcImage,Size(20,20));
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(4);
    }
}
void get_3(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/3";
    cout << "获取/home/forest/图片/Data/train/3" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(20,20));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(3);
    }
}
void get_2(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/2";
    cout << "获取/home/forest/图片/Data/train/2" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        resize(SrcImage,SrcImage,Size(20,20));
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(2);
    }
}
void get_null(Mat& trainingImages, vector<int>& trainingLabels)
{
    string filePath = "/home/forest/图片/Data/train/null";
    cout << "获取/home/forest/图片/Data/train/null" << endl;
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout << " files.size: " << number << endl;
    for (int i = 0; i < number; i++)
    {
        Mat  SrcImage = imread(files[i].c_str());
        cvtColor(SrcImage,SrcImage,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(SrcImage,SrcImage,20,255,THRESH_BINARY_INV);
        resize(SrcImage,SrcImage,Size(50,50));
        SrcImage = SrcImage.reshape(1, 1);
        trainingImages.push_back(SrcImage);
        trainingLabels.push_back(0);
    }
}

*/

//////////////////////////////////////////////////////////////测试//////////////////////////////////////////////////////////////////////
///*
#include <stdio.h>
#include <time.h>
#include <opencv2/opencv.hpp>
//#include <opencv/cv.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
//#include <io.h>//这玩意儿没起作用阿。。
using namespace std;
using namespace cv;
using namespace ml;

void getFiles(string path, vector<string>& files);
int main()
{
    int result0 = 0;
    int result1 = 0;
    int result3 = 0;
    int result4 = 0;
    int result5 = 0;
    int result6 = 0;
    int result22 = 0;
    string filePath = "/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/R_photos";
    vector<string> files;
    glob(filePath, files,false);
    int number = files.size();
    cout <<"共有测试图片"<< number<<"张" << endl;
    Ptr<ml::SVM>svm = ml::SVM::load("/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/R_svm.xml");

    for (int i = 0; i < number; i++)
    {

        Mat inMat = imread(files[i].c_str());
        resize(inMat,inMat,Size(50,50));
        cvtColor(inMat,inMat,COLOR_BGR2GRAY);//以下为作二值化处理
        threshold(inMat,inMat,20,255,THRESH_BINARY_INV);
        Mat p = inMat.reshape(1, 1);
        p.convertTo(p, CV_32FC1);
        int response = (int)svm->predict(p);
//        cout << "识别的数字为：" << response << endl;
        if (response == 0)
        {
            result0++;
        }
//        else if (response == 3)
//        {
//            result3++;
//        }
//        else if (response == 4)
//        {
//            result4++;
//        }
//        else if (response == 3)
//        {
//            result3++;
//        }
//        else if (response == 1)
//        {
//            result1++;
//        }
//        else if (response == 6)
//        {
//            result6++;
//        }
        else
        {
            result22++;
        }

    }
//    cout << "识别的图案5的个数为：" << result5 << endl;
//    cout << "识别的图案4的个数为：" << result4 << endl;
//    cout << "识别的图案3的个数为：" << result3 << endl;
//    cout << "识别的图案1的个数为：" << result1 << endl;
//    cout << "识别的图案0的个数为：" << result0 << endl;
//    cout << "识别的图案6的个数为：" << result6 << endl;
    cout << "识别的图案0的个数为" << result0 << endl;
    cout << "识别的图案R的个数为: " << result22 << endl;
    getchar();
    return  0;
}

//*/


















