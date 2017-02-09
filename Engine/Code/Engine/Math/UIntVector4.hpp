#pragma once
#include <math.h>


class UIntVector4
{
public:
	UIntVector4();
	UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW);
	UIntVector4(const UIntVector4& v);
	
	void SetXYZ(unsigned int newX, unsigned int newY, unsigned int newZ, unsigned int newW);
	unsigned int GetMagnitude() const;
	void SetNormalized();
	UIntVector4 GetNormalized() const;
	

	const UIntVector4 UIntVector4::operator+(const UIntVector4 other) const;
	const UIntVector4 UIntVector4::operator-(const UIntVector4 other) const;
	UIntVector4& UIntVector4::operator=(const UIntVector4 &rhs);
	UIntVector4& UIntVector4::operator+=(const UIntVector4 other);
	UIntVector4 UIntVector4::operator-=(const UIntVector4 other);
	UIntVector4 operator* (unsigned int scalar);
	const UIntVector4 operator*(const unsigned int& scalar) const;
	friend UIntVector4 operator*(unsigned int scalar, const UIntVector4& vec);
	const UIntVector4 operator/(const unsigned int& scalar) const;
	friend bool operator==(const UIntVector4& lhs, const UIntVector4& rhs);
	friend bool operator!=(const UIntVector4& lhs, const UIntVector4& rhs);
	const UIntVector4 operator-() const;

	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;

	static UIntVector4 ZERO;
	static UIntVector4 ONE;
};

