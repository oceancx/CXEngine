#pragma once

#include "define_types.h"



namespace cx
{
	const float kEpsilon = 1E-05F;
	const float kEpsilonNormalSqrt = 1E-15F;
	//Quaternion public const float kEpsilon = 1E-06F;

	class Vector3;
	class Matrix4x4;

	class Vector3 {
	public:
		float x;
		float y;
		float z;

		Vector3() :x(0), y(0), z(0) {};
		Vector3(float x, float y) : Vector3(x, y, 0) {};
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
		void Print();

		static Vector3 right;
		static Vector3 left;
		static Vector3 up;
		static Vector3 back;
		static Vector3 forward;
		static Vector3 one;
		static Vector3 zero;
		static Vector3 negativeInfinity;
		static Vector3 positiveInfinity;
		static Vector3 down;

		float sqrMagnitude();
		Vector3 normalized();
		float magnitude();
		float& operator[](int index);

		static	Vector3 Cross(Vector3 v1, Vector3 v2);
		static	float Distance(Vector3 v1, Vector3 v2);
		static float Dot(Vector3 v1, Vector3 v2);
		static Vector3 Lerp(Vector3 a, Vector3 b, float t);
	};

	Vector3 operator +(Vector3 a, Vector3 b);
	Vector3 operator -(Vector3 a, Vector3 b);
	Vector3 operator *(Vector3 a, Vector3 b);
	Vector3 operator *(Vector3 a, float scaler);
	Vector3 operator /(Vector3 a, float scaler);
	bool operator ==(Vector3 a, Vector3 b);
	bool operator !=(Vector3 a, Vector3 b);

	class Vector4 :public Vector3 {
	public:
		static Vector4  zero;

		float w;

		Vector4() :Vector3(0, 0, 0), w(0) {};

		Vector4(float x, float y) : Vector3(x, y, 0), w(0) {};

		Vector4(float x, float y, float z, float w) : Vector3(x, y, z), w(w) {};

		//Vector4(Vector4 v) : Vector3(x, y, z), w(w) {};

		float& operator[](int index);
	};


	class Quaternion {
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion() : Quaternion(0, 0, 0, 0) {};

		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};

		static Quaternion identity;

		Vector3 eulerAngles();

		Quaternion normalized();

		// Returns the angle in degrees between two rotations a and b.
		static float Angle(Quaternion a, Quaternion b);
		// Creates a rotation which rotates angle degrees around axis.
		static Quaternion AngleAxis(float angle, Vector3 axis);

		static float Dot(Quaternion a, Quaternion b);
		static Quaternion Euler(Vector3 euler);
		static Quaternion Euler(float x, float y, float z);
		static Quaternion EulerAngles(float x, float y, float z);
		static Quaternion EulerAngles(Vector3 euler);
		static Quaternion EulerRotation(float x, float y, float z);
		static Quaternion EulerRotation(Vector3 euler);
		static Quaternion FromToRotation(Vector3 fromDirection, Vector3 toDirection);
		static Quaternion Inverse(Quaternion rotation);
		static Quaternion Lerp(Quaternion a, Quaternion b, float t);
		static Quaternion LookRotation(Vector3 forward);
		static Quaternion LookRotation(Vector3 forward, Vector3 upwards);
		static Quaternion Normalize(Quaternion q);
		static Quaternion RotateTowards(Quaternion from, Quaternion to, float maxDegreesDelta);
		static Quaternion Slerp(Quaternion a, Quaternion b, float t);
		static Vector3 ToEulerAngles(Quaternion rotation);

		bool Equals(Quaternion other);
		void Normalize();
		void Set(float newX, float newY, float newZ, float newW);
		void SetAxisAngle(Vector3 axis, float angle);
		void SetEulerAngles(Vector3 euler);
		void SetEulerAngles(float x, float y, float z);
		void SetEulerRotation(float x, float y, float z);
		void SetEulerRotation(Vector3 euler);
		void SetFromToRotation(Vector3 fromDirection, Vector3 toDirection);
		void SetLookRotation(Vector3 view, Vector3 up);
		void SetLookRotation(Vector3 view);
		void ToAngleAxis(float& angle, Vector3& axis);
		void ToAxisAngle(Vector3& axis, float& angle);
		Vector3 ToEuler();
		Vector3 ToEulerAngles();

		float& operator[](int index);
	};

	Vector3 operator *(Quaternion rotation, Vector3 point);
	Quaternion operator *(Quaternion lhs, Quaternion rhs);
	bool operator !=(Quaternion lhs, Quaternion rhs);
	bool operator ==(Quaternion lhs, Quaternion rhs);

	//Quaternion operator*(Quaternion  const& p1, Quaternion const& p2);
	//Vector3  operator+(Vector3 const& p1, Vector3 const& p2);
	class Matrix3x3 {
	public:
		float mat[9];
	};

	class Matrix4x4 {
	public:
		Vector4 r0;
		Vector4 r1;
		Vector4 r2;
		Vector4 r3;

		Matrix4x4() : r0(0, 0, 0, 0), r1(0, 0, 0, 0), r2(0, 0, 0, 0), r3(0, 0, 0, 0) {};

		Matrix4x4(Vector4 r0, Vector4 r1, Vector4 r2, Vector4 r3) : r0(r0), r1(r1), r2(r2), r3(r3) {};

		/*static Matrix4x4 zero{ get; }
		static Matrix4x4 identity{ get; }*/
		Vector4& operator[](int index);


		Matrix4x4 transpose();
		Quaternion rotation();
		Vector3 lossyScale();
		bool isIdentity();
		float determinant();
		/*FrustumPlanes decomposeProjection{ get; }*/
		Matrix4x4 inverse();
		static float Determinant(Matrix4x4 m);
		static Matrix4x4 Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
		//static Matrix4x4 Frustum(FrustumPlanes fp);
		static Matrix4x4 Inverse(Matrix4x4 m);
		static bool Inverse3DAffine(Matrix4x4 input, Matrix4x4& result);
		static Matrix4x4 LookAt(Vector3 from, Vector3 to, Vector3 up);
		static Matrix4x4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4x4 Perspective(float fov, float aspect, float zNear, float zFar);
		static Matrix4x4 Rotate(Quaternion q);
		static Matrix4x4 Scale(Vector3 vector);
		static Matrix4x4 Translate(Vector3 vector);
		static Matrix4x4 Transpose(Matrix4x4 m);
		static Matrix4x4 TRS(Vector3 pos, Quaternion q, Vector3 s);

		Vector4 GetColumn(int index);
		Vector4 GetRow(int index);
		Vector3 MultiplyPoint(Vector3 point);
		Vector3 MultiplyPoint3x4(Vector3 point);
		Vector3 MultiplyVector(Vector3 vector);
		void SetColumn(int index, Vector4 column);
		void SetRow(int index, Vector4 row);
		void SetTRS(Vector3 pos, Quaternion q, Vector3 s);
		/*override string ToString();
		string ToString(string format);
		Plane TransformPlane(Plane plane);*/
		bool ValidTRS();
		/*static Vector4 operator *(Matrix4x4 lhs, Vector4 vector);
		static Matrix4x4 operator *(Matrix4x4 lhs, Matrix4x4 rhs);
		static bool operator ==(Matrix4x4 lhs, Matrix4x4 rhs);
		static bool operator !=(Matrix4x4 lhs, Matrix4x4 rhs);*/

	};


}