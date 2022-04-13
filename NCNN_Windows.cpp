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
    const char* imagepath = "D:\\ncnn\\NCNN_Windows\\NCNN_Windows\\quexianyolov5\\20190810_8256.jpg";
	
    
    cv::Mat m = cv::imread(imagepath, 1);
    
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    std::vector<yolov5::Object> objects;

    const char* parapath = "D:\\ncnn\\NCNN_Windows\\NCNN_Windows\\quexianyolov5\\quexianyolov5.param";
    const char* binpath = "D:\\ncnn\\NCNN_Windows\\NCNN_Windows\\quexianyolov5\\quexianyolov5.bin";
	
    yolov5::detect_yolov5(parapath, binpath, m, objects);

    yolov5::draw_objects(m, objects); 

    system("pause");

    return 0;
}