// NCNN_Windows.cpp: 定义应用程序的入口点。
//

#include "NCNN_Windows.h"
#include "ncnn_yolov5.hpp"

using namespace std;

int main(int argc, char** argv)
{
	/*
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
        return -1;
    }
    */
    //const char* imagepath = argv[1];
    const char* imagepath = "20200108_503.jpg";
	
    
    cv::Mat m = cv::imread(imagepath, 1);
    
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    std::vector<yolov5::Object> objects;

    const char* parapath = "quexianyolov5-int8.param";
    const char* binpath = "quexianyolov5-int8.bin";

    //const char* parapath = "quexianyolov5.param";
    //const char* binpath = "quexianyolov5.bin";
	
    yolov5::detect_yolov5(parapath, binpath, m, objects);

    yolov5::draw_objects(m, objects); 

    system("pause");

    return 0;
}