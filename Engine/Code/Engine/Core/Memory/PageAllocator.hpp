#pragma once


struct Page
{
	Page* next;
};

class PageAllocator
{
	PageAllocator(size_t pageSize, size_t initialCount);

	void* Alloc(size_t size);
	void Free(void* data);

	size_t m_pageSize;
	Page* m_freeList;
};