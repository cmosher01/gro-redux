// UnicodeSel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnicodeSel dialog

class CUnicodeSel : public CDialog
{
// Construction
public:
	CUnicodeSel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnicodeSel)
	enum { IDD = IDD_UNICODE };
	BOOL m_bUnicode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnicodeSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ReadFromRegistry();
	void WriteToRegistry();

	// Generated message map functions
	//{{AFX_MSG(CUnicodeSel)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
