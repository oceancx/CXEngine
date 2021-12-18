#include "vector.h"


namespace cx {

	Vector3 Vector3::right(-1, 0, 0);
	Vector3 Vector3::left(1, 0, 0);
	Vector3 Vector3::up(0, 1, 0);
	Vector3 Vector3::down(0, -1, 0);
	Vector3 Vector3::back(0, 0, -1);
	Vector3 Vector3::forward(0, 0, 1);
	Vector3 Vector3::one(1, 1, 1);
	Vector3 Vector3::zero(0, 0, 0);
	Vector3 Vector3::negativeInfinity(-999999999, -999999999, -999999999);
	Vector3 Vector3::positiveInfinity(999999999, 999999999, 999999999);

	Vector4 Vector4::zero(0, 0, 0, 0);

	float& Vector3::operator[](int index) {
		if (index == 0)return this->x;
		if (index == 1)return this->y;
		if (index == 2)return this->z;
		static float zero = 0;
		zero = 0;
		return zero;
	}

	Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t)
	{
		a = a * (1 - t) + b * t;
		return a;
	}

	void Vector3::Print() {
		printf("%f %f %f\n", x, y, z);
	}

	float Vector3::sqrMagnitude()
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 Vector3::normalized()
	{
		float mag = sqrMagnitude();
		Vector3 v(x / mag, y / mag, z / mag);
		return v;
	}

	float Vector3::magnitude()
	{
		return x * x + y * y + z * z;
	}

	Vector3 Vector3::Cross(Vector3 a, Vector3 b)
	{
		Vector3 c;
		c.x = a.y * b.z - a.z * b.y;
		c.y = a.z * b.x - a.x * b.z;
		c.z = a.x * b.y - a.y * b.x;
		return c;
	}

	float Vector3::Distance(Vector3 v1, Vector3 v2)
	{
		float dist = std::sqrt(std::powf(v1.x - v2.x, 2) + std::powf(v1.y - v2.y, 2) + std::powf(v1.z - v2.z, 2));
		return dist;
	}

	float Vector3::Dot(Vector3 v1, Vector3 v2)
	{
		float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return dot;
	}

	Vector3 operator+(Vector3 a, Vector3 b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	Vector3 operator-(Vector3 a, Vector3 b)
	{
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	Vector3 operator*(Vector3 a, Vector3 b)
	{
		return Vector3::Cross(a, b);
	}

	Vector3 operator*(Vector3 v, float scaler)
	{
		v.x *= scaler;
		v.y *= scaler;
		v.z *= scaler;
		return v;
	}

	Vector3 operator/(Vector3 v, float scaler)
	{
		v.x /= scaler;
		v.y /= scaler;
		v.z /= scaler;
		return v;
	}

	bool operator==(Vector3 a, Vector3 b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	bool operator!=(Vector3 a, Vector3 b)
	{
		return a.x != b.x || a.y != b.y || a.z == b.z;
	}

	Vector3 Quaternion::eulerAngles()
	{
		return Vector3();
	}

	Quaternion Quaternion::normalized()
	{
		return Quaternion();
	}

	float& Vector4::operator[](int index)
	{
		if (index == 0)return this->x;
		if (index == 1)return this->y;
		if (index == 2)return this->z;
		if (index == 3)return this->w;
		static float zero = 0;
		zero = 0;
		return zero;
	}

	Vector4& Matrix4x4::operator[](int index)
	{
		if (index == 0)return this->r0;
		if (index == 1)return this->r1;
		if (index == 2)return this->r2;
		if (index == 3)return this->r3;
		return Vector4::zero;
	}


}

