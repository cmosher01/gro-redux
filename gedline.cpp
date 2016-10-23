#include "stdafx.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"

CGedLine::CGedLine(CGedtreeDoc* pDoc, const CString& strLine):
	m_pDoc(pDoc)
{
	if (!strLine.IsEmpty())
	{
		ParseGedLine(strLine);
	}
}

CString GetNextWord(CString& strLine)
{
	CString strWord;
	int i = strLine.Find(cDelim);
	if (i==-1)
	{
		strWord = strLine;
		strLine.Empty();
	}
	else
	{
		strWord = strLine.Left(i);
		strLine = strLine.Mid(i+1);
	}
	return strWord;
}

void CGedLine::ParseGedLine(const CString& strLine)
{
	CString strL(strLine);
	CString strLev;

	strLev = GetNextWord(strL);
	m_strID = GetNextWord(strL);

	if (!m_strID.IsEmpty()&&m_strID[0]==cID)
	{
		m_strID = m_strID.Mid(1,m_strID.GetLength()-2);
		m_strTok = GetNextWord(strL);
	}
	else
	{
		m_strTok = m_strID;
		m_strID.Empty();
	}

	m_strVal = strL;

	if (strLev.GetLength())
		m_nLev = _ttoi(strLev);
	else
		m_nLev = -1;

	Calc();
}

CString CGedLine::GetGedLine()
{
	CString str;

	str.Format(_T("%d"),m_nLev);

	if (!m_strID.IsEmpty())
	{
		str += cDelim;
		str += cID;
		str += m_strID;
		str += cID;
	}

	str += cDelim;
	str += m_strTok;

	if (!m_strVal.IsEmpty())
	{
		str += cDelim;
		str += m_strVal;
	}

	str += "\r\n";

	return str;
}

void CGedLine::Calc()
{
	m_nVal = _ttoi(m_strVal);

	m_strValAsID = m_strVal;
	int i = m_strValAsID.Find(cID);
	if (i>=0)
	{
		m_strValAsID = m_strValAsID.Mid(i+1);
		i = m_strValAsID.Find(cID);
		if (i>=0)
		{
			m_strValAsID = m_strValAsID.Left(i);
		}
		else
		{
			m_strValAsID = "";
		}
	}
}

CString CGedLine::GetCleanValue(int nFlags)
{
	CString s = m_strVal;

	// Collapse assumes trim left and right
	if (nFlags&COLLAPSE)
		nFlags |= TRIM_LEFT|TRIM_RIGHT;

	if (nFlags&TRIM_LEFT)
		s.TrimLeft();

	if (nFlags&TRIM_RIGHT)
		s.TrimRight();

	if (nFlags&COLLAPSE)
	{
		CString sOut;
		BOOL bSpace(FALSE);
		for (int i(0); i<s.GetLength(); i++)
		{
			_TCHAR c = s[i];
			if (!(bSpace&&_istspace(c))) sOut += c;
			bSpace = _istspace(c);
		}
		s = sOut;
	}

	if (nFlags&UPCASE)
		s.MakeUpper();

	if (nFlags&DOWNCASE)
		s.MakeLower();

	if (nFlags&MIXCASE)
	{
		CString sOut;
		s.MakeLower();
		BOOL bUpper(TRUE);
		for (int i(0); i<s.GetLength(); i++)
		{
			_TCHAR c = s[i];
			if (bUpper) c = (_TCHAR)_totupper(c);
			bUpper = !_istalpha(c);
			sOut += c;
		}
		s = sOut;
	}

	return s;
}

static CString rXrefTok[][2] =
{
	"SOUR","SOUR",
	"HUSB","INDI",
	"WIFE","INDI",
	"CHIL","INDI",
	"FAMC","FAM",
	"FAMS","FAM",
	"NOTE","NOTE",
	"REPO","REPO",
	"SUBM","SUBM",
	"ANCI","SUBM",
	"DESI","SUBM",
	"OBJE","OBJE",
	"ALIA","INDI",
	"ASSO","INDI",
	"SUBN","SUBN",
	"",""
};

void CGedLine::CalcID()
{
	if (!m_strID.IsEmpty()) return; // definition, not xref
	if (m_strVal==m_strValAsID) return; // val is not an ID

	Calc();

	BOOL bFound(FALSE);
	int i(0);
	while (!rXrefTok[i][0].IsEmpty()&&!bFound)
	{
		bFound = m_strTok==rXrefTok[i][0];
		if (!bFound) i++;
	}
	if (bFound)
	{
		if (rXrefTok[i][1]=="INDI")
		{
			int i = m_pDoc->LookupIndividual(m_strValAsID);
			if (i<0) return;
			CIndividual& x = m_pDoc->m_rIndividual[i];
			m_strVal = x.GetID();
			Calc();
		}
		else if (rXrefTok[i][1]=="FAM")
		{
			int i = m_pDoc->LookupFamily(m_strValAsID);
			if (i<0) return;
			CFamily& x = m_pDoc->m_rFamily[i];
			m_strVal = x.GetID();
			Calc();
		}
		else if (rXrefTok[i][1]=="OBJE")
		{
		}
		else if (rXrefTok[i][1]=="NOTE")
		{
			int i = m_pDoc->LookupNote(m_strValAsID);
			if (i<0) return;
			CNote& x = m_pDoc->m_rNote[i];
			m_strVal = x.GetID();
			Calc();
		}
		else if (rXrefTok[i][1]=="SOUR")
		{
			int i = m_pDoc->LookupSource(m_strValAsID);
			if (i<0) return;
			CSource& x = m_pDoc->m_rSource[i];
			m_strVal = x.GetID();
			Calc();
		}
		else if (rXrefTok[i][1]=="REPO")
		{
			int i = m_pDoc->LookupRepository(m_strValAsID);
			if (i<0) return;
			CRepository& x = m_pDoc->m_rRepository[i];
			m_strVal = x.GetID();
			Calc();
		}
		else if (rXrefTok[i][1]=="SUBM")
		{
		}
		else if (rXrefTok[i][1]=="SUBN")
		{
		}
	}
}

BOOL CGedLine::IsChildOf(const CString& strTok, const CString& strID)
{
	if (!m_strID.IsEmpty()) return FALSE; // definition, not xref
	if (m_strVal==m_strValAsID) return FALSE; // val is not an ID

	BOOL bFound(FALSE);
	int i(0);
	while (!rXrefTok[i][0].IsEmpty()&&!bFound)
	{
		bFound = m_strTok==rXrefTok[i][0];
		if (!bFound) i++;
	}
	return
		bFound &&
		rXrefTok[i][1]==strTok &&
		m_strValAsID==strID;
}
