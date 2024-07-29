#pragma once

//#include "Reflect.h"
#include "Hitable.h"


class Material
{
public:
	/*
		*材料的散射行为
		*参数包括:入射光线、命中信息、衰减，散射。
	*/
	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const = 0;
};

class Lambertian : public Material
{
	Vec3 Albedo;  //反射率
	double p;	//发生散射的概率

public:
	Lambertian(const Vec3& A) :Albedo(A), p(1.0) {}
	Lambertian(const Vec3& A, const double& p) :Albedo(A), p(p) {}

	//散射行为
	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		//入射点单位切球内随机点
		Vec3 Target = Rec.P + Rec.Normal + random_unit_vector();	//以入射点法线端点为圆心的圆内的 的随机射线

		Scattered = Ray(Rec.P, Target - Rec.P);//光线Ray的散射光线
		Attenuation = Albedo;	//返回衰减变量
		return true;
	}
};

/*
*	金属材质
*/
class Metal : public Material
{
	Vec3 Albedo; //反射率
	double Fuzz;//粗糙度

public:

	Metal(const Vec3& A) :Albedo(A), Fuzz(0.5) {}

	Metal(const Vec3& A, double F) :Albedo(A) 
	{
		Fuzz = F > 1 ? 1 : F < 0 ? 0 : F;	//控制粗糙度在0和1之间
	}

	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		Vec3 Reflected = Reflect(UnitVector(RIn.Direction()), Rec.Normal);//获得反射光线
		//Scattered = Ray(Rec.P, Reflected);//得到从入射点反射的光线

		//根据粗糙度获取随机反射光线，粗糙度越大，反射光线离散性更大
		Scattered = Ray(Rec.P, Reflected + Fuzz * RandomInUnitSphere());
		Attenuation = Albedo;//返回 衰减
		return (Dot(Scattered.Direction(), Rec.Normal) > 0); //如果反射光线与法线的夹角为锐角则return true 镜面反射
	}
};

//用于返回当前角度下反射率
inline double Schlick(double Cosine, double RefIdx_)
{
	double R0 = (1 - RefIdx_) / (1 + RefIdx_);
	R0 = R0 * R0;
	return R0 + (1 - R0) * pow((1 - Cosine), 5);
}

//透明体
class Dielectric : public Material
{
	double RefIdx;	//折射du

public:
	Dielectric(double Ri) :RefIdx(Ri) {}

	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		Attenuation = Vec3(1.0, 1.0, 1.0);
		//确定方向
		double etai_over_etat = (Rec.front_face) ? (1.0 / RefIdx) : (RefIdx);

		Vec3 unit_direction = UnitVector(RIn.Direction());
		double cos_theta = ffmin(Dot(-unit_direction, Rec.Normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		//所有的光线都不发生折射, 转而发生了反射
		if (etai_over_etat * sin_theta > 1.0) 
		{
			Vec3 reflected = Reflect(unit_direction, Rec.Normal);
			Scattered = Ray(Rec.P, reflected);
			return true;
		}

		//发生折射的概率会随着入射角而改变
		double reflect_prob = Schlick(cos_theta, etai_over_etat);
		if (random_double(0, 1) < reflect_prob)
		{
			//发生反射
			Vec3 reflected = Reflect(unit_direction, Rec.Normal);
			Scattered = Ray(Rec.P, reflected);
			return true;
		}

		//计算折射光线
		Vec3 refracted = refract(unit_direction, Rec.Normal, etai_over_etat);
		Scattered = Ray(Rec.P, refracted);

		return true;
	}

	friend inline double Schlick(double Cosine, double RefIdx_);
};



