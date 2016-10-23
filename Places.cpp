// Places.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Places.h"
#include "gedtreedoc.h"
#include "progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nColExtraWidth(15);
static const int nColMaxWidth(250);
static enum
{
	nColSurname,
	nColGivenName,
	nColEvent,
	nColDate,
	nColPlace
};

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

/////////////////////////////////////////////////////////////////////////////
// Places dialog


static CArray<data,const data&>* rpData;

Places::Places(CGedtreeDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(Places::IDD, pParent),
	m_pDoc(pDoc),
	m_bFirst(true)
{
	rpData = &m_rpData;
	//{{AFX_DATA_INIT(Places)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Places::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Places)
	DDX_Control(pDX, IDC_EVENT, m_listEvent);
	DDX_Control(pDX, IDC_PLACE, m_treePlace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Places, CDialog)
	//{{AFX_MSG_MAP(Places)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PLACE, OnSelchangedPlace)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_EVENT, OnGetdispinfoEvent)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENT, OnColumnclickEvent)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Places message handlers

void Places::AddPlaces()
{
	m_treePlace.SendMessage(TVM_SETUNICODEFORMAT,TRUE,0);
	CProgress prg;
	prg.Reset("Searching individuals.",m_pDoc->m_rIndividual.GetSize());
	for (int iIndi(0); iIndi<m_pDoc->m_rIndividual.GetSize(); iIndi++)
	{
		CIndividual& indi = m_pDoc->m_rIndividual[iIndi];
		prg.Set(iIndi+1,indi.Name());
		if (indi.m_bSelected)
		{
			for (int i(0); i<indi.m_revt.GetSize(); i++)
			{
				CEvt& evt = indi.m_revt[i];
				AddPlace(evt.m_strPlace,iIndi,i,-1);
			}
			for (i = 0; i<indi.m_rattr.GetSize(); i++)
			{
				CAttr& attr = indi.m_rattr[i];
				AddPlace(attr.m_evt.m_strPlace,iIndi,-1,i);
			}
			for (i = 0; i<indi.m_riSpouseToFamily.GetSize(); ++i)
			{
				int iFami(indi.m_riSpouseToFamily[i]);
				for (int j(0); j<m_pDoc->m_rFamily[iFami].m_revt.GetSize(); ++j)
				{
					CEvt& evt = m_pDoc->m_rFamily[iFami].m_revt[j];
					AddPlace(evt.m_strPlace,iIndi,-1,-1,i,j);
				}
			}
		}
	}
}

void Places::AddPlace(const CString& strPlace, int iIndi, int iEvt, int iAttr, int iFam, int iFamEvt)
{
	if (strPlace.IsEmpty())
		return;

	CString strConglom(strPlace), strPart;
	HTREEITEM hCurrent(TVI_ROOT);
	while (SplitPlace(strConglom,strPart))
	{
		hCurrent = FindPlace(hCurrent,strPart);
		place_set* pr = (place_set*)m_treePlace.GetItemData(hCurrent);
		evtattr ea(iIndi,iEvt,iAttr,iFam,iFamEvt);
		pr->Add(ea);
	}
}

bool Places::SplitPlace(CString& strConglom, CString& strPart)
{
	int i = strConglom.ReverseFind(',');
	if (i<0)
	{
		strPart = strConglom;
		strConglom.Empty();
	}
	else
	{
		if (i+1>=strConglom.GetLength())
			strPart.Empty();
		else
			strPart = strConglom.Mid(i+1);
		strConglom = strConglom.Left(i);
	}
	strConglom.TrimLeft();
	strConglom.TrimRight();
	strPart.TrimLeft();
	strPart.TrimRight();
	return !strPart.IsEmpty();
}

// Given a perent item (e.g., New York, USA), see if the
// specified place (e.g., "Hamilton") exists as a child of the parent.
// If so, return its handle, else insert it and return the new handle.
HTREEITEM Places::FindPlace(HTREEITEM hParent, const CString& strPlace)
{
	HTREEITEM hPrevChild = TVI_FIRST;
	HTREEITEM hChild = m_treePlace.GetChildItem(hParent);

	bool bPast(false);
	while (hChild && !bPast)
	{
		if (m_treePlace.GetItemText(hChild) == strPlace)
			return hChild;
		else if (m_treePlace.GetItemText(hChild) < strPlace)
		{
			hPrevChild = hChild;
			hChild = m_treePlace.GetNextSiblingItem(hChild);
		}
		else
			bPast = true;
	}

	place_set* pp = new place_set;
	m_rEvtattr.Add(pp);

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = hPrevChild;
	tvInsert.item.mask = TVIF_TEXT|TVIF_PARAM;
	tvInsert.item.pszText = (LPTSTR)(LPCTSTR)strPlace;
	tvInsert.item.lParam = (LPARAM)pp;

	return m_treePlace.InsertItem(&tvInsert);
}

BOOL Places::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitList();

	try
	{
		AddPlaces();
	}
	catch (CUserException* pe)
	{
		pe->Delete();
		OnOK();
	}

	SetForegroundWindow();
	return TRUE;
}

void Places::InitList()
{
	m_listEvent.SendMessage(LVM_SETUNICODEFORMAT,TRUE,0);

	LV_COLUMN lv;
	lv.mask = LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;

	lv.pszText = _T("Surname");
	lv.iSubItem = nColSurname;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Given Name");
	lv.iSubItem = nColGivenName;
	lv.cx = m_listEvent.GetStringWidth(lv.pszText)+nColExtraWidth;
	lv.fmt = LVCFMT_LEFT;
	m_listEvent.InsertColumn(lv.iSubItem,&lv);
	lv.pszText = _T("Event/Attr");
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
}

void Places::CheckColumnWidth(int nCol, const CString& str)
{
	int nWidth = m_listEvent.GetStringWidth(str)+nColExtraWidth;
	if (nWidth>nColMaxWidth)
		m_listEvent.SetColumnWidth(nCol,nColMaxWidth);
	else if (nWidth>m_listEvent.GetColumnWidth(nCol))
		m_listEvent.SetColumnWidth(nCol,nWidth);
}

void Places::OnOK() 
{
	for (int i(0); i<m_rEvtattr.GetSize(); i++)
		delete m_rEvtattr[i];

	DeleteData();

	CDialog::OnOK();
}

void Places::OnSelchangedPlace(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (m_bFirst)
		m_bFirst = false;
	else
		FillEventList();
}

static CGedtreeDoc* pDoc;

void Places::FillEventList()
{
	HTREEITEM h = m_treePlace.GetSelectedItem();
	if (!h)
		return;

	place_set* pp = (place_set*)m_treePlace.GetItemData(h);

	DeleteData();
	m_listEvent.DeleteAllItems();

	LV_ITEM lvi;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	for (int i(0); i<pp->GetSize(); i++)
	{
		const evtattr& ea = pp->GetAt(i);
		CIndividual& indi = m_pDoc->m_rIndividual[ea.iIndi];

		CString strEvt;
		CString strPlace;
		CDateValue dv;
		if (ea.iEvt>=0)
		{
			CEvt& evt = indi.m_revt[ea.iEvt];
			dv = evt.m_dvDate;
			strEvt = evt.m_strType;
			strPlace = evt.m_strPlace;
		}
		else if (ea.iAttr>=0)
		{
			CAttr& attr = indi.m_rattr[ea.iAttr];
			dv = attr.m_evt.m_dvDate;
			strEvt = attr.m_strType+": "+attr.m_strValue;
			strPlace = attr.m_evt.m_strPlace;
		}
		else
		{
			int iFami(indi.m_riSpouseToFamily[ea.iFam]);
			CFamily& fami = m_pDoc->m_rFamily[iFami];
			CEvt& evt = fami.m_revt[ea.iFamEvt];
			dv = evt.m_dvDate;
			strEvt = evt.m_strType;
			strPlace = evt.m_strPlace;
		}

		data d(ea.iIndi,dv,strEvt,strPlace);
		int id = m_rpData.Add(d);
		lvi.lParam = id;
		lvi.iItem = i;

		VERIFY(m_listEvent.InsertItem(&lvi)>=0);

		CheckColumnWidth(nColSurname,indi.m_name.m_strSurname);
		CheckColumnWidth(nColGivenName,indi.m_name.m_strRest);
		CheckColumnWidth(nColEvent,strEvt);
		CheckColumnWidth(nColDate,dv.Sort());
		CheckColumnWidth(nColPlace,strPlace);
	}

	pDoc = m_pDoc;
	m_listEvent.SortItems(SortItems,nColDate);
}

void Places::OnGetdispinfoEvent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	const data& pd = rpData->GetAt(pDispInfo->item.lParam);
	data* pp = (data*)&pd;
	CIndividual& indi = m_pDoc->m_rIndividual[pp->iIndi];
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
		case nColEvent:
			pDispInfo->SetDispText(pp->strEvt);
		break;
		case nColDate:
			pDispInfo->SetDispText(pp->dDate.Sort());
		break;
		case nColPlace:
			pDispInfo->SetDispText(pp->strPlace);
		break;
		}
	}
}

static int CALLBACK SortItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const data& pd1 = rpData->GetAt(lParam1);
	const data& pd2 = rpData->GetAt(lParam2);
	data* pp1 = (data*)&pd1;
	data* pp2 = (data*)&pd2;
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
		case nColEvent:
			s1 = pp1->strEvt;
			s2 = pp2->strEvt;
		break;
		case nColDate:
			s1 = pp1->dDate.Sort();
			s2 = pp2->dDate.Sort();
		break;
		case nColPlace:
			s1 = pp1->strPlace;
			s2 = pp2->strPlace;
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

void Places::DeleteData()
{
	m_rpData.RemoveAll();
}

void Places::OnColumnclickEvent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	pDoc = m_pDoc;
	m_listEvent.SortItems(SortItems,pNMListView->iSubItem);
}

void Places::OnClose() 
{
	OnOK();
	CDialog::OnClose();
}
