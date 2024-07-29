#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "header/rtweekend.h"
#include "header/Sphere.h"
#include "header/HitableList.h"
#include "header/Camera.h"
#include "header/Material.h"

using namespace cv;

static Vec3 Color(const Ray& r, const Hitable& world, int depth)
{
	HitRecord Rec;

	//递归多次直接返回黑色
	if (depth <= 0)
		return Vec3(0, 0, 0);

	//忽略掉0附近的一部分范围, 防止物体发出的光线再次与自己相交
	if (world.Hit(r, 0.001, infinity, Rec))
	{
		Ray scattered;	//散射
		Vec3 attenuation;	//衰减

		if (Rec.mat_ptr->Scatter(r, Rec, attenuation, scattered))
		{
			//以入射点为起点，方向为与相切球相交任一点 发出射线
			//若与物体相撞，则继续反射
			return attenuation * Color(scattered, world, depth - 1);
		}
		return Vec3(0, 0, 0);	//没有反射，即全部吸收
	}

	//返回背景色
	Vec3 UnitDirection = UnitVector(r.Direction());		 //获取单位方向向量
	double T = 0.5 * (UnitDirection.Y() + 1.0);			 //插值量，范围[0,1]
	return (1.0 - T) * Vec3(1.0, 1.0, 1.0) + T * Vec3(0.5, 0.7, 1.0); //返回一个颜色
}

static HitableList random_scene()
{
	HitableList world;

	world.add(make_shared<Sphere>(Vec3(0, -1000, 0), 1000, make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5))));

	int i = 1;
	for (int a = -11; a < 11; a++) 
	{
		for (int b = -11; b < 11; b++) 
		{
			auto choose_mat = random_double(0, 1);
			Vec3 center(a + 0.9 * random_double(0,1), 0.2, b + 0.9 * random_double(0,1));
			if ((center - Vec3(4, 0.2, 0)).Vec3Length() > 0.9) 
			{
				if (choose_mat < 0.8) 
				{
					// diffuse
					auto albedo = Vec3(random_double(0, 1), random_double(0, 1), random_double(0, 1)) * Vec3(random_double(0, 1), random_double(0, 1), random_double(0, 1));
					world.add(make_shared<Sphere>(center, 0.2, make_shared<Lambertian>(albedo)));
				}
				else if (choose_mat < 0.95) 
				{
					// metal
					auto albedo = Vec3(random_double(0.5, 1), random_double(0.5, 1), random_double(0.5, 1));
					auto fuzz = random_double(0, .5);
					world.add(make_shared<Sphere>(center, 0.2, make_shared<Metal>(albedo, fuzz)));
				}
				else 
				{
					// glass
					world.add(make_shared<Sphere>(center, 0.2, make_shared<Dielectric>(1.5)));
				}
			}
		}
	}

	world.add(make_shared<Sphere>(Vec3(0, 1, 0), 1.0, make_shared<Dielectric>(1.5)));

	world.add(make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1))));

	world.add(make_shared<Sphere>(Vec3(4, 1, 0), 1.0, make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0)));

	return world;
}

int main()
{
	const int nx = 800;//图片宽度（单位为像素）
	const int ny = 400;//图片高度（单位为像素）
	const int ns = 100;//扫描次数
	const int max_depth = 50;
	const double aspect_ratio = double(nx) / ny;

	//文件头写入
	std::cout << "P3" << std::endl << nx << " " << ny << std::endl << "255" << std::endl;

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

	Vec3 lookfrom(13, 2, 3);
	Vec3 lookat(0, 0, 0);
	Vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	//物体初始化
	HitableList world = random_scene();
	//world.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5, make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5))));
	//world.add(make_shared<Sphere>(Vec3(0, -100.5, -1), 100, make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0))));
	//world.add(make_shared<Sphere>(Vec3(1, 0, -1), 0.5, make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.0)));
	////world.add(make_shared<Sphere>(Vec3(-1, 0, -1), 0.5, make_shared<Metal>(Vec3(0.8, 0.8, 0.8), 1.0)));
	//world.add(make_shared<Sphere>(Vec3(-1, 0, -1), 0.5, make_shared<Dielectric>(1.5)));
	//world.add(make_shared<Sphere>(Vec3(-1, 0, -1), -0.45, make_shared<Dielectric>(1.5)));

	for (int j = ny - 1; j >= 0; j--)//行信息
	{
		for (int i = 0; i < nx; i++)//列信息
		{
			//初始化颜色
			Vec3 Col(0, 0, 0);

			for (int s = 0; s < ns; s++)
			{
				double U = double((i + random_double(0, 1)) / double(nx));
				double V = double((j + random_double(0, 1)) / double(ny));
				//得到原点向量A和方向向量B
				Ray r = camera.GetRay(U, V);
				//检测射线是否经过物体，返回颜色
				//累加，取均值
				Col += Color(r, world, max_depth);
			}
			//取均值
			Col /= double(ns);
			//伽马校正
			Col = Vec3(sqrt(Col[0]), sqrt(Col[1]), sqrt(Col[2]));

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
