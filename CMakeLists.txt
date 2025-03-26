cmake_minimum_required(VERSION 3.10)
project(DisplayImage)


# 查找 OpenCV 库
find_package(OpenCV REQUIRED)
# find_package(Eigen3 REQUIRED)
# include_directories(${EIGEN3_INCLUDE_DIR})


# Add MindVision SDK
set(MV_SDK_PATH "/home/newmaker11/gby总/linuxSDK_V2.1.0.49(250108)")
include_directories(${MV_SDK_PATH}/include)
link_directories(${MV_SDK_PATH}/lib)


# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 添加可执行文件 DisplayImage
add_executable(DisplayImage
    predict.cpp
    predict_utils.cpp
    serial.cpp
    mvvideocapture.cpp
    CRC.cpp
)


# target_link_libraries(DisplayImage ${OpenCV_LIBS} pthread)
# target_link_libraries(${PROJECT_NAME} "/usr/lib/x86_64-linux-gnu/libpthread.so.0" "/lib/x64/libMVSDK.so")


# 指定库文件路径
link_directories( /home/newmaker11/gby总/linuxSDK_V2.1.0.49(250108)/lib/x64)


# 添加编译器警告选项
add_compile_options(-Wall -Wextra -Wpedantic -fPIC)


# 链接 Camera SDK 库，使用完整库文件名
target_link_libraries(DisplayImage 
    libMVSDK.so
    ${OpenCV_LIBS}
    pthread
)


target_link_libraries(DisplayImage opencv_ml)

# target_link_libraries(DisplayImage 
#     predict.so
#     predict_utils.so
#     serial.so
#     mvvideocapture.so
#     CRC.so
# )


# 包含头文件路径
target_include_directories(DisplayImage PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/predict_C++
    /home/newmaker11/gby总/linuxSDK_V2.1.0.49(250108)/include
)


