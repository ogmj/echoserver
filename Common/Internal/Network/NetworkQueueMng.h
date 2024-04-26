#pragma once

class CNetworkQueueMng
{
public:
	CNetworkQueueMng() {};
	~CNetworkQueueMng() {};

	bool Enqueue( DWORD dwLen, const char * src )
	{
		if ( NULL == src)
			return false;
		for( DWORD dw = 0; dw < dwLen; ++dw )
		{
			m_Queue.Push( *( src + dw ) );
		}
		m_QueueReadLen.Push( dwLen );
		return true;
	}

	bool Dequeue( DWORD dwLen, char * dest )
	{
		if( NULL == dest )
			return false;
		if( dwLen > 0 )
		{
			for( DWORD dw = 0; dw < dwLen; ++dw )
			{
				m_Queue.Pop( *( dest + dw ) );
			}
			return true;
		}
		return false;
	}

	bool ReadLen( DWORD & dwLen )
	{
		m_QueueReadLen.Pop( dwLen );
		if( dwLen > 0 )
		{
			return true;
		}
		return false;
	}

private:
	//by ogmj : 스택이 아니므로 데이터를 가져오면서, 동시에 들어가더라도 순서대로 나온다.
	CQueue< char > m_Queue;			//by ogmj: 실데이터 저장
	CQueue< DWORD > m_QueueReadLen;	//by ogmj: 데이터 길이 저장
};


class CTestNetworkQueueMng
{
public:
	CTestNetworkQueueMng() {};
	~CTestNetworkQueueMng() {};

	bool Enqueue( DWORD dwLen, const char * src )
	{
		if ( NULL == src)
			return false;
		for( DWORD dw = 0; dw < dwLen; ++dw )
		{
			m_Queue.push( *( src + dw ) );
		}
		m_QueueReadLen.push( dwLen );
		return true;
	}

	bool Dequeue( DWORD dwLen, char * dest )
	{
		if( NULL == dest )
			return false;
		if( dwLen > 0 )
		{
			for( DWORD dw = 0; dw < dwLen; ++dw )
			{
				*( dest + dw ) = m_Queue.front();
				m_Queue.pop();
			}
			return true;
		}
		return false;
	}

	bool ReadLen( DWORD & dwLen )
	{
		dwLen = m_QueueReadLen.front();
		m_QueueReadLen.pop();
		if( dwLen > 0 )
		{
			return true;
		}
		return false;
	}

private:
	//by ogmj : 스택이 아니므로 데이터를 가져오면서, 동시에 들어가더라도 순서대로 나온다.
	queue< char > m_Queue;			//by ogmj: 실데이터 저장
	queue< DWORD > m_QueueReadLen;	//by ogmj: 데이터 길이 저장
};
