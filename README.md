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