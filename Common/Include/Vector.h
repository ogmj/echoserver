#pragma once

#include "concurrent_vector.h"
#include "tbb_allocator.h"

using namespace tbb;
template< typename T >
class CVector
{
public:
	typedef internal::vector_iterator< CVector, T >  iterator;
	typedef internal::vector_iterator< CVector, const T >  const_iterator;

	CVector() 
	{
		Clear();
	};
	virtual ~CVector() {};

	inline void PushBack( T & type )
	{
		m_Vector.push_back( type );
	}

	inline size_t Size() const
	{
		return m_Vector.size();
	}
	inline T At( int index ) const
	{
		return m_Vector.at( index );
	}
	inline T Front() const
	{
		return m_Vector.front();
	}

	inline T Back() const
	{
		return m_Vector.back();
	}

	inline iterator Begin()
	{
		return m_Vector.begin();
	}

	inline iterator End()
	{
		return m_Vector.end();
	}
	inline void Clear()
	{
		m_Vector.clear();
	}
	inline bool Empty()
	{
		return m_Vector.empty();
	}
private:
	tbb::concurrent_vector< T > m_Vector;
};