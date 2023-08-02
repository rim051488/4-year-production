#include "Random.h"



Random::Random()
{
	mt_.seed(std::random_device()());
}

bool Random::GetRandom(const double& p)
{
	std::bernoulli_distribution d(p);
	return d(mt_);
}

int Random::GetRandom(const int& minVal, const int& maxVal)
{
	std::uniform_int_distribution<> d(minVal, maxVal);
	return d(mt_);
}

float Random::GetRandom(const float& minVal, const float& maxVal)
{
	std::uniform_real_distribution<float> d(minVal, maxVal);
	return d(mt_);
}

double Random::GetRandom(const double& minVal, const double& maxVal)
{
	std::uniform_real_distribution<double> d(minVal, maxVal);
	return d(mt_);
}


