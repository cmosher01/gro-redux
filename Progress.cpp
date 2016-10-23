// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CProgressDlg : public CDialog
{
public:
	BOOL m_bCancel;
	CProgressDlg() : m_bCancel(FALSE) { }
	virtual ~CProgressDlg() { DestroyWindow(); }
	void OnCancel() { m_bCancel = TRUE; }
};

/////////////////////////////////////////////////////////////////////////////
// CProgress

//IMPLEMENT_DYNCREATE(CProgress, CWinThread)

CProgress::CProgress():
	m_cItem(0),
	m_pDlg(0)
{
	m_bAutoDelete = FALSE;
	CreateThread();
}

CProgress::~CProgress()
{
	WaitForReady();

//	CProgressDlg* pd = m_pDlg;
	HANDLE hThread = m_hThread;
	DWORD nThreadID = m_nThreadID;

	do
		::PostThreadMessage(nThreadID,WM_QUIT,ERROR_SUCCESS,NULL);
	while (::WaitForSingleObject(hThread,10)==WAIT_TIMEOUT);

//	if (pd) delete pd;
}

BOOL CProgress::InitInstance()
{
	ASSERT(!m_pDlg);
	m_pDlg = new CProgressDlg;
	m_pDlg->Create(IDD_PROGRESS,CWnd::GetDesktopWindow());
	m_pDlg->CenterWindow(AfxGetMainWnd());
	m_pDlg->SetDefID(IDCANCEL);
	m_pDlg->ShowWindow(SW_SHOW);
	m_pDlg->SetForegroundWindow();

	m_progress.SubclassWindow(m_pDlg->GetDlgItem(IDC_PROGRESS)->GetSafeHwnd());
	m_staticDetail.SubclassWindow(m_pDlg->GetDlgItem(IDC_DETAIL)->GetSafeHwnd());

	m_progress.SetFocus();
	m_progress.SetPos(0);

	Ready();

	return TRUE;
}

int CProgress::ExitInstance()
{
	delete m_pDlg;
	return CWinThread::ExitInstance();
}

void CProgress::Reset(const CString& strMessage, int cItem)
{
	WaitForReady();

	if (cItem)
	{
		m_cItem = cItem;
		m_progress.SetPos(0);
	}

	m_pDlg->SetDlgItemText(IDC_MESSAGE,strMessage);
	m_staticDetail.SetWindowText(_T(""));

	Ready();
}

void CProgress::Set(int cFinished, CString strDetail)
{
	WaitForReady();

	ASSERT(0<cFinished && cFinished<=m_cItem);

	if (m_cItem)
	{
		double rFractionDone = (double)cFinished/(double)m_cItem;
		int nPercentDone = (int)(rFractionDone*100);
		m_progress.SetPos(nPercentDone);
	}
	if (!strDetail.IsEmpty())
		m_staticDetail.SetWindowText(strDetail);

	BOOL bCancel = m_pDlg->m_bCancel;

	Ready();

	if (bCancel) AfxThrowUserException();
}

BEGIN_MESSAGE_MAP(CProgress, CWinThread)
	//{{AFX_MSG_MAP(CProgress)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgress message handlers
