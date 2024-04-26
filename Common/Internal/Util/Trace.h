#pragma once

#define SelectFont(hdc, hfont)  ((HFONT)SelectObject((hdc), (HGDIOBJ)(HFONT)(hfont)))

enum { MAX_RECT_LINE = 10 };
static RECT g_Rect[ MAX_RECT_LINE ];

class CTraceData
{
public:
	CTraceData() {};
	virtual ~CTraceData() {};

	void PushBack( std::string str )
	{
		m_vTrace.PushBack( str );
	}

	std::string GetString( int nIdx ) const
	{
		return m_vTrace.At( nIdx );
	}

	int Size()
	{
		return static_cast< int >( m_vTrace.Size() );
	}
	bool Empty()
	{
		return m_vTrace.Empty();
	}

private:
	CVector< std::string > m_vTrace;
};

class CStateOfTrace
{
public:
	CStateOfTrace() {};
	virtual ~CStateOfTrace() {};
	virtual void Print( HWND hWnd, HDC hdc, RECT ClientRect, RECT InputRect, std::string str ) = 0;

	void Clear( const RECT & rect )
	{
		FillRect( m_BackHdc, &rect, m_Brush );
	}

protected:
	HDC m_hdc;
	HDC m_BackHdc;
	HBITMAP m_BackBitmap;
	HFONT m_OldFont;
	HFONT m_Font;
	HGDIOBJ m_OldObject;
	HBRUSH m_Brush;
};

class CNoTrace : public CStateOfTrace
{
public:
	CNoTrace () {};
	virtual ~CNoTrace() {};

	virtual void Print( HWND hWnd, HDC hdc, RECT ClientRect, RECT InputRect, std::string str )
	{
	}
};

class CPrintInInputLine : public CStateOfTrace
{
public:
	CPrintInInputLine () {};
	virtual ~CPrintInInputLine() {};

	virtual void Print( HWND hWnd, HDC hdc, RECT ClientRect, RECT InputRect, std::string str )
	{
		m_BackHdc = CreateCompatibleDC( hdc );
		m_BackBitmap = CreateCompatibleBitmap( hdc, 
			ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top );
		m_OldObject = SelectObject( m_BackHdc, m_BackBitmap );

		m_Font = CreateFont( 14, 7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "굴림체" );
		m_Brush = CreateSolidBrush( RGB( 255,255,255) );
		m_OldFont = SelectFont( m_BackHdc, m_Font );

		SetTextColor( m_BackHdc, RGB( 0, 0, 0 ) );
		SetBkMode( m_BackHdc, TRANSPARENT );

		Clear( ClientRect );

		//첫줄을 읽어서 입력 렉에 뿌려준다.
		TextOut( m_BackHdc, InputRect.left, InputRect.top, str.c_str(), static_cast< int >( str.length() ) );

		int nSize = CSingleton< CTraceData >::GetInstance()->Size();

		if( false == CSingleton< CTraceData >::GetInstance()->Empty() )
		{
			if( nSize > MAX_RECT_LINE )
			{
				nSize = nSize - MAX_RECT_LINE;
				for( int i = 0; i < MAX_RECT_LINE; ++i, ++nSize )
				{
					const std::string strTrace = CSingleton< CTraceData >::GetInstance()->GetString( nSize );
					TextOut( m_BackHdc, g_Rect[ i ].left, g_Rect[ i ].top, strTrace.c_str(), static_cast< int >( strTrace.length() ) );
				}

			}
			else
			{
				for( int i = 0; i < nSize; ++i )
				{
					const std::string strTrace = CSingleton< CTraceData >::GetInstance()->GetString( i );
					TextOut( m_BackHdc, g_Rect[ i ].left, g_Rect[ i ].top, strTrace.c_str(), static_cast< int >( strTrace.length() ) );
				}
			}
		}

		BitBlt( hdc, 0, 0, ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top, m_BackHdc, 0, 0, SRCCOPY );

		SelectObject( m_BackHdc, m_OldObject );
		SelectFont( m_BackHdc, m_OldFont );
		DeleteDC( m_BackHdc );
		DeleteObject( m_BackBitmap );
		DeleteObject( m_Brush );
		DeleteObject( m_Font );

		if( "" != str )
		{
			CSingleton< CTraceData >::GetInstance()->PushBack( str );
		}
	}
};

class CPrintInTraceLine : public CStateOfTrace
{
public:
	CPrintInTraceLine (){};
	virtual ~CPrintInTraceLine(){};

	virtual void Print( HWND hWnd, HDC hdc, RECT ClientRect, RECT InputRect, std::string str )
	{

		m_BackHdc = CreateCompatibleDC( hdc );
		m_BackBitmap = CreateCompatibleBitmap( hdc, 
			ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top );
		m_OldObject = SelectObject( m_BackHdc, m_BackBitmap );

		m_Font = CreateFont( 14, 7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "굴림체" );
		m_Brush = CreateSolidBrush( RGB( 255,255,255) );
		m_OldFont = SelectFont( m_BackHdc, m_Font );

		SetTextColor( m_BackHdc, RGB( 0, 0, 0 ) );
		SetBkMode( m_BackHdc, TRANSPARENT );

		Clear( ClientRect );

		//입력창 초기화
		TextOut( m_BackHdc, InputRect.left, InputRect.top, "", 0 );
		int nSize = CSingleton< CTraceData >::GetInstance()->Size();

		if( false == CSingleton< CTraceData >::GetInstance()->Empty() )
		{
			if( nSize > MAX_RECT_LINE )
			{
				nSize = nSize - MAX_RECT_LINE;
				for( int i = 0; i < MAX_RECT_LINE; ++i, ++nSize )
				{
					const std::string strTrace = CSingleton< CTraceData >::GetInstance()->GetString( nSize );
					TextOut( m_BackHdc, g_Rect[ i ].left, g_Rect[ i ].top, strTrace.c_str(), static_cast< int >( strTrace.length() ) );
				}

			}
			else
			{
				for( int i = 0; i < nSize; ++i )
				{
					const std::string strTrace = CSingleton< CTraceData >::GetInstance()->GetString( i );
					TextOut( m_BackHdc, g_Rect[ i ].left, g_Rect[ i ].top, strTrace.c_str(), static_cast< int >( strTrace.length() ) );
				}
			}
		}
		BitBlt( hdc, 0, 0, ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top, m_BackHdc, 0, 0, SRCCOPY );

		SelectObject( m_BackHdc, m_OldObject );
		SelectFont( m_BackHdc, m_OldFont );
		DeleteDC( m_BackHdc );
		DeleteObject( m_BackBitmap );
		DeleteObject( m_Brush );
		DeleteObject( m_Font );
	}
};

class CTrace
{
public:
	CTrace( int nWidth = 120, int nHeight = 25 ) : m_Inputstr(" ")
	{
		int nY = 0;
		for( int i = 0; i < MAX_RECT_LINE; ++i )
		{
			nY = 25 * i;
			SetRect( &g_Rect[ i ], 25, 25+nY, 400, 50+nY );
		}
		SetRect( &m_InputRect, 25, 50+nY, 400, 75+nY );
		m_hWnd = nullptr;
		//m_Inputstr = "";

		m_pInputLine = new CPrintInInputLine;
		m_pTraceLine = new CPrintInTraceLine;
		m_pNoTrace = new CNoTrace;
		m_pTrace = static_cast< CStateOfTrace * >( m_pNoTrace );

	};
	virtual ~CTrace()
	{
		delete m_pInputLine;
		delete m_pTraceLine;
		delete m_pNoTrace;
	};

	void SetTrace( HWND hWnd )
	{
		m_hWnd = hWnd;
		GetClientRect( m_hWnd, &m_ClientRect );
	};

	RECT GetRect( int idx ) const
	{
		return g_Rect[ idx ];
	}

	void SetString( std::string strText )
	{
		m_qInputString.Push( strText );
	}

	bool CheckInputString()
	{
		if( m_Inputstr == "")
		{
			return false;
		}
		return true;
	}

	void Process()
	{
		//한 사이클이 끝났을때만 호출해준다.
		if( m_pTrace == m_pNoTrace )
		{
			if( false == m_qInputString.Empty() )
			{
				m_qInputString.Pop( m_Inputstr );
				InvalidateRect( m_hWnd, &m_ClientRect, false );
			}
		}
	}

	void Print( HDC hdc )
	{
		m_pTrace->Print( m_hWnd, hdc ,m_ClientRect, m_InputRect, m_Inputstr );
	}

	void SetState()
	{
		if( m_pTrace == m_pNoTrace )
		{
			m_pTrace = static_cast< CStateOfTrace * >( m_pInputLine );
			InvalidateRect( m_hWnd, &m_ClientRect, false );

		}
		else if( m_pTrace == m_pInputLine )
		{
			m_pTrace = static_cast< CStateOfTrace * >( m_pTraceLine );
			InvalidateRect( m_hWnd, &m_ClientRect, false );
		}
		else if( m_pTrace == m_pTraceLine )
		{
			m_pTrace = static_cast< CStateOfTrace * >( m_pNoTrace );
		}
	}

private:
	enum { MAX_RECT_LINE = 10 };
	RECT m_ClientRect;
	RECT m_InputRect;
	HWND m_hWnd;

	std::string m_Inputstr;
	CQueue< std::string > m_qInputString;

	CStateOfTrace * m_pTrace;
	CPrintInInputLine * m_pInputLine;
	CPrintInTraceLine * m_pTraceLine;
	CNoTrace * m_pNoTrace;

};
