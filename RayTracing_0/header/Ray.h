#pragma once
#include "Vec3.h"
//#include "Random.h"

class Ray
{
	//A��BΪ��ĳ�Ա����
	Vec3 A;
	Vec3 B;
public:
	Ray() { A = Vec3(0, 0, 0), B = Vec3(0, 0, 0); }
	Ray(const Vec3& a, const Vec3& b) { A = a; B = b; }	 //���캯������ʼ��ԭ������A�ͷ�������B
	Vec3 Origin() const { return A; } //����ԭ������A
	Vec3 Direction() const { return B; }//���ط�������B
	Vec3 PointAtParameter(double T)const { return A + T * B; }//ͨ��T�������P(t) = A + t * B
	
	//��λ�����ڵ������
	friend inline Vec3 RandomInUnitSphere();
	friend inline Vec3 random_unit_vector();
	friend inline Vec3 random_in_hemisphere(const Vec3& normal);

	friend inline Vec3 Reflect(const Vec3& V, const Vec3& N);//�������
	//friend inline bool Refract(const Vec3& V, const Vec3& N, double NiOverNt, Vec3& Refracted);//�������
	friend inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat);
};

Vec3 RandomInUnitSphere()
{
	while (true) 
	{
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
		if (p.SquaredLength() >= 1) continue;	//�����������������ֱ���õ�������
		return p;
	}
}

//���߾��뷨�߱ȽϽ��ĸ��ʻ����
Vec3 random_unit_vector() 
{
	auto a = random_double(0, 2 * pi);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);
	return Vec3(r * cos(a), r * sin(a), z);
}

//ֱ�Ӵ�����㿪ʼѡȡһ������ķ���, Ȼ�����ж��Ƿ��ڷ��������ڵ��Ǹ�����
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

////�ж��Ƿ���������������
////�����ֱ�������㣬����㷨�ߣ������ʣ������Ĺ���
//inline bool Refract(const Vec3& V, const Vec3& N, double NiOverNt, Vec3& Refracted)
//{
//	Vec3 UV = UnitVector(V);//�����λ�õĵ�λ����
//	double Dt = Dot(UV, N);	//DtΪUV�ڷ���N�������ϵ�ͶӰ���ȡ�
//	double Discriminant = 1.0 - NiOverNt * NiOverNt * (1 - Dt * Dt);//1-sin(theta1)^2
//
//	//����Ҫע����ǣ�DIscriminant>0��ִ������
//	if (Discriminant > 0)  //��ʾ������������
//	{
//		Refracted = NiOverNt * (UV - N * Dt) - N * sqrt(Discriminant);	//�õ������Ĺ���
//		return true;
//	}
//	else
//		return false;
//}

//					�������        ������         ����������Ľ��
Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) 
{
	auto cos_theta = Dot(-uv, n);
	Vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);	//ƽ���ڷ������ķ���
	Vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.SquaredLength()) * n;	//��ֱ�ڷ������ķ���
	return r_out_parallel + r_out_perp;		//֮��Ϊ�������
}