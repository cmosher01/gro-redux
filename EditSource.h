// EditSource.h : header file
//

#include "sour.h"
//{{AFX_INCLUDES()
#include "webbrowser.h"
//}}AFX_INCLUDES

/////////////////////////////////////////////////////////////////////////////
// CEditSource dialog

class CEditSource : public CDialog
{
// Construction
public:
	CEditSource(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CSource m_sour;
	CString m_strDefaultDir;
	//{{AFX_DATA(CEditSource)
	enum { IDD = IDD_SOURCE };
	CTabCtrl	m_tab;
	CEdit	m_editPubl;
	CEdit	m_editTitle;
	CEdit	m_editAuthor;
	CEdit	m_editText;
	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CButton	m_buttonDelete;
	CStatic	m_staticRepo;
	CString	m_strAuthor;
	CString	m_strPubl;
	CString	m_strText;
	CString	m_strTitle;
	CWebBrowser	m_ie;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSource)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DataToValue();
	void ValueToData();
	void SetStaticTitles();
	void Enable();
	void PositionControls(int cx, int cy);
	void OnRefresh();
	void SetDefaultDir();
	void RestoreDefaultDir();
	void SetDir(const CString& s);

	// Generated message map functions
	//{{AFX_MSG(CEditSource)
	afx_msg void OnChangerepo();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNewWindow2Ie(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
