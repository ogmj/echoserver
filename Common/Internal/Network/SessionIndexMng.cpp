#include  "stdafx.h"
#include  "SessionIndexMng.h"

CSessionIndexMng :: CSessionIndexMng(void)
{
}

CSessionIndexMng :: ~CSessionIndexMng(void)
{
}

int CSessionIndexMng::Pop()
{	
	int nIndex = -1;
	m_QueueSessionIndexMng.Pop( nIndex );
	return nIndex;
}

void CSessionIndexMng::Push( int nIndex )
{
	m_QueueSessionIndexMng.Push( nIndex );
}

//DWORD   CSessionPool::GetSize()
//{
//	return m_QueueSessionPool.size();
//}


