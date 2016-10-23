// EditSubm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditSubm dialog

class CEditSubm : public CDialog
{
// Construction
public:
	CEditSubm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditSubm)
	enum { IDD = IDD_SUBM };
	CString	m_strName;
	CString	m_strAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSubm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ReadFromRegistry();
	void WriteToRegistry();

	// Generated message map functions
	//{{AFX_MSG(CEditSubm)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
