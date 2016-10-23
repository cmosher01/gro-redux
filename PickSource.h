// PickSource.h : header file
//

class CGedtreeDoc;

/////////////////////////////////////////////////////////////////////////////
// CPickSource dialog

class CPickSource : public CDialog
{
// Construction
public:
	CPickSource(CGedtreeDoc* pDoc = NULL, CWnd* pParent = NULL, BOOL bEdit = FALSE);

// Dialog Data
	CGedtreeDoc* m_pDoc;
	int m_iSource;
	BOOL m_bEdit;
	//{{AFX_DATA(CPickSource)
	enum { IDD = IDD_SOURCES };
	CListCtrl	m_listSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickSource)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitList();
	void Reset();
	void CheckColumnWidth(int nCol, const CString& str);

	// Generated message map functions
	//{{AFX_MSG(CPickSource)
	afx_msg void OnNew();
	afx_msg void OnGetdispinfoSources(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	afx_msg void OnDblclkSources(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
