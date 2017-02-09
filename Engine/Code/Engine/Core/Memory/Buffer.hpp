#pragma once



struct Buffer
{
	unsigned char* m_buffer;
	size_t m_offsetFromStart;
	size_t m_TotalSize;
	//T Alloc<T>();
	unsigned char* GetFront() { return (m_buffer + m_offsetFromStart); }

};