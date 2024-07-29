#pragma once
#include "Vec3.h"
//#include "Random.h"

class Ray
{
	//A和B为类的成员变量
	Vec3 A;
	Vec3 B;
public:
	Ray() { A = Vec3(0, 0, 0), B = Vec3(0, 0, 0); }
	Ray(const Vec3& a, const Vec3& b) { A = a; B = b; }	 //构造函数，初始化原点向量A和方向向量B
	Vec3 Origin() const { return A; } //返回原点向量A
	Vec3 Direction() const { return B; }//返回方向向量B
	Vec3 PointAtParameter(double T)const { return A + T * B; }//通过T计算光线P(t) = A + t * B
	
	//单位球体内的随机点
	friend inline Vec3 RandomInUnitSphere();
	friend inline Vec3 random_unit_vector();
	friend inline Vec3 random_in_hemisphere(const Vec3& normal);

	friend inline Vec3 Reflect(const Vec3& V, const Vec3& N);//反射光线
	//friend inline bool Refract(const Vec3& V, const Vec3& N, double NiOverNt, Vec3& Refracted);//折射光线
	friend inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat);
};

Vec3 RandomInUnitSphere()
{
	while (true) 
	{
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
		if (p.SquaredLength() >= 1) continue;	//点在球外就重新生成直到该点在球内
		return p;
	}
}

//光线距离法线比较近的概率会更高
Vec3 random_unit_vector() 
{
	auto a = random_double(0, 2 * pi);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);
	return Vec3(r * cos(a), r * sin(a), z);
}

//直接从入射点开始选取一个随机的方向, 然后再判断是否在法向量所在的那个半球
Vec3 random_in_hemisphere(const Vec3& normal) 
{
	Vec3 in_unit_sphere = random_unit_vector();
	if (Dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

inline Vec3 Reflect(const Vec3& V, const Vec3& N)
{
	return V - 2 * Dot(V, N) * N;
}

////判断是否可以生成折射光线
////参数分别是入射点，入射点法线，折射率，折射后的光线
//inline bool Refract(const Vec3& V, const Vec3& N, double NiOverNt, Vec3& Refracted)
//{
//	Vec3 UV = UnitVector(V);//入射点位置的单位向量
//	double Dt = Dot(UV, N);	//Dt为UV在法线N正方向上的投影长度。
//	double Discriminant = 1.0 - NiOverNt * NiOverNt * (1 - Dt * Dt);//1-sin(theta1)^2
//
//	//这里要注意的是，DIscriminant>0才执行折射
//	if (Discriminant > 0)  //表示满足折射条件
//	{
//		Refracted = NiOverNt * (UV - N * Dt) - N * sqrt(Discriminant);	//得到折射后的光线
//		return true;
//	}
//	else
//		return false;
//}

//					入射光线        法向量         折射率相除的结果
Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) 
{
	auto cos_theta = Dot(-uv, n);
	Vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);	//平行于法向量的分量
	Vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.SquaredLength()) * n;	//垂直于法向量的分量
	return r_out_parallel + r_out_perp;		//之和为折射光线
}