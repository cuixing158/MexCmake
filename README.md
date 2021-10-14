# 基于CMake交叉编译C/C++得到适用与Matlab的Mex加速库文件
提供基于CMake的方式编译C/C++代码，代替mex函数打包，配置简单，通用，可修改，不依赖于特定的编译器，平台，工具IDE等，本示例展示了使用一个`expore.c`文件如何通过交叉编译为对应平台的mex文件（即不同平台对应的库文件）。

## 编译后缀文件
- windows: *.mexw64
- Unix: *.mexa64
- Mac: *.mexmaci64

## 如何"食"用
设置好电脑上matlab安装路径位置即可，在`CMakeLists.txt`中指定`MATLAB_INSTALL_ROOT`安装路径，如windows上为：
`set(MATLAB_INSTALL_ROOT "C:/Program Files/Polyspace/R2021a")`,
然后
```cmake
mkdir build
cd build
cmake -G"MinGw Makefiles" ..
make 
make install
```
即可得到对应平台的mex文件
