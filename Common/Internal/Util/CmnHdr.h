#pragma once

#define BEGIN_MSG	\
	void ( theClass::*pfn )( theParameters );
#define	USES_PFNENTRIES	\
	public:	\
	map < DWORD, void (theClass::*)( theParameters ) >	m_pfnEntries;	\
	map < DWORD, void (theClass::*)( theParameters ) >::iterator m_ItorEntries; \
	pair< DWORD, void (theClass::*)( theParameters ) > m_Pair; \
	void ( theClass::*GetHandler( DWORD dwType ) )( theParameters )	\
	{	\
		void ( theClass::*pfn )( theParameters );	\
		m_ItorEntries = m_pfnEntries.find( dwType );	\
		if( m_ItorEntries != m_pfnEntries.end() )	\
		{	\
			return m_ItorEntries->second;	\
		}	\
		return NULL;	\
	}
#define	ON_MSG( dwKey, hndlr )	\
	pfn		= hndlr;	\
	m_pfnEntries.insert( pair< DWORD, void (theClass::*)( theParameters ) >( dwKey, pfn ) );


#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if (x) { delete x; x = NULL; } }
#endif


enum
{
	PN_JHCSRVR = 12000,
};

enum
{ 
	MAX_SESSION = 10,
};

enum
{
	TO_ME= -1,
	TO_ALL= 0
};