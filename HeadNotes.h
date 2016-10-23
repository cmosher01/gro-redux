// HeadNotes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeadNotes dialog

class CHeadNotes : public CDialog
{
// Construction
public:
	CHeadNotes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeadNotes)
	enum { IDD = IDD_NOTES };
	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CButton	m_buttonDelete;
	CEdit	m_editText;
	CString	m_strText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeadNotes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void PositionControls(int cx, int cy);

	// Generated message map functions
	//{{AFX_MSG(CHeadNotes)
	afx_msg void OnDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
