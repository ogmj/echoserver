#pragma once
using namespace tbb;

typedef concurrent_hash_map < SOCKET, int > CMapToGetIndexBySocketMng;
typedef concurrent_hash_map < SOCKET, int >::iterator CMapToGetIndexBySocketMngIter;

class CToGetIndexBySocketMng
{
public:
	CToGetIndexBySocketMng();
	~CToGetIndexBySocketMng();

	bool Find( SOCKET s, int & nIndex );
	void Insert( SOCKET s, int nIndex );
	void Erase( SOCKET s );
	//void Clear();

private:
	CMapToGetIndexBySocketMng m_mapToGetIndexBySocketMng;

};