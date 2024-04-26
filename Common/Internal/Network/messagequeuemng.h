#pragma once

using namespace tbb;

class CMessageQueueMng
{
public:
	CMessageQueueMng(){}
	virtual ~CMessageQueueMng()
	{
		MESSAGE_INFO * p = nullptr;
		while( 1 )
		{   
			p = this->Pop();
			if( nullptr != p )
			{
				delete p;
			}
			else
			{
				break;
			}
		}
	}
/*
	LPMESSAGE_INFO Pop( MESSAGE_INFO * p )
	{
		m_queueMessageMng.Pop( p );
		return p;
	}
*/
	LPMESSAGE_INFO Pop()
	{
		MESSAGE_INFO * p = nullptr;
		m_queueMessageMng.Pop( p );
		return p;
	}

	void Push( MESSAGE_INFO * p )
	{
		m_queueMessageMng.Push( p );
	}

	size_t Size()
	{
		return m_queueMessageMng.Size();
	}
private:
	CQueue<MESSAGE_INFO*> m_queueMessageMng;
	
};