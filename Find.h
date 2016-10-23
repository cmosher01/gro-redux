// Find.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFind dialog

class CFind : public CDialog
{
// Construction
public:
	CFind(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFind)
	enum { IDD = IDD_FIND };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFind)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFind)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
