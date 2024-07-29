#pragma once

#include "Ray.h"
#include "Vec3.h"

class Camera
{
	Vec3 LowerLeftCorner;//���½�����
	Vec3 Horizontal;//��Ļˮƽ���
	Vec3 Vertical;//��Ļ��ֱ�߶�
	Vec3 Origin;//�ӵ�
	Vec3 u, v, w;
	double lens_radius;

public:
	//������  ��߱�  ���Ź�Ȧ  ����
	Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect, double aperture, double focus_dist) 
	{
		Origin = lookfrom;
		lens_radius = aperture / 2;
		Vec3 u, v, w;

		auto theta = degrees_to_radians(vfov);
		auto half_height = 1 * tan(theta / 2);	//����z=-1ƽ��
		auto half_width = aspect * half_height;
		w = UnitVector(lookfrom - lookat);
		u = UnitVector(Cross(vup, w));
		v = Cross(w, u);

		LowerLeftCorner = Origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;	 //���½�����
		Horizontal = 2 * half_width * focus_dist * u;			 //��Ļˮƽ���
		Vertical = 2 * half_height * focus_dist * v;			 //��Ļ��ֱ�߶�
	}

	Ray GetRay(double U, double V)
	{
		Vec3 rd = lens_radius * Vec3::random_in_unit_disk();
		Vec3 offset = u * rd.X() + v * rd.Y();

		//RΪ��ǰ�ļ������		����ԭ�㲻����һ���㣬����һ����Χ��������ǵĹ���ԭ����Ҫ����ƫ��
		return Ray(Origin + offset, LowerLeftCorner + U * Horizontal + V * Vertical - Origin - offset);
	}
};