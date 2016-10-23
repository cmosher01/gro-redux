// PickRepo.h : header file
//

class CGedtreeDoc;

/////////////////////////////////////////////////////////////////////////////
// CPickRepo dialog

class CPickRepo : public CDialog
{
// Construction
public:
	CPickRepo(CGedtreeDoc* pDoc = NULL, CWnd* pParent = NULL, BOOL bEdit = FALSE);   // standard constructor

// Dialog Data
	CGedtreeDoc* m_pDoc;
	int m_iRepository;
	BOOL m_bEdit;
	//{{AFX_DATA(CPickRepo)
	enum { IDD = IDD_REPOS };
	CListCtrl	m_listRepo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickRepo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitList();
	void Reset();
	void CheckColumnWidth(int nCol, const CString& str);

	// Generated message map functions
	//{{AFX_MSG(CPickRepo)
	afx_msg void OnNew();
	afx_msg void OnGetdispinfoRepos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	afx_msg void OnDblclkRepos(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
