#pragma once
#include <vector>
#include "Engine/Math/Matrix44.hpp"
//#TODO: Put this in cpp
class Matrix4x4Stack
{
public:
	std::vector<mat44_fl> matrices;

	Matrix4x4Stack()
	{
		matrices.push_back(mat44_fl::identity);
	}

	bool IsEmpty()
	{
		return(matrices.size() == 1);//if it equals identity
	}

	mat44_fl& GetTop()
	{
		return matrices.back();
	}

	void Push(mat44_fl const &transform)
	{
		//Mult on left, pre mutiply
		//Mult on right, post multiply (should be us, col major)
		//This is Mult on right
		mat44_fl top = GetTop();
		mat44_fl new_top = top * transform;
		matrices.push_back(new_top);
	}

	void Pop()
	{
		if(!IsEmpty())
		{
			matrices.pop_back();
		}
	}
};
