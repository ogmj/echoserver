#pragma once

#include "concurrent_queue.h"

using namespace tbb;

template< typename T >
class CQueue
{
public:
	CQueue() 
	{
		Clear();
	};
	virtual ~CQueue() {};

	inline bool Pop( T & type )
	{
		return m_Queue.try_pop( type );
	}

	inline void GetAllocate()
	{
		 m_Queue.get_allocator();
	}

	inline void Push( T type)
	{
		m_Queue.push( type );
	}

	inline bool	Empty() const
	{
		return m_Queue.empty();
	}

	inline size_t Size() const
	{
		return m_Queue.size();
	}

	inline void Clear()
	{
		m_Queue.clear();
	}

private:
	tbb::concurrent_bounded_queue< T > m_Queue;
};