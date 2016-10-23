// Progress.h : header file
//

class CProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CProgress thread

class CProgress : public CWinThread
{
//	DECLARE_DYNCREATE(CProgress)
public:
	CProgress();
	virtual ~CProgress();

// Attributes
protected:
	int m_cItem;
	CProgressCtrl m_progress;
	CProgressDlg* m_pDlg;
	CEvent m_evtReady;
	CStatic m_staticDetail;
public:

// Operations
public:
	void Reset(const CString& strMessage = "", int cItem = 0);
	void Set(int cFinished, CString strDetail = "");

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgress)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Ready() { m_evtReady.SetEvent(); }
	void WaitForReady() { CSingleLock waitReady(&m_evtReady,TRUE); }

	// Generated message map functions
	//{{AFX_MSG(CProgress)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
