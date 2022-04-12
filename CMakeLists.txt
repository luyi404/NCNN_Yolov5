cmake_minimum_required (VERSION 3.8)

project ("NCNN_Windows")

set(ncnn_DIR "D:\\ncnn\\ncnn\\build\\install\\lib\\cmake\\ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
find_package(ncnn REQUIRED)

include_directories(
        "D:\\ncnn\\opencv2\\opencv\\build\\include",
		"D:\\ncnn\\opencv2\\opencv\\build\\include\\opencv2\\opencv",
		"D:\\ncnn\\opencv2\\opencv\\sources\\3rdparty\\include"
)

link_directories(
		"D:\\ncnn\\opencv2\\opencv\\build\\x64\\vc15\\lib"
)

add_executable (NCNN_Windows "NCNN_Windows.cpp" "NCNN_Windows.h"  "ncnn_yolov5.hpp")


set(OpenCV_INCLUDE_DIRS "D:\\ncnn\\opencv2\\opencv\\build\\include" CACHE PATH "Directory that contains OpenCV headers")
set(OpenCV_LIBS "D:\\ncnn\\opencv2\\opencv\\build\\x64\\vc15\\lib\\opencv_world455.lib" CACHE PATH "Directory that contains OpenCV libraries")

target_include_directories(NCNN_Windows PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(NCNN_Windows ncnn ${OpenCV_LIBS})