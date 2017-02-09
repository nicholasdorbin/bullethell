#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <stdint.h>

class Quaternion;
class Rgba;
class AABB2;

#include "math.h"

const float pi = 3.141592653589793f;

float DegToRad(float deg);

float RadToDeg(float rad);

float SinDegrees(float deg);

float CosDegrees(float deg);

bool DoDiscsOverlap(const Vector2& center1, float radius1, const Vector2& center2, float radius2);

bool IsPointInDisc(const Vector2& center1, float radius1, const Vector2& center2);
bool IsPointInBounds(const Vector2& point, const AABB2& bounds);


float CalcSquaredDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2);

float CalcDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2);
//Vector3 CalcPointAtDistance(const Vector3& pos1, const EulerAngles& angle, const float& dist);

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);

int ClampInt(int inValue, int low, int high);

float ClampFloat(float inValue, float low, float high);

float ClampFloatCircular(float inValue, float low, float high);

float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees);

Vector2 CalcDirToPoint(const Vector2& from, const Vector2& to);

float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);

float DotProduct(const Vector4& a, const Vector4& b);
Vector3 CrossProduct(const Vector3& a, const Vector3& b);

float SmoothStep(float inputZeroToOne);

float ClampUpToZero(float value);

unsigned char FloatToUChar(float input);

bool IsColinear(const Vector3& a, const Vector3& b);

int RandomInt(int HI = 0, int LO = 0);

uint32_t GetNuonce();

float GetRandomFloatBetween(float low, float high);

bool GetRandomTrueOrFalseWithinProbability(float probability);

Rgba Lerp(Rgba a, Rgba b, float percent);
Vector3 Slerp(Vector3 start, Vector3 end, float percent);
Vector3 SlerpForseth(Vector3 const &a, Vector3 const &b, float t);
Vector3 SlerpUnit(Vector3 const &a, Vector3 const &b, float t);

Quaternion SlerpQuats(Quaternion const &quatA, Quaternion const &quatB, float t);

Vector3 HermiteSpline(const Vector3& startPosition, const Vector3& endPosition, const Vector3& startVelocity, const Vector3& endVelocity, const float& percentComplete);
Vector3 GetCatmullRomVelocity(const Vector3& thisPoint, const Vector3& prevPoint, const Vector3& nextPoint);

EulerAngles GetLookAtAngle(const Vector3  &start, const Vector3&  posToLookAt);


//Header Math Stuff
template <typename Tvalue, typename Ttime>
inline Tvalue Lerp(Tvalue const &y0, Tvalue const &y1, Ttime const &t)
{
	Ttime const one = Ttime(1);
	Tvalue ret = y0 * (one - t) + y1 * t;
	return ret;
}

//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Lerp(Ttime const &x0, Ttime const &x1,
	Tvalue const &y0, Tvalue const &y1,
	Ttime const &x)
{
	Ttime const one = Ttime(1);
	Ttime const t = (x - x0) / (x1 - x0);
	return Lerp(y0, y1, t);
}


//------------------------------------------------------------------------
template <typename T>
inline T Lerp(T const &x, T const &y, float const &t)
{
	return x * (1.0f - t) + y * t;
}

//------------------------------------------------------------------------
template <>
inline float Lerp(float const &a, float const &b, float const &t)
{
	return Lerp<float, float>(a, b, t);
}





//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Cubic(Ttime const &x0, Ttime const &x1,
	Tvalue const &y0, Tvalue const &y1,
	Tvalue const &vel0, Tvalue const &vel1,
	Ttime const &x)
{
	Ttime const domain = x1 - x0;
	Tvalue const range = y1 - y0;
	Ttime const t = (x - x0) / domain;
	Tvalue const a = vel0 * (domain)-range;
	Tvalue const b = vel1 * (domain)+range;

	Ttime it = Ttime(1) - t;

	Tvalue const ret = it * y0
		+ t * y1
		+ (t * it) * ((a * it) + (b * t));

	return ret;
}

//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Cubic(Tvalue const &y0, Tvalue const &y1,
	Ttime const &t)
{
	Tvalue const range = y1 - y0;
	Tvalue const a = -range;
	Tvalue const b = range;

	Ttime const it = Ttime(1) - t;

	Tvalue const ret = it * y0
		+ t * y1
		+ (t * it) * ((a * it) + (b * t));

	return ret;
}