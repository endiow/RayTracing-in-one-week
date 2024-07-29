#pragma once

#include "Ray.h"
#include "Vec3.h"

class Camera
{
	Vec3 LowerLeftCorner;//左下角坐标
	Vec3 Horizontal;//屏幕水平宽度
	Vec3 Vertical;//屏幕垂直高度
	Vec3 Origin;//视点
	Vec3 u, v, w;
	double lens_radius;

public:
	//俯仰角  宽高比  快门光圈  焦距
	Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect, double aperture, double focus_dist) 
	{
		Origin = lookfrom;
		lens_radius = aperture / 2;
		Vec3 u, v, w;

		auto theta = degrees_to_radians(vfov);
		auto half_height = 1 * tan(theta / 2);	//射向z=-1平面
		auto half_width = aspect * half_height;
		w = UnitVector(lookfrom - lookat);
		u = UnitVector(Cross(vup, w));
		v = Cross(w, u);

		LowerLeftCorner = Origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;	 //左下角坐标
		Horizontal = 2 * half_width * focus_dist * u;			 //屏幕水平宽度
		Vertical = 2 * half_height * focus_dist * v;			 //屏幕垂直高度
	}

	Ray GetRay(double U, double V)
	{
		Vec3 rd = lens_radius * Vec3::random_in_unit_disk();
		Vec3 offset = u * rd.X() + v * rd.Y();

		//R为当前的检测射线		光线原点不再是一个点，而是一个范围，因此我们的光线原点需要发生偏移
		return Ray(Origin + offset, LowerLeftCorner + U * Horizontal + V * Vertical - Origin - offset);
	}
};