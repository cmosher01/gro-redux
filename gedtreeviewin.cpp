// gedtreeViewIN.cpp : implementation of the CGedtreeViewIN class
//

#include "stdafx.h"
#include "gedtree.h"

#include "gedtreeDoc.h"
#include "gedtreeViewIN.h"
#include "gedtreeViewDL.h"
#include "gedtreeViewPD.h"
#include "editevent.h"
#include "editname.h"
#include "editsex.h"
#include "childfrm.h"
#include "reg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CSize MARGIN(10,10);
const CRect TAB_MARGIN(1,22,2,2);

static const int nColExtraWidth(15);
static enum
{
	nColEvent,
	nColDate,
	nColPlace,
	nColSource
};
static enum
{
	nColPName,
	nColPEvent,
	nColPDate,
	nColPPlace
};
static enum
{
	nColAAttr,
	nColAVal,
	nColADate,
	nColAPlace
};
struct partner
{
	int iIndi, iEvent, iFami;
	CString strName, strEvt;
	partner(int i, int j, const CString& s, int iF = -1, const CString& sEvt = _T("")):
		iIndi(i), iEvent(j), strName(s), iFami(iF), strEvt(sEvt) { }
};
struct evtData
{
	int iEvent;
	CString strCita;
	CString strSort;
	evtData(int i, const CString& sCita, const CString& sSort):
		iEvent(i), strCita(sCita), strSort(sSort) { }
};
struct attrData
{
	int iAttr;
	CString strSort;
	attrData(int i, const CString& sSort):
		iAttr(i), strSort(sSort) { }
};

static enum
{
	nTabEvent,
	nTabAttr,
	nTabAnc,
	nTabPart
};

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK SortAItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN

IMPLEMENT_DYNCREATE(CGedtreeViewIN, CGedtreeView)

BEGIN_MESSAGE_MAP(CGedtreeViewIN, CGedtreeView)
	//{{AFX_MSG_MAP(CGedtreeViewIN)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_DROPLINECHART, OnViewDroplinechart)
	ON_COMMAND(ID_VIEW_OPENPEDIGREE, OnViewOpenpedigree)
	ON_COMMAND(ID_VIEW_INDI, OnViewIndi)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INDI, OnUpdateViewIndi)
	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_NOTIFY(TVN_ITEMEXPANDING, 1024, OnExpandPedigree)
	ON_NOTIFY(NM_DBLCLK, 1024, OnSelectPedigree)
	ON_NOTIFY(LVN_GETDISPINFO, 1025, OnGetdispinfoEventlist)
	ON_NOTIFY(NM_DBLCLK, 1025, OnEditEvent)
	ON_NOTIFY(LVN_GETDISPINFO, 1026, OnGetdispinfoPartnerlist)
	ON_NOTIFY(NM_DBLCLK, 1026, OnSelectPartner)
	ON_NOTIFY(LVN_GETDISPINFO, 1028, OnGetdispinfoAttrlist)
	ON_NOTIFY(NM_DBLCLK, 1028, OnSelectAttr)
	ON_NOTIFY(TCN_SELCHANGE, 1027, OnSelectTab)
	ON_BN_CLICKED(1029, OnName)
	ON_BN_CLICKED(1030, OnSex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN construction/destruction

CGedtreeViewIN::CGedtreeViewIN():
	m_iIndi(-1),
	m_bInit(FALSE),
	m_bFontTooHigh(FALSE),
	m_bBigFontTooHigh(FALSE)
{
}

CGedtreeViewIN::~CGedtreeViewIN()
{
}

BOOL CGedtreeViewIN::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

CString CGedtreeViewIN::GetWindowTitle()
{
	CString s;
	if (m_iIndi>=0)
	{
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		s = indi.Name();
	}
	return s;
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN drawing

void CGedtreeViewIN::OnDraw(CDC* pDC)
{
	CGedtreeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];

	CRect rectClient;
	GetClientRect(rectClient);
	COLORREF colorBk = ::GetSysColor(COLOR_3DFACE);
	pDC->FillSolidRect(rectClient,colorBk);

	COLORREF colorText = ::GetSysColor(COLOR_WINDOWTEXT);
	pDC->SetTextColor(colorText);

	UINT nDrawFormat(DT_SINGLELINE|DT_NOCLIP|DT_NOPREFIX);
	UINT nDrawFormatBig(nDrawFormat);
	nDrawFormat |= (m_bFontTooHigh) ? DT_TOP : DT_VCENTER;
	nDrawFormatBig |= (m_bBigFontTooHigh) ? DT_TOP : DT_VCENTER;

	CRect r(m_rectName);
	pDC->SelectObject(&theApp.m_fontBigBold);
	pDC->DrawText(indi.Name(),r,nDrawFormatBig);

	r = m_rectSex;
	pDC->SelectObject(&theApp.m_font);
	pDC->DrawText(indi.Sex(),r,nDrawFormat);
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN printing

BOOL CGedtreeViewIN::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGedtreeViewIN::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGedtreeViewIN::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN diagnostics

#ifdef _DEBUG
void CGedtreeViewIN::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGedtreeViewIN::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewIN message handlers

void CGedtreeViewIN::Init(int iIndi)
{
	m_iIndi = iIndi;
	Reset();
}

void CGedtreeViewIN::AddParents(HTREEITEM htvi, int iIndi, int cDepth)
{
	CString strFather;//("Father:   ");
	CString strMother;//("Mother:  ");
	int iFather(-1);
	int iMother(-1);

	if (iIndi>=0)
	{
		CIndividual& indi = GetDocument()->m_rIndividual[iIndi];

		CIndividual* pindiFather = NULL;
		iFather = indi.m_iFather;
		if (iFather>=0)
			pindiFather = &(GetDocument()->m_rIndividual[iFather]);
		if (pindiFather)
			strFather += pindiFather->Name();
		else
			strFather.Empty();// += "[unknown]";

		CIndividual* pindiMother = NULL;
		iMother = indi.m_iMother;
		if (iMother>=0)
			pindiMother = &(GetDocument()->m_rIndividual[iMother]);
		if (pindiMother)
			strMother += pindiMother->Name();
		else
			strMother.Empty();// += "[unknown]";
	}

	// remove all child items (the father and the mother)
	if (htvi==TVI_ROOT)
	{
		HTREEITEM htviSub;
		while (htviSub = m_treePedigree.GetRootItem())
			m_treePedigree.DeleteItem(htviSub);
	}
	else
	{
		HTREEITEM htviSub;
		while (htviSub = m_treePedigree.GetChildItem(htvi))
			m_treePedigree.DeleteItem(htviSub);
	}

	if (!strFather.IsEmpty())
	{
		HTREEITEM htviFather = m_treePedigree.InsertItem(strFather,htvi);
		m_treePedigree.SetItemData(htviFather,iFather);
		if (cDepth&&iFather>=0)
			AddParents(htviFather,iFather,cDepth-1);
	}

	if (!strMother.IsEmpty())
	{
		HTREEITEM htviMother = m_treePedigree.InsertItem(strMother,htvi);
		m_treePedigree.SetItemData(htviMother,iMother);
		if (cDepth&&iMother>=0)
			AddParents(htviMother,iMother,cDepth-1);
	}
}

void CGedtreeViewIN::OnExpandPedigree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
	if (pnmtv->action==TVE_EXPAND)
	{
		HTREEITEM htvi = pnmtv->itemNew.hItem;
		int i = m_treePedigree.GetItemData(htvi);
		AddParents(htvi,i);
	}
}

void CGedtreeViewIN::OnSelectPedigree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM htvi = m_treePedigree.GetSelectedItem();
	int i = m_treePedigree.GetItemData(htvi);
	if (i>-1)
	{
		CIndividual& indi = GetDocument()->m_rIndividual[i];
		indi.OpenView();
	}
}

void CGedtreeViewIN::OnSelectTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	CRect r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
}

void CGedtreeViewIN::InitCtrls()
{
	DWORD styleBase = WS_VISIBLE;
	m_listEvent.Create(styleBase|LVS_REPORT|LVS_SINGLESEL,
		CRect(0,0,1,1),this,1025);
	m_listPartner.Create(styleBase|LVS_REPORT|LVS_SINGLESEL,
		CRect(0,0,1,1),this,1026);
	m_treePedigree.Create(styleBase|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,
		CRect(0,0,1,1),this,1024);
	m_listAttr.Create(styleBase|LVS_REPORT|LVS_SINGLESEL,
		CRect(0,0,1,1),this,1028);
	m_tab.Create(WS_VISIBLE,CRect(0,0,1,1),this,1027);

	m_listEvent.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);
	m_listPartner.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);
	m_listAttr.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);

	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = _T("Events");
	m_tab.InsertItem(nTabEvent,&tci);
	tci.pszText = _T("Attributes");
	m_tab.InsertItem(nTabAttr,&tci);
	tci.pszText = _T("Ancestors");
	m_tab.InsertItem(nTabAnc,&tci);
	tci.pszText = _T("Partnerships");
	m_tab.InsertItem(nTabPart,&tci);


	InitButton(m_buttonName,"name",1029);
	InitButton(m_buttonSex,"gender",1030);

	LV_COLUMN lv;
	lv.mask = LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;

	lv.pszText = _T("Event");
	lv.iSubItem = nColEvent;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Date");
	lv.iSubItem = nColDate;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Place");
	lv.iSubItem = nColPlace;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Source");
	lv.iSubItem = nColSource;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);

	lv.pszText = _T("Attribute");
	lv.iSubItem = nColAAttr;
	lv.cx = m_listAttr.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listAttr.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Value");
	lv.iSubItem = nColAVal;
	lv.cx = m_listAttr.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listAttr.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Date");
	lv.iSubItem = nColADate;
	lv.cx = m_listAttr.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listAttr.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Place");
	lv.iSubItem = nColAPlace;
	lv.cx = m_listAttr.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listAttr.InsertColumn(lv.iSubItem,&lv);

	lv.pszText = _T("Name");
	lv.iSubItem = nColPName;
	lv.cx = m_listPartner.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listPartner.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Event");
	lv.iSubItem = nColPEvent;
	lv.cx = m_listPartner.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listPartner.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Date");
	lv.iSubItem = nColPDate;
	lv.cx = m_listPartner.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listPartner.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Place");
	lv.iSubItem = nColPPlace;
	lv.cx = m_listPartner.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listPartner.InsertColumn(lv.iSubItem,&lv);
}

void CGedtreeViewIN::InitButton(CButton& button, const CString& strName, int nID)
{
	CFont* pFontButton = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));

	button.Create(strName,WS_VISIBLE,CRect(0,0,1,1),this,nID);
	button.SetFont(pFontButton);
}

void CGedtreeViewIN::OnInitialUpdate() 
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

void CGedtreeViewIN::FillEvents()
{
	DeleteEventData();
	m_listEvent.DeleteAllItems();

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	for (int i(0); i<indi.m_revt.GetSize(); i++)
	{
		CEvt& evt = indi.m_revt[i];
		lvi.iItem = i;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LONG)new evtData(i,evt.m_cita.Display(),evt.m_dvDate.Sort());
		int ret = m_listEvent.InsertItem(&lvi);
		if (ret==-1)
			::AfxMessageBox(L"error inserting event into list");
		CheckColumnWidth(nColEvent,evt.m_strType);
		CheckColumnWidth(nColDate,evt.m_dvDate.Sort());
		CheckColumnWidth(nColPlace,evt.m_strPlace);
		CheckColumnWidth(nColSource,evt.m_cita.Display());
	}
	i = indi.m_revt.GetSize();
	lvi.iItem = i;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.lParam = (LONG)new evtData(-1,"","");
	int ret = m_listEvent.InsertItem(&lvi);
	if (ret==-1)
		::AfxMessageBox(L"error inserting event into list");
	CheckColumnWidth(nColEvent,"<new event>");
	m_listEvent.SortItems(SortItems,0);
}

void CGedtreeViewIN::CheckColumnWidth(int nCol, const CString& str, BOOL bForce)
{
	int nWidth = m_listEvent.GetStringWidth(str)+nColExtraWidth;
	if (bForce || nWidth>m_listEvent.GetColumnWidth(nCol))
		m_listEvent.SetColumnWidth(nCol,nWidth);
}

void CGedtreeViewIN::FillPartners()
{
	int iItem(0);
	int i,j,k;

	DeletePartnerData();
	m_listPartner.DeleteAllItems();

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];

	CArray<int,int> riSpouseToFamily;
	indi.GetSortedSpouseFamilies(riSpouseToFamily);

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	for (i = 0; i<riSpouseToFamily.GetSize(); i++)
	{
		int iFami = riSpouseToFamily[i];
		CFamily& fami = GetDocument()->m_rFamily[iFami];

		int iSpouse;
		if (fami.m_iHusband==m_iIndi)
			iSpouse = fami.m_iWife;
		else
		{
			ASSERT(fami.m_iWife==m_iIndi);
			iSpouse = fami.m_iHusband;
		}
		if (iSpouse>=0)
		{
			CIndividual& indiSpouse = GetDocument()->m_rIndividual[iSpouse];

			int iBirth(-1), iDeath(-1);
			for (int k(0); k<indiSpouse.m_revt.GetSize(); k++)
			{
				CEvt& evt = indiSpouse.m_revt[k];

				if (evt.m_strTypeTok=="BIRT")
				{
					iBirth = k;
				}
				else if (evt.m_strTypeTok=="DEAT")
				{
					iDeath = k;
				}
			}

			lvi.iItem = iItem++;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LONG)new partner(iSpouse,iBirth,indiSpouse.Name(),-1,_T("birth"));
			m_listPartner.InsertItem(&lvi);
			CheckPColumnWidth(nColPName,indiSpouse.Name());
			if (iBirth>-1)
			{
				CEvt& evt = indiSpouse.m_revt[iBirth];
				CheckPColumnWidth(nColPEvent,evt.m_strType);
				CheckPColumnWidth(nColPDate,evt.m_dvDate.Sort());
				CheckPColumnWidth(nColPPlace,evt.m_strPlace);
			}
			else
			{
				CheckPColumnWidth(nColPEvent,_T("birth"));
			}
			lvi.iItem = iItem++;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LONG)new partner(iSpouse,iDeath,_T(""),-1,_T("death"));
			m_listPartner.InsertItem(&lvi);
			if (iDeath>-1)
			{
				CEvt& evt = indiSpouse.m_revt[iDeath];
				CheckPColumnWidth(nColPEvent,evt.m_strType);
				CheckPColumnWidth(nColPDate,evt.m_dvDate.Sort());
				CheckPColumnWidth(nColPPlace,evt.m_strPlace);
			}
			else
			{
				CheckPColumnWidth(nColPEvent,_T("death"));
			}
		}
		else
		{
			lvi.iItem = iItem++;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LONG)new partner(-1,-1,_T("[unknown partner]"));
			m_listPartner.InsertItem(&lvi);
			CheckPColumnWidth(nColPName,"[unknown partner]");
		}

		if (!fami.m_revt.GetSize())
		{
			lvi.iItem = iItem++;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LONG)new partner(iSpouse,-1,_T("    partnership events:"),iFami);
			m_listPartner.InsertItem(&lvi);
			CheckPColumnWidth(nColPName,_T("  partnership events:"));
		}
		else
		{
			for (k = 0; k<fami.m_revt.GetSize(); k++)
			{
				CEvt& evt = fami.m_revt[k];

				lvi.iItem = iItem++;
				lvi.pszText = LPSTR_TEXTCALLBACK;
				lvi.lParam = (LONG)new partner(iSpouse,k,k?_T(""):_T("    partnership events:"),iFami);
				m_listPartner.InsertItem(&lvi);

				if (!k) CheckPColumnWidth(nColPName,_T("    partnership events:"));
				CheckPColumnWidth(nColPEvent,evt.m_strType);
				CheckPColumnWidth(nColPDate,evt.m_dvDate.Sort());
				CheckPColumnWidth(nColPPlace,evt.m_strPlace);
			}
		}

		lvi.iItem = iItem++;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LONG)new partner(iSpouse,-2,_T("        <new event>"),iFami);
		m_listPartner.InsertItem(&lvi);
		CheckPColumnWidth(nColPName,_T("        <new event>"));

		if (fami.m_riChild.GetSize())
		{
			lvi.iItem = iItem++;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LONG)new partner(-1,-1,_T("    children:"));
			m_listPartner.InsertItem(&lvi);

			CArray<int,int> riChild;
			fami.GetSortedChildren(riChild);

			for (j = 0; j<riChild.GetSize(); j++)
			{
				int iChild = riChild[j];
				CIndividual& indiChild = GetDocument()->m_rIndividual[iChild];

				int iBirth(-1), iDeath(-1);
				for (int k(0); k<indiChild.m_revt.GetSize(); k++)
				{
					CEvt& evt = indiChild.m_revt[k];

					if (evt.m_strTypeTok=="BIRT")
					{
						iBirth = k;
					}
					else if (evt.m_strTypeTok=="DEAT")
					{
						iDeath = k;
					}
				}

				lvi.iItem = iItem++;
				lvi.pszText = LPSTR_TEXTCALLBACK;
				lvi.lParam = (LONG)new partner(iChild,iBirth,_T("        ")+indiChild.Name(),-1,_T("birth"));
				m_listPartner.InsertItem(&lvi);
				CheckPColumnWidth(nColPName,_T("        ")+indiChild.Name());
				if (iBirth>-1)
				{
					CEvt& evt = indiChild.m_revt[iBirth];
					CheckPColumnWidth(nColPEvent,evt.m_strType);
					CheckPColumnWidth(nColPDate,evt.m_dvDate.Sort());
					CheckPColumnWidth(nColPPlace,evt.m_strPlace);
				}
				else
				{
					CheckPColumnWidth(nColPEvent,_T("birth"));
				}
				lvi.iItem = iItem++;
				lvi.pszText = LPSTR_TEXTCALLBACK;
				lvi.lParam = (LONG)new partner(iChild,iDeath,_T(""),-1,_T("death"));
				m_listPartner.InsertItem(&lvi);
				if (iDeath>-1)
				{
					CEvt& evt = indiChild.m_revt[iDeath];
					CheckPColumnWidth(nColPEvent,evt.m_strType);
					CheckPColumnWidth(nColPDate,evt.m_dvDate.Sort());
					CheckPColumnWidth(nColPPlace,evt.m_strPlace);
				}
				else
				{
					CheckPColumnWidth(nColPEvent,_T("death"));
				}
			}
		}
	}
}

void CGedtreeViewIN::CheckPColumnWidth(int nCol, const CString& str, BOOL bForce)
{
	int nWidth = m_listPartner.GetStringWidth(str)+nColExtraWidth;
	if (bForce || nWidth>m_listPartner.GetColumnWidth(nCol))
		m_listPartner.SetColumnWidth(nCol,nWidth);
}

void CGedtreeViewIN::FillAttributes()
{
	DeleteAttrData();
	m_listAttr.DeleteAllItems();

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	for (int i(0); i<indi.m_rattr.GetSize(); i++)
	{
		CAttr& attr = indi.m_rattr[i];
		lvi.iItem = i;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LONG)new attrData(i,attr.m_evt.m_dvDate.Sort());
		m_listAttr.InsertItem(&lvi);
		CheckAColumnWidth(nColAAttr,attr.m_strType);
		CheckAColumnWidth(nColAVal,attr.m_strValue);
		CheckAColumnWidth(nColADate,attr.m_evt.m_dvDate.Sort());
		CheckAColumnWidth(nColAPlace,attr.m_evt.m_strPlace);
	}
	i = indi.m_rattr.GetSize();
	lvi.iItem = i;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.lParam = (LONG)new attrData(-1,"");
	m_listAttr.InsertItem(&lvi);
	CheckAColumnWidth(nColAAttr,_T("<new attribute>"));
	m_listAttr.SortItems(SortAItems,0);
}

void CGedtreeViewIN::CheckAColumnWidth(int nCol, const CString& str, BOOL bForce)
{
	int nWidth = m_listAttr.GetStringWidth(str)+nColExtraWidth;
	if (bForce || nWidth>m_listAttr.GetColumnWidth(nCol))
		m_listAttr.SetColumnWidth(nCol,nWidth);
}

void CGedtreeViewIN::OnGetdispinfoEventlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	evtData* pp = (evtData*)pDispInfo->item.lParam;
	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	if (pp->iEvent>=0)
	{
		CEvt& evt = indi.m_revt[pp->iEvent];
		if (pDispInfo->item.mask&LVIF_TEXT)
		{
			switch (pDispInfo->item.iSubItem)
			{
			case nColEvent:
				pDispInfo->SetDispText(evt.m_strType);
			break;
			case nColDate:
				pDispInfo->SetDispText(pp->strSort);
			break;
			case nColPlace:
				pDispInfo->SetDispText(evt.m_strPlace);
			break;
			case nColSource:
				pDispInfo->SetDispText(pp->strCita);
			break;
			}
		}
	}
	else
	{
		if (pDispInfo->item.mask&LVIF_TEXT)
		{
			switch (pDispInfo->item.iSubItem)
			{
			case nColEvent:
				pDispInfo->item.pszText = _T("<new event>");
			break;
			}
		}
	}
	*pResult = 0;
}

void CGedtreeViewIN::OnGetdispinfoAttrlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	attrData* pp = (attrData*)pDispInfo->item.lParam;
	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	if (pp->iAttr>=0)
	{
		CAttr& attr = indi.m_rattr[pp->iAttr];
		if (pDispInfo->item.mask&LVIF_TEXT)
		{
			switch (pDispInfo->item.iSubItem)
			{
			case nColAAttr:
				pDispInfo->SetDispText(attr.m_strType);
			break;
			case nColAVal:
				pDispInfo->SetDispText(attr.m_strValue);
			break;
			case nColADate:
				pDispInfo->SetDispText(pp->strSort);
			break;
			case nColAPlace:
				pDispInfo->SetDispText(attr.m_evt.m_strPlace);
			break;
			}
		}
	}
	else
	{
		if (pDispInfo->item.mask&LVIF_TEXT)
		{
			switch (pDispInfo->item.iSubItem)
			{
			case nColAAttr:
				pDispInfo->item.pszText = _T("<new attribute>");
			break;
			}
		}
	}
	*pResult = 0;
}

void CGedtreeViewIN::OnEditEvent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int i(-1);
	i = m_listEvent.GetNextItem(i,LVNI_SELECTED);
	if (i>=0)
	{
		evtData* pev = (evtData*)m_listEvent.GetItemData(i);
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		EditEvent(indi,pev->iEvent);
	}
}

void CGedtreeViewIN::OnGetdispinfoPartnerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	partner* pp = (partner*)pDispInfo->item.lParam;
	if (pDispInfo->item.mask&LVIF_TEXT)
	{
		if (pp->iEvent>=0)
		{
			CEvt* pEvt;
			if (pp->iFami>=0)
			{
				CFamily& fami = GetDocument()->m_rFamily[pp->iFami];
				pEvt = &fami.m_revt[pp->iEvent];
			}
			else
			{
				CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
				pEvt = &indi.m_revt[pp->iEvent];
			}
			switch (pDispInfo->item.iSubItem)
			{
				case nColPName:
					pDispInfo->SetDispText(pp->strName);
				break;
				case nColPEvent:
					pDispInfo->SetDispText(pEvt->m_strType);
				break;
				case nColPDate:
					pDispInfo->SetDispText(pEvt->m_dvDate.Sort());
				break;
				case nColPPlace:
					pDispInfo->SetDispText(pEvt->m_strPlace);
				break;
			}
		}
		else
		{
			switch (pDispInfo->item.iSubItem)
			{
				case nColPName:
					pDispInfo->SetDispText(pp->strName);
				break;

				case nColPEvent:
					if (pp->strEvt)
						pDispInfo->SetDispText(pp->strEvt);
				break;

				case nColPDate:
				case nColPPlace:
//					if ((pp->iIndi>=0||pp->iFami>=0)&&(pp->iEvent>=-1))
//						pDispInfo->item.pszText = _T("----");
//					else
						pDispInfo->item.pszText = _T("");
			}
		}
	}
	*pResult = 0;
}

void CGedtreeViewIN::OnSelectPartner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int i(-1);
	i = m_listPartner.GetNextItem(i,LVNI_SELECTED);
	if (i>=0)
	{
		partner* pp = (partner*)m_listPartner.GetItemData(i);
		if (pp->iFami>=0 && pp->iEvent>=0)
		{
			CFamily& fami = GetDocument()->m_rFamily[pp->iFami];
			EditEvent(fami,pp->iEvent);
		}
		else if (pp->iEvent==-2)
		{
			CFamily& fami = GetDocument()->m_rFamily[pp->iFami];
			NewEvent(fami);
		}
		else if (pp->iIndi>=0)
		{
			CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
			indi.OpenView();
		}
		else if (pp->iEvent==-1)
		{
			// partnership events: ---- ---- ----
			// Do nothing.
		}
	}
}

void CGedtreeViewIN::OnSelectAttr(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int i(-1);
	i = m_listAttr.GetNextItem(i,LVNI_SELECTED);
	if (i>=0)
	{
		attrData* pattr = (attrData*)m_listAttr.GetItemData(i);
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		EditAttr(indi,pattr->iAttr);
	}
}

void CGedtreeViewIN::OnSize(UINT nType, int cx, int cy) 
{
	CGedtreeView::OnSize(nType, cx, cy);

	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)
	{
		if (m_treePedigree.m_hWnd)
		{
			PositionControls(cx,cy);
		}
	}
}

void CGedtreeViewIN::PositionControls(int cx, int cy)
{
	if (m_iIndi==-1) return;

	SetRedraw(FALSE);

	SetScrollSizes(MM_TEXT,CSize(cx,cy));

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectSub(rectClient);
	rectSub.DeflateRect(MARGIN);

	CRect r(rectSub);
	CRect rect1(rectSub);

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	CDC dc;
	dc.CreateCompatibleDC(NULL);

	r.right = r.left + 46;
	r.bottom = r.top + 19;
	m_buttonName.MoveWindow(r);

	m_rectName = rectSub;
	m_rectName.top -= 4;
	m_rectName.bottom = m_rectName.top;
	m_rectName.left += 54;

	dc.SelectObject(&theApp.m_fontBigBold);
	dc.DrawText(indi.Name(),m_rectName,DT_SINGLELINE|DT_CALCRECT);
	m_bBigFontTooHigh = m_rectName.Height()>19;

	r.top = m_rectName.bottom+2;
	r.bottom = r.top + 19;
	m_buttonSex.MoveWindow(r);

	m_rectSex = m_rectName;
	m_rectSex.top = m_rectSex.bottom = r.top+2;

	dc.SelectObject(&theApp.m_font);
	dc.DrawText(indi.Sex(),m_rectSex,DT_SINGLELINE|DT_CALCRECT);
	m_bFontTooHigh = m_rectSex.Height()>19;
	if (m_bFontTooHigh)
	{
		m_rectSex.top -= 4;
		rect1.top = m_rectSex.bottom+4;
	}
	else
		rect1.top = r.bottom+4;

/*
	rect1.bottom = rect1.top+50;

	int nItemHeight = (rectClient.Height()-rect1.bottom-3*MARGIN.cy)/2;

	rect2.top = rect1.bottom+MARGIN.cy;
	rect2.bottom = rect2.top+nItemHeight;
	rect3.top = rect2.bottom+MARGIN.cy;
	rect3.bottom = rect3.top+nItemHeight;

	rect2.right -= 27;
	rect3.right -= 27;
*/
	m_tab.MoveWindow(rect1);

	rect1.left += TAB_MARGIN.left;
	rect1.top += TAB_MARGIN.top;
	rect1.right -= TAB_MARGIN.right;
	rect1.bottom -= TAB_MARGIN.bottom;

	CRect rectHide(-10,-10,-11,-11);
	CRect rectEvent(rectHide);
	CRect rectAttr(rectHide);
	CRect rectPedigree(rectHide);
	CRect rectPartner(rectHide);
	switch (m_tab.GetCurSel())
	{
	case nTabEvent:
		rectEvent = rect1;
		break;
	case nTabAttr:
		rectAttr = rect1;
		break;
	case nTabAnc:
		rectPedigree = rect1;
		break;
	case nTabPart:
		rectPartner = rect1;
		break;
	default:
		ASSERT(FALSE);
	}

	m_treePedigree.MoveWindow(rectPedigree);
	m_listEvent.MoveWindow(rectEvent);
	m_listPartner.MoveWindow(rectPartner);
	m_listAttr.MoveWindow(rectAttr);

	SetRedraw();
	Invalidate();
}

void CGedtreeViewIN::DeletePartnerData()
{
	for (int i(0); i<m_listPartner.GetItemCount(); i++)
		delete (partner*)m_listPartner.GetItemData(i);
}

void CGedtreeViewIN::DeleteEventData()
{
	for (int i(0); i<m_listEvent.GetItemCount(); i++)
		delete (evtData*)m_listEvent.GetItemData(i);
}

void CGedtreeViewIN::DeleteAttrData()
{
	for (int i(0); i<m_listAttr.GetItemCount(); i++)
		delete (attrData*)m_listAttr.GetItemData(i);
}

void CGedtreeViewIN::OnDestroy() 
{
	CGedtreeView::OnDestroy();
	
	// TODO: Add your message handler code here

	DeletePartnerData();
	DeleteEventData();
	DeleteAttrData();

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	indi.m_pFrame = NULL;
}

void CGedtreeViewIN::Reset(UINT flagsChanged)
{
	SetRedraw(FALSE);

	if (flagsChanged & font)
	{
		m_listEvent.SetFont(&theApp.m_font);
		m_listAttr.SetFont(&theApp.m_font);
		m_listPartner.SetFont(&theApp.m_font);
		m_treePedigree.SetFont(&theApp.m_font);

		CFont* pFontButton = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		m_tab.SetFont(pFontButton);

		CheckColumnWidth(nColEvent,"Event",TRUE);
		CheckColumnWidth(nColDate,"Date",TRUE);
		CheckColumnWidth(nColPlace,"Place",TRUE);
		CheckColumnWidth(nColSource,"Source",TRUE);

		CheckPColumnWidth(nColPName,"Name",TRUE);
		CheckPColumnWidth(nColPEvent,"Event",TRUE);
		CheckPColumnWidth(nColPDate,"Date",TRUE);
		CheckPColumnWidth(nColPPlace,"Place",TRUE);

		CRect r;
		GetClientRect(&r);
		PositionControls(r.right,r.bottom);
		SetRedraw(FALSE);
	}

	if (flagsChanged & (lists|font))
	{
		AddParents(TVI_ROOT,m_iIndi);
		FillPartners();
		FillEvents();
		FillAttributes();
	}

	SetRedraw();
	Invalidate();
}

void CGedtreeViewIN::EditEvent(CIndividual& indi, int iEvent)
{
	if (0<=iEvent && iEvent<indi.m_revt.GetSize())
	{
		CEvt& evt = indi.m_revt[iEvent];
		CEditEvent d;
		d.m_evt = evt;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			evt = d.m_evt;
			evt.PutToTree();
			indi.CalcFull();
		}
		else if (nButton==IDC_DELETE)
		{
			evt.Delete();
			indi.m_revt.RemoveAt(iEvent);
			indi.CalcFull();
		}
	}
	else
	{
		reg_t reg = GetRegistration();
		if (reg==registered_bad)
		{
			AfxMessageBox(_T("You cannot save, because this program has not been properly REGISTERED yet. Please click OK, then wait 10 seconds...."));
			::Sleep(9994);
		}
		else
		{
			CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
			NewEvent(indi);
		}
	}
}

void CGedtreeViewIN::EditEvent(CFamily& fami, int iEvent)
{
	CEvt& evt = fami.m_revt[iEvent];
	CEditEvent d(TRUE);
	d.m_evt = evt;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		evt = d.m_evt;
		evt.PutToTree();
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		indi.CalcFull();
	}
	else if (nButton==IDC_DELETE)
	{
		evt.Delete();
		fami.m_revt.RemoveAt(iEvent);
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		indi.CalcFull();
	}
}

void CGedtreeViewIN::EditAttr(CIndividual& indi, int iAttr)
{
	if (0<=iAttr && iAttr<indi.m_rattr.GetSize())
	{
		CAttr& attr = indi.m_rattr[iAttr];
		CEditEvent d(FALSE,TRUE);
		d.m_attr = attr;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			attr = d.m_attr;
			attr.PutToTree();
			indi.CalcFull();
		}
		else if (nButton==IDC_DELETE)
		{
			attr.Delete();
			indi.m_rattr.RemoveAt(iAttr);
			indi.CalcFull();
		}
	}
	else
	{
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		NewAttr(indi);
	}
}

void CGedtreeViewIN::NewEvent(CIndividual& indi)
{
	CEditEvent d;

	CGedtreeDoc* pDoc = GetDocument();

	HTREEITEM hti = pDoc->InsertChild(_T("EVEN"),indi.m_hTreeItem,1);
	CEvt evt(pDoc,hti);

	d.m_evt = evt;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		evt = d.m_evt;
		evt.PutToTree();

		indi.m_revt.Add(evt);

		indi.CalcFull();
	}
	else
		pDoc->DeleteItem(hti);

	m_listEvent.SetFocus();
}

void CGedtreeViewIN::NewEvent(CFamily& fami)
{
	CEditEvent d(TRUE);

	CGedtreeDoc* pDoc = GetDocument();

	HTREEITEM hti = pDoc->InsertChild(_T("EVEN"),fami.m_hTreeItem,1);
	CEvt evt(pDoc,hti);

	d.m_evt = evt;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		evt = d.m_evt;

		fami.m_revt.Add(evt);

		evt.PutToTree();
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		indi.CalcFull();
	}
	else
		pDoc->DeleteItem(hti);

	m_listPartner.SetFocus();
}

void CGedtreeViewIN::NewAttr(CIndividual& indi)
{
	CEditEvent d(FALSE,TRUE);

	CGedtreeDoc* pDoc = GetDocument();

	HTREEITEM hti = pDoc->InsertChild(_T("TITL"),indi.m_hTreeItem,1);
	CAttr attr(pDoc,hti);

	d.m_attr = attr;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		attr = d.m_attr;
		attr.PutToTree();

		indi.m_rattr.Add(attr);

		indi.CalcFull();
	}
	else
		pDoc->DeleteItem(hti);

	m_listAttr.SetFocus();
}

void CGedtreeViewIN::OnName()
{
	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	CName& name = indi.m_name;

	CEditName d;

	d.m_name = name;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		name = d.m_name;
		name.PutToTree(indi.m_hTreeItem);
		GetDocument()->m_bsortIndiNeedsUpdate = true;
		GetDocument()->clearAppletChart();
		indi.CalcFull();
		UpdateTitle();
	}
}

void CGedtreeViewIN::OnSex()
{
	CGedtreeDoc* pDoc = GetDocument();
	CIndividual& indi = pDoc->m_rIndividual[m_iIndi];

	CEditSex d;

	d.m_nSex = indi.m_nSex;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		indi.SetSex(d.m_nSex);
		GetDocument()->clearAppletChart();
	}
}

void CGedtreeViewIN::OnViewDroplinechart() 
{
	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
	CFrameWnd* pFrame = p->CreateNewFrame(GetDocument(),NULL);
	p->InitialUpdateFrame(pFrame,GetDocument());
	CGedtreeViewDL* pDL = (CGedtreeViewDL*)pFrame->GetActiveView();
	pDL->DeselectAll();
	indi.Select();
	pDL->ScrollToIndi(&indi);
}

void CGedtreeViewIN::OnViewOpenpedigree() 
{
	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
	p = theApp.GetNextDocTemplate(pos); // IN tmpl
	p = theApp.GetNextDocTemplate(pos); // PD tmpl
	CChildFrame* pFrame = (CChildFrame*)p->CreateNewFrame(GetDocument(),NULL);
	p->InitialUpdateFrame(pFrame,GetDocument());
	((CGedtreeViewPD*)pFrame->GetActiveView())->Init(m_iIndi);
	p->InitialUpdateFrame(pFrame,GetDocument());
}

void CGedtreeViewIN::OnViewIndi() 
{
	if (m_tab.GetCurSel()==nTabPart)
	{
		int i(-1);
		i = m_listPartner.GetNextItem(i,LVNI_SELECTED);
		if (i>=0)
		{
			partner* pp = (partner*)m_listPartner.GetItemData(i);
			if (pp->iFami>=0 && pp->iEvent>=0)
			{
			}
			else if (pp->iEvent==-2)
			{
			}
			else if (pp->iIndi>=0)
			{
				CIndividual& indi = GetDocument()->m_rIndividual[pp->iIndi];
				indi.OpenView();
			}
			else if (pp->iEvent==-1)
			{
				// partnership events: ---- ---- ----
				// Do nothing.
			}
		}
	}
	else if (m_tab.GetCurSel()==nTabAnc)
	{
		HTREEITEM htvi = m_treePedigree.GetSelectedItem();
		if (htvi)
		{
			int i = m_treePedigree.GetItemData(htvi);
			if (i>-1)
			{
				CIndividual& indi = GetDocument()->m_rIndividual[i];
				indi.OpenView();
			}
		}
	}
}

void CGedtreeViewIN::OnUpdateViewIndi(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	if (m_tab.GetCurSel()==nTabPart)
	{
		int i(-1);
		i = m_listPartner.GetNextItem(i,LVNI_SELECTED);
		if (i>=0)
		{
			partner* pp = (partner*)m_listPartner.GetItemData(i);
			if (pp->iFami>=0 && pp->iEvent>=0)
			{
			}
			else if (pp->iEvent==-2)
			{
			}
			else if (pp->iIndi>=0)
			{
				pCmdUI->Enable(TRUE);
			}
			else if (pp->iEvent==-1)
			{
				// partnership events: ---- ---- ----
				// Do nothing.
			}
		}
	}
	else if (m_tab.GetCurSel()==nTabAnc)
	{
		HTREEITEM htvi = m_treePedigree.GetSelectedItem();
		if (htvi)
		{
			int i = m_treePedigree.GetItemData(htvi);
			if (i>-1)
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}
}

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	evtData* pp1 = (evtData*)lParam1;
	evtData* pp2 = (evtData*)lParam2;

	if (!pp1->strSort.IsEmpty() && !pp2->strSort.IsEmpty())
		return pp1->strSort.CollateNoCase(pp2->strSort);

	// Handle empty strings specially, so they will
	// sort to the bottom (instead of the top as usual):
	else if (!pp1->strSort.IsEmpty())
		return -1; // s2 is empty, so s2 is greater than s1
	else if (!pp2->strSort.IsEmpty())
		return 1; // s1 is empty, so s1 is greater than s2
	else
		return 0;
}

static int CALLBACK SortAItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	attrData* pp1 = (attrData*)lParam1;
	attrData* pp2 = (attrData*)lParam2;

	if (!pp1->strSort.IsEmpty() && !pp2->strSort.IsEmpty())
		return pp1->strSort.CollateNoCase(pp2->strSort);

	// Handle empty strings specially, so they will
	// sort to the bottom (instead of the top as usual):
	else if (!pp1->strSort.IsEmpty())
		return -1; // s2 is empty, so s2 is greater than s1
	else if (!pp2->strSort.IsEmpty())
		return 1; // s1 is empty, so s1 is greater than s2
	else
		return 0;
}
