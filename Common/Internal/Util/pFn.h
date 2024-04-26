#pragma once

class CTrace
{
public:
	CTrace() {};
	CTrace( CRect rc ) : m_Rect( rc )
	{
		m_StrText = "";
	}
	virtual ~CTrace() {};

	CString m_StrText;
	CRect m_Rect;
};