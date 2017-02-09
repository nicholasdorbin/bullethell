#include "Engine/Core/Memory/PageAllocator.hpp"
#include <cstdlib>
typedef unsigned char byte_t;
PageAllocator::PageAllocator(size_t pageSize, size_t initialCount)
{
	size_t totalSize = pageSize * initialCount;

	byte_t* block = (byte_t*) malloc(totalSize);
	m_freeList = nullptr;

	for (size_t i = 0; i < initialCount; ++i)
	{
		byte_t* p = block + (pageSize) * i;
		Page* page = (Page*)p;
		page->next = m_freeList;
		m_freeList = page;
	}
}

void* PageAllocator::Alloc(size_t size)
{
	size;
	Page* p = m_freeList;
	m_freeList = m_freeList->next;
	return p;
}

void PageAllocator::Free(void* data)
{
	Page* temp = m_freeList;

	m_freeList = (Page*)data;
	m_freeList->next = temp;
}
