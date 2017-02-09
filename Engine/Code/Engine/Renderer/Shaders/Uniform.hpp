#pragma once

#include <string>


//-------------------------------------------------------------------------------------------------
struct Uniform
{
public:
	int m_bindPoint;
	int m_length;
	int m_size;
	unsigned int m_type;
	std::string m_name;
	void* const m_data;

public:
	Uniform(int bindPoint, int length, int size, unsigned int type, std::string const &name, void* const data);
	~Uniform();

	int GetBindPoint() const { return m_bindPoint; }
};