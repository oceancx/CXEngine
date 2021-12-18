#pragma once
#include "pos.h"
#include <glm/glm.hpp>

#define PI 3.141592f
#define DegreeToRadian(d) (d*PI/180.f)
#define RadianToDegree(r) (r*180.f/PI)
#define PIXEL_PER_CM  37.71f

class GMath
{
public:
	static float Astar_GetDistance(float sx, float sy, float ex, float ey);
	static float Astar_GetDistanceSquare(float sx, float sy, float ex, float ey);
	static float Astar_GetAngle(float sx, float sy, float ex, float ey);
	static int Astar_GetAngleUseBoxXY(int sx, int sy, int ex, int ey);
	static int Astar_GetDirUseInt(int degree);
	static int Astar_GetDir(float degree);
	static int Astar_GetDir4(float degree);
	static int GetReverseDir(int dir);
	static int Clamp(int value, int min, int max);
	static glm::vec3 CubicBezierCurve(glm::vec3 p1, glm::vec3 p2, float t);
	static glm::vec3 CubicBezierVelocity(glm::vec3 p1, glm::vec3 p2, float t);
	static inline float Meter2Pixel(float meter) { return meter * 100.f * PIXEL_PER_CM; };
	static int Dir8toDir4(int dir);
	static int NextDir4(int dir);
};

namespace utils
{
	inline static bool BoundHitTest(Bound bound, Pos pt) {
		if (pt.x >= bound.left && pt.x <= bound.right && pt.y >= bound.top && pt.y <= bound.bottom)
			return true;
		else
			return false;
	}
	inline static bool BoundHitTest(Bound b1, Bound b2) { if (b1.right < b2.left || b1.left > b2.right || b1.bottom < b2.top || b1.top >  b2.bottom)return false; else return true; }
}

