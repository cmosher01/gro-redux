#include "stdafx.h"
#include "date.h"
#include "util.h"

const int nMonthSize[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
const CString rsMonthFull[] =
{
	"January","February","March","April","May","June",
	"July","August","September","October","November","December"
};
const CString rsWeekdayFull[] =
{
	"Sunday","Monday","Tuesday","Wednesday",
	"Thursday","Friday","Saturday"
};

static int ceildiv(int numerator, int denominator);
static int month_size(int month, BOOL bLeap);

inline int ceildiv(int numerator, int denominator)
{
	// divide numerator by denominator and apply ceiling to the result
	return (numerator+denominator-1)/denominator;
}

int month_size(int month, BOOL bLeap)
{
	// return the number of days in the given month (1==January)
	// leap indicates whether the month is in a leap year or not
	const int leapMonth = 2;
	int m = nMonthSize[month];
	if (month==leapMonth&&bLeap) m++;
	return m;
}

CDate::CDate():
	m_nJD(0),
	m_nYear(0),
	m_nMonth(0),
	m_nDay(0),
	m_bJulian(FALSE),
	m_bVisible(TRUE)
{
}

CDate::~CDate()
{
}

void CDate::SetJulian(int nYear, int nMonth, int nDay)
{
	m_bJulian = TRUE;
	m_nYear = nYear; m_nMonth = nMonth; m_nDay = nDay;

	if (m_nYear && m_nMonth && m_nDay)
	{
		//	For the given year, calculate the years since the beginning
		//	of the Julian period (e.g., 1 AD -> JY 4713, and 4713 BC ->
		//	JY 0).  Note the a zero year is invalid (i.e., 1 BC was
		//	followed immediately by 1 AD).
		nYear = nYear>0 ? nYear+4712 : nYear+4713;

		//	Calculate the JD of January 1 of the given year.  Note that
		//	1461 is the number of days in four (Julian) years (365*3+366).
		//	The addition of one is necessary because Juian Days are numbered
		//	from one, not zero.
		m_nJD = ceildiv(nYear*1461,4)+1;

		//	Now, add the number of days from the beginning of the year to
		//	the beginning of the month.  We need to know if the given year
		//	is a leap year in order to accomplish this (it is a leap year
		//	if it's remainder when dividing by 4 is zero (is false)).
		//	We decrement nMonth first because it is one-origin.
		BOOL bLeap = !(nYear%4);
		while (--nMonth>0)
			m_nJD += month_size(nMonth,bLeap);

		//	Now, add the number of days from the beginning of the month to
		//	this day.  We subtract one because nDay is one_origin.
		m_nJD += nDay-1;
	}

	if (m_nJD>=0 && m_bVisible) CalcDisplay();
}

void CDate::GetJulian(int* pnYear, int* pnMonth, int* pnDay) const
{
	if (!(m_nYear && m_nMonth && m_nDay))
	{
		ASSERT(m_bJulian);
		*pnYear = m_nYear;
		*pnMonth = m_nMonth;
		*pnDay = m_nDay;
		return;
	}

	//	Find the multiple of 4 year, and the remaining days.
	int dr = m_nJD-1;
	int jy = dr/1461*4;
	dr %= 1461;

	//	Normalize remaining days (the first year in 4-year group
	//	is the leap year, so we subtract one and the division will
	//	work).  This is to account for the leap year (the first
	//	year in the 4-year group); we will add this day back in
	//	later.
	--dr;

	//	Find the year within the 4-year group (0 through 3) and add
	//	to the year.  This gives us the year since 4713 BC.  Then
	//	we fill in the argument as appropriate.
	int leap = dr/365;
	jy += leap;
	*pnYear = jy>4712 ? jy-4712 : jy-4713;

	//	See if this is a leap year.  The calculation of the year
	//	within the 4-year group above gives us the info we need
	//	to see if this is a leap year.  Only the first year (zero)
	//	within a 4-year group is the leap year.
	leap = !leap;

	//	Find the remaining days within the year.  Since the total
	//	remaining days could be -1 (due to the decrement above),
	//	we must check for this case so we don't pass a negative
	//	int to the % operator (because its behavior in such a case
	//	is implementation-dependent).  We might as well check for
	//	>0 rather than >=0 (after all, we know that 0%365 is 0).
	if (dr>0) dr %= 365;

	//	Add that day back in if it's a leap year.
	if (leap) dr++;

	//	Find the month:
	//	Start with January.
	//	While we still have more days left than are in this month,
	//		subtract this month's days, and check the next
	int m = 0;
	int ms;
	while (dr >= (ms = month_size(++m,leap)))
		dr -= ms;
	*pnMonth = m;

	//	The days left is the day since the beginning of the month,
	//	so we add one (because days are 1-origin) and fill in the arg.
	*pnDay = dr+1;
}

/*
	4442 is the number of days before 2/29/4701J BC (the
	first day in the Julian Calendar that is omitted in the
	Gregorian Calendar) in the Julian Period.
*/
const int leap_offset(4442);

/*
	38 is the offset of the Gregorian Calendar's dates within the
	Julian Period.  It is the number of "Gregorian centesimal leap
	omissions" before March 1, AD 200 (the first day the two
	calendars were synchronous).
*/
const int greg_offset(38);

void CDate::Set(int nYear, int nMonth, int nDay)
{
/*
	SetJulian calculates the Julian calendar
	version of the given date; we just need to convert it to
	the Gregorian calendar by adding the appropriate number of
	days. For 1993, for example, we need to subtract 13 days.

	Assumptions about the Julian and Gregorian Calendars and
	Julian Days:

		1/1/-4713J == JD1
		1/1/-4713G == JD39
		12/31/-1 is followed immediately by 1/1/1
		3/1/200J == 3/1/200G == JD1794169
		2/29/300J == 3/1/300G == JD 1830693
		(i.e., the G. and J. Calendars were synchronous from
		March 1, AD 200, through February 28, AD 300, inclusive)
		12/31/3267G == JD2914673
		12/31/3267J == JD2914695 (the last day of the Julian Period)
		AD 4, and 1 BC were leap years
*/

	BOOL bVis = SetVisible(FALSE);
	SetJulian(nYear,nMonth,nDay);
	SetVisible(bVis);
	m_bJulian = FALSE;

	if (m_nYear && m_nMonth && m_nDay)
	{
		--m_nJD; // make 0-origin

		int nd = m_nJD-leap_offset;
		if (nd>=0)
		{
		/*
			146100 is the number of days in a Julian Quadrennium.
			36525 is the number of days in a Julian Century.
		*/
			int qd = nd/146100*3;
			int rd = nd%146100/36525;
			if (rd>2) rd = 2;
			m_nJD -= qd+rd+1;
		}

		m_nJD += greg_offset;

		m_nJD++; // make 1-origin
	}

	if (m_nJD>=0 && m_bVisible) CalcDisplay();
}

void CDate::Reset()
{
	if (m_bJulian)
		SetJulian(m_nYear,m_nMonth,m_nDay);
	else
		Set(m_nYear,m_nMonth,m_nDay);
}

void CDate::Get(int* pnYear, int* pnMonth, int* pnDay) const
{
	if (!(m_nYear && m_nMonth && m_nDay))
	{
		ASSERT(!m_bJulian);
		*pnYear = m_nYear;
		*pnMonth = m_nMonth;
		*pnDay = m_nDay;
		return;
	}
	/*
		Adjust from Gregorian days to Julian days.
	*/

	// cast away const
	int &m_nJD = ((CDate*)this)->m_nJD;

	int orig_d = m_nJD;

	--m_nJD; // make 0-origin

	m_nJD -= greg_offset;

	int nd = m_nJD-leap_offset;
	if (nd>=0)
	{
		/*
			146097 is the number of days in a Gregorian Quadrennium.
			36524 is the number of days in a (non-leap-century) Gregorian Century.
		*/
		int qd = nd/146097*3;
		int rd = nd%146097/36524;
		if (rd>2) rd = 2;
		m_nJD += qd+rd+1;
	}

	m_nJD++; // make 1-origin

	GetJulian(pnYear,pnMonth,pnDay);

	m_nJD = orig_d;
}

BOOL CDate::SetVisible(BOOL bVisible)
{
	BOOL bOld = m_bVisible;
	m_bVisible = bVisible;
	return bOld;
}

CString CDate::Display(DWORD dwFlags) const
{
	if (dwFlags&DATE_SHORTDATE)
		return m_strDisplayShort;
	else
		return m_strDisplayLong;
}

void CDate::CalcDisplay()
{
	CString strOS, strBC, strWk;

	if (m_bJulian)
		strOS = " OS";

	int nYear = m_nYear;
	if (m_nYear<0)
	{
		strBC = "BC";
		nYear = -m_nYear;
	}
	if (m_nYear&&m_nMonth&&m_nDay)
		strWk = rsWeekdayFull[GetWeekday()];

	CString strYear;
	if (m_nYear)
	{
		// Build the year string. Check to see if we need
		// to do "double" years (e.g., for 5 Mar 1734/5).
		if (1582<m_nYear && m_nYear<=1752 && (m_nMonth || m_nDay)
			&& (m_nMonth<3 || (m_nMonth==3 && m_nDay<25)))
		{
			int y1 = nYear-1;
			int y2 = nYear%10;
			if (y2==0)
				y2 = nYear%100;
			//"1599/600" looks funny, so put "1599/1600" instead
			//if (y2==0)
			//	y2 = nYear%1000;
			if (y2==0)
				y2 = nYear;
			strYear = CUtil::str(y1)+"/"+CUtil::str(y2);
		}
		else
			strYear = CUtil::str(nYear);
	}

	m_strDisplayLong.Empty();
	if (m_nMonth)
		m_strDisplayLong += rsMonthFull[m_nMonth-1];

	if (m_nMonth&&(m_nDay||nYear))
		m_strDisplayLong += " ";

	if (m_nDay)
		m_strDisplayLong += CUtil::str(m_nDay);

	if (m_nDay&&nYear)
		m_strDisplayLong += ", ";

	m_strDisplayLong += strYear;

	if (!strBC.IsEmpty())
		m_strDisplayLong += " "+strBC;

	if (!strOS.IsEmpty())
		m_strDisplayLong += strOS;

	if (m_nYear&&m_nMonth&&m_nDay)
		m_strDisplayLong += " ("+strWk+")";

	if (m_nJD)
		m_strDisplayLong += " (JD"+CUtil::str(m_nJD)+")";



	m_strDisplayShort.Empty();
	if (m_nDay)
	{
		CString s;
		s.Format(_T("%2.2d"),m_nDay);
		m_strDisplayShort += s;//CUtil::str(m_nDay);
	}

	if (m_nDay&&(m_nMonth||nYear))
		m_strDisplayShort += " ";

	if (m_nMonth)
		m_strDisplayShort += rsMonthFull[m_nMonth-1].Left(3);

	if (m_nMonth&&m_nYear)
		m_strDisplayShort += " ";

	m_strDisplayShort += strYear;

	if (!strBC.IsEmpty())
		m_strDisplayShort += " "+strBC;

	if (!strOS.IsEmpty())
		m_strDisplayShort += strOS;

//	if (m_nYear&&m_nMonth&&m_nDay)
//		m_strDisplayShort += " ("+strWk.Left(3)+")";



	CString strHoliday = HolidayDisplay();
	if (!strHoliday.IsEmpty())
	{
		m_strDisplayLong  += strHoliday;
		//m_strDisplayShort += strHoliday;
	}

	CString strWeekShort;
	if (!strWk.IsEmpty())
		strWeekShort = " ("+strWk.Left(3)+")";
	strWeekShort.Empty();//don't show this
	m_strSort.Format(_T("%s%4.4d/%2.2d/%2.2d%s%s"),
		(LPCTSTR)strBC,
		nYear,
		m_nMonth,
		m_nDay,
		(LPCTSTR)strOS,
		(LPCTSTR)strWeekShort);
}

CDate& CDate::operator+=(int nDays)
{
	ASSERT(m_nJD);
	m_nJD += nDays;
	return *this;
}

CDate& CDate::operator-=(int nDays)
{
	ASSERT(m_nJD);
	m_nJD -= nDays;
	return *this;
}

CDate operator+(const CDate& date, int nDays)
{
	CDate dateNew(date);
	return dateNew += nDays;
}

CDate operator-(const CDate& date, int nDays)
{
	CDate dateNew(date);
	return dateNew -= nDays;
}

CDate CDate::HolidayDate(int nYear, Holiday holiday)
{
	CDate d;
	d.SetHoliday(nYear,holiday);
	return d;
}

BOOL CDate::IsHoliday(CDate::Holiday holiday)
{
	BOOL bHoliday(FALSE);
	if (m_nYear&&m_nMonth&&m_nDay)
	{
		CDate dHoliday;
		dHoliday.SetVisible(FALSE);
		dHoliday.SetHoliday(m_nYear,holiday);
		bHoliday = dHoliday.GetJD()==m_nJD;
	}
	return bHoliday;
}

CString CDate::HolidayDisplay()
{
	CString s;
	#undef h
	#define h(x) \
	if (IsHoliday(x)) \
	{ \
		s += " ("; \
		s += #x; \
		s += ")"; \
	}
	#include "holidaydef.h"

	return s;
}

void CDate::SetHoliday(int nYear, Holiday holiday)
{
	m_nYear = nYear;
	m_nMonth = 0;
	m_nDay = 0;
	m_nJD = 0;
	m_bJulian = FALSE;

	if (holiday==Easter)
	{
		if (m_nYear>=1583)
		{
			int a = m_nYear%19;
			int b = m_nYear/100;
			int c = m_nYear%100;
			int d = b/4;
			int e = b%4;
			int f = (b+8)/25;
			int g = (b-f+1)/3;
			int h = (19*a+b-d-g+15)%30;
			int i = c/4;
			int k = c%4;
			int l = (32+2*e+2*i-h-k)%7;
			int m = (a+11*h+22*l)/451;
			int n = h+l-7*m+114;
			m_nMonth = n/31;
			m_nDay = n%31+1;
			Reset();
		}
	}
	else if (holiday==EasterOrthodox)
	{
		if (m_nYear>=325)
		{
			m_bJulian = TRUE;
			m_nDay = (m_nYear%19*19+15)%30;
			m_nDay -= (m_nDay+m_nYear+m_nYear/4)%7;
			m_nDay += 28;
			m_nMonth = 3;
			if (m_nDay>nMonthSize[m_nMonth])
			{
				m_nDay -= nMonthSize[m_nMonth];
				m_nMonth++;
			}
			Reset();
		}
	}
	else if (holiday==Presidents)
	{
		SetNthWeekday(nYear,2,3,Monday); // third Monday in February
	}
	else if (holiday==Labor)
	{
		SetNthWeekday(nYear,9,1,Monday); // first Monday in September
	}
	else if (holiday==Thanksgiving)
	{
		if (m_nYear>=1620)
			SetNthWeekday(nYear,11,4,Thursday); // fourth Thursday in November
	}
	else if (holiday==KingMartinLuther)
	{
		if (m_nYear>=1986)
			SetNthWeekday(nYear,1,3,Monday); // third Monday in January
	}
	else if (holiday==ColumbusChristopher)
	{
		SetNthWeekday(nYear,10,2,Monday); // second Monday in October
	}
	else if (holiday==Census)
	{
		if (nYear>=1790 && nYear%10==0)
		{
			if (nYear<=1820)
				SetNthWeekday(nYear,8,1,Monday); // first Monday in August
			else if (nYear<=1900)
				Set(nYear,6,1); // June 1
			else if (nYear==1910)
				Set(nYear,4,15); // April 15
			else if (nYear==1920)
				Set(nYear,1,1); // January 1
			else if (nYear>=1930)
				Set(nYear,4,1); // April 1
		}
	}
}

void CDate::AdvanceToNthWeekdayInMonth(int n, Weekday wk)
{
	ASSERT(m_nDay==1);

	m_nDay += (7-GetWeekday()+wk)%7 + 7*--n;
	Reset();
}

void CDate::SetNthWeekday(int nYear, int nMonth, int n, Weekday wk)
{
	m_nYear = nYear;
	m_nMonth = nMonth;
	m_nDay = 1;
	Reset();

	AdvanceToNthWeekdayInMonth(n,wk);
}

BOOL CDate::IsValid()
{
	BOOL bValid(TRUE);
	if (m_nYear&&m_nMonth&&m_nDay)
	{
		int nYear = m_nYear>0 ? m_nYear+4712 : m_nYear+4713;
		BOOL bLeap = nYear%4==0;
		if (!m_bJulian)
			bLeap = bLeap && (nYear%100!=0 || nYear%400==0);
		bValid = m_nDay<=month_size(m_nMonth,bLeap);
	}
	return bValid;
}

int CDate::MonthSize(int nYear, int nMonth)
{
	if (nYear>0)
		nYear += 4712;
	else
		nYear += 4713;
	bool bLeap = (nYear%4==0 && nYear%100!=0) || nYear%400==0;
	return month_size(nMonth,bLeap);
}
