#pragma once

#include "Hitable.h"

class Sphere : public Hitable
{
	Vec3 Center = { 0,0,0 }; //����
	double Radius = 0;//�뾶
	shared_ptr<Material> mat_ptr;

public:
	Sphere() {}
	Sphere(Vec3 Cen, double R, shared_ptr<Material> m) :Center(Cen), Radius(R), mat_ptr(m) {};	//���캯������ʼ��������뾶

	inline virtual bool Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const;	//�̳�Hitable��Hit
};


//�������R��Tmin��Tmax��Χ�����е�ǰ���壬�򷵻�true
bool Sphere::Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const
{
	//��һԪ���η���
	Vec3 OC = R.Origin() - Center;//�����ĵ����߷���������
	double A = Dot(R.Direction(), R.Direction());
	double B = Dot(OC, R.Direction());
	double C = Dot(OC, OC) - Radius * Radius;
	double Discriminant = B * B - A * C;

	if (Discriminant > 0) //�����������
	{

		//Rec.MatPtr = MatPtr;
		//����Ƚ�С�Ľ�
		double Temp = (-B - sqrt(B * B - A * C)) / A;
		if (Temp < TMax && Temp >TMin) //�����������T��TMin��TMax֮�䣬��������Ҫ��ֵ
		{
			//���������Ľ⡢�����λ�ú�����㷨�ߵ�Rec��
			Rec.T = Temp;
			Rec.P = R.PointAtParameter(Rec.T);
			Vec3 outward_normal = (Rec.P - Center) / Radius;
			Rec.set_face_normal(R, outward_normal);
			Rec.mat_ptr = mat_ptr;
			return true;
		}
		//����ϱߵĵ㲻�����ǵļ�ⷶΧ�ڣ�����ȥ��ͬ���ķ��������һ����
		Temp = (-B + sqrt(B * B - A * C)) / A;
		if (Temp < TMax && Temp >TMin)
		{
			//���������Ľ⡢�����λ�ú�����㷨�ߵ�Rec��
			Rec.T = Temp;
			Rec.P = R.PointAtParameter(Rec.T);
			Vec3 outward_normal = (Rec.P - Center) / Radius;
			Rec.set_face_normal(R, outward_normal);
			Rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	//�������û���������򷵻�false
	return false;
}