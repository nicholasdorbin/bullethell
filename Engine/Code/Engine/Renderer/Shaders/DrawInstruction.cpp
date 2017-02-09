#include "Engine/Renderer/Shaders/DrawInstruction.hpp"


//-------------------------------------------------------------------------------------------------
DrawInstruction::DrawInstruction(bool useIndexBuffer /*= false*/, unsigned int primitiveType /*= 0x0004*/, unsigned int start /*= 0*/, unsigned int count /*= 0 */)
	: m_primitiveType(primitiveType)
	, m_startIndex(start)
	, m_count(count)
	, m_useIndexBuffer(useIndexBuffer)
{
}