#pragma once
#include "singleton.h"
class Random : public Singleton<Random>
{
public:
	Random() ;
	~Random() {};
	int NextIntSrand(int rangeMin, int rangeMax);
	int NextInt(int rangeMin,int rangeMax);
};
#define RANDOM_INSTANCE Random::GetInstance()