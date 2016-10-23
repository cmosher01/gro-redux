// EditDate.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "EditDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDate dialog


CEditDate::CEditDate(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditDate)
	m_strPhrase = _T("");
	m_nMonth = -1;
	m_nDay = 0;
	m_nYear = 0;
	m_bAD = -1;
	m_bJulian = -1;
	m_nMonth2 = -1;
	m_nDay2 = 0;
	m_nYear2 = 0;
	m_bAD2 = -1;
	m_bJulian2 = -1;
	m_nType = -1;
	//}}AFX_DATA_INIT
}

void CEditDate::FixNullEdit(int nID)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (!pEdit->GetWindowTextLength())
		pEdit->SetWindowText(_T("0"));
}

void CEditDate::FixZeroEdit(int nID)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(nID);
	if (pEdit->GetWindowTextLength()==1)
	{
		_TCHAR c[2];
		pEdit->GetWindowText(c,2);
		if (c[0]=='0') pEdit->SetWindowText(_T(""));
	}
}

void CEditDate::DoDataExchange(CDataExchange* pDX)
{
	FixNullEdit(IDC_DAY);
	FixNullEdit(IDC_YEAR);
	FixNullEdit(IDC_DAY2);
	FixNullEdit(IDC_YEAR2);

	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MONTH, m_comboMonth);
	DDX_Control(pDX, IDC_MONTH2, m_comboMonth2);
	if (!m_comboMonth.GetCount())
		FillMonths();

	if (!pDX->m_bSaveAndValidate)
	{
		ValueToData();
	}

	//{{AFX_DATA_MAP(CEditDate)
	DDX_Control(pDX, IDC_SHORT, m_statShort);
	DDX_Control(pDX, IDC_LONG, m_statLong);
	DDX_Control(pDX, IDC_GED, m_statGED);
	DDX_Control(pDX, IDC_PHRASETEXT, m_editPhrase);
	DDX_Control(pDX, IDC_STATICBEFORE, m_buttonBefore);
	DDX_Control(pDX, IDC_STATICAFTER, m_buttonAfter);
	DDX_Text(pDX, IDC_PHRASETEXT, m_strPhrase);
	DDX_CBIndex(pDX, IDC_MONTH, m_nMonth);
	DDX_Text(pDX, IDC_DAY, m_nDay);
	DDX_Text(pDX, IDC_YEAR, m_nYear);
	DDX_Radio(pDX, IDC_BC, m_bAD);
	DDX_Radio(pDX, IDC_GREGORIAN, m_bJulian);
	DDX_CBIndex(pDX, IDC_MONTH2, m_nMonth2);
	DDX_Text(pDX, IDC_DAY2, m_nDay2);
	DDX_Text(pDX, IDC_YEAR2, m_nYear2);
	DDX_Radio(pDX, IDC_BC2, m_bAD2);
	DDX_Radio(pDX, IDC_GREGORIAN2, m_bJulian2);
	DDX_CBIndex(pDX, IDC_DATETYPE, m_nType);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DataToValue();
	}

	Enable();
	SetStaticTitles();

	FixZeroEdit(IDC_DAY);
	FixZeroEdit(IDC_YEAR);
	FixZeroEdit(IDC_DAY2);
	FixZeroEdit(IDC_YEAR2);
}

void CEditDate::FillMonths()
{
	for (int i(0); i<12; i++)
	{
		m_comboMonth.AddString(rsMonthFull[i]);
		m_comboMonth2.AddString(rsMonthFull[i]);
	}
}

static CDateValue::Type rType[] =
{
	CDateValue::NODATE,
	CDateValue::DATE,
	CDateValue::ABOUT,
	CDateValue::PERIOD,
	CDateValue::RANGE,
	CDateValue::RANGE,
	CDateValue::RANGE,
	CDateValue::ESTIMATED,
	CDateValue::CALCULATED,
	CDateValue::PHRASE,
	CDateValue::INTERPRETED
};

void CEditDate::DataToValue()
{
	CDateValue::Type t = rType[m_nType];

	CDate d;
	if (m_nType!=0&&m_nType!=6&&m_nType!=9)
	{
		int nYear(m_nYear);
		if (!m_bAD)
			nYear = -m_nYear;
		if (m_bJulian)
			d.SetJulian(nYear,m_nMonth+1,m_nDay);
		else
			d.Set(nYear,m_nMonth+1,m_nDay);
	}

	CDate d2;
	if (m_nType==3||m_nType==4||m_nType==6)
	{
		int nYear(m_nYear2);
		if (!m_bAD2)
			nYear = -m_nYear2;
		if (m_bJulian2)
			d2.SetJulian(nYear,m_nMonth2+1,m_nDay2);
		else
			d2.Set(nYear,m_nMonth2+1,m_nDay2);
	}

	m_dv.Set(t,d,d2,m_strPhrase);
}

static int rTypeData[] =
{
	0,1,3,4,2,8,7,9,10
};

void CEditDate::ValueToData()
{
	CDateValue::Type t;
	CDate d;
	CDate d2;
	m_dv.Get(t,d,d2,m_strPhrase);

	m_nType = rTypeData[(int)t];
	if (t==CDateValue::RANGE)
	{
		if (d && d2)
			ASSERT(m_nType==4);
		else if (d)
			m_nType = 5;
		else if (d2)
			m_nType = 6;
	}

	m_bJulian = d.IsJulian();
	if (m_bJulian)
		d.GetJulian(&m_nYear,&m_nMonth,&m_nDay);
	else
		d.Get(&m_nYear,&m_nMonth,&m_nDay);
	m_bAD = m_nYear >= 0;
	if (!m_bAD)
		m_nYear = -m_nYear;
	m_nMonth--;

	m_bJulian2 = d2.IsJulian();
	if (m_bJulian2)
		d2.GetJulian(&m_nYear2,&m_nMonth2,&m_nDay2);
	else
		d2.Get(&m_nYear2,&m_nMonth2,&m_nDay2);
	m_bAD2 = m_nYear2 >= 0;
	if (!m_bAD2)
		m_nYear2 = -m_nYear2;
	m_nMonth2--;
}

void CEditDate::SetStaticTitles()
{
	CString strAfter;
	CString strBefore;

	if (m_nType==1)
	{
		strAfter = "date";
	}
	else if (m_nType==2)
	{
		strAfter = "about";
	}
	else if (m_nType==3)
	{
		strAfter = "from";
		strBefore = "to";
	}
	else if (m_nType==4)
	{
		strAfter = "between";
		strBefore = "and";
	}
	else if (m_nType==5)
	{
		strAfter = "after";
	}
	else if (m_nType==6)
	{
		strBefore = "before";
	}
	else if (m_nType==7)
	{
		strAfter = "estimated as";
	}
	else if (m_nType==8)
	{
		strAfter = "calculated as";
	}
	else if (m_nType==10)
	{
		strAfter = "interpreted as";
	}

	m_buttonAfter.SetWindowText(strAfter);
	m_buttonBefore.SetWindowText(strBefore);

	m_statGED.SetWindowText(m_dv.Sort());
	m_statLong.SetWindowText(m_dv.Display(DATE_LONGDATE));
	m_statShort.SetWindowText(_T("")/*m_dv.Display(DATE_SHORTDATE)*/);
	RedrawWindow();
}

void CEditDate::Enable()
{
/*
	if (m_nType==9||m_nType==10)
	{
		m_editPhrase.EnableWindow();
	}
	else
	{
		m_strPhrase = _T("");
		m_editPhrase.SetWindowText(_T(""));
		m_editPhrase.EnableWindow(FALSE);
	}
*/
	if (m_nType==9||m_nType==10)
		m_editPhrase.ShowWindow(SW_SHOW);
	else
	{
		m_editPhrase.ShowWindow(SW_HIDE);
		m_strPhrase.Empty();
	}

	if (m_nType==0||m_nType==9)
	{
		EnableDate(FALSE);
		EnableDate2(FALSE);
	}

	if (m_nType==1||m_nType==2||m_nType==5||m_nType==7||m_nType==8||m_nType==10)
	{
		EnableDate();
		EnableDate2(FALSE);
	}

	if (m_nType==3||m_nType==4)
	{
		EnableDate();
		EnableDate2();
	}
	if (m_nType==6)
	{
		EnableDate(FALSE);
		EnableDate2();
	}
}

BEGIN_MESSAGE_MAP(CEditDate, CDialog)
	//{{AFX_MSG_MAP(CEditDate)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_DATETYPE,OnChange)
	ON_EN_CHANGE(IDC_PHRASETEXT,OnChange)
	ON_CBN_SELCHANGE(IDC_MONTH,OnChange)
	ON_CONTROL_RANGE(EN_CHANGE,IDC_DAY,IDC_YEAR,OnType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BC,IDC_AD,OnType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_GREGORIAN,IDC_JULIAN,OnCalendar)
	ON_CBN_SELCHANGE(IDC_MONTH2,OnChange)
	ON_CONTROL_RANGE(EN_CHANGE,IDC_DAY2,IDC_YEAR2,OnType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BC2,IDC_AD2,OnType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_GREGORIAN2,IDC_JULIAN2,OnCalendar2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDate message handlers

void CEditDate::OnChange() 
{
	UpdateData();
}

void CEditDate::OnType(UINT nID) 
{
	UpdateData();
}

void CEditDate::OnCalendar(UINT nID) 
{
	BOOL bToGreg = nID==IDC_GREGORIAN;
	DataToValue();
	CDateValue::Type type;
	CDate d;
	CDate d2;
	CString strPhrase;
	m_dv.Get(type,d,d2,strPhrase);
	if (d.GetJD())
	{
		int nYear, nMonth, nDay;
		if (d.IsJulian()&&bToGreg)
		{
			d.Get(&nYear,&nMonth,&nDay);
			d.Set(nYear,nMonth,nDay);
		}
		else
		{
			ASSERT(!d.IsJulian()&&!bToGreg);
			d.GetJulian(&nYear,&nMonth,&nDay);
			d.SetJulian(nYear,nMonth,nDay);
		}
		m_dv.Set(type,d,d2,strPhrase);
		UpdateData(FALSE);
	}
}

void CEditDate::OnCalendar2(UINT nID) 
{
	BOOL bToGreg = nID==IDC_GREGORIAN2;
	DataToValue();
	CDateValue::Type type;
	CDate d;
	CDate d2;
	CString strPhrase;
	m_dv.Get(type,d,d2,strPhrase);
	if (d2.GetJD())
	{
		int nYear, nMonth, nDay;
		if (d2.IsJulian()&&bToGreg)
		{
			d2.Get(&nYear,&nMonth,&nDay);
			d2.Set(nYear,nMonth,nDay);
		}
		else
		{
			ASSERT(!d2.IsJulian()&&!bToGreg);
			d2.GetJulian(&nYear,&nMonth,&nDay);
			d2.SetJulian(nYear,nMonth,nDay);
		}
		m_dv.Set(type,d,d2,strPhrase);
		UpdateData(FALSE);
	}
}

void CEditDate::EnableDate(BOOL bEnable)
{
	GetDlgItem(IDC_MONTH)->EnableWindow(bEnable);
	GetDlgItem(IDC_DAY)->EnableWindow(bEnable);
	GetDlgItem(IDC_YEAR)->EnableWindow(bEnable);
	GetDlgItem(IDC_BC)->EnableWindow(bEnable);
	GetDlgItem(IDC_AD)->EnableWindow(bEnable);
	GetDlgItem(IDC_JULIAN)->EnableWindow(bEnable);
	GetDlgItem(IDC_GREGORIAN)->EnableWindow(bEnable);
	if (!bEnable)
	{
		m_comboMonth.SetCurSel(-1);
		GetDlgItem(IDC_DAY)->SetWindowText(_T(""));
		GetDlgItem(IDC_YEAR)->SetWindowText(_T(""));
		m_nMonth = -1;
		m_nDay = 0;
		m_nYear = 0;
		DataToValue();
	}
}

void CEditDate::EnableDate2(BOOL bEnable)
{
	GetDlgItem(IDC_MONTH2)->EnableWindow(bEnable);
	GetDlgItem(IDC_DAY2)->EnableWindow(bEnable);
	GetDlgItem(IDC_YEAR2)->EnableWindow(bEnable);
	GetDlgItem(IDC_BC2)->EnableWindow(bEnable);
	GetDlgItem(IDC_AD2)->EnableWindow(bEnable);
	GetDlgItem(IDC_JULIAN2)->EnableWindow(bEnable);
	GetDlgItem(IDC_GREGORIAN2)->EnableWindow(bEnable);
	if (!bEnable)
	{
		m_comboMonth2.SetCurSel(-1);
		GetDlgItem(IDC_DAY2)->SetWindowText(_T(""));
		GetDlgItem(IDC_YEAR2)->SetWindowText(_T(""));
		m_nMonth2 = -1;
		m_nDay2 = 0;
		m_nYear2 = 0;
		DataToValue();
	}
}

/*
 0 Unknown
 1 Exact date
 2 Approximate date
 3 Period of time
 4 At some time within a range of dates
 5 At some time after a date
 6 At some time before a date
 7 Estimated date
 8 Calculated date
 9 Uninterpreted phrase
10 Interpreted phrase
*/

void CEditDate::OnOK() 
{
	if (m_dv.IsValid())
	{
		CDialog::OnOK();
	}
	else
	{
		AfxMessageBox(_T("You entered an invalid date. Please try again."));
	}
}
