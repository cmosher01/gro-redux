// DocSize.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "DocSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocSize dialog


CDocSize::CDocSize(CWnd* pParent /*=NULL*/)
	: CDialog(CDocSize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDocSize)
	//}}AFX_DATA_INIT
	m_nMinWidth = 0;
	m_nMinHeight = 0;
}


void CDocSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (!m_nMinWidth)
	{
		m_nMinWidth = m_nWidth;
		m_nMinHeight = m_nHeight;
	}

	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	if (m_nWidth<m_nMinWidth)
	{
		AfxMessageBox(_T("Width too small. Try using Shrink To Fit."));
		pDX->Fail();
	}
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
	if (m_nHeight<m_nMinHeight)
	{
		AfxMessageBox(_T("Height too small. Try using Shrink To Fit."));
		pDX->Fail();
	}
	//{{AFX_DATA_MAP(CDocSize)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDocSize, CDialog)
	//{{AFX_MSG_MAP(CDocSize)
	ON_BN_CLICKED(IDC_NORMALIZE, OnNormalize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocSize message handlers

void CDocSize::OnNormalize() 
{
	EndDialog(IDC_NORMALIZE);
}
