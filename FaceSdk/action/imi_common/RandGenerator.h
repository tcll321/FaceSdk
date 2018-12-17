#pragma once

#include <random>
#include "RandGeneratorBasic.h"
#include <Eigen/Core>
class RandGenerator: public RandGeneratorBasic
{
public:
	static Eigen::Vector3f random_uniform(Eigen::Vector3f* l_, Eigen::Vector3f* h_)
	{
		Eigen::Vector3f l = *l_, h = *h_;
		return Eigen::Vector3f(UF(l(0), h(0)), UF(l(1), h(1)), UF(l(2), h(2)));
	}
	static Eigen::Vector4f random_uniform(Eigen::Vector4f* l_, Eigen::Vector4f* h_)
	{
		Eigen::Vector4f l = *l_, h = *h_;
		return Eigen::Vector4f(UF(l(0), h(0)), UF(l(1), h(1)), UF(l(2), h(2)), UF(l(3), h(3)));
	}
	static Eigen::Vector3f random_normal(Eigen::Vector3f* l_, Eigen::Vector3f* h_, float mean, float div)
	{
		Eigen::Vector3f l = *l_, h = *h_;
		Eigen::Vector3f res;
		for (int i = 0; i < 3; i++)
		{
			float f = RandGenerator::NF(mean, div, 0, 1);
			res(i) = l(i)*(1 - f) + h(i)*f;
		}
		return res;
	}
	static Eigen::Vector4f random_normal(Eigen::Vector4f* l_, Eigen::Vector4f *h_, float mean, float div)
	{
		Eigen::Vector4f l = *l_, h = *h_;
		Eigen::Vector4f res;
		for (int i = 0; i < 4; i++)
		{
			float f = RandGenerator::NF(mean, div, 0, 1);
			res(i) = l(i)*(1 - f) + h(i)*f;
		}
		return res;
	}
};