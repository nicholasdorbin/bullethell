#pragma once


//-------------------------------------------------------------------------------------------------
struct DrawInstruction
{
public:
	bool m_useIndexBuffer;
	unsigned int m_primitiveType;
	unsigned int m_startIndex;
	unsigned int m_count;

public:
	DrawInstruction(bool useIndexBuffer = false, unsigned int primitiveType = 0x0004, unsigned int start = 0, unsigned int count = 0);
	//0x0004 = GL_TRIANGLES
};