#pragma once

class CIOCPWorker
{
public:
	CIOCPWorker();
	virtual ~CIOCPWorker();

	bool Start( HANDLE hIOCP );
	bool Clean();
	HANDLE GetThreadHandle(){ return m_hThreadHandle; }
	static unsigned int WINAPI Work( LPVOID param );
private:

	HANDLE m_hIOCP;
	HANDLE m_hThreadHandle;
	DWORD m_dwThreadId;
};