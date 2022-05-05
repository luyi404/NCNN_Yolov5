# NCNN yolov5

模型转换工具： https://convertmodel.com/#outputFormat=ncnn

# 编译过程
## Windows下
**首先把CMAKE里面的库的地址换成本机的，然后修改NCNN_Windows.cpp的main函数里面的各种路径**

然后
```bash
mkdir build
cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release
```

##  Linux下编译

**切换到项目linux分支**

如果NCNN和OPENCV都已经 `make install` 过的话，可以直接

```bash
mkdir build
cd build

# 需要将quexianyolov5文件夹下的模型参数与结构文件(.param和.bin)拷贝到build文件夹中

cmake ..
make
```



### 运行结果

#### With GPU:
![with GPU](https://s2.loli.net/2022/04/14/qnwDbg9EiIS4Tcj.png)
#### With CPU:
![with CPU](https://s2.loli.net/2022/04/14/cBMvQRzUn3PSjdC.png)





## 控制是否使用gpu

在文件`ncnn_yolov5.cpp`中的`detect_yolov5`函数中
```
yolov5.opt.use_vulkan_compute = true; //这一行控制是否使用gpu
```
## 量化
在校验数据集目录（images目录）的前一个目录下，先获取图片的地址，然后使用ncnn2table.ext
```bash
find images/ -type f > imagelist.txt
./ncnn2table.exe quexianyolov5.param quexianyolov5.bin imagelist.txt quexianyolov5.table mean=[104,117,123] norm=[0.017,0.017,0.017] shape=[320,320,3] pixel=BGR thread=8 method=kl
./ncnn2int8.exe quexianyolov5.param quexianyolov5.bin quexianyolov5-int8.param quexianyolov5-int8.bin quexianyolov5.table
```

然后就生成了`quexianyolov5-int8.bin`和`quexianyolov5-int8.param`文件。
### 是否使用量化
1. 在加载的模型文件用量化后的模型文件
2. `ncnn_yolov5.hpp`文件顶上的 `#define USE_INT8` 控制是否使用INT8模型

### 量化后的运行结果
#### With GPU
![](https://s2.loli.net/2022/04/14/oJ6LZHr8QKlS1PI.png)
#### With CPU
![](https://s2.loli.net/2022/04/14/c3OaYlMVF6Pwnr9.png)


# 编译错误

## 检测到"_ITERATOR_DEBUG_LEVEL"的不匹配项的解决方案
如果是用VS来编译运行，需要使用X64 Release，不能用DEBUG，因为我NCNN是编译的Release版本。如果想要用Debug的话：
### 第一种：把当前跑ncnn的工程里，Debug改成RelWithDebInfo
![](https://s2.loli.net/2022/04/08/qCmGkg3yQUpjVMz.png)

然后就可以设断点、调试运行了

### 第二种方式
自行编译，并且编译时指定 -DCMAKE_BUILD_TYPE=Debug。

而如果编译ncnn时编译了tools，那么需要protobuf，就需要protobuf也提供了debug版本的库。

而如果你的工程中用到了OpenCV，OpenCV也需要Debug版本的。
详情见 https://github.com/Tencent/ncnn/issues/2500
