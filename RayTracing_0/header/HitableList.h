#pragma once

#include "Hitable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HitableList :
	public Hitable
{
	std::vector<shared_ptr<Hitable>> objects;	//���������б�
public:
	HitableList() {}
	HitableList(shared_ptr<Hitable> object) { add(object); }	//��ʼ��/��� ���� 

	void clear() { objects.clear(); }
	void add(shared_ptr<Hitable> object) { objects.push_back(object); }

	virtual bool Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const;
};

//���ٵ��ҳ��������еľ�������Ķ�����Ϊ��Ⱦֻ��Ⱦ��ǰ��ģ�����ı�ǰ��ĵ�ס�ˣ�
bool HitableList::Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const
{
	HitRecord TempRec; //��¼���н��
	bool HitAnything = false;//��ʾ�Ƿ���������
	double ClosestSoFar = TMax;//��̬���������ߵ�β��

	for (const auto& object : objects)//�����б�
	{
		if (object->Hit(R, TMin, ClosestSoFar, TempRec))//�����ǰ���б�Ԫ�ر�����R����
		{
			HitAnything = true;	//��HitAnything����Ϊtrue����ʾ����R����������
			ClosestSoFar = TempRec.T;
			Rec = TempRec;
		}
	}
	return HitAnything;
}