#ifndef _datevalue_
#define _datevalue_

#include "date.h"

class CDateValue
{
public:
	enum Type
	{
		NODATE,
		DATE,
		PERIOD,
		RANGE,
		ABOUT,
		CALCULATED,
		ESTIMATED,
		PHRASE,
		INTERPRETED
	};

private:
	CString m_strDisplayShort;
	CString m_strDisplayLong;
	CString m_strSort;

	Type m_type;

	CDate m_d;
	// "before" or "and" (for range) or "to" (for period)
	CDate m_dOther;

	CString m_strPhrase;

	BOOL ParseDate(CDate& date, CString& strRest);
	int ParseYear(const CString& str);
	int Month(const CString& str);
	CString NextWord(CString& strRest);
	CString FormatDate(const CDate& d);
	void CalcDisplay();
	CString GetDisplayString(DWORD dwFlags = DATE_LONGDATE) const;
	CString GetSortString() const;

public:
	CDateValue();
	virtual ~CDateValue();

	void Set(const CString& strGedcom);
	void Set(Type type, CDate d, CDate d2, const CString& strPhrase);
	void Get(Type& type, CDate& d, CDate& d2, CString& strPhrase);
	CString Get();
	operator BOOL () { return m_type != NODATE; }
	BOOL IsValid() { return m_d.IsValid() && m_dOther.IsValid(); }

	CString Display(DWORD dwFlags = DATE_LONGDATE) const;
	CString Sort() { return m_strSort; }
	CDate GetExact() { ASSERT(IsExact()); return m_d; }
	int GetSimpleYear();
	int GetSimpleYMD();
	int GetSimpleMonthCount();
	bool IsExact() { return m_type==DATE; }
	void SetExact(const CDate& d) { m_type = DATE; m_d = d; CalcDisplay(); }
};

#endif
