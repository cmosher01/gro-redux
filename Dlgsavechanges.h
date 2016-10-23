// DlgSaveChanges.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveChanges dialog

class CDlgSaveChanges : public CDialog
{
// Construction
public:
	CDlgSaveChanges(CString& strFileName, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSaveChanges)
	enum { IDD = IDD_SAVECHANGES };
	CString	m_strPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSaveChanges)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSaveChanges)
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_strFileName;
};
