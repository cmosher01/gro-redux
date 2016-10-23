#if !defined(AFX_BIRTHCALC_H__DE913440_F5FF_11D3_821D_00A0C97AE306__INCLUDED_)
#define AFX_BIRTHCALC_H__DE913440_F5FF_11D3_821D_00A0C97AE306__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BirthCalc.h : header file
//

#include "DateValue.h"

/////////////////////////////////////////////////////////////////////////////
// CBirthCalc dialog

class CBirthCalc : public CDialog
{
// Construction
public:
	CBirthCalc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBirthCalc)
	enum { IDD = IDD_BIRTH_CALC };
	int		m_nAge;
	int		m_nYear1;
	int		m_nYear2;
	int		m_nMonth;
	int		m_nDay;
	int		m_nYear3;
	CString	m_strDateInput;
	CString	m_strBirthDate;
	CString	m_strCensusBirthDate;
	CString	m_strCensusDateStatic;
	CString	m_strYearOfAge;
	//}}AFX_DATA

	CDateValue m_dvDate;
	CDateValue m_dvDateBirth;
	CDateValue m_dvDateCensus;
	CDateValue m_dvDateBirthCensus;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBirthCalc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void CalcBirth(CDateValue& dvInput, CDateValue& dvBirth);

	// Generated message map functions
	//{{AFX_MSG(CBirthCalc)
	afx_msg void OnDate();
	afx_msg void OnChangeDays();
	afx_msg void OnChangeMonths();
	afx_msg void OnChangeYears();
	afx_msg void OnChangeYears2();
	afx_msg void OnChangeYears3();
	afx_msg void OnAge1();
	afx_msg void OnAge2();
	afx_msg void OnAge3();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIRTHCALC_H__DE913440_F5FF_11D3_821D_00A0C97AE306__INCLUDED_)
