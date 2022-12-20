# Dustball's Interpreter

西电函数绘图语言解释器

## 构建方法

### linux平台

在linux上要有gcc,g++,makefile,cmake等工具,没有直接apt install

创建build路径

```
mkdir build && cd build
```

以下所有工作均在build目录中完成

外部构建

```sh
cmake ..
```

编译链接

```
make
```

解释

```
./DustComp ../test.txt
```

python运行

```
python ./Drawer.py
```

这一步在wsl终端上不行,需要图形界面支持,可以用kex登录wsl桌面或者直接本机上使用python绘图

### windows平台

```
mkdir build && cd build
cmake ..
```

此后在build目录下生成sln解决方案文件,使用visual studio打开之即可


