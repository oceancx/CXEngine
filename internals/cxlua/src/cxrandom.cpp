#include "cxrandom.h"
#include <cstdlib>
#include <ctime>
#include <chrono>

Random::Random()
{
	auto now = std::chrono::system_clock::now();
   	auto currentTime = now.time_since_epoch().count() /1000;
	std::srand(static_cast<unsigned int>( currentTime)); 
}

int Random::NextInt(int rangeMin, int rangeMax)
{
	int randomVariable = std::rand();
	return randomVariable % (rangeMax - rangeMin+1) + rangeMin;
}


int Random::NextIntSrand(int rangeMin, int rangeMax)
{
	auto now = std::chrono::system_clock::now();
	auto currentTime = now.time_since_epoch().count() / 1000;
	std::srand(static_cast<unsigned int>(currentTime));
	int randomVariable = std::rand();
	return randomVariable % (rangeMax - rangeMin + 1) + rangeMin;
}
