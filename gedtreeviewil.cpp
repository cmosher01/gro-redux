// gedtreeViewIL.cpp : implementation of the CGedtreeViewIL class
//

#include "stdafx.h"
#include "gedtree.h"

#include "gedtreeDoc.h"
#include "gedtreeViewIL.h"
#include "editevent.h"
#include "editname.h"
#include "editsex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nColExtraWidth(15);
enum
{
	nColSurname,
	nColGivenName,
	nColBirth,
	nColDeath
};
struct data
{
	int iIndi;
	CDateValue dBirth, dDeath;
	data(int i, CDateValue b, CDateValue d):
		iIndi(i), dBirth(b), dDeath(d) { }
	virtual ~data() {}
};

static CGedtreeDoc* pDoc;
static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL

IMPLEMENT_DYNCREATE(CGedtreeViewIL, CGedtreeView)

BEGIN_MESSAGE_MAP(CGedtreeViewIL, CGedtreeView)
	//{{AFX_MSG_MAP(CGedtreeViewIL)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_INDI, OnViewIndi)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INDI, OnUpdateViewIndi)
	ON_COMMAND(ID_VIEW_OPENPEDIGREE, OnViewOpenpedigree)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENPEDIGREE, OnUpdateViewOpenpedigree)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INDEXOFINDIVIDUALS, OnUpdateViewIndexofindividuals)
	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_NOTIFY(LVN_GETDISPINFO, 1024, OnGetdispinfoList)
	ON_NOTIFY(LVN_COLUMNCLICK, 1024, OnColumnclickList)
	ON_NOTIFY(LVN_KEYDOWN, 1024, OnKeydownList)
	ON_NOTIFY(NM_DBLCLK, 1024, OnDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL construction/destruction

CGedtreeViewIL::CGedtreeViewIL():
	m_bInit(FALSE)
{
}

CGedtreeViewIL::~CGedtreeViewIL()
{
}

BOOL CGedtreeViewIL::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

CString CGedtreeViewIL::GetWindowTitle()
{
	return "Index of Individuals";
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL drawing

void CGedtreeViewIL::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL printing

BOOL CGedtreeViewIL::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGedtreeViewIL::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGedtreeViewIL::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL diagnostics

#ifdef _DEBUG
void CGedtreeViewIL::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGedtreeViewIL::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIL message handlers

void CGedtreeViewIL::Init()
{
	Reset();
}

void CGedtreeViewIL::OnInitialUpdate() 
{
	CGedtreeView::OnInitialUpdate();

	if (!m_bInit) InitCtrls();

	CRect r;
	GetClientRect(&r);

	if (m_bInit)
		PositionControls(r.right,r.bottom);
	else
		SetScrollSizes(MM_TEXT,r.Size());

	m_bInit= TRUE;
}

void CGedtreeViewIL::InitCtrls()
{
	m_list.Create(WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL,
		CRect(0,0,1,1),this,1024);

	m_list.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);

	LV_COLUMN lv;
	lv.mask = LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;

	lv.pszText = _T("Surname");
	lv.iSubItem = nColSurname;
	lv.cx = m_list.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_list.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Given Name");
	lv.iSubItem = nColGivenName;
	lv.cx = m_list.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_list.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Birth Date");
	lv.iSubItem = nColBirth;
	lv.cx = m_list.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_list.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Death Date");
	lv.iSubItem = nColDeath;
	lv.cx = m_list.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_list.InsertColumn(lv.iSubItem,&lv);
}

void CGedtreeViewIL::CheckColumnWidth(int nCol, const CString& str, BOOL bForce)
{
	int nWidth = m_list.GetStringWidth(str)+nColExtraWidth;
	if (bForce || nWidth>m_list.GetColumnWidth(nCol))
		m_list.SetColumnWidth(nCol,nWidth);
}

void CGedtreeViewIL::PositionControls(int cx, int cy)
{
	SetRedraw(FALSE);

	SetScrollSizes(MM_TEXT,CSize(cx,cy));

	CRect rectClient;
	GetClientRect(rectClient);
	m_list.MoveWindow(rectClient);

	SetRedraw();
	Invalidate();
}

void CGedtreeViewIL::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)
	{
		if (m_list.m_hWnd)
		{
			PositionControls(cx,cy);
		}
	}
}

void CGedtreeViewIL::OnDestroy() 
{
	CGedtreeView::OnDestroy();

	DeleteData();

	GetDocument()->m_pIndiList = NULL;
}

void CGedtreeViewIL::Reset(UINT flagsChanged)
{
	SetRedraw(FALSE);

	if (flagsChanged & font)
	{
		m_list.SetFont(&theApp.m_font);

		CheckColumnWidth(nColSurname,"Surname",TRUE);
		CheckColumnWidth(nColGivenName,"Given Name",TRUE);
		CheckColumnWidth(nColBirth,"Birth Date",TRUE);
		CheckColumnWidth(nColDeath,"Death Date",TRUE);

		CRect r;
		GetClientRect(&r);
		PositionControls(r.right,r.bottom);
		SetRedraw(FALSE);
	}

	if (flagsChanged & (lists|font))
	{
		Fill();
	}

	SetRedraw();
	Invalidate();
}

void CGedtreeViewIL::Fill()
{
	DeleteData();
	m_list.DeleteAllItems();

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	for (int i(0); i<GetDocument()->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = GetDocument()->m_rIndividual[i];
		lvi.iItem = i;
		lvi.pszText = LPSTR_TEXTCALLBACK;

		CDateValue dBirth, dDeath;
		for (int j(0); j<indi.m_revt.GetSize(); j++)
		{
			CEvt& evt = indi.m_revt[j];
			if (evt.m_strTypeTok=="BIRT")
				dBirth = evt.m_dvDate;
			else if (evt.m_strTypeTok=="DEAT")
				dDeath = evt.m_dvDate;
		}

		lvi.lParam = (LONG)new data(i,dBirth,dDeath);
		m_list.InsertItem(&lvi);

		CheckColumnWidth(nColSurname,indi.m_name.m_strSurname);
		CheckColumnWidth(nColGivenName,indi.m_name.m_strRest);
		CheckColumnWidth(nColBirth,dBirth.Sort());
		CheckColumnWidth(nColDeath,dDeath.Sort());
	}

	pDoc = GetDocument();
	m_list.SortItems(SortItems,nColGivenName);
	m_list.SortItems(SortItems,nColSurname);
	m_list.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
}


void CGedtreeViewIL::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	data* pp = (data*)pDispInfo->item.lParam;
	CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
	if (pDispInfo->item.mask&LVIF_TEXT)
	{
		switch (pDispInfo->item.iSubItem)
		{
		case nColSurname:
			pDispInfo->SetDispText(indi.m_name.m_strSurname);
		break;
		case nColGivenName:
			pDispInfo->SetDispText(indi.m_name.m_strRest);
		break;
		case nColBirth:
			pDispInfo->SetDispText(pp->dBirth.Sort());
		break;
		case nColDeath:
			pDispInfo->SetDispText(pp->dDeath.Sort());
		break;
		}
	}
	*pResult = 0;
}

void CGedtreeViewIL::DeleteData()
{
	for (int i(0); i<m_list.GetItemCount(); i++)
		delete (data*)m_list.GetItemData(i);
}

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	data* pp1 = (data*)lParam1;
	data* pp2 = (data*)lParam2;
	CIndividual& indi1 = pDoc->m_rIndividual[pp1->iIndi];
	CIndividual& indi2 = pDoc->m_rIndividual[pp2->iIndi];

	CString s1, s2;
	switch (lParamSort) // iSubItem
	{
		case nColSurname:
			s1 = indi1.m_name.m_strSurname;
			s2 = indi2.m_name.m_strSurname;
		break;
		case nColGivenName:
			s1 = indi1.m_name.m_strRest;
			s2 = indi2.m_name.m_strRest;
		break;
		case nColBirth:
			s1 = pp1->dBirth.Sort();
			s2 = pp2->dBirth.Sort();
		break;
		case nColDeath:
			s1 = pp1->dDeath.Sort();
			s2 = pp2->dDeath.Sort();
		break;
	}

	if (!s1.IsEmpty() && !s2.IsEmpty())
		return s1.CollateNoCase(s2);

	// Handle empty strings specially, so they will
	// sort to the bottom (instead of the top as usual):
	else if (!s1.IsEmpty())
		return -1; // s2 is empty, so s2 is greater than s1
	else if (!s2.IsEmpty())
		return 1; // s1 is empty, so s1 is greater than s2
	else
		return 0;
}

void CGedtreeViewIL::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	pDoc = GetDocument();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_list.SortItems(SortItems,pNMListView->iSubItem);
	*pResult = 0;
}

void CGedtreeViewIL::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnViewIndi();
}

void CGedtreeViewIL::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	if (pLVKeyDown->wVKey==VK_RETURN)
		OnDblclk(NULL,pResult);
}

void CGedtreeViewIL::OnSetFocus(CWnd* pOldWnd) 
{
	CGedtreeView::OnSetFocus(pOldWnd);
	m_list.SetFocus();
}

void CGedtreeViewIL::OnViewIndi() 
{
	int i(-1);
	i = m_list.GetNextItem(i,LVNI_SELECTED);
	if (i>=0)
	{
		data* pp = (data*)m_list.GetItemData(i);
		CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
		indi.OpenView();
	}
}

void CGedtreeViewIL::OnUpdateViewIndi(CCmdUI* pCmdUI) 
{
	int i(-1);
	i = m_list.GetNextItem(i,LVNI_SELECTED);
	pCmdUI->Enable(i>=0);
}

void CGedtreeViewIL::OnViewOpenpedigree() 
{
	int i(-1);
	i = m_list.GetNextItem(i,LVNI_SELECTED);
	if (i>=0)
	{
		data* pp = (data*)m_list.GetItemData(i);
		CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
		indi.OpenPedigree();
	}
}

void CGedtreeViewIL::OnUpdateViewOpenpedigree(CCmdUI* pCmdUI) 
{
	int i(-1);
	i = m_list.GetNextItem(i,LVNI_SELECTED);
	pCmdUI->Enable(i>=0);
}

void CGedtreeViewIL::OnUpdateViewIndexofindividuals(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
