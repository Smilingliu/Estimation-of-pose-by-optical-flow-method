# Estimation-of-pose-by-optical-flow-method
Estimation of pose by optical flow method

设目标上的一个角点在世界坐标系下的坐标为（X,Y,Z），绕世界坐标系的角速度为( \Omega_{1} , \Omega_{2} , \Omega_{3} )，沿世界坐标系的平移速度为( T_{X} , T_{Y} , T_{Z} )，由欧拉方程得，在世界坐标系下该点的速度为：
![picture](https://github.com/Smilingliu/Estimation-of-pose-by-optical-flow-method/blob/master/1.jpg)

对相机的观测方程求导，并令
![picture](https://github.com/Smilingliu/Estimation-of-pose-by-optical-flow-method/blob/master/2.jpg)

将第一个式子 \dot{X} , \dot{Y} , \dot{Z} 的表达式带入观测方程求导后的式子，整理可得：
![picture](https://github.com/Smilingliu/Estimation-of-pose-by-optical-flow-method/blob/master/3.jpg)

这里的 \dot{u} , \dot{v} 即为相邻两帧追踪的角点的光流，x、y、Z都可求，所以至少需3个点就可以把( \Omega_{1} , \Omega_{2} , \Omega_{3} )与( T_{X} , T_{Y} , T_{Z} )求出来。整理成如下形式：
![picture](https://github.com/Smilingliu/Estimation-of-pose-by-optical-flow-method/blob/master/4.jpg)
将上式记为AX=B。由上式可知，一个点只能提供两个约束，所以至少需要3个点才能求得矩阵X。当追踪的角点数为3时，A构成一个满秩的矩阵，上式变为下式
![picture](https://github.com/Smilingliu/Estimation-of-pose-by-optical-flow-method/blob/master/5.jpg)
