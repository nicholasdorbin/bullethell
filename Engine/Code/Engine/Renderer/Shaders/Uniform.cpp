#include "Engine/Renderer/Shaders/Uniform.hpp"


//-------------------------------------------------------------------------------------------------
Uniform::Uniform(int bindPoint, int length, int size, unsigned int type, std::string const &name, void* const data)
	: m_bindPoint(bindPoint)
	, m_length(length)
	, m_size(size)
	, m_type(type)
	, m_name(name)
	, m_data(data)
{
}


//-------------------------------------------------------------------------------------------------
Uniform::~Uniform()
{
	delete m_data;
}
