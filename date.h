#ifndef _date_
#define _date_

#include <winnls.h>
class CDate
{
	int m_nYear, m_nMonth, m_nDay;
	BOOL m_bJulian;
	int m_nJD;
	CString m_strDisplayShort;
	CString m_strDisplayLong;
	CString m_strSort;
	BOOL m_bVisible;

	CString HolidayDisplay();

public:

	CDate();
	virtual ~CDate();

	// Julian:
	void SetJulian(int nYear, int nMonth, int nDay);
	void GetJulian(int* pnYear, int* pnMonth, int* pnDay) const;
	BOOL IsJulian() const { return m_bJulian; }

	// Gregorian:
	void Set(int nYear, int nMonth, int nDay);
	void Get(int* pnYear, int* pnMonth, int* pnDay) const;

	void Reset();

	int GetJD() const { return m_nJD; }

	enum Weekday
	{
		Sunday,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday
	};
	Weekday GetWeekday() const
	{
		ASSERT(m_nJD);
		return (Weekday)(m_nJD%7);
	}
	void AdvanceToNthWeekdayInMonth(int n, Weekday wk);
	void SetNthWeekday(int nYear, int nMonth, int n, Weekday wk);

	BOOL SetVisible(BOOL bVisible = TRUE);
	CString Display(DWORD dwFlags = DATE_LONGDATE) const;
	void CalcDisplay();
	CString Sort() const { return m_strSort; }

	BOOL IsValid();

	int operator-(const CDate& date) const 
	{
		ASSERT(m_nJD);
		ASSERT(date.m_nJD);
		return m_nJD-date.m_nJD;
	}
	operator BOOL () const
	{
		return m_nYear||m_nMonth||m_nDay;
	}
	int GetSimpleYear() { return m_nYear; }
	int GetSimpleYMD() { return m_nYear*10000+m_nMonth*100+m_nDay; }
	int GetSimpleMonthCount() { return m_nYear*12+m_nMonth; }

	//	Add or subtract a given number of days.
	CDate& operator+=(int nDays);
	CDate& operator-=(int nDays);

	enum Holiday
	{
		#define h(x) x,
		#include "holidaydef.h"
		cHoliday
	};
	void SetHoliday(int nYear, Holiday holiday);
	BOOL IsHoliday(Holiday holiday);
	static CDate HolidayDate(int nYear, Holiday holiday);
	static int MonthSize(int nYear, int nMonth);
};

CDate operator+(const CDate& date, int nDays);
CDate operator-(const CDate& date, int nDays);

extern const CString rsMonthFull[];
extern const CString rsWeekdayFull[];

#endif
