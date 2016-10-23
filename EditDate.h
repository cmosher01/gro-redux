// EditDate.h : header file
//

#include "datevalue.h"

/////////////////////////////////////////////////////////////////////////////
// CEditDate dialog

class CEditDate : public CDialog
{
// Construction
public:
	CEditDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CDateValue m_dv;
	//{{AFX_DATA(CEditDate)
	enum { IDD = IDD_DATE };
	CStatic	m_statShort;
	CStatic	m_statLong;
	CStatic	m_statGED;
	CEdit	m_editPhrase;
	CButton	m_buttonBefore;
	CButton	m_buttonAfter;
	CComboBox	m_comboMonth2;
	CComboBox	m_comboMonth;
	CString	m_strPhrase;
	int		m_nMonth;
	int		m_nDay;
	int		m_nYear;
	int		m_bAD;
	int		m_bJulian;
	int		m_nMonth2;
	int		m_nDay2;
	int		m_nYear2;
	int		m_bAD2;
	int		m_bJulian2;
	int		m_nType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FixNullEdit(int nID);
	void FixZeroEdit(int nID);
	void DataToValue();
	void ValueToData();
	void SetStaticTitles();
	void Enable();
	void EnableDate(BOOL bEnable = TRUE);
	void EnableDate2(BOOL bEnable = TRUE);
	void FillMonths();

	// Generated message map functions
	//{{AFX_MSG(CEditDate)
	afx_msg void OnChange();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnType(UINT nID);
	afx_msg void OnCalendar(UINT nID);
	afx_msg void OnCalendar2(UINT nID);

	DECLARE_MESSAGE_MAP()
};
