#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <utility>

#define STATIC
STATIC const Matrix4 Matrix4::BASIS(Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 0.f, 1.f));
STATIC const Matrix4 Matrix4::IDENTITY(Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(0.f, 0.f, 0.f, 1.f));


//-----------------------------------------------------------------------------------------------------------------------------------------
//STRUCTORS
//-----------------------------------------------------------------------------------------------------------------------------------------
Matrix4::Matrix4() {
    matrix[0] = 1.f;   matrix[1] = 0.f;   matrix[2] = 0.f;   matrix[3] = 0.f;
    matrix[4] = 0.f;   matrix[5] = 1.f;   matrix[6] = 0.f;   matrix[7] = 0.f;
    matrix[8] = 0.f;   matrix[9] = 0.f;   matrix[10] = 1.f;   matrix[11] = 0.f;
    matrix[12] = 0.f;   matrix[13] = 0.f;   matrix[14] = 0.f;   matrix[15] = 1.f;
}
Matrix4::Matrix4(Vector4 r1, Vector4 r2, Vector4 r3, Vector4 r4) {
    matrix[0] = r1.x;   matrix[1] = r1.y;   matrix[2] = r1.z;  matrix[3] = r1.w;
    matrix[4] = r2.x;   matrix[5] = r2.y;   matrix[6] = r2.z;  matrix[7] = r2.w;
    matrix[8] = r3.x;   matrix[9] = r3.y;   matrix[10] = r3.z;  matrix[11] = r3.w;
    matrix[12] = r4.x;   matrix[13] = r4.y;   matrix[14] = r4.z;  matrix[15] = r4.w;
}
Matrix4::Matrix4(float arr[16]) {
    matrix[0] = arr[0];   matrix[1] = arr[1];   matrix[2] = arr[2];   matrix[3] = arr[3];
    matrix[4] = arr[4];   matrix[5] = arr[5];   matrix[6] = arr[6];   matrix[7] = arr[7];
    matrix[8] = arr[8];   matrix[9] = arr[9];   matrix[10] = arr[10];  matrix[11] = arr[11];
    matrix[12] = arr[12];  matrix[13] = arr[13];  matrix[14] = arr[14];  matrix[15] = arr[15];
}

STATIC Matrix4 Matrix4::MakeScaleMatrix(float scale) {
    Matrix4 scaleMatrix;

    scaleMatrix.matrix[0] = scale;    scaleMatrix.matrix[1] = 0.f;      scaleMatrix.matrix[2] = 0.f;      scaleMatrix.matrix[3] = 0.f;
    scaleMatrix.matrix[4] = 0.f;      scaleMatrix.matrix[5] = scale;    scaleMatrix.matrix[6] = 0.f;      scaleMatrix.matrix[7] = 0.f;
    scaleMatrix.matrix[8] = 0.f;      scaleMatrix.matrix[9] = 0.f;      scaleMatrix.matrix[10] = scale;   scaleMatrix.matrix[11] = 0.f;
    scaleMatrix.matrix[12] = 0.f;     scaleMatrix.matrix[13] = 0.f;     scaleMatrix.matrix[14] = 0.f;     scaleMatrix.matrix[15] = 1.f;

    return scaleMatrix;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//MATH
//-----------------------------------------------------------------------------------------------------------------------------------------
void Matrix4::Transpose() {
    std::swap(matrix[1], matrix[4]);
    std::swap(matrix[2], matrix[8]);
    std::swap(matrix[3], matrix[12]);
    std::swap(matrix[6], matrix[9]);
    std::swap(matrix[7], matrix[13]);
    std::swap(matrix[11], matrix[14]);
}
void Matrix4::ChangeToProjectionMatrix(float xNearClip, float xFarClip, float yNearClip, float yFarClip, float zNearClip, float zFarClip) {
    float sx = 1.0f / (xFarClip - xNearClip);
    float sy = 1.0f / (yFarClip - yNearClip);
    float sz = 1.0f / (zFarClip - zNearClip);

    matrix[0] = 2.0f * sx;  matrix[1] = 0.f;       matrix[2] = 0.f;        matrix[3] = -(xFarClip + xNearClip) * sx;
    matrix[4] = 0.f;        matrix[5] = 2.0f * sy; matrix[6] = 0.f;        matrix[7] = -(yFarClip + yNearClip) * sy;
    matrix[8] = 0.f;        matrix[9] = 0.f;       matrix[10] = 2.0f * sz; matrix[11] = -(zFarClip + zNearClip) * sz;
    matrix[12] = 0.f;       matrix[13] = 0.f;      matrix[14] = 0.f;       matrix[15] = 1.f;

}
void Matrix4::ChangeToProjectionMatrix(float screenWidth, float screenHeight, float nearClip, float farClip) {
    float sz = 1.0f / (farClip - nearClip);

    matrix[0] = 2.f / screenWidth;  matrix[1] = 0.f;                matrix[2] = 0.f;        matrix[3] = 0.f;
    matrix[4] = 0.f;                matrix[5] = 2.f / screenHeight; matrix[6] = 0.f;        matrix[7] = 0.f;
    matrix[8] = 0.f;                matrix[9] = 0.f;                matrix[10] = 2.f * sz;  matrix[11] = -(farClip + nearClip) * sz;
    matrix[12] = 0.f;               matrix[13] = 0.f;               matrix[14] = 0.f;       matrix[15] = 1.f;
}

void Matrix4::ChangeToPerspectiveProjection(float yFovDegrees, float aspect, float nearClippingPlane, float farClippingPlane) {
    float rads = DegToRad(yFovDegrees);
    float size = 1.f / ((float)tan(rads / 2.f));
    //float size = atan(rads / 2.0f);

    float w = size;
    float h = size;

    if (aspect > 1.0f) {
        w /= aspect;
    }
    else {
        h *= aspect;
    }

    //Swapped the above * and /

    //Made W = 1/W
    //Made H = 1/H
    float q = 1.f / (farClippingPlane - nearClippingPlane);
    float bottomLeftVal1 = (farClippingPlane + nearClippingPlane) * q;
    float bottomLeftVal2 = -2.f * nearClippingPlane * farClippingPlane * q;

    matrix[0] = w;            matrix[1] = 0.f;                matrix[2] = 0.f;              matrix[3] = 0.f;
    matrix[4] = 0.f;                matrix[5] = h;            matrix[6] = 0.f;              matrix[7] = 0.f;
    matrix[8] = 0.f;                matrix[9] = 0.f;                matrix[10] = bottomLeftVal1;  matrix[11] = 1.f; //Flipped this with bottomLeftVal2 with this for row major
    matrix[12] = 0.f;               matrix[13] = 0.f;               matrix[14] = bottomLeftVal2;  matrix[15] = 0.f;
}
void Matrix4::ChangeToTranslationMatrix(Vector3 position) {
    matrix[12] = position.x;
    matrix[13] = position.y;
    matrix[14] = position.z;
}
void Matrix4::ChangeToLookAtMatrix(Vector3 posToLookAt, Vector3 currPos) {
    Vector3 dir = posToLookAt - currPos;
    dir.SetNormalized();

    Vector3 right = CrossProduct(dir, Vector3::UP);
    Vector3 up = CrossProduct(right, dir);
    Vector3 forward = dir;

    matrix[0] = right.x;        matrix[1] = right.y;     matrix[2] = right.z;      matrix[3] = 0.f;
    matrix[4] = forward.x;      matrix[5] = forward.y;   matrix[6] = forward.z;    matrix[7] = 0.f;
    matrix[8] = up.x;           matrix[9] = up.y;        matrix[10] = up.z;        matrix[11] = 0.f;

}

void Matrix4::TransposeRotation() {
    std::swap(matrix[1], matrix[4]);
    std::swap(matrix[2], matrix[8]);
    std::swap(matrix[6], matrix[9]);
}

void Matrix4::InvertOrthonormal()
{
    //(R*T)^-1 = T^-1 * R^-1

    //Rotation

    Matrix4 matRotation = *this;

    //Clearing the sides
    matRotation.matrix[3] = 0.f;
    matRotation.matrix[7] = 0.f;
    matRotation.matrix[11] = 0.f;

    matRotation.matrix[12] = 0.f;
    matRotation.matrix[13] = 0.f;
    matRotation.matrix[14] = 0.f;
    matRotation.matrix[15] = 1.f;

    //Invert Rotation

    matRotation.TransposeRotation();

    //Get Translation and Invert
    Vector3 translation = this->GetPosition();
    translation = -translation;
    Matrix4 matTrans = Matrix4::IDENTITY;
    matTrans.ChangePosition(translation);

    //(R * T)^-1 = T^-1 * R^1
    Matrix4 newMat = matTrans * matRotation;

    memcpy(matrix, newMat.matrix, sizeof(newMat.matrix));
    //MatrixMultiply(this, &matTrans, &matRotation);
}

void Matrix4::MatrixMakeRotationEuler(float yaw, float pitch, float roll) {
    float sx = SinDegrees(pitch);
    float cx = CosDegrees(pitch);

    float sy = SinDegrees(roll);
    float cy = CosDegrees(roll);

    float sz = SinDegrees(yaw);
    float cz = CosDegrees(yaw);


    matrix[0] = cy * cz;
    matrix[1] = cz * sx * sy - cx * sz;
    matrix[2] = cx * cz * sy + sx * sz;
    matrix[3] = matrix[12];
 
    matrix[4] = cy * sz;
    matrix[5] = cx * cz + sx * sy * sz;
    matrix[6] = -cz * sx + cx * sy * sz;
    matrix[7] = matrix[13];
 
    matrix[8] = -sy;
    matrix[9] = cy * sx;
    matrix[10] = cx * cy;
    matrix[11] = matrix[14];
 
    matrix[12] = 0.f;
    matrix[13] = 0.f;
    matrix[14] = 0.f;
    matrix[15] = 1.f;

    //matrix[0] = cy*cz + sx*sy*sz;
    //matrix[1] = -cz*sy + cy*sx*sz;
    //matrix[2] = -cx*sz;
    //matrix[3] = 0.f;
    //
    //matrix[4] = cy*sz - cz*sx*sy;
    //matrix[5] = -sy*sz - cy*cz*sx;
    //matrix[6] = cx*cz;
    //matrix[7] = 0.f;
    //
    //matrix[8] = cx*sy;
    //matrix[9] = cx*cy;
    //matrix[10] = sx;
    //matrix[11] = 0.f;
    //
    //matrix[12] = 0.f;
    //matrix[13] = 0.f;
    //matrix[14] = 0.f;
    //matrix[15] = 1.f;


    //TODO: Transpose this by hand
    Transpose();
}

void Matrix4::MatrixMakeRotationEuler(float yaw, float pitch, float roll, Vector3 position)
{
    float sx = SinDegrees(pitch);
    float cx = CosDegrees(pitch);

    float sy = SinDegrees(roll);
    float cy = CosDegrees(roll);

    float sz = SinDegrees(yaw);
    float cz = CosDegrees(yaw);

    matrix[0] = cy*cz + sx*sy*sz;
    matrix[1] = -cz*sy + cy*sx*sz;
    matrix[2] = -cx*sz;
    matrix[3] = 0.f;

    matrix[4] = cy*sz - cz*sx*sy;
    matrix[5] = -sy*sz - cy*cz*sx;
    matrix[6] = cx*cz;
    matrix[7] = 0.f;

    matrix[8] = cx*sy;
    matrix[9] = cx*cy;
    matrix[10] = sx;
    matrix[11] = 0.f;

    matrix[12] = position.x;
    matrix[13] = position.y;
    matrix[14] = position.z;
    matrix[15] = 1.f;

    Transpose();
}

EulerAngles Matrix4::GetEulerAngles()
{
	EulerAngles angles;
	Transpose();

	/*

	//cos p = 0
	roll = 0
	sin roll = 0
	cos roll = 1



	float sx = SinDegrees(pitch);
	float cx = CosDegrees(pitch);

	float sy = SinDegrees(roll);
	float cy = CosDegrees(roll);

	float sz = SinDegrees(yaw);
	float cz = CosDegrees(yaw);


	matrix[0] = cy * cz;
	matrix[1] = cz * sx * sy - cx * sz;
	matrix[2] = cx * cz * sy + sx * sz;
	matrix[3] = matrix[12];

	matrix[4] = cy * sz;
	matrix[5] = cx * cz + sx * sy * sz;
	matrix[6] = -cz * sx + cx * sy * sz;
	matrix[7] = matrix[13];

	matrix[8] = -sy;
	matrix[9] = cy * sx;
	matrix[10] = cx * cy;
	matrix[11] = matrix[14];

	matrix[12] = 0.f;
	matrix[13] = 0.f;
	matrix[14] = 0.f;
	matrix[15] = 1.f;
	*/

	float yaw;
	float pitch;
	float roll;

	float sy = -matrix[8];

	roll = RadToDeg(asin(sy));

	pitch = RadToDeg(asin(matrix[9] / CosDegrees(roll)));

	float pitchCos = SinDegrees(pitch);
	if (pitchCos >= 0.9999f)
	{
		roll = 0.f;
		yaw = RadToDeg(atan2(-matrix[6] , matrix[2]));
	}
	else
	{
		float a = acos(matrix[0]);
		float b = CosDegrees(roll);

		yaw = RadToDeg(a / b);
	}

	

	if (isnan(yaw) || isnan(roll) || isnan(pitch))
	{
		ERROR_AND_DIE("Euler angle conversion returned a NAN.");
	}

	angles.m_pitchDegreesAboutX = pitch;
	angles.m_rollDegreesAboutY = roll;
	angles.m_yawDegreesAboutZ = yaw;

	Transpose();
	return angles;
}

void Matrix4::SetBasis(Vector3 right, Vector3 up, Vector3 forward) {
    matrix[0] = right.x;     matrix[1] = right.y;     matrix[2] = right.z;      matrix[3] = 0.f;
    matrix[4] = up.x;        matrix[5] = up.y;        matrix[6] = up.z;         matrix[7] = 0.f;
    matrix[8] = forward.x;   matrix[9] = forward.y;   matrix[10] = forward.z;   matrix[11] = 0.f;
}


void Matrix4::ChangePosition(Vector3 pos) {
    matrix[12] = pos.x;
    matrix[13] = pos.y;
    matrix[14] = pos.z;
}

void Matrix4::ChangeToXForward() {

}

void Matrix4::NegateTranslation(Vector3 position) {
    Vector3 nPos = -1.f * position;

    Vector3 col1 = Vector3(matrix[0], matrix[4], matrix[8]);
    Vector3 col2 = Vector3(matrix[1], matrix[5], matrix[9]);
    Vector3 col3 = Vector3(matrix[2], matrix[6], matrix[10]);
    Vector3 col4 = Vector3(matrix[3], matrix[7], matrix[11]);


    Vector3 translatedPos = Vector3(DotProduct(nPos, col1), DotProduct(nPos, col2), DotProduct(nPos, col3));

    matrix[12] = translatedPos.x;
    matrix[13] = translatedPos.y;
    matrix[14] = translatedPos.z;
}

Vector3 Matrix4::GetForward() const {
    return Vector3(matrix[8], matrix[9], matrix[10]);
}
void Matrix4::SetForward(const Vector3& forward) {
    matrix[8] = forward.x;
    matrix[9] = forward.y;
    matrix[10] = forward.z;
}

// Courtesy of Chris Forseth
Matrix4 Matrix4::Inverse() {

    Matrix4 inv;
    double det;
    int i;

    inv.matrix[0] = matrix[5] * matrix[10] * matrix[15] -
        matrix[5] * matrix[11] * matrix[14] -
        matrix[9] * matrix[6] * matrix[15] +
        matrix[9] * matrix[7] * matrix[14] +
        matrix[13] * matrix[6] * matrix[11] -
        matrix[13] * matrix[7] * matrix[10];

    inv.matrix[4] = -matrix[4] * matrix[10] * matrix[15] +
        matrix[4] * matrix[11] * matrix[14] +
        matrix[8] * matrix[6] * matrix[15] -
        matrix[8] * matrix[7] * matrix[14] -
        matrix[12] * matrix[6] * matrix[11] +
        matrix[12] * matrix[7] * matrix[10];

    inv.matrix[8] = matrix[4] * matrix[9] * matrix[15] -
        matrix[4] * matrix[11] * matrix[13] -
        matrix[8] * matrix[5] * matrix[15] +
        matrix[8] * matrix[7] * matrix[13] +
        matrix[12] * matrix[5] * matrix[11] -
        matrix[12] * matrix[7] * matrix[9];

    inv.matrix[12] = -matrix[4] * matrix[9] * matrix[14] +
        matrix[4] * matrix[10] * matrix[13] +
        matrix[8] * matrix[5] * matrix[14] -
        matrix[8] * matrix[6] * matrix[13] -
        matrix[12] * matrix[5] * matrix[10] +
        matrix[12] * matrix[6] * matrix[9];

    inv.matrix[1] = -matrix[1] * matrix[10] * matrix[15] +
        matrix[1] * matrix[11] * matrix[14] +
        matrix[9] * matrix[2] * matrix[15] -
        matrix[9] * matrix[3] * matrix[14] -
        matrix[13] * matrix[2] * matrix[11] +
        matrix[13] * matrix[3] * matrix[10];

    inv.matrix[5] = matrix[0] * matrix[10] * matrix[15] -
        matrix[0] * matrix[11] * matrix[14] -
        matrix[8] * matrix[2] * matrix[15] +
        matrix[8] * matrix[3] * matrix[14] +
        matrix[12] * matrix[2] * matrix[11] -
        matrix[12] * matrix[3] * matrix[10];

    inv.matrix[9] = -matrix[0] * matrix[9] * matrix[15] +
        matrix[0] * matrix[11] * matrix[13] +
        matrix[8] * matrix[1] * matrix[15] -
        matrix[8] * matrix[3] * matrix[13] -
        matrix[12] * matrix[1] * matrix[11] +
        matrix[12] * matrix[3] * matrix[9];

    inv.matrix[13] = matrix[0] * matrix[9] * matrix[14] -
        matrix[0] * matrix[10] * matrix[13] -
        matrix[8] * matrix[1] * matrix[14] +
        matrix[8] * matrix[2] * matrix[13] +
        matrix[12] * matrix[1] * matrix[10] -
        matrix[12] * matrix[2] * matrix[9];

    inv.matrix[2] = matrix[1] * matrix[6] * matrix[15] -
        matrix[1] * matrix[7] * matrix[14] -
        matrix[5] * matrix[2] * matrix[15] +
        matrix[5] * matrix[3] * matrix[14] +
        matrix[13] * matrix[2] * matrix[7] -
        matrix[13] * matrix[3] * matrix[6];

    inv.matrix[6] = -matrix[0] * matrix[6] * matrix[15] +
        matrix[0] * matrix[7] * matrix[14] +
        matrix[4] * matrix[2] * matrix[15] -
        matrix[4] * matrix[3] * matrix[14] -
        matrix[12] * matrix[2] * matrix[7] +
        matrix[12] * matrix[3] * matrix[6];

    inv.matrix[10] = matrix[0] * matrix[5] * matrix[15] -
        matrix[0] * matrix[7] * matrix[13] -
        matrix[4] * matrix[1] * matrix[15] +
        matrix[4] * matrix[3] * matrix[13] +
        matrix[12] * matrix[1] * matrix[7] -
        matrix[12] * matrix[3] * matrix[5];

    inv.matrix[14] = -matrix[0] * matrix[5] * matrix[14] +
        matrix[0] * matrix[6] * matrix[13] +
        matrix[4] * matrix[1] * matrix[14] -
        matrix[4] * matrix[2] * matrix[13] -
        matrix[12] * matrix[1] * matrix[6] +
        matrix[12] * matrix[2] * matrix[5];

    inv.matrix[3] = -matrix[1] * matrix[6] * matrix[11] +
        matrix[1] * matrix[7] * matrix[10] +
        matrix[5] * matrix[2] * matrix[11] -
        matrix[5] * matrix[3] * matrix[10] -
        matrix[9] * matrix[2] * matrix[7] +
        matrix[9] * matrix[3] * matrix[6];

    inv.matrix[7] = matrix[0] * matrix[6] * matrix[11] -
        matrix[0] * matrix[7] * matrix[10] -
        matrix[4] * matrix[2] * matrix[11] +
        matrix[4] * matrix[3] * matrix[10] +
        matrix[8] * matrix[2] * matrix[7] -
        matrix[8] * matrix[3] * matrix[6];

    inv.matrix[11] = -matrix[0] * matrix[5] * matrix[11] +
        matrix[0] * matrix[7] * matrix[9] +
        matrix[4] * matrix[1] * matrix[11] -
        matrix[4] * matrix[3] * matrix[9] -
        matrix[8] * matrix[1] * matrix[7] +
        matrix[8] * matrix[3] * matrix[5];

    inv.matrix[15] = matrix[0] * matrix[5] * matrix[10] -
        matrix[0] * matrix[6] * matrix[9] -
        matrix[4] * matrix[1] * matrix[10] +
        matrix[4] * matrix[2] * matrix[9] +
        matrix[8] * matrix[1] * matrix[6] -
        matrix[8] * matrix[2] * matrix[5];

    det = matrix[0] * inv.matrix[0] + matrix[1] * inv.matrix[4] + matrix[2] * inv.matrix[8] + matrix[3] * inv.matrix[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    Matrix4 invOut;

    for (i = 0; i < 16; i++) {
        invOut.matrix[i] = inv.matrix[i] * (float)det;
    }

    return invOut;
}

void Matrix4::Scale(const Vector3& scale) {
    matrix[0] *= scale.x;
    matrix[5] *= scale.y;
    matrix[10] *= scale.z;
}

void Matrix4::GetBasis(Vector3* r, Vector3* u, Vector3* f, Vector3* t) {
    *r = Vector3(matrix[0], matrix[1], matrix[2]);
    *u = Vector3(matrix[4], matrix[5], matrix[6]);
    *f = Vector3(matrix[8], matrix[9], matrix[10]);
    *t = Vector3(matrix[12], matrix[13], matrix[14]);
}

STATIC Matrix4 Matrix4::MatrixLerp(Matrix4 a, Matrix4 b, float time) {
    Vector3 r0, u0, f0, t0;
    a.GetBasis(&r0, &u0, &f0, &t0);

    Vector3 r1, u1, f1, t1;
    b.GetBasis(&r1, &u1, &f1, &t1);

    Vector3 r, u, f, t;
    r = Slerp(r0, r1, time);
    u = Slerp(u0, u1, time);
    f = Slerp(f0, f1, time);
    t = Lerp(t0, t1, time);

    Vector4 r4 = Vector4(r.x, r.y, r.z, 0.f);
    Vector4 u4 = Vector4(u.x, u.y, u.z, 0.f);
    Vector4 f4 = Vector4(f.x, f.y, f.z, 0.f);
    Vector4 t4 = Vector4(t.x, t.y, t.z, 1.f);

    return Matrix4(r4, u4, f4, t4);
}

Vector4 Matrix4::MultiplyRight(Vector4 other)
{
	Vector4 result = Vector4::ZERO;

	result.x = matrix[0] * other.x + matrix[1] * other.y + matrix[2] * other.z + matrix[3] * other.w;
	result.y = matrix[4] * other.x + matrix[5] * other.y + matrix[6] * other.z + matrix[7] * other.w;
	result.z = matrix[8] * other.x + matrix[9] * other.y + matrix[10] * other.z + matrix[11] * other.w;
	result.w = matrix[12] * other.x + matrix[13] * other.y + matrix[14] * other.z + matrix[15] * other.w;

	return result;
}
