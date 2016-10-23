// DocSize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocSize dialog

class CDocSize : public CDialog
{
// Construction
public:
	CDocSize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDocSize)
	enum { IDD = IDD_DOCSIZE };
	//}}AFX_DATA
	int		m_nWidth;
	int		m_nHeight;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocSize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_nMinWidth;
	int		m_nMinHeight;

	// Generated message map functions
	//{{AFX_MSG(CDocSize)
	afx_msg void OnNormalize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
