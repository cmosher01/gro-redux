// PickSource.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "PickSource.h"
#include "EditSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nColExtraWidth(15);
static const int nColMaxWidth(250);
static enum
{
	nColTitle,
	nColAuthor,
	nColPublish,
	nColRepo
};

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

/////////////////////////////////////////////////////////////////////////////
// CPickSource dialog


CPickSource::CPickSource(CGedtreeDoc* pDoc, CWnd* pParent, BOOL bEdit)
	: CDialog(CPickSource::IDD, pParent),
	m_pDoc(pDoc),
	m_iSource(-1),
	m_bEdit(bEdit)
{
	//{{AFX_DATA_INIT(CPickSource)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPickSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickSource)
	DDX_Control(pDX, IDC_SOURCES, m_listSource);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
		InitList();

	if (pDX->m_bSaveAndValidate)
	{
		m_iSource = -1;
		m_pDoc->m_iSourceLast = m_iSource;
		int i(-1);
		i = m_listSource.GetNextItem(i,LVNI_SELECTED);
		if (i>=0)
		{
			m_iSource = m_listSource.GetItemData(i);
			m_pDoc->m_iSourceLast = m_iSource;
		}
	}
}

void CPickSource::InitList()
{
	if (m_bEdit)
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	m_listSource.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);

	LV_COLUMN lv;
	lv.mask = LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;

	lv.pszText = _T("Author");
	lv.iSubItem = nColAuthor;
	lv.cx = m_listSource.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listSource.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Title");
	lv.iSubItem = nColTitle;
	lv.cx = m_listSource.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listSource.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Publication");
	lv.iSubItem = nColPublish;
	lv.cx = m_listSource.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listSource.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Repository");
	lv.iSubItem = nColRepo;
	lv.cx = m_listSource.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listSource.InsertColumn(lv.iSubItem,&lv);

	if (m_pDoc->m_iSourceLast>=0)
		m_iSource = m_pDoc->m_iSourceLast;

	Reset();
}

void CPickSource::CheckColumnWidth(int nCol, const CString& str)
{
	int nWidth = m_listSource.GetStringWidth(str)+nColExtraWidth;
	if (nWidth>nColMaxWidth)
		m_listSource.SetColumnWidth(nCol,nColMaxWidth);
	else if (nWidth>m_listSource.GetColumnWidth(nCol))
		m_listSource.SetColumnWidth(nCol,nWidth);
}

void CPickSource::Reset()
{
	m_listSource.DeleteAllItems();

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	for (int i(0); i<m_pDoc->m_rSource.GetSize(); i++)
	{
		CSource& sour = m_pDoc->m_rSource[i];
		if (sour.Exists())
		{
			lvi.iItem = i;
			lvi.lParam = i;
			VERIFY(m_listSource.InsertItem(&lvi)>=0);

			CheckColumnWidth(nColAuthor,sour.m_strAuthor);
			CheckColumnWidth(nColTitle,sour.m_strTitle);
			CheckColumnWidth(nColPublish,sour.m_strPublish);
			if (sour.m_iRepository>=0)
			{
				CRepository& repo = m_pDoc->m_rRepository[sour.m_iRepository];
				CheckColumnWidth(nColRepo,repo.m_strName);
			}
			if (i==m_iSource)
				m_listSource.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		}
	}
	m_listSource.SortItems(SortItems,(LPARAM)m_pDoc);

	for (i = 0; i<m_pDoc->m_rSource.GetSize(); i++)
	{
		if ((int)m_listSource.GetItemData(i)==m_iSource)
		{
			m_listSource.EnsureVisible(i,FALSE);
			break;
		}
	}
}

BEGIN_MESSAGE_MAP(CPickSource, CDialog)
	//{{AFX_MSG_MAP(CPickSource)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SOURCES, OnGetdispinfoSources)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_SOURCES, OnDblclkSources)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGedtreeDoc* pDoc = (CGedtreeDoc*)lParamSort;

	CSource& sour1 = pDoc->m_rSource[lParam1];
	CSource& sour2 = pDoc->m_rSource[lParam2];

	return sour1.m_strTitle.CollateNoCase(sour2.m_strTitle);
}

/////////////////////////////////////////////////////////////////////////////
// CPickSource message handlers

void CPickSource::OnNew() 
{
	// TODO: Add your control notification handler code here
	
	CEditSource d;

	HTREEITEM hti = m_pDoc->InsertChild(_T("SOUR"));
	CSource sour(m_pDoc,hti);

	d.m_sour = sour;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		sour = d.m_sour;

		int i = m_pDoc->m_rSource.Add(sour);
		m_pDoc->m_rSource[i].SetIndex(i);
		m_pDoc->m_rSource[i].CalcID();

		m_iSource = i;
		m_pDoc->m_iSourceLast = m_iSource;

		sour.PutToTree();
		Reset();
		m_pDoc->ResetAllViews();
	}
	else
		m_pDoc->DeleteItem(hti);

	m_listSource.SetFocus();
}

void CPickSource::OnGetdispinfoSources(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int iSource = pDispInfo->item.lParam;
	CSource& sour = m_pDoc->m_rSource[iSource];
	if (pDispInfo->item.mask&LVIF_TEXT)
	{
		switch (pDispInfo->item.iSubItem)
		{
		case nColAuthor:
			pDispInfo->SetDispText(sour.m_strAuthor);
		break;
		case nColTitle:
			pDispInfo->SetDispText(sour.m_strTitle);
		break;
		case nColPublish:
			pDispInfo->SetDispText(sour.m_strPublish);
		break;
		case nColRepo:
			if (sour.m_iRepository>=0)
			{
				CRepository& repo = m_pDoc->m_rRepository[sour.m_iRepository];
				pDispInfo->SetDispText(repo.m_strName);
			}
			else
				pDispInfo->SetDispText(_T(""));
		break;
		}
	}
	*pResult = 0;
}

void CPickSource::OnEdit() 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_iSource>=0)
	{
		CSource& sour = m_pDoc->m_rSource[m_iSource];
		CEditSource d;
		d.m_sour = sour;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			sour = d.m_sour;
			sour.PutToTree();
			Reset();
			m_pDoc->ResetAllViews();
		}
		else if (nButton==IDC_DELETE)
		{
			m_iSource = -1;
			m_pDoc->m_iSourceLast = m_iSource;
			sour.Delete();
			Reset();
			m_pDoc->ResetAllViews();
		}
	}
	m_listSource.SetFocus();
}

void CPickSource::OnDblclkSources(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_iSource>=0)
	{
//		if (m_bEdit)
			OnEdit();
//		else
//			OnOK();
	}

	*pResult = 0;
}

BOOL CPickSource::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDD_SOURCE),TRUE);
	return TRUE;
}
