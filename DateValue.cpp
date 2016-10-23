#include "stdafx.h"
#include "datevalue.h"
#include "gedline.h"
#include "util.h"

static const int cMonth(12);
static char* rsMonth[cMonth] =
{
	"JAN","FEB","MAR","APR","MAY","JUN",
	"JUL","AUG","SEP","OCT","NOV","DEC"
};

CDateValue::CDateValue() :
	m_type(NODATE)
{
}

CDateValue::~CDateValue()
{
}

void CDateValue::Set(const CString& strGedcom)
{
	// assume strGedcom is clean (trimleft,trimright,collapse,upcase)
	CString strKey, strRest;
	BOOL bOK;
	strRest = strGedcom;

do_next:

	strKey = NextWord(strRest);
	if (strKey.Right(1)==".")
		strKey = strKey.Left(strKey.GetLength()-1);

	bOK = TRUE;

	if (strKey=="FROM")
	{
		m_type = PERIOD;
		bOK = ParseDate(m_d,strRest);
		goto do_next;
	}
	else if (strKey=="TO")
	{
		m_type = PERIOD;
		bOK = ParseDate(m_dOther,strRest);
	}
	else if (strKey=="AFT" || strKey=="AFTER")
	{
		m_type = RANGE;
		bOK = ParseDate(m_d,strRest);
	}
	else if (strKey=="BEF" || strKey=="BEFORE")
	{
		m_type = RANGE;
		bOK = ParseDate(m_dOther,strRest);
	}
	else if (strKey=="BET" || strKey=="BETWEEN")
	{
		m_type = RANGE;
		bOK = ParseDate(m_d,strRest);
		goto do_next;
	}
	else if (strKey=="AND" || strKey=="-")
	{
		m_type = RANGE;
		bOK = ParseDate(m_dOther,strRest);
	}
	else if (strKey=="ABT" || strKey=="ABOUT")
	{
		m_type = ABOUT;
		bOK = ParseDate(m_d,strRest);
	}
	else if (strKey=="CAL")
	{
		m_type = CALCULATED;
		bOK = ParseDate(m_d,strRest);
	}
	else if (strKey=="EST")
	{
		m_type = ESTIMATED;
		bOK = ParseDate(m_d,strRest);
	}
	else if (strKey=="INT")
	{
		m_type = INTERPRETED;
		bOK = ParseDate(m_d,strRest);
		goto do_next;
	}
	else if (strKey.Left(1)=="(")
	{
		if (m_type != INTERPRETED)
			m_type = PHRASE;
		m_strPhrase = strKey.Mid(1);
		if (!strRest.IsEmpty())
			m_strPhrase += CString(cDelim)+strRest;
		if (m_strPhrase.Right(1)==")")
			m_strPhrase = m_strPhrase.Left(m_strPhrase.GetLength()-1);
	}
	else
	{
		if (m_type != PERIOD || !strRest.IsEmpty())
		{
			strRest = strGedcom;
			m_type = DATE;
			bOK = ParseDate(m_d,strRest);
		}
	}

	if (!bOK)
	{
		m_type = PHRASE;
		m_strPhrase = strGedcom;
	}

	CalcDisplay();

/*
	CString strOutVal = Get();

	if (strOutVal!=strGedcom)
	{
		int nYear, nMonth, nDay;
		int nYear2, nMonth2, nDay2;
		m_d.Get(&nYear,&nMonth,&nDay);
		m_dOther.Get(&nYear2,&nMonth2,&nDay2);

		CString sMsg;
		sMsg.Format(
			_T("%s --> %d %4.4d.%2.2d.%2.2d %4.4d.%2.2d.%2.2d \"%s\" --> \"%s\"\n"),
			(LPCTSTR)strGedcom,
			m_type,
			nYear,nMonth,nDay,
			nYear2,nMonth2,nDay2,
			(LPCTSTR)m_strPhrase,
			(LPCTSTR)strOutVal);
		OutputDebugString(sMsg);
	}
*/
}

BOOL CDateValue::ParseDate(CDate& date, CString& strRest)
{
	CString s(strRest);

	BOOL bGregorian(TRUE);
	int n;

	if (s.Left(3)=="@#D")
	{
		CString strCalendar = s.Mid(3);

		n = strCalendar.Find('@');
		if (n==-1) return FALSE;

		s = strCalendar.Mid(n+1);
		s.TrimLeft();

		strCalendar = strCalendar.Left(n);
		strCalendar.TrimLeft();
		strCalendar.TrimRight();

		if (strCalendar==_T("GREGORIAN"))
			ASSERT(bGregorian);
		else if (strCalendar==_T("JULIAN"))
			bGregorian = FALSE;
		else
			return FALSE;
	}
//NEED TO PARSE [B.C.] after year
	// s is now of the form: [[DAY ]MONTH ]YEAR[/year][ B.C.][ rest]
	strRest = s;

	int nYear(0), nMonth(0), nDay(0);

	CString strItem1 = NextWord(strRest);
	nMonth = Month(strItem1);
	if (nMonth)
	{
		// MONTH YEAR[/year][ B.C.][ rest]
		//       ^
		CString strItem2 = NextWord(strRest);
		// MONTH YEAR[/year][ B.C.][ rest]
		//                   ^
		CString strTemp(strRest); // look ahead for B.C.
		CString strItem3 = NextWord(strTemp);
		// MONTH YEAR[/year][ B.C.][ rest]
		//                          ^temp
		if (strItem3=="B.C.")
		{
			nYear = -ParseYear(strItem2);
			strRest = strTemp;
		}
		else
			nYear = ParseYear(strItem2);
	}
	else
	{
		// DAY MONTH YEAR[/year][ B.C.][ rest]
		//     ^
		//---or---
		// YEAR[/year][ B.C.][ rest]
		//             ^
		CString strTemp(strRest); // look ahead for month
		CString strItem2 = NextWord(strTemp);
		nMonth = Month(strItem2);
		if (nMonth)
		{
			// DAY MONTH YEAR[/year][ B.C.][ rest]
			//           ^temp
			nDay = _ttoi(strItem1);
			CString strItem3 = NextWord(strTemp);
			// DAY MONTH YEAR[/year][ B.C.][ rest]
			//                       ^temp
			strRest = strTemp; // found month when looking ahead

			CString strItem4 = NextWord(strTemp);
			if (strItem4=="B.C.")
			{
				nYear = -ParseYear(strItem3);
				strRest = strTemp;
			}
			else
				nYear = ParseYear(strItem3);
		}
		else
		{
			// YEAR[/year][ B.C.][ rest]
			//             ^
			CString strTemp(strRest); // look ahead for B.C.
			CString strItem2 = NextWord(strTemp);
			if (strItem2=="B.C.")
			{
				nYear = -ParseYear(strItem1);
				strRest = strTemp;
			}
			else
				nYear = ParseYear(strItem1);
		}
	}

	if (bGregorian)
		date.Set(nYear,nMonth,nDay);
	else
		date.SetJulian(nYear,nMonth,nDay);

	return date;
}

int CDateValue::ParseYear(const CString& str)
{
	// str is of the form: y[/y2]
	// where y and y2 are strings of digits

	int n = str.Find('/');
	if (n==-1) return _ttoi(str);

	return _ttoi(str.Left(n))+1;
}

int CDateValue::Month(const CString& str)
{
	BOOL bFound(FALSE);
	int nMonth(0);

	for (int i(0); !bFound && i<cMonth; i++)
	{
		if (str==rsMonth[i])
		{
			bFound = TRUE;
			nMonth = i+1;
		}
	}

	return nMonth;
}

CString CDateValue::NextWord(CString& strLine)
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

CString CDateValue::Get()
{
	CString s;
	switch (m_type)
	{
		case DATE:
			s = FormatDate(m_d);
		break;
		case PERIOD:
			if (m_d)
				s += "FROM "+FormatDate(m_d);
			if (m_d&&m_dOther) s += " ";
			if (m_dOther)
				s += "TO "+FormatDate(m_dOther);
		break;
		case RANGE:
			if (m_d&&m_dOther)
				s += "BET "+FormatDate(m_d)+" AND "+FormatDate(m_dOther);
			else if (m_d)
				s += "AFT "+FormatDate(m_d);
			else if (m_dOther)
				s += "BEF "+FormatDate(m_dOther);
		break;
		case ABOUT:
			s = "ABT "+FormatDate(m_d);
		break;
		case CALCULATED:
			s = "CAL "+FormatDate(m_d);
		break;
		case ESTIMATED:
			s = "EST "+FormatDate(m_d);
		break;
		case PHRASE:
			s = "("+m_strPhrase+")";
		break;
		case INTERPRETED:
			s = "INT "+FormatDate(m_d)+" ("+m_strPhrase+")";
		break;
		case NODATE:
			ASSERT(s.IsEmpty());
		break;
		default:
			ASSERT(FALSE); // bad m_type value
	}
	return s;
}

CString CDateValue::FormatDate(const CDate& d)
{
	CString s;

	int nYear, nMonth, nDay;
	if (d.IsJulian())
	{
		d.GetJulian(&nYear,&nMonth,&nDay);
		s += "@#DJULIAN@ ";
	}
	else
		d.Get(&nYear,&nMonth,&nDay);

	if (nDay)
		s += CUtil::str(nDay)+" ";
	if (nMonth)
		s += CString(rsMonth[nMonth-1])+" ";

	if (nYear>0)
	{
		s += CUtil::str(nYear);
	}
	else if (nYear<0)
	{
		s += CUtil::str(-nYear)+" B.C.";
	}

	return s;
}

CString CDateValue::GetDisplayString(DWORD dwFlags) const
{
	CString s;

	switch (m_type)
	{
		case DATE:
			s = m_d.Display(dwFlags);
		break;
		case PERIOD:
			ASSERT(s.IsEmpty());
			if (m_d)
				s += m_d.Display(dwFlags);
			else
				s += "?";
			s += "-";
			if (m_dOther)
				s += m_dOther.Display(dwFlags);
			else
				s += "?";
/*
			if (m_d&&m_dOther)
			{
				int d1 = m_d.GetJD();
				int d2 = m_dOther.GetJD();
				if (d1 && d2 && d1<d2)
				{
					int dif = d2-d1;
					if (dif==1)
						s += " (1 day)";
					else
						s += " ("+CUtil::str(dif)+" days)";
				}
			}
*/
		break;
		case RANGE:
			ASSERT(s.IsEmpty());
			if (m_d&&m_dOther)
				s += "btw "+m_d.Display(dwFlags)+" & "+m_dOther.Display(dwFlags);
			else if (m_d)
				s += "aft "+m_d.Display(dwFlags);
			else if (m_dOther)
				s += "bef "+m_dOther.Display(dwFlags);
		break;
		case ABOUT:
			s = "c. "+m_d.Display(dwFlags);
		break;
		case CALCULATED:
			s = "calc "+m_d.Display(dwFlags);
		break;
		case ESTIMATED:
			s = "estd "+m_d.Display(dwFlags);
		break;
		case PHRASE:
			s = "("+m_strPhrase+")";
		break;
		case INTERPRETED:
			s = "interpreted as "+m_d.Display(dwFlags)+" from \""+m_strPhrase+"\"";
		break;
		case NODATE:
			ASSERT(s.IsEmpty());
		break;
		default:
			ASSERT(FALSE); // bad m_type value
	}

	return s;
}

CString CDateValue::GetSortString() const
{
	CString s;

	switch (m_type)
	{
		case DATE:
			s = m_d.Sort();
		break;
		case PERIOD:
			ASSERT(s.IsEmpty());
			if (m_d)
				s += m_d.Sort();
			else
				s += "?";
			s += "-";
			if (m_dOther)
				s += m_dOther.Sort();
			else
				s += "?";
		break;
		case RANGE:
			ASSERT(s.IsEmpty());
			if (m_d&&m_dOther)
				s += m_d.Sort()+"/"+m_dOther.Sort();
			else if (m_d)
				s += m_d.Sort()+"/(after)";
			else if (m_dOther)
				s += m_dOther.Sort()+"/(before)";
		break;
		case ABOUT:
			s = m_d.Sort()+" (circa)";
		break;
		case CALCULATED:
			s = m_d.Sort()+" (calc.)";
		break;
		case ESTIMATED:
			s = m_d.Sort()+" (est.)";
		break;
		case PHRASE:
			s = "("+m_strPhrase+")";
		break;
		case INTERPRETED:
			s = m_d.Sort()+" ("+m_strPhrase+")";
		break;
		case NODATE:
			ASSERT(s.IsEmpty());
		break;
		default:
			ASSERT(FALSE); // bad m_type value
	}

	return s;
}

void CDateValue::CalcDisplay()
{
	m_strDisplayShort = GetDisplayString(DATE_SHORTDATE);
	m_strDisplayLong = GetDisplayString(DATE_LONGDATE);
	m_strSort = GetSortString();
}

void CDateValue::Set(Type type, CDate d, CDate d2, const CString& strPhrase)
{
	m_type = type;
	m_d = d;
	m_dOther = d2;
	m_strPhrase = strPhrase;
	CalcDisplay();
}

void CDateValue::Get(Type& type, CDate& d, CDate& d2, CString& strPhrase)
{
	type = m_type;
	d = m_d;
	d2 = m_dOther;
	strPhrase = m_strPhrase;
}

CString CDateValue::Display(DWORD dwFlags) const
{
	if (dwFlags&DATE_SHORTDATE)
		return m_strDisplayShort;
	else
		return m_strDisplayLong;
}

int CDateValue::GetSimpleYear()
{
	int nYear1 = m_d.GetSimpleYear();
	int nYear2 = m_dOther.GetSimpleYear();
	if (nYear1==0)
		return nYear2;
	if (nYear2==0)
		return nYear1;
	return min(nYear1,nYear2);
}

int CDateValue::GetSimpleYMD()
{
	int nYMD1 = m_d.GetSimpleYMD();
	int nYMD2 = m_dOther.GetSimpleYMD();
	if (nYMD1==0)
		return nYMD2;
	if (nYMD2==0)
		return nYMD1;
	return min(nYMD1,nYMD2);
}
int CDateValue::GetSimpleMonthCount()
{
	int nM1 = m_d.GetSimpleMonthCount();
	int nM2 = m_dOther.GetSimpleMonthCount();
	if (nM1==0)
		return nM2;
	if (nM2==0)
		return nM1;
	return min(nM1,nM2);
}
