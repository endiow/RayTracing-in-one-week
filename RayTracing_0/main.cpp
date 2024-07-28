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
		//���ط�����ͼ
		return 0.5 * Vec3(Rec.Normal.X() + 1, Rec.Normal.Y() + 1, Rec.Normal.Z() + 1);//ӳ�䵽[0,1]
	}

	//���ر���ɫ
	Vec3 UnitDirection = UnitVector(r.Direction());		 //��ȡ��λ��������
	double T = 0.5 * (UnitDirection.Y() + 1.0);			 //��ֵ������Χ[0,1]
	return (1.0 - T) * Vec3(1.0, 1.0, 1.0) + T * Vec3(0.5, 0.7, 1.0); //����һ����ɫ
}

int main()
{
	int nx = 800;//ͼƬ��ȣ���λΪ���أ�
	int ny = 400;//ͼƬ�߶ȣ���λΪ���أ�
	int ns = 100;//ɨ�����
	//�ļ�ͷд��
	std::cout << "P3" << std::endl << nx << " " << ny << std::endl << "255" << std::endl;

	Vec3 lowerleftCorner(-2.0, -1.0, -1.0);
	Vec3 Horiaontal(4.0, 0.0, 0.0);
	Vec3 Vertical(0.0, 2.0, 0.0);
	Vec3 Origin(0.0, 0.0, 0.0);

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

	HitableList world;
	world.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(Vec3(0, -100.5, -1), 100));

	for (int j = ny - 1; j >= 0; j--)//����Ϣ
	{
		for (int i = 0; i < nx; i++)//����Ϣ
		{
			double U = double (i / double(nx));
			double V = double (j / double(ny));
			Ray r(Origin, lowerleftCorner + U * Horiaontal + V * Vertical);
			Vec3 Col = Color(r, world);
			

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
