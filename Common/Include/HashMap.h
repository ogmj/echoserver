#pragma once

#include "concurrent_hash_map.h"
// #include "tbb_allocator.h"

using namespace tbb;
template< typename _Key, typename T >
class CHashMap
{
public:
	typedef std::pair< const _Key, T > value_type;
	//typedef interface4::internal::hash_map_iterator< CHashMap, value_type > iterator;
	//typedef interface4::internal::hash_map_iterator< CHashMap, const value_type > const_iterator;
	typedef interface5::internal::hash_map_iterator< CHashMap, value_type > iterator;
	typedef interface5::internal::hash_map_iterator< CHashMap, const value_type > const_iterator;

	CHashMap() 
	{
		Clear();
	};
	virtual ~CHashMap() {};

	bool Insert( const _Key & key ,T & t )
	{
		bool bResult = false;
		tbb::concurrent_hash_map< _Key, T >::accessor accessor;
		bResult = m_HashMsp.insert( accessor, key );
		if( true == bResult )
		{
			accessor->second = t;
		}
		accessor.release();
		return bResult;
	}

	bool Find( const _Key & key, T & t )
	{
		bool bResult = false;
		tbb::concurrent_hash_map< _Key, T >::accessor accessor;
		bResult = m_HashMsp.find( accessor, key );
		if ( true == bResult )
		{
			t = accessor->second;
		}
		accessor.release();
		return bResult;
	}

	bool Const_Find( const _Key & key, T & t )
	{
		bool bResult = false;
		tbb::concurrent_hash_map< _Key, T >::const_accessor c_accessor;
		bResult = m_HashMsp.find( c_accessor, key );
		if ( true == bResult )
		{
			t = c_accessor->second;
		}
		c_accessor.release();
		return bResult;
	}

	//iterator Begin() const
	//{
	//	m_HashMsp.
	//}
	inline bool Erase( const _Key & key )
	{
		return m_HashMsp.erase( key );
	}

	inline size_t Size() const
	{
		return m_HashMsp.size();
	}
	inline bool Empty()
	{
		return m_HashMsp.empty();
	}
	inline void Clear()
	{
		m_HashMsp.clear();
	}
private:
	tbb::concurrent_hash_map< _Key, T > m_HashMsp;
};