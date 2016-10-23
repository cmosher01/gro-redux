// Zoom.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Zoom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoom dialog


CZoom::CZoom(CWnd* pParent /*=NULL*/)
	: CDialog(CZoom::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZoom)
	m_nScale = 0;
	//}}AFX_DATA_INIT
}


void CZoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZoom)
	DDX_Text(pDX, IDC_SCALE, m_nScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZoom, CDialog)
	//{{AFX_MSG_MAP(CZoom)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoom message handlers
