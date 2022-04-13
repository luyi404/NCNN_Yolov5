cmake_minimum_required (VERSION 3.8)

project ("NCNN_Windows")

set(ncnn_DIR "D:\\ncnn\\ncnn\\build\\install\\lib\\cmake\\ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
set(opencv_DIR "D:\\ncnn\\opencv2\\opencv")
#[[
	The opencv_DIR's folder architecture like:
opencv
├── build
│   ├── bin
│   ├── etc
│   ├── include
│   ├── java
│   ├── python
│   └── x64
└── sources
    ├── 3rdparty
    ├── apps
    ├── cmake
    ├── data
    ├── doc
    ├── include
    ├── modules
    ├── platforms
    └── samples
]]#

find_package(ncnn REQUIRED)

include_directories(
		"${opencv_DIR}/build/include",
		"${opencv_DIR}/sources/3rdparty/include",
		"${opencv_DIR}/build/include/opencv2/opencv"
)

link_directories(
		"${opencv_DIR}/build/x64/vc15/lib"
)


set(OpenCV_INCLUDE_DIRS "${opencv_DIR}/build/include" CACHE PATH "Directory that contains OpenCV headers")
set(OpenCV_LIBS "${opencv_DIR}/build/x64/vc15/lib/opencv_world455.lib" CACHE PATH "Directory that contains OpenCV libraries")


link_libraries(
	${OpenCV_LIBS}
)

add_executable (NCNN_Windows "NCNN_Windows.cpp" "NCNN_Windows.h"  "ncnn_yolov5.hpp")


target_include_directories(NCNN_Windows PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(NCNN_Windows ncnn ${OpenCV_LIBS})