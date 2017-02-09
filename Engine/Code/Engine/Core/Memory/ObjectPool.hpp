#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"

#pragma warning( disable : 4127 )  // conditional expression is constant

struct PageNode
{
	PageNode* next;
};

template <typename T>
class ObjectPool
{
private:
	T* m_freeStack;

	size_t m_countPerBlock;
	T* m_buffer;
public:

	ObjectPool(size_t const countPerBlock)
	{
		m_countPerBlock = countPerBlock;
		size_t buffer_size = sizeof(T) * m_countPerBlock;
		//This below can be a grow func
		ASSERT_OR_DIE(sizeof(T) >= sizeof(PageNode), "Object is bigger than buffer");
		m_buffer = (T*)malloc(buffer_size);
		m_freeStack = nullptr;
		for (int i = m_countPerBlock - 1; i >= 0; --i)
		{
			T* ptr = &m_buffer[i];
			PageNode* node = (PageNode*)ptr;
			node->next = (PageNode*)m_freeStack;
			m_freeStack = (T*)node;
		}
	}
	T* Alloc()
	{
		T* newData = (T*)m_freeStack;
		m_freeStack = (T*)(((PageNode*)m_freeStack)->next);

		new(newData) T();

		return newData;

	}
	void Delete(T* t)
	{
		t->~T();


		PageNode *n = (PageNode*)t;
		n->next = (PageNode*)m_freeStack;

		m_freeStack = (T*)n;
	}

	void Destroy()
	{
		free( m_buffer);
	}

	
	/*
	m_freeStack(page_node*)
	m_count_per_block

	m_buffers;
	*/


};