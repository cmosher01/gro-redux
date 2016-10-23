// Setup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetup dialog

class CSetup : public CDialog
{
// Construction
public:
	CSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetup)
	enum { IDD = IDD_SETUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetup)
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUninstall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
