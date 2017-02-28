#include "Engine/Math/Rgba.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string>

const Rgba Rgba::WHITE(255, 255, 255, 255);

const Rgba Rgba::BLACK(0, 0, 0, 255);

const Rgba Rgba::RED(255, 0, 0, 255);

const Rgba Rgba::ORANGE_RED(255, 69, 0, 255);

const Rgba Rgba::GREEN(0, 255, 0, 255);

const Rgba Rgba::BLUE(0, 0, 255, 255);

const Rgba Rgba::CYAN(0, 255, 255, 255);

const Rgba Rgba::PINK(255, 0, 255, 255);

const Rgba Rgba::YELLOW(255, 255, 0, 255);

const Rgba Rgba::PURPLE(255, 0, 255, 255);

const Rgba Rgba::LTGRAY(190, 190, 190, 255);

const Rgba Rgba::LTBLUE(0, 191, 255, 255);

const Rgba Rgba::CLEAR(255, 255, 255, 0);



Rgba::Rgba()
{
	m_red = 0;
	m_green = 0;
	m_blue = 0;
	m_alpha = 0;
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_alpha = alpha;
}


Rgba::Rgba(Vector4 color)
{
	float red = ClampFloat(color.x, 0.f, 1.f);
	float green = ClampFloat(color.y, 0.f, 1.f);
	float blue = ClampFloat(color.z, 0.f, 1.f);
	float alpha = ClampFloat(color.w, 0.f, 1.f);

	SetRFloat(red);
	SetGFloat(green);
	SetBFloat(blue);
	SetAFloat(alpha);
}

Rgba::Rgba(std::string colorStr)
{
	std::string colorStrCopy = colorStr;

	bool usesFloats = false;

	size_t periodPos = colorStrCopy.find(".");
	if (periodPos != std::string::npos)
	{
		usesFloats = true;
	}

	size_t pos = colorStrCopy.find(",");
	std::string rStr = colorStrCopy.substr(0, pos);
	colorStrCopy.erase(0, pos + 1);

	pos = colorStrCopy.find(",");
	std::string gStr = colorStrCopy.substr(0, pos);
	colorStrCopy.erase(0, pos + 1);

	pos = colorStrCopy.find(",");
	std::string bStr = colorStrCopy.substr(0, pos);
	colorStrCopy.erase(0, pos + 1);


	std::string aStr = colorStrCopy;

	Vector4 vectorColor = Vector4(stof(rStr), stof(gStr), stof(bStr), stof(aStr));
	if (usesFloats)
	{
		float red = ClampFloat(vectorColor.x, 0.f, 1.f);
		float green = ClampFloat(vectorColor.y, 0.f, 1.f);
		float blue = ClampFloat(vectorColor.z, 0.f, 1.f);
		float alpha = ClampFloat(vectorColor.w, 0.f, 1.f);

		SetRFloat(red);
		SetGFloat(green);
		SetBFloat(blue);
		SetAFloat(alpha);
	}
	else
	{
		float red = ClampFloat(vectorColor.x, 0.f, 255.f);
		float green = ClampFloat(vectorColor.y, 0.f, 255.f);
		float blue = ClampFloat(vectorColor.z, 0.f, 255.f);
		float alpha = ClampFloat(vectorColor.w, 0.f, 255.f);

		m_red = (unsigned char)red;
		m_green = (unsigned char)green;
		m_blue = (unsigned char)blue;
		m_alpha = (unsigned char)alpha;
	}
	

	
}

float Rgba::GetRFloat() const
{
	return RangeMap((float)m_red, 0.f, 255.f, 0.f, 1.f);
}

float Rgba::GetGFloat() const
{
	return RangeMap((float)m_green, 0.f, 255.f, 0.f, 1.f);
}

float Rgba::GetBFloat() const
{
	return RangeMap((float)m_blue, 0.f, 255.f, 0.f, 1.f);
}

float Rgba::GetAFloat() const
{
	return RangeMap((float)m_alpha, 0.f, 255.f, 0.f, 1.f);
}

void Rgba::SetRFloat(float val)
{
	m_red = (unsigned char)RangeMap(val, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::SetGFloat(float val)
{
	m_green = (unsigned char)RangeMap(val, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::SetBFloat(float val)
{
	m_blue = (unsigned char)RangeMap(val, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::SetAFloat(float val)
{
	m_alpha = (unsigned char) RangeMap(val, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
	m_alpha = alpha;
}

Vector4 Rgba::FloatRepresentation() const
{
	float r = RangeMap((float)m_red, 0.f, 255.f, 0.f, 1.f);
	float g = RangeMap((float)m_green, 0.f, 255.f, 0.f, 1.f);
	float b = RangeMap((float)m_blue, 0.f, 255.f, 0.f, 1.f);
	float a = RangeMap((float)m_alpha, 0.f, 255.f, 0.f, 1.f);

	return Vector4(r, g, b, a);
}

bool operator==(const Rgba& lhs, const Rgba& rhs)
{
	if (lhs.m_red == rhs.m_red && 
		lhs.m_green == rhs.m_green &&
		lhs.m_blue == rhs.m_blue &&
		lhs.m_alpha == rhs.m_alpha)
		return true;
	return false;
}

const Rgba Rgba::operator+(const Rgba other) const
{
	Rgba result = *this;
	result.m_red = m_red + other.m_red;
	result.m_green = m_green + other.m_green;
	result.m_blue = m_blue + other.m_blue;
	result.m_alpha = m_alpha + other.m_alpha;

	return result;
}

const Rgba Rgba::operator*(const float& scalar) const
{
	Rgba result;

	float r = RangeMap((float)m_red * scalar, 0.f, 255.f, 0.f, 1.f);
	float g = RangeMap((float)m_green * scalar, 0.f, 255.f, 0.f, 1.f);
	float b = RangeMap((float)m_blue * scalar, 0.f, 255.f, 0.f, 1.f);
	float a = RangeMap((float)m_alpha * scalar, 0.f, 255.f, 0.f, 1.f);

	Vector4 outColor = Vector4(r, g, b, a);

	return Rgba(outColor);
}




//-----------------------------------------------------------------------------------------------
bool operator!=(const Rgba& lhs, const Rgba& rhs)
{
	if (lhs.m_red == rhs.m_red &&
		lhs.m_green == rhs.m_green &&
		lhs.m_blue == rhs.m_blue &&
		lhs.m_alpha == rhs.m_alpha)
	{
		return false;
	}

	return true;
}

bool operator<(const Rgba& lhs, const Rgba& rhs)
{
	if (lhs.m_alpha < rhs.m_alpha)
	{
		return true;
	}
	else if (lhs.m_alpha > rhs.m_alpha)
	{
		return false;
	}

	if (lhs.m_blue < rhs.m_blue)
	{
		return true;
	}
	else if (lhs.m_blue > rhs.m_blue)
	{
		return false;
	}

	if (lhs.m_green < rhs.m_green)
	{
		return true;
	}
	else if (lhs.m_green > rhs.m_green)
	{
		return false;
	}
	
	if (lhs.m_red < rhs.m_red)
	{
		return true;
	}
	else if (lhs.m_red > rhs.m_red)
	{
		return false;
	}


	return false;
}