# NCNN_Windows

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