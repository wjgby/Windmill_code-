#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;
using namespace ml;

int main() {
    // 训练数据准备
    vector<Mat> images;  // 存储训练图像
    vector<int> labels;  // 存储对应的标签（类别）

    // 假设你有两类图像，分别为类别 0 和 1
    // 这里的代码只是示例，实际上，你需要将图像读取到 images 向量中，并将它们的标签放入 labels 向量中
    for (int i = 0; i < 100; i++) {
        string filename = "/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/L_photos" + to_string(i) + ".jpg";  // 图片路径
        Mat image = imread(filename, IMREAD_GRAYSCALE);  // 读取灰度图像
        resize(image, image, Size(50, 50));  // 调整为 50x50 大小
        images.push_back(image);
        labels.push_back(i % 2);  // 假设标签是 0 或 1，按序列填充
    }

    // 将图像数据转换为训练所需的矩阵
    Mat trainData;
    for (size_t i = 0; i < images.size(); i++) {
        Mat flattened = images[i].reshape(1, 1);  // 将图像展平成一维向量
        trainData.push_back(flattened);  // 将数据存储到训练数据矩阵中
    }
    trainData.convertTo(trainData, CV_32F);  // 转换为浮动类型（SVM 模型通常需要浮动数据类型）

    // 创建 SVM 模型
    Ptr<SVM> svm = SVM::create();

    // 设置 SVM 参数
    svm->setKernel(SVM::LINEAR);  // 线性核函数
    svm->setType(SVM::C_SVC);     // 支持向量分类
    svm->setC(1);  // 正则化参数
    svm->setGamma(1);  // 核函数的gamma值
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));  // 最大迭代次数和精度

    // 训练 SVM 模型
    svm->train(trainData, ROW_SAMPLE, labels);
    svm->save("/home/newmaker11/gby总/能量机关分文件(made by GG_Bond)/predict_C++/L_svm(2025_2_23).xml");  // 将训练好的模型保存为 XML 文件

    cout << "SVM 模型训练并保存成功！" << endl;
    return 0;
}
