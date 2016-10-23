// HeadNotes.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "HeadNotes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeadNotes dialog


CHeadNotes::CHeadNotes(CWnd* pParent /*=NULL*/)
	: CDialog(CHeadNotes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeadNotes)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CHeadNotes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CHeadNotes)
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELETE, m_buttonDelete);
	DDX_Control(pDX, IDC_TEXT, m_editText);
	DDX_Text(pDX, IDC_TEXT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeadNotes, CDialog)
	//{{AFX_MSG_MAP(CHeadNotes)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeadNotes message handlers
void CHeadNotes::OnDelete() 
{
	if (theApp.ConfirmDelete("note"))
	{
		m_strText = "";
		UpdateData(FALSE);
		OnOK();
	}
}

void CHeadNotes::OnSize(UINT nType, int cx, int cy) 
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

void CHeadNotes::PositionControls(int cx, int cy)
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
	rectText.top = nDlgMargin;
	rectText.left = nDlgMargin;
	rectText.right = rectClient.right-nDlgMargin;
	rectText.bottom = rectOK.top-nDlgMargin;
	m_editText.MoveWindow(rectText);

	SetRedraw();
	Invalidate();
}

BOOL CHeadNotes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	RECT r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
	m_editText.SetFocus();
	m_editText.SetSel(0,0);
	return FALSE;
}
