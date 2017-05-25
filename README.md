# imageTools

图像处理的小工具，平时工作学习会经常对图像做一些处理，经常会写一小段代码，时间久了，积累了好多小的project，不如整理成一个Tool Set。

## 依赖项
现在在Windows上工作时间比较多，所以现在还是基于Windows工具集，以后工作时间充足了增加跨平台构建。

- Visual Studio 2015
- OpenCV 2.4.13
- gflags
- gtest

## RGB图像转灰度图
命令格式如下：
```
imageTools.toGray --srcImg=image_full_name.jpg
```
目前还是存储在当前路径下。