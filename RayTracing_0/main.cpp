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

	//�ݹ���ֱ�ӷ��غ�ɫ
	if (depth <= 0)
		return Vec3(0, 0, 0);

	//���Ե�0������һ���ַ�Χ, ��ֹ���巢���Ĺ����ٴ����Լ��ཻ
	if (world.Hit(r, 0.001, infinity, Rec))
	{
		Ray scattered;	//ɢ��
		Vec3 attenuation;	//˥��

		if (Rec.mat_ptr->Scatter(r, Rec, attenuation, scattered))
		{
			//�������Ϊ��㣬����Ϊ���������ཻ��һ�� ��������
			//����������ײ�����������
			return attenuation * Color(scattered, world, depth - 1);
		}
		return Vec3(0, 0, 0);	//û�з��䣬��ȫ������
	}

	//���ر���ɫ
	Vec3 UnitDirection = UnitVector(r.Direction());		 //��ȡ��λ��������
	double T = 0.5 * (UnitDirection.Y() + 1.0);			 //��ֵ������Χ[0,1]
	return (1.0 - T) * Vec3(1.0, 1.0, 1.0) + T * Vec3(0.5, 0.7, 1.0); //����һ����ɫ
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
	const int nx = 800;//ͼƬ��ȣ���λΪ���أ�
	const int ny = 400;//ͼƬ�߶ȣ���λΪ���أ�
	const int ns = 100;//ɨ�����
	const int max_depth = 50;
	const double aspect_ratio = double(nx) / ny;

	//�ļ�ͷд��
	std::cout << "P3" << std::endl << nx << " " << ny << std::endl << "255" << std::endl;

	//Ԥ���������
	int WindowWidth;//���ڿ��
	int WindowHeight;//���ڸ߶�
	if (nx > ny)  //�����Ⱦ��ͼƬΪ��棬���㴰�ڿ����߶�
	{
		WindowHeight = 720;
		WindowWidth = (int)((double)WindowHeight * (double)nx / (double)ny);
	}
	else //�����Ⱦ��ͼƬΪ���棬���㴰�ڿ����߶�
	{
		WindowWidth = 1200;
		WindowHeight = (int)((double)WindowWidth * (double)ny / (double)nx);
	}

	Mat RenderingImage(ny, nx, CV_8UC3, Scalar(50, 50, 50)); //����һ��ͼƬ
	namedWindow("ͼ��Ԥ������Ⱦ�У�", WINDOW_NORMAL);//���ñ���
	moveWindow("ͼ��Ԥ������Ⱦ�У�", (int)((1920.0 - WindowWidth) / 2), (int)((1080.0 - WindowHeight) / 2) - 50);//���ô���λ��
	resizeWindow("ͼ��Ԥ������Ⱦ�У�", WindowWidth, WindowHeight);//���ô��ڴ�С

	Vec3 lookfrom(13, 2, 3);
	Vec3 lookat(0, 0, 0);
	Vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

	//�����ʼ��
	HitableList world = random_scene();
	//world.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5, make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5))));
	//world.add(make_shared<Sphere>(Vec3(0, -100.5, -1), 100, make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0))));
	//world.add(make_shared<Sphere>(Vec3(1, 0, -1), 0.5, make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.0)));
	////world.add(make_shared<Sphere>(Vec3(-1, 0, -1), 0.5, make_shared<Metal>(Vec3(0.8, 0.8, 0.8), 1.0)));
	//world.add(make_shared<Sphere>(Vec3(-1, 0, -1), 0.5, make_shared<Dielectric>(1.5)));
	//world.add(make_shared<Sphere>(Vec3(-1, 0, -1), -0.45, make_shared<Dielectric>(1.5)));

	for (int j = ny - 1; j >= 0; j--)//����Ϣ
	{
		for (int i = 0; i < nx; i++)//����Ϣ
		{
			//��ʼ����ɫ
			Vec3 Col(0, 0, 0);

			for (int s = 0; s < ns; s++)
			{
				double U = double((i + random_double(0, 1)) / double(nx));
				double V = double((j + random_double(0, 1)) / double(ny));
				//�õ�ԭ������A�ͷ�������B
				Ray r = camera.GetRay(U, V);
				//��������Ƿ񾭹����壬������ɫ
				//�ۼӣ�ȡ��ֵ
				Col += Color(r, world, max_depth);
			}
			//ȡ��ֵ
			Col /= double(ns);
			//٤��У��
			Col = Vec3(sqrt(Col[0]), sqrt(Col[1]), sqrt(Col[2]));

			//�������������߼�⵽����ɫ���Ϊ�졢�̡�������ͨ��
			int ir = int(255.99 * Col.R());
			int ig = int(255.99 * Col.G());
			int ib = int(255.99 * Col.B());

			//����ǰ���ص�����ͨ��ֵд���ļ�
			std::cout << ir << " " << ig << " " << ib << std::endl;
			//ʵʱ��ʾ��Ⱦ֡
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[0] = ib;
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[1] = ig;
			RenderingImage.at<cv::Vec3b>(ny - 1 - j, i)[2] = ir;
			//Sleep(0);

		}
		//ÿ�м������Ժ�ˢ��Ԥ������
		if (!(j % (ny / 100))) //ÿ��Ⱦny/100�к����Ԥ������ͼƬ
		{
			imshow("ͼ��Ԥ������Ⱦ�У�", RenderingImage);
			waitKey(1);//�ȴ�1�����¼��ô���ˢ�����
		}
	}
	imshow("ͼ��Ԥ������Ⱦ�У�", RenderingImage);
	waitKey(5000); //�ȴ�3000����
	destroyAllWindows();//�رմ���
	return 0;
}
