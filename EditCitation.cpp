// EditCitation.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "EditCitation.h"
#include "EditSource.h"
#include "PickSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCitation dialog


CEditCitation::CEditCitation(CWnd* pParent /*=NULL*/)
	: CDialog(CEditCitation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditCitation)
	m_strPage = _T("");
	m_nQuality = -1;
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CEditCitation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
	{
		ValueToData();
	}

	//{{AFX_DATA_MAP(CEditCitation)
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELETE, m_buttonDelete);
	DDX_Control(pDX, IDC_TEXT, m_editText);
	DDX_Control(pDX, IDC_SOURCE, m_staticSource);
	DDX_Text(pDX, IDC_PAGE, m_strPage);
	DDX_CBIndex(pDX, IDC_QUAY, m_nQuality);
	DDX_Text(pDX, IDC_TEXT, m_strText);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DataToValue();
	}

	Enable();
	SetStaticTitles();
}


BEGIN_MESSAGE_MAP(CEditCitation, CDialog)
	//{{AFX_MSG_MAP(CEditCitation)
	ON_BN_CLICKED(IDC_CHANGESOURCE, OnChangesource)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCitation message handlers

void CEditCitation::DataToValue()
{
	m_cita.m_strPage = m_strPage;
	m_cita.m_nQuality = m_nQuality;
	m_cita.m_strText = m_strText;
	m_cita.Calc();
}

void CEditCitation::ValueToData()
{
	m_strPage = m_cita.m_strPage;
	m_nQuality = m_cita.m_nQuality;
	m_strText = m_cita.m_strText;
}

void CEditCitation::SetStaticTitles()
{
	if (m_cita.m_iSource>=0)
	{
		CSource& sour = m_cita.m_pDoc->m_rSource[m_cita.m_iSource];
		m_staticSource.SetWindowText(sour.GetDisplay());
	}
	else
		m_staticSource.SetWindowText(_T(""));
}

void CEditCitation::Enable()
{
}

void CEditCitation::OnChangesource() 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_cita.m_iSource>=0)
	{
		CSource& sour = m_cita.m_pDoc->m_rSource[m_cita.m_iSource];
		CEditSource d;
		d.m_sour = sour;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			sour = d.m_sour;
			sour.PutToTree();
			m_cita.m_pDoc->ResetAllViews();
		}
		else if (nButton==IDC_DELETE)
		{
			m_cita.m_iSource = -1;
			sour.Delete();
			m_cita.m_pDoc->ResetAllViews();
		}
	}
	else
	{
		CPickSource d(m_cita.m_pDoc);
		d.m_iSource = m_cita.m_iSource;
		int nButton = d.DoModal();
		if (nButton==IDOK)
		{
			m_cita.m_iSource = d.m_iSource;
			m_cita.Calc();
		}
	}

	UpdateData(FALSE);
}

void CEditCitation::OnDelete() 
{
	m_cita.Clear();
	UpdateData(FALSE);
	OnOK();
}

void CEditCitation::OnSize(UINT nType, int cx, int cy) 
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

void CEditCitation::PositionControls(int cx, int cy)
{
	SetRedraw(FALSE);

	CRect rectClient;
	GetClientRect(rectClient);

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

	CRect rectText;
	rectText.top = 100;
	rectText.left = nDlgMargin;
	rectText.right = rectClient.right-nDlgMargin;
	rectText.bottom = rectOK.top-nDlgMargin;
	m_editText.MoveWindow(rectText);

	SetRedraw();
	Invalidate();
}

BOOL CEditCitation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RECT r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
	SetIcon(theApp.LoadIcon(IDD),TRUE);
	return TRUE;
}
