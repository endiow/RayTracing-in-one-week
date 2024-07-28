#pragma once

#include "Ray.h"

class Material;
struct HitRecord   //���н��
{
	double T;		//�����������T
	Vec3 P;			//�����
	Vec3 Normal;	//����
	bool front_face;//�жϷ��߷���
	Material* MatPtr;

	inline void set_face_normal(const Ray& r, const Vec3& outward_normal) 
	{
		front_face = Dot(r.Direction(), outward_normal) < 0;
		Normal = front_face ? outward_normal : -outward_normal;//�������ǳ���
	}
};

class Hitable
{
public:
	//TMin��TMax�ֱ��ʾ������ߵĿ�ʼ��������Ӧ��ϵ��T
	//Ҳ���Ǽ������ΪR = P(TMin)��R=P(TMax��֮����������
	virtual bool Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec) const = 0;//�麯��
};