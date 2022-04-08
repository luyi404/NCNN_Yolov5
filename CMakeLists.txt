cmake_minimum_required (VERSION 3.8)

project ("NCNN_Windows")
set( CMAKE_CXX_FLAGS "-std=c++11" )

include_directories(
        "D:\\ncnn\\opencv2\\opencv\\build\\include",
		"D:\\ncnn\\opencv2\\opencv\\build\\include\\opencv2\\opencv",
		"D:\\ncnn\\ncnn\\build\\install\\include\\ncnn",
		"D:\\ncnn\\ncnn\\build\\install\\include",
		"D:\\ncnn\\opencv2\\opencv\\sources\\3rdparty\\include"
)

link_directories(
		"D:\\ncnn\\ncnn\\build\\install\\lib",
		"D:\\ncnn\\opencv2\\opencv\\build\\x64\\vc15\\lib"
)

add_executable (NCNN_Windows "NCNN_Windows.cpp" "NCNN_Windows.h")

target_link_libraries(NCNN_Windows "opencv_world455.lib" "ncnn.lib")