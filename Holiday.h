// Holiday.h : header file
//

#include "date.h"

/////////////////////////////////////////////////////////////////////////////
// CHoliday dialog

class CHoliday : public CDialog
{
// Construction
public:
	CHoliday(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHoliday)
	enum { IDD = IDD_HOLIDAY };
	CComboBox	m_comboHoliday;
	int		m_iHoliday;
	int		m_nYear;
	CString	m_strDate;
	//}}AFX_DATA
	CDate m_date;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoliday)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FixNullEdit(int nID);
	void FixZeroEdit(int nID);

	// Generated message map functions
	//{{AFX_MSG(CHoliday)
	afx_msg void OnSelchangeHoliday();
	afx_msg void OnChangeYear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
