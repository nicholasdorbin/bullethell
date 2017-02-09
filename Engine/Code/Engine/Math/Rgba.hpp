#pragma once
#include <string>
class Vector4;
class Rgba
{
public:
	static const Rgba CLEAR;
	static const Rgba WHITE;
	static const Rgba BLACK;
	static const Rgba RED;
	static const Rgba ORANGE_RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba PINK;
	static const Rgba YELLOW;
	static const Rgba PURPLE;
	static const Rgba LTGRAY;
	static const Rgba LTBLUE;

	Rgba();
	Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	Rgba(Vector4 color);
	Rgba(std::string colorStr);
	unsigned char m_red;
	unsigned char m_green;
	unsigned char m_blue;
	unsigned char m_alpha;

	const Rgba operator*(const float& scalar) const;
	const Rgba Rgba::operator+(const Rgba other) const;

	void Rgba::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	Vector4 Rgba::FloatRepresentation() const;

	float GetRFloat() const;
	float GetGFloat() const;
	float GetBFloat() const;
	float GetAFloat() const;

	void SetRFloat(float val);
	void SetGFloat(float val);
	void SetBFloat(float val);
	void SetAFloat(float val);

	friend bool operator==(const Rgba& lhs, const Rgba& rhs);
	friend bool operator!=(const Rgba& lhs, const Rgba& rhs);
	friend bool operator<(const Rgba& lhs, const Rgba& rhs);
};