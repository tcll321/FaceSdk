#pragma once

#include <random>
class RandGeneratorBasic
{
public:
	static void set_seed(int seed)
	{
		srand(seed);
	}
	static float UF(float lower, float upper)
	{
		double r = rand() / double(RAND_MAX);
		return lower + (upper - lower)*r;
	};
	static int UI(int a, int b)
	{
		if (b > a)
			return rand() % (b - a + 1) + a;
		else
			return rand() % (a - b + 1) + b;
	};
	static int BI(float positive)
	{
		double r = rand() / double(RAND_MAX);
		return !!(r >= 1 - positive);
	};
	static float NF(float mean, float div, float lower, float upper)
	{
		const double epsilon = (std::numeric_limits<double>::min)();
		const double two_pi = 2.0*3.14159265358979323846;
		double u1, u2;
		do
		{
			u1 = rand() * (1.0 / RAND_MAX);
			u2 = rand() * (1.0 / RAND_MAX);
		} while (u1 <= epsilon);

		double z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
		
		z0 = z0*div + mean;

		if (z0 < lower)
			z0 = lower;
		else if (z0 > upper)
			z0 = upper;
		
		return z0;
	};
	static int NI(float mean, float div, int lower, int upper)
	{
		float z = NF(mean, div, lower, upper);
		return z;
	};

	static float random_uniform(float* l, float* h)
	{
		return UF(*l, *h);
	}
	
	static float random_normal(float* l_, float* h_, float mean, float div)
	{
		float l = *l_, h = *h_;
		float f = RandGeneratorBasic::NF(mean, div, 0, 1);
		return l*(1 - f) + h*f;
	}
};

typedef RandGeneratorBasic RandB;