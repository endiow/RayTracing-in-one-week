#pragma once

//#include "Reflect.h"
#include "Hitable.h"


class Material
{
public:
	/*
		*���ϵ�ɢ����Ϊ
		*��������:������ߡ�������Ϣ��˥����ɢ�䡣
	*/
	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const = 0;
};

class Lambertian : public Material
{
	Vec3 Albedo;  //������
	double p;	//����ɢ��ĸ���

public:
	Lambertian(const Vec3& A) :Albedo(A), p(1.0) {}
	Lambertian(const Vec3& A, const double& p) :Albedo(A), p(p) {}

	//ɢ����Ϊ
	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		//����㵥λ�����������
		Vec3 Target = Rec.P + Rec.Normal + random_unit_vector();	//������㷨�߶˵�ΪԲ�ĵ�Բ�ڵ� ���������

		Scattered = Ray(Rec.P, Target - Rec.P);//����Ray��ɢ�����
		Attenuation = Albedo;	//����˥������
		return true;
	}
};

/*
*	��������
*/
class Metal : public Material
{
	Vec3 Albedo; //������
	double Fuzz;//�ֲڶ�

public:

	Metal(const Vec3& A) :Albedo(A), Fuzz(0.5) {}

	Metal(const Vec3& A, double F) :Albedo(A) 
	{
		Fuzz = F > 1 ? 1 : F < 0 ? 0 : F;	//���ƴֲڶ���0��1֮��
	}

	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		Vec3 Reflected = Reflect(UnitVector(RIn.Direction()), Rec.Normal);//��÷������
		//Scattered = Ray(Rec.P, Reflected);//�õ�������㷴��Ĺ���

		//���ݴֲڶȻ�ȡ���������ߣ��ֲڶ�Խ�󣬷��������ɢ�Ը���
		Scattered = Ray(Rec.P, Reflected + Fuzz * RandomInUnitSphere());
		Attenuation = Albedo;//���� ˥��
		return (Dot(Scattered.Direction(), Rec.Normal) > 0); //�����������뷨�ߵļн�Ϊ�����return true ���淴��
	}
};

//���ڷ��ص�ǰ�Ƕ��·�����
inline double Schlick(double Cosine, double RefIdx_)
{
	double R0 = (1 - RefIdx_) / (1 + RefIdx_);
	R0 = R0 * R0;
	return R0 + (1 - R0) * pow((1 - Cosine), 5);
}

//͸����
class Dielectric : public Material
{
	double RefIdx;	//����du

public:
	Dielectric(double Ri) :RefIdx(Ri) {}

	virtual bool Scatter(const Ray& RIn, const HitRecord& Rec, Vec3& Attenuation, Ray& Scattered) const
	{
		Attenuation = Vec3(1.0, 1.0, 1.0);
		//ȷ������
		double etai_over_etat = (Rec.front_face) ? (1.0 / RefIdx) : (RefIdx);

		Vec3 unit_direction = UnitVector(RIn.Direction());
		double cos_theta = ffmin(Dot(-unit_direction, Rec.Normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		//���еĹ��߶�����������, ת�������˷���
		if (etai_over_etat * sin_theta > 1.0) 
		{
			Vec3 reflected = Reflect(unit_direction, Rec.Normal);
			Scattered = Ray(Rec.P, reflected);
			return true;
		}

		//��������ĸ��ʻ���������Ƕ��ı�
		double reflect_prob = Schlick(cos_theta, etai_over_etat);
		if (random_double(0, 1) < reflect_prob)
		{
			//��������
			Vec3 reflected = Reflect(unit_direction, Rec.Normal);
			Scattered = Ray(Rec.P, reflected);
			return true;
		}

		//�����������
		Vec3 refracted = refract(unit_direction, Rec.Normal, etai_over_etat);
		Scattered = Ray(Rec.P, refracted);

		return true;
	}

	friend inline double Schlick(double Cosine, double RefIdx_);
};



