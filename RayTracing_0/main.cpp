#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "header/rtweekend.h"
#include "header/Sphere.h"
#include "header/HitableList.h"

using namespace cv;

Vec3 Color(const Ray& r, const Hitable& world)
{
	HitRecord Rec;

	if (world.Hit(r, 0.0, infinity, Rec))
	{
		//返回法线贴图
		return 0.5 * Vec3(Rec.Normal.X() + 1, Rec.Normal.Y() + 1, Rec.Normal.Z() + 1);//映射到[0,1]
	}

	//返回背景色
	Vec3 UnitDirection = UnitVector(r.Direction());		 //获取单位方向向量
	double T = 0.5 * (UnitDirection.Y() + 1.0);			 //插值量，范围[0,1]
	return (1.0 - T) * Vec3(1.0, 1.0, 1.0) + T * Vec3(0.5, 0.7, 1.0); //返回一个颜色
}

int main()
{
	int nx = 800;//图片宽度（单位为像素）
	int ny = 400;//图片高度（单位为像素）
	int ns = 100;//扫描次数
	//文件头写入
	std::cout << "P3" << std::endl << nx << " " << ny << std::endl << "255" << std::endl;

	Vec3 lowerleftCorner(-2.0, -1.0, -1.0);
	Vec3 Horiaontal(4.0, 0.0, 0.0);
	Vec3 Vertical(0.0, 2.0, 0.0);
	Vec3 Origin(0.0, 0.0, 0.0);

	//预览窗口相关
	int WindowWidth;//窗口宽度
	int WindowHeight;//窗口高度
	if (nx > ny)  //如果渲染的图片为横版，计算窗口宽度与高度
	{
		WindowHeight = 720;
		WindowWidth = (int)((double)WindowHeight * (double)nx / (double)ny);
	}
	else //如果渲染的图片为竖版，计算窗口宽度与高度
	{
		WindowWidth = 1200;
		WindowHeight = (int)((double)WindowWidth * (double)ny / (double)nx);

	}

	Mat RenderingImage(ny, nx, CV_8UC3, Scalar(50, 50, 50)); //创建一张图片
	namedWindow("图像预览（渲染中）", WINDOW_NORMAL);//设置标题
	moveWindow("图像预览（渲染中）", (int)((1920.0 - WindowWidth) / 2), (int)((1080.0 - WindowHeight) / 2) - 50);//设置窗口位置
	resizeWindow("图像预览（渲染中）", WindowWidth, WindowHeight);//设置窗口大小

	HitableList world;
	world.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(Vec3(0, -100.5, -1), 100));

	for (int j = ny - 1; j >= 0; j--)//行信息
	{
		for (int i = 0; i < nx; i++)//列信息
		{
			double U = double (i / double(nx));
			double V = double (j / double(ny));
			Ray r(Origin, lowerleftCorner + U * Horiaontal + V * Vertical);
			Vec3 Col = Color(r, world);
			

			//下面三个将射线检测到的颜色拆分为红、绿、蓝三个通道
			int ir = int(255.99 * Col.R());
			int ig = int(255.99 * Col.G());
			int ib = int(255.99 * Col.B());

			//将当前像素的三个通道值写入文件
			std::cout << ir << " " << ig << " " << ib << std::endl;
			//实时显示渲染帧
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[0] = ib;
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[1] = ig;
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[2] = ir;
			//Sleep(0);

		}
		//每行计算完以后刷新预览窗口
		if (!(j % (ny / 100))) //每渲染ny/100行后更新预览窗口图片
		{
			imshow("图像预览（渲染中）", RenderingImage);
			waitKey(1);//等待1毫秒事件让窗口刷新完毕
		}

	}
	imshow("图像预览（渲染中）", RenderingImage);
	waitKey(5000); //等待3000毫秒
	destroyAllWindows();//关闭窗口
	return 0;
}
