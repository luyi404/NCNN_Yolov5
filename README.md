# NCNN_Windows

ģ��ת�����ߣ� https://convertmodel.com/#outputFormat=ncnn

# �������
## Windows��
**���Ȱ�CMAKE����Ŀ�ĵ�ַ���ɱ����ģ�Ȼ���޸�NCNN_Windows.cpp��main��������ĸ���·��**

Ȼ��
```bash
mkdir build
cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release
```
## �����Ƿ�ʹ��gpu
���ļ�`ncnn_yolov5.cpp`�е�`detect_yolov5`������
```
yolov5.opt.use_vulkan_compute = true; //��һ�п����Ƿ�ʹ��gpu
```
## ����
��У�����ݼ�Ŀ¼��imagesĿ¼����ǰһ��Ŀ¼�£��Ȼ�ȡͼƬ�ĵ�ַ��Ȼ��ʹ��ncnn2table.ext
```bash
find images/ -type f > imagelist.txt
./ncnn2table.exe quexianyolov5.param quexianyolov5.bin imagelist.txt quexianyolov5.table mean=[104,117,123] norm=[0.017,0.017,0.017] shape=[320,320,3] pixel=BGR thread=8 method=kl
./ncnn2int8.exe quexianyolov5.param quexianyolov5.bin quexianyolov5-int8.param quexianyolov5-int8.bin quexianyolov5.table
```

Ȼ���������`quexianyolov5-int8.bin`��`quexianyolov5-int8.param`�ļ���
### �Ƿ�ʹ������
1. �ڼ��ص�ģ���ļ����������ģ���ļ�
2. `ncnn_yolov5.hpp`�ļ����ϵ� `#define USE_INT8` �����Ƿ�ʹ��INT8ģ��

### ncnn2table:
![](https://s2.loli.net/2022/04/14/4v8sTcVMwa2J3It.png)
### ncnn2int8:
![](https://s2.loli.net/2022/04/14/gwdot5FXaiUOKy6.png)


# �������

## ��⵽"_ITERATOR_DEBUG_LEVEL"�Ĳ�ƥ����Ľ������
�������VS���������У���Ҫʹ��X64 Release��������DEBUG����Ϊ��NCNN�Ǳ����Release�汾�������Ҫ��Debug�Ļ���
### ��һ�֣��ѵ�ǰ��ncnn�Ĺ����Debug�ĳ�RelWithDebInfo
![](https://s2.loli.net/2022/04/08/qCmGkg3yQUpjVMz.png)

Ȼ��Ϳ�����ϵ㡢����������

### �ڶ��ַ�ʽ
���б��룬���ұ���ʱָ�� -DCMAKE_BUILD_TYPE=Debug��

���������ncnnʱ������tools����ô��Ҫprotobuf������ҪprotobufҲ�ṩ��debug�汾�Ŀ⡣

�������Ĺ������õ���OpenCV��OpenCVҲ��ҪDebug�汾�ġ�
����� https://github.com/Tencent/ncnn/issues/2500