#pragma once

class CSessionIndexMng
{
public:
	CSessionIndexMng();
	~CSessionIndexMng();

	int Pop();
	void  Push( int dwIndex );

private:
	CQueue< int > m_QueueSessionIndexMng;
};
