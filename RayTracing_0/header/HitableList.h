#pragma once

#include "Hitable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HitableList :
	public Hitable
{
	std::vector<shared_ptr<Hitable>> objects;	//存放物体的列表
public:
	HitableList() {}
	HitableList(shared_ptr<Hitable> object) { add(object); }	//初始化/添加 物体 

	void clear() { objects.clear(); }
	void add(shared_ptr<Hitable> object) { objects.push_back(object); }

	virtual bool Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const;
};

//快速地找出射线命中的距离最近的对象（因为渲染只渲染最前面的，后面的被前面的挡住了）
bool HitableList::Hit(const Ray& R, double TMin, double TMax, HitRecord& Rec)const
{
	HitRecord TempRec; //记录命中结果
	bool HitAnything = false;//表示是否命中物体
	double ClosestSoFar = TMax;//动态的设置射线的尾端

	for (const auto& object : objects)//遍历列表
	{
		if (object->Hit(R, TMin, ClosestSoFar, TempRec))//如果当前的列表元素被射线R命中
		{
			HitAnything = true;	//将HitAnything设置为true，表示射线R命中了物体
			ClosestSoFar = TempRec.T;
			Rec = TempRec;
		}
	}
	return HitAnything;
}