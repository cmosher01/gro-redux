// EditSource.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "EditSource.h"
#include "gedtreedoc.h"
#include "EditRepo.h"
#include "PickRepo.h"
#include "repo.h"

#include <mshtml.h>
#pragma warning(push)
#pragma warning(disable: 4192)
#import "shdocvw.dll"
#pragma warning(pop)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CRect TAB_MARGIN(1,22,2,2);
static enum
{
	nTabPreview,
	nTabEdit
};

/////////////////////////////////////////////////////////////////////////////
// CEditSource dialog


CEditSource::CEditSource(CWnd* pParent /*=NULL*/)
	: CDialog(CEditSource::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSource)
	m_strAuthor = _T("");
	m_strPubl = _T("");
	m_strText = _T("");
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
}


void CEditSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
	{
		ValueToData();
	}

	//{{AFX_DATA_MAP(CEditSource)
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_PUBL, m_editPubl);
	DDX_Control(pDX, IDC_TITLE, m_editTitle);
	DDX_Control(pDX, IDC_AUTHOR, m_editAuthor);
	DDX_Control(pDX, IDC_TEXT, m_editText);
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELETE, m_buttonDelete);
	DDX_Control(pDX, IDC_REPO, m_staticRepo);
	DDX_Text(pDX, IDC_AUTHOR, m_strAuthor);
	DDX_Text(pDX, IDC_PUBL, m_strPubl);
	DDX_Text(pDX, IDC_TEXT, m_strText);
	DDX_Text(pDX, IDC_TITLE, m_strTitle);
	DDX_Control(pDX, IDC_IE, m_ie);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DataToValue();
	}

	Enable();
	SetStaticTitles();
}

BEGIN_MESSAGE_MAP(CEditSource, CDialog)
	//{{AFX_MSG_MAP(CEditSource)
	ON_BN_CLICKED(IDC_CHANGEREPO, OnChangerepo)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSource message handlers

void CEditSource::DataToValue()
{
	m_sour.m_strAuthor = m_strAuthor;
	m_sour.m_strTitle = m_strTitle;
	m_sour.m_strPublish = m_strPubl;
	m_sour.m_strText = m_strText;
	m_sour.Calc();
}

//_COM_SMARTPTR_TYPEDEF(IHTMLDocument2,IID_IHTMLDocument2);
void CEditSource::ValueToData()
{
	m_strAuthor = m_sour.m_strAuthor;
	m_strTitle = m_sour.m_strTitle;
	m_strPubl = m_sour.m_strPublish;
	m_strText = m_sour.m_strText;
}

void CEditSource::SetStaticTitles()
{
	if (m_sour.m_iRepository>=0)
	{
		CRepository& repo = m_sour.m_pDoc->m_rRepository[m_sour.m_iRepository];
		m_staticRepo.SetWindowText(repo.m_strName);
	}
	else
		m_staticRepo.SetWindowText(_T(""));
}

void CEditSource::Enable()
{
}

void CEditSource::OnChangerepo() 
{
	UpdateData();

	if (m_sour.m_iRepository>=0)
	{
		CRepository& repo = m_sour.m_pDoc->m_rRepository[m_sour.m_iRepository];
		CEditRepo d;
		d.m_repo = repo;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			repo = d.m_repo;
			repo.PutToTree();
			m_sour.m_pDoc->ResetAllViews();
		}
		else if (nButton==IDC_DELETE)
		{
			m_sour.m_iRepository = -1;
			repo.Delete();
			m_sour.m_pDoc->ResetAllViews();
		}
	}
	else
	{
		CPickRepo d(m_sour.m_pDoc);
		d.m_iRepository = m_sour.m_iRepository;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			m_sour.m_iRepository = d.m_iRepository;
			m_sour.Calc();
		}
	}

	UpdateData(FALSE);
}

void CEditSource::OnDelete() 
{
	if (m_sour.m_i<0)
	{
		OnCancel();
	}
	else
	{
		if (theApp.ConfirmDelete("source"))
		{
			if (m_sour.m_pDoc->HasChildren(m_sour))
			{
				AfxMessageBox(_T("You cannot delete this source because it has other ")
					_T("records that depend on it.\nYou must remove all dependencies ")
					_T("before you can delete this source."));
			}
			else
			{
				EndDialog(IDC_DELETE);
			}
		}
	}
}

BOOL CEditSource::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDefaultDir();
	RECT r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
	SetIcon(theApp.LoadIcon(IDD),TRUE);
	return TRUE;
}

void CEditSource::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)
	{
		if (m_editText.m_hWnd)
		{
			PositionControls(cx,cy);
		}
	}
}

static const int nDlgMargin(10);
static const CSize sizButton(75,23);

void CEditSource::PositionControls(int cx, int cy)
{
	SetRedraw(FALSE);

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectEdit;
	m_editAuthor.GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.right = rectClient.right-nDlgMargin;
	m_editAuthor.MoveWindow(rectEdit);
	m_editTitle.GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.right = rectClient.right-nDlgMargin;
	m_editTitle.MoveWindow(rectEdit);
	m_editPubl.GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.right = rectClient.right-nDlgMargin;
	m_editPubl.MoveWindow(rectEdit);

	CRect rectOK;
	rectOK.bottom = rectClient.bottom-nDlgMargin;
	rectOK.top = rectOK.bottom-sizButton.cy;
	rectOK.left = nDlgMargin;
	rectOK.right = rectOK.left+sizButton.cx;
	m_buttonOK.MoveWindow(rectOK);

	CRect rectCancel;
	rectCancel.bottom = rectOK.bottom;
	rectCancel.top = rectOK.top;
	rectCancel.left = rectOK.right+nDlgMargin;
	rectCancel.right = rectCancel.left+sizButton.cx;
	m_buttonCancel.MoveWindow(rectCancel);

	CRect rectDelete;
	rectDelete.bottom = rectOK.bottom;
	rectDelete.top = rectOK.top;
	rectDelete.right = rectClient.right-nDlgMargin;
	rectDelete.left = rectClient.right-2*sizButton.cx;
	m_buttonDelete.MoveWindow(rectDelete);

	// add tabs to tab control (only once though)
	if (!m_tab.GetItemCount())
	{
		TC_ITEM tci;
		tci.mask = TCIF_TEXT;
		tci.pszText = _T("preview");
		m_tab.InsertItem(nTabPreview,&tci);
		tci.pszText = _T("edit");
		m_tab.InsertItem(nTabEdit,&tci);
		// also, go to edit tab if there is no text at all
		if (m_strText.IsEmpty())
			m_tab.SetCurSel(nTabEdit);
	}

	// position tab control
	CRect rectTab;
	rectTab.top = 150;
	rectTab.left = nDlgMargin;
	rectTab.right = rectClient.right-nDlgMargin;
	rectTab.bottom = rectOK.top-nDlgMargin;
	m_tab.MoveWindow(rectTab);

	// position the controls to similate tab switching
	// (i.e., hide controls that are on other tabs)
	CRect rect1(rectTab);
	rect1.left += TAB_MARGIN.left;
	rect1.top += TAB_MARGIN.top;
	rect1.right -= TAB_MARGIN.right;
	rect1.bottom -= TAB_MARGIN.bottom;

	bool bRefresh(false);
	CRect rectHide(-10,-10,-11,-11);
	CRect rectPreview(rectHide);
	CRect rectEditt(rectHide);
	switch (m_tab.GetCurSel())
	{
	case nTabPreview:
		rectPreview = rect1;
		bRefresh = true;
	break;
	case nTabEdit:
		rectEditt = rect1;
	break;
	default:
		ASSERT(FALSE);
	}

	m_ie.MoveWindow(rectPreview);
	m_editText.MoveWindow(rectEditt);

	if (bRefresh)
		OnRefresh();

	SetRedraw();
	Invalidate();
}

void CEditSource::OnRefresh() 
{
	IHTMLDocument2Ptr pdoc = m_ie.GetDocument();
	if (pdoc==0)
	{
		m_ie.Navigate(_bstr_t(L"about:blank"),0,0,0,0);
		pdoc = m_ie.GetDocument();
	}

	IDispatch* pd = 0;
	pdoc->open(_bstr_t(L""),_variant_t(_bstr_t(L"replace")),_variant_t(_bstr_t(L"")),_variant_t(VARIANT_FALSE,VT_BOOL),&pd);

	UpdateData();
	CString html;
	html = "<base href=\"file:\">";
	html += m_strText;
//	_bstr_t bstr(html);

	SAFEARRAY *sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	if (sfArray)
	{
		VARIANT *param;
		HRESULT hresult = SafeArrayAccessData(sfArray,(LPVOID*)&param);
		param->vt = VT_BSTR;
		param->bstrVal = ::SysAllocString(html);
		hresult = SafeArrayUnaccessData(sfArray);
		pdoc->write(sfArray);
		SafeArrayDestroy(sfArray);
	}
	pdoc->close();
}

BEGIN_EVENTSINK_MAP(CEditSource, CDialog)
    //{{AFX_EVENTSINK_MAP(CEditSource)
	ON_EVENT(CEditSource, IDC_IE, 251 /* NewWindow2 */, OnNewWindow2Ie, VTS_PDISPATCH VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CEditSource::OnNewWindow2Ie(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel) 
{
	*Cancel = TRUE;	
}

void CEditSource::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	CRect r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
}

void CEditSource::SetDefaultDir()
{
	::GetCurrentDirectory(MAX_PATH+1,m_strDefaultDir.GetBuffer(MAX_PATH+1));
	m_strDefaultDir.ReleaseBuffer();

	CGedtreeDoc* pDoc = m_sour.m_pDoc;
	CString sPath = pDoc->GetPathName();

	SetDir(sPath);
}

void CEditSource::RestoreDefaultDir()
{
	SetDir(m_strDefaultDir);
}

void CEditSource::SetDir(const CString& s)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(s,drive,dir,fname,ext);

	CString sPath = CString(drive)+CString(dir);

	::SetCurrentDirectory(sPath);
}

void CEditSource::OnDestroy() 
{
	CDialog::OnDestroy();
	RestoreDefaultDir();	
}
