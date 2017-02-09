#include "Engine/MathUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include <cstdlib>
#include <limits>

float DegToRad(float deg)
{
	return (deg * (pi/180.f));
}

float RadToDeg(float rad)
{
	return (rad * (180.f / pi));
}

float SinDegrees(float deg)
{
	float radians = DegToRad(deg);
	return (float)sin(radians);
}

float CosDegrees(float deg)
{
	float radians = DegToRad(deg);
	return (float)cos(radians);
}


float CalcSquaredDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2)
{
	float xDist = pos2.x - pos1.x;
	float yDist = pos2.y - pos1.y;
	return(xDist*xDist) + (yDist*yDist);
}

float CalcDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2)
{
	float xDist = pos2.x - pos1.x;
	float yDist = pos2.y - pos1.y;

	return (float)sqrt((xDist*xDist) + (yDist*yDist));
}

// Vector3 CalcPointAtDistance(const Vector3& pos1, const EulerAngles& angle, const float& dist)
// {
// 	//return dist * angle + pos;
// }

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	float inputDiff = inValue - inStart;
	float inputRange = inEnd - inStart;
	float outputRange = outEnd - outStart;

	float result = outStart + ((inputDiff / inputRange) * outputRange);
	return result;
}

int ClampInt(int inValue, int low, int high)
{
	if (inValue < low)
		return low;
	else if (inValue > high)
		return high;
	else
		return inValue;
}

float ClampFloat(float inValue, float low, float high)
{
	if (inValue < low)
		return low;
	else if (inValue > high)
		return high;
	else
		return inValue;
}

float ClampFloatCircular(float inValue, float low, float high)
{
	float result = inValue;
	float delta = high - low;

	while (result < low)
	{
		result += delta;
	}

	while (result > high)
	{
		result -= delta;
	}

	return result;
}

float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees)
{
	float angularDisplacement = toDegrees - fromDegrees;
	while (angularDisplacement > 180.f)
		angularDisplacement -= 360.f;
	while (angularDisplacement < -180.f)
		angularDisplacement += 360.f;
	return angularDisplacement;
}

Vector2 CalcDirToPoint(const Vector2& from, const Vector2& to)
{
	Vector2 result = to - from;
	result.SetNormalized();
	return result;
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y);
	return result;
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z);
	return result;
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
	return result;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	return result;
}

float SmoothStep(float inputZeroToOne)
{
	float inputSquared = (inputZeroToOne * inputZeroToOne);
	return (3.f * inputSquared) - (2.f * inputSquared * inputZeroToOne);
}

float ClampUpToZero(float value)
{
	float result = value;
	if (result < 0.f)
	{
		result = 0.f;
	}
	return result;
}

unsigned char FloatToUChar(float input)
{
	float result = input;
	result = RangeMap((float)result, 0.f, 1.f, 0.f, 255.f);
	return (unsigned char)result;
}

bool IsColinear(const Vector3& a, const Vector3& b)
{
	float dot = DotProduct(a, b);

	float mag = a.GetMagnitude() * b.GetMagnitude();

	if (dot == mag)
	{
		return true;
	}
	return false;
}

int RandomInt(int HI /*= 0*/, int LO /*= 0*/)
{
	//NOTE: HI IS NOT INCLUSIVE
	return (rand() % (HI - LO)) + LO;
}

uint32_t GetNuonce()
{
	return (uint32_t)rand();
}

float GetRandomFloatBetween(float low, float high)
{
	return low + (rand() / (RAND_MAX / (high - low)));
}

bool GetRandomTrueOrFalseWithinProbability(float probabilityTrue) //input a value between 0 and 1
{
	return GetRandomFloatBetween(0.f, 1.f) <= probabilityTrue;
}

bool DoDiscsOverlap(const Vector2& center1, float radius1, const Vector2& center2, float radius2)
{
	float distSquared = CalcSquaredDistanceBetweenPoints(center1, center2);

	float radii = radius1 + radius2;
	return(distSquared < (radii*radii));
}

bool IsPointInDisc(const Vector2& center1, float radius1, const Vector2& center2)
{
	float distSquared = CalcSquaredDistanceBetweenPoints(center1, center2);
	return (distSquared < (radius1 * radius1));
}


bool IsPointInBounds(const Vector2& point, const AABB2& bounds)
{
	Vector2 mins = bounds.m_mins;
	Vector2 maxs = bounds.m_maxs;

	if (mins.x < point.x && maxs.x > point.x)
	{
		if (mins.y < point.y && maxs.y > point.y)
		{
			return true;
		}
	}

	return false;
}

Vector3 Slerp(Vector3 start, Vector3 end, float percent)
{
	// Dot product - the cosine of the angle between 2 vectors.
	float dot = DotProduct(start, end);
	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = ClampFloat(dot, -1.0f, 1.0f);
	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = acosf(dot)*percent;
	Vector3 RelativeVec = end - start*dot;
	RelativeVec.SetNormalized();     // Orthonormal basis
								 // The final result.
	return ((start*cos(theta)) + (RelativeVec*sin(theta)));
}

//------------------------------------------------------------------------
Vector3 SlerpForseth(Vector3 const &a, Vector3 const &b, float t)
{
	float al = a.GetMagnitude();
	float bl = b.GetMagnitude();

	float len = Lerp<float>(al, bl, t);
	Vector3 u = SlerpUnit(a / al, b / bl, t);
	return len * u;
}

//------------------------------------------------------------------------
Vector3 SlerpUnit(Vector3 const &a, Vector3 const &b, float t)
{
	float cosangle = ClampFloat(DotProduct(a, b), -1.0f, 1.0f);
	float angle = acosf(cosangle); 
	if (angle < std::numeric_limits<float>::epsilon()) {
		return Lerp(a, b, t);
	}
	else {
		float pos_num = sinf(t * angle);
		float neg_num = sinf((1.0f - t) * angle);
		float den = sinf(angle);

		return (neg_num / den) * a + (pos_num / den) * b;
	}
}

Quaternion SlerpQuats(const Quaternion &quatA, const Quaternion &quatB, float t)
{
	//Assuming t is from 0->1
	//We first need the Cos of the angle between the two Quats
	float theta = quatA.DotProduct(quatB);
	Quaternion a = quatA;
	Quaternion b = quatB;
	;
	if (theta < 0.f) {
		b = -1.f * quatB;
		theta = -1.f * theta;
	}
	Quaternion intermediateQuaternion;
	if (theta > 0.9999f)
	{
		float k0 = 1.f - t;
		float k1 = t;

		intermediateQuaternion = (a * k0) + (b * k1);
	}
	else
	{
		theta = acos(theta);

		intermediateQuaternion = (a * sin((1.f - t) * theta) + b * sin(t * theta));
		intermediateQuaternion = intermediateQuaternion * (1 / sin(theta));
	}
	

	//intermediateQuaternion.ToRotationMatrix(m);

	return intermediateQuaternion;
}

Vector3 HermiteSpline(const Vector3& startPosition, const Vector3& endPosition, const Vector3& startVelocity, const Vector3& endVelocity, const float& percentComplete)
{

	//Calculate the Hermite basis

	float t = percentComplete;
	float t2 = t*t;
	float t3 = t * t2;


	float H0 = 1 - (3 * t2) + (2 * t3);
	float H1 = t - (2 * t2) + t3;
	float H2 = -t2 + t3;
	float H3 =  (3 * t2) - (2 * t3);


	//Combine the hermite basis with the components of the curve
	//p(t) = H0(t)p0 + H1(t)v0 + H2(t)v1 + H3(t)p1

	Vector3 result = H0*startPosition + H1*startVelocity + H2*endVelocity + H3*endPosition;

	return result;
}

Vector3 GetCatmullRomVelocity(const Vector3& thisPoint, const Vector3& prevPoint, const Vector3& nextPoint)
{
	Vector3 a = nextPoint - thisPoint;
	Vector3 b = thisPoint - prevPoint;

	Vector3 result = (a + b) / 2.f;

	return result;
}

EulerAngles GetLookAtAngle(const Vector3  &start, const Vector3&  posToLookAt)
{
	Vector3 forwardVector = Vector3(posToLookAt - start);
	forwardVector.SetNormalized();

	Vector3 rightVec = CrossProduct(forwardVector, Vector3::UP);


	rightVec.SetNormalized();

	Vector3 upVec = CrossProduct(rightVec, forwardVector);
	upVec.SetNormalized();

	Matrix4 mat;
	mat.SetBasis(rightVec, forwardVector, upVec);

	EulerAngles retAngles = mat.GetEulerAngles();
	if (start.x >= 0)
	{
		retAngles.NegateYaw();
	}

	return retAngles;
}



Rgba Lerp(Rgba a, Rgba b, float percent)
{
	Vector4 vecA = a.FloatRepresentation();
	Vector4 vecB = b.FloatRepresentation();

	Vector4 lerpVec = Lerp(vecA, vecB, percent);

	return Rgba(lerpVec);
}
