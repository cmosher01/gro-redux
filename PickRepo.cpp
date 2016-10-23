// PickRepo.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "PickRepo.h"
#include "EditRepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nColExtraWidth(15);
static const int nColMaxWidth(250);
static enum
{
	nColName,
	nColAddr
};

/////////////////////////////////////////////////////////////////////////////
// CPickRepo dialog


CPickRepo::CPickRepo(CGedtreeDoc* pDoc, CWnd* pParent, BOOL bEdit)
	: CDialog(CPickRepo::IDD, pParent),
	m_pDoc(pDoc),
	m_iRepository(-1),
	m_bEdit(bEdit)
{
	//{{AFX_DATA_INIT(CPickRepo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPickRepo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickRepo)
	DDX_Control(pDX, IDC_REPOS, m_listRepo);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
		InitList();

	if (pDX->m_bSaveAndValidate)
	{
		m_iRepository = -1;
		int i(-1);
		i = m_listRepo.GetNextItem(i,LVNI_SELECTED);
		if (i>=0)
		{
			m_iRepository = m_listRepo.GetItemData(i);
		}
	}
}


void CPickRepo::InitList()
{
	if (m_bEdit)
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	m_listRepo.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);

	LV_COLUMN lv;
	lv.mask = LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;

	lv.pszText = _T("Name");
	lv.iSubItem = nColName;
	lv.cx = m_listRepo.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listRepo.InsertColumn(lv.iSubItem,&lv);

	lv.pszText = _T("Address");
	lv.iSubItem = nColAddr;
	lv.cx = m_listRepo.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listRepo.InsertColumn(lv.iSubItem,&lv);

	Reset();
}

void CPickRepo::CheckColumnWidth(int nCol, const CString& str)
{
	int nWidth = m_listRepo.GetStringWidth(str)+nColExtraWidth;
	if (nWidth>nColMaxWidth)
		m_listRepo.SetColumnWidth(nCol,nColMaxWidth);
	else if (nWidth>m_listRepo.GetColumnWidth(nCol))
		m_listRepo.SetColumnWidth(nCol,nWidth);
}

void CPickRepo::Reset()
{
	m_listRepo.DeleteAllItems();

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	for (int i(0); i<m_pDoc->m_rRepository.GetSize(); i++)
	{
		CRepository& repo = m_pDoc->m_rRepository[i];
		if (repo.Exists())
		{
			lvi.iItem = i;
			lvi.lParam = i;
			VERIFY(m_listRepo.InsertItem(&lvi)>=0);

			CheckColumnWidth(nColName,repo.m_strName);
			CheckColumnWidth(nColAddr,repo.m_strAddr);
			if (i==m_iRepository)
				m_listRepo.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		}
	}
	m_listRepo.EnsureVisible(m_iRepository,FALSE);
}

BEGIN_MESSAGE_MAP(CPickRepo, CDialog)
	//{{AFX_MSG_MAP(CPickRepo)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_REPOS, OnGetdispinfoRepos)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_REPOS, OnDblclkRepos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickRepo message handlers
void CPickRepo::OnNew() 
{
	// TODO: Add your control notification handler code here

	CEditRepo d;

	HTREEITEM hti = m_pDoc->InsertChild("REPO");
	CRepository repo(m_pDoc,hti);

	d.m_repo = repo;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		repo = d.m_repo;

		int i = m_pDoc->m_rRepository.Add(repo);
		m_pDoc->m_rRepository[i].SetIndex(i);
		m_pDoc->m_rRepository[i].CalcID();

		m_iRepository = i;

		repo.PutToTree();
		Reset();
		m_pDoc->ResetAllViews();
	}
	else
		m_pDoc->DeleteItem(hti);

	m_listRepo.SetFocus();
}

void CPickRepo::OnGetdispinfoRepos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int iRepository = pDispInfo->item.lParam;
	CRepository& repo = m_pDoc->m_rRepository[iRepository];
	if (pDispInfo->item.mask&LVIF_TEXT)
	{
		switch (pDispInfo->item.iSubItem)
		{
		case nColName:
			pDispInfo->SetDispText(repo.m_strName);
		break;
		case nColAddr:
			pDispInfo->SetDispText(repo.m_strAddr);
		break;
		}
	}
	*pResult = 0;
}

void CPickRepo::OnEdit() 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_iRepository>=0)
	{
		CRepository& repo = m_pDoc->m_rRepository[m_iRepository];
		CEditRepo d;
		d.m_repo = repo;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			repo = d.m_repo;
			repo.PutToTree();
			Reset();
			m_pDoc->ResetAllViews();
		}
		else if (nButton==IDC_DELETE)
		{
			m_iRepository = -1;
			repo.Delete();
			Reset();
			m_pDoc->ResetAllViews();
		}
	}
	m_listRepo.SetFocus();
}

void CPickRepo::OnDblclkRepos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_iRepository>=0)
	{
//		if (m_bEdit)
			OnEdit();
//		else
//			OnOK();
	}

	*pResult = 0;
}

BOOL CPickRepo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDD_REPO),TRUE);
	return TRUE;
}
