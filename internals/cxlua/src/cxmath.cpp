#include "cxmath.h"
#include <cmath>
#include <iostream>
#include "define_types.h"


float GMath::Astar_GetDistance(float sx, float sy, float ex, float ey)
{
	float dx = sx - ex;
	float dy = sy - ey;
	return sqrtf(powf(dx, 2) + powf(dy, 2));
}

float GMath::Astar_GetDistanceSquare(float sx, float sy, float ex, float ey)
{
	float dx = sx - ex;
	float dy = sy - ey;
	return powf(dx, 2) + powf(dy, 2);
}

int GMath::Astar_GetAngleUseBoxXY(int sx, int sy, int ex, int ey)
{

	return (int)(Astar_GetAngle(sx * 1.f, sy * 1.f, ex * 1.f, ey * 1.f));  // not possible
}

int GMath::Astar_GetDirUseInt(int degree) {
	//右下，左下，左上，右上，下，左，上，右
	// 0    1    2    3   4  5   6  7
	//  2 3 0 1       6 7   4 5
	//int innerDirs[8] = { 7,3,6,2,5,1,4,0 };

	return (int)Astar_GetDir(degree * 1.f);
}

float GMath::Astar_GetAngle(float sx, float sy, float ex, float ey)
{
	//	Logger::Print("Astar_GetAngle: sx=%lf sy=%lf ex=%lf ey=%lf\n", sx, sy, ex, ey);

	float dx = ex - sx;
	float dy = ey - sy;
	float degree = std::atan2f(dy, dx) / std::acosf(-1) * 180;
	return degree;
}

int GMath::Astar_GetDir(float degree) {
	//右下，左下，左上，右上，下，左，上，右
	// 0    1    2    3   4  5   6  7
	//  2 3 0 1       6 7   4 5
	//int innerDirs[8] = { 7,3,6,2,5,1,4,0 };
	  /*int innerDirs[8] = { 7,0,4,1,5,2,6,3};
	  if ((360 - 22.5 <= degree&&degree<360) || (degree>=0&& degree < 22.5)) {
		  return innerDirs[0];
	  }
	  else {
		  for (int i = 1; i < 8; i++) {
			  if (i * 45 - 22.5 <= degree && degree < i * 45 + 22.5) {
				  return innerDirs[i];
			  }
		  }
		  return innerDirs[0];
	  }*/

	int innerDirs[8] = { 7,0,4,1,5,2,6,3 };
	if (degree >= 0) //degree: 0 ~ 180
	{

		float step = 45.f;
		float init = -22.5f;
		for (int i = 0; i < 5; i++)
		{
			if (degree >= init && degree < init + step)
			{
				return innerDirs[i];
			}
			else
			{
				init += step;
			}
		}
	}
	else
	{	// degree -180~-0
		if (degree  < 0 && degree > -22.5)return innerDirs[0];
		float step = -45.f;
		float init = -22.5f;
		for (int i = 7; i >= 4; i--)
		{
			if (degree > init + step && degree <= init)
			{
				return innerDirs[i];
			}
			else
			{
				init += step;
			}
		}
	}
	std::cout << "Astar_GetDir error!!! degree:" << degree << std::endl;
	return 0;
}
//enum Dir
//{
//	N = 6,
//	S = 4,
//	W = 5,
//	E = 7,
//	N_E = 3,
//	N_W = 2,
//	S_E = 0,
//	S_W = 1,
//};
int GMath::Astar_GetDir4(float degree)
{
	if (degree >= 0 && degree < 90.0f)
	{
		return Direction::S_E;
	}
	else if (degree >= 90.0f && degree < 180.f)
	{
		return Direction::S_W;
	}
	else if (degree >= -180.f && degree <= -90.f)
	{
		return Direction::N_W;
	}
	else if (degree > -90.f && degree < 0.f)
	{
		return Direction::N_E;
	}
	assert(false);
	std::cout << "Astar_GetDir4 error!!! degree:" << degree << std::endl;
	return 0;
}

int GMath::GetReverseDir(int dir)
{
	switch (dir)
	{
	case 6: dir = 4; break;
	case 4: dir = 6; break;
	case 5: dir = 7; break;
	case 7: dir = 5; break;
	case 3: dir = 1; break;
	case 1: dir = 3; break;
	case 2: dir = 0; break;
	case 0: dir = 2; break;
	}
	return dir;
}
int GMath::Clamp(int value, int min, int max)
{
	if (value < min)value = min;
	if (value > max)value = max;
	return value;
}

glm::vec3 GMath::CubicBezierCurve(glm::vec3 p1, glm::vec3 p2, float t)
{
	glm::vec3 p0(0, 0, 0);
	p0 = p0 * (std::powf(1.f - t, 3));
	glm::vec3 bezier;
	p1 = p1 * (3 * (t * std::powf(1.f - t, 2)));
	p2 = p2 * (3 * std::powf(t, 2) * (1 - t));
	glm::vec3 p3(1.f, 1.f, 0);
	p3 = p3 * (std::powf(t, 3));
	bezier = p1 + p2 + p3;
	return bezier;
}

//p0(3t^2 + 1 - 3t - t^3) + 3p1(1 + 3t^2 - 4*t) + 3p2(2*t - 3*t^2) + p3 * 3*t^2
glm::vec3 GMath::CubicBezierVelocity(glm::vec3 p1, glm::vec3 p2, float t)
{
	glm::vec3 p0(0, 0, 0);
	p0 = p0 * (-3.f * std::powf(t - 1.f, 2));
	p1 = p1 * (3 * (1 + 3 * std::powf(t, 2) - 4 * t));
	p2 = p2 * (3 * (2 * t - 3 * std::powf(t, 2)));
	glm::vec3 p3(1.f, 1.f, 0);
	p3 = p3 * (3 * std::powf(t, 2));
	glm::vec3 bezier = p1 + p2 + p3;
	return bezier;
}


int GMath::Dir8toDir4(int dir)
{
	if (dir == Direction::N)return Direction::N_E;
	if (dir == Direction::E)return Direction::S_E;
	if (dir == Direction::S)return Direction::S_W;
	if (dir == Direction::W)return Direction::N_W;
	return dir;
}
int GMath::NextDir4(int dir)
{
	dir = Dir8toDir4(dir);
	if (dir == Direction::N_E)return Direction::S_E;
	if (dir == Direction::S_E)return Direction::S_W;
	if (dir == Direction::S_W)return Direction::N_W;
	if (dir == Direction::N_W)return Direction::N_E;
	return dir;
}
//
//float diagonal = 23; //寸
//float diagonal_cm = 58.42;
//float diagonal_pixs = 2202.9;
//
//float pixs_per_inch = diagonal_pixs / diagonal;	 //95.78
//float cm = 50 / pixs_per_cm;
//float m = cm / 100.f;
