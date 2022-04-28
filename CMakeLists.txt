cmake_minimum_required (VERSION 3.8)

project ("NCNN_Windows")
set(ncnn_DIR "/home/yaoyuh/ncnn/build/install/lib/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
find_package(OpenCV REQUIRED)
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

include_directories(${OpenCV_INCLUDE_DIRS}) 
message(STATUS "Opnecv ;ibrary status: ")
message(STATUS "> version: ${OpenCV_VERSION} ")
message(STATUS "libraries: ${OpenCV_LIBS} ")
message(STATUS "> include: ${OpenCV_INCLUDE_DIRS}  ")

link_libraries(
	${OpenCV_LIBS}
)

add_executable (NCNN_Windows "NCNN_Windows.cpp" "NCNN_Windows.h"  "ncnn_yolov5.hpp")


target_include_directories(NCNN_Windows PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(NCNN_Windows ncnn ${OpenCV_LIBS})
