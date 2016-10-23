// Holiday.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Holiday.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoliday dialog


CHoliday::CHoliday(CWnd* pParent /*=NULL*/)
	: CDialog(CHoliday::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHoliday)
	m_iHoliday = -1;
	m_nYear = 0;
	m_strDate = _T("");
	//}}AFX_DATA_INIT
}


void CHoliday::FixNullEdit(int nID)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (!pEdit->GetWindowTextLength())
		pEdit->SetWindowText(_T("0"));
}

void CHoliday::FixZeroEdit(int nID)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (pEdit->GetWindowTextLength()==1)
	{
		_TCHAR c[2];
		pEdit->GetWindowText(c,2);
		if (c[0]=='0') pEdit->SetWindowText(_T(""));
	}
}

void CHoliday::DoDataExchange(CDataExchange* pDX)
{
	FixNullEdit(IDC_YEAR);

	CDialog::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
	{
		if (m_iHoliday<0 || m_nYear==0)
			m_strDate = "";
		else if (m_date.GetJD())
			m_strDate = m_date.Display();
		else
			m_strDate = "(That holiday does not exist in that year.)";
	}

	//{{AFX_DATA_MAP(CHoliday)
	DDX_Control(pDX, IDC_HOLIDAY, m_comboHoliday);
	DDX_CBIndex(pDX, IDC_HOLIDAY, m_iHoliday);
	DDX_Text(pDX, IDC_YEAR, m_nYear);
	DDX_Text(pDX, IDC_DATE, m_strDate);
	//}}AFX_DATA_MAP

	if (!m_comboHoliday.GetCount())
	{
		#undef h
		#define h(s) \
		m_comboHoliday.AddString(_T(#s));
		#include "holidaydef.h"
	}

	if (pDX->m_bSaveAndValidate)
	{
		if (m_nYear && m_iHoliday>=0)
			m_date.SetHoliday(m_nYear,(CDate::Holiday)m_iHoliday);
	}

	FixZeroEdit(IDC_YEAR);
}


BEGIN_MESSAGE_MAP(CHoliday, CDialog)
	//{{AFX_MSG_MAP(CHoliday)
	ON_CBN_SELCHANGE(IDC_HOLIDAY, OnSelchangeHoliday)
	ON_EN_CHANGE(IDC_YEAR, OnChangeYear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoliday message handlers

void CHoliday::OnSelchangeHoliday() 
{
	UpdateData();
	UpdateData(FALSE);
}

void CHoliday::OnChangeYear() 
{
	UpdateData();
	UpdateData(FALSE);
}
