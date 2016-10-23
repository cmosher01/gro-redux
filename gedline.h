class CGedtreeDoc;

class CGedLine
{
public:
	CGedLine(CGedtreeDoc* pDoc, const CString& strLine);
	CGedtreeDoc* m_pDoc;
	CString m_strID;
	CString m_strTok;
	CString m_strVal;
	CString m_strValAsID;
	int m_nVal;
	int m_nLev;

	void ParseGedLine(const CString& strLine);
	void Calc();
	void CalcID();
	CString GetGedLine();
	int Level() { return m_nLev; }

	enum
	{
		TRIM_LEFT  = 1<<0,
		TRIM_RIGHT = 1<<1,
		COLLAPSE   = 1<<2,
		UPCASE     = 1<<3,
		DOWNCASE   = 1<<4,
		MIXCASE    = 1<<5
	};
	CString GetCleanValue(int nFlags);
	BOOL IsChildOf(const CString& strTok, const CString& strID);
};

const char cDelim(' ');
const char cID('@');
