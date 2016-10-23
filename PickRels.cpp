// PickRels.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "PickRels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickRels dialog


CPickRels::CPickRels(CWnd* pParent /*=NULL*/)
	: CDialog(CPickRels::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPickRels)
	m_bAncestors = FALSE;
	m_bAncestorsFemale = FALSE;
	m_bAncestorsMale = FALSE;
	m_bDescendants = FALSE;
	m_bDescendantsFemale = FALSE;
	m_bDescendantsMale = FALSE;
	m_bMtdna = FALSE;
	m_bYchrom = FALSE;
	//}}AFX_DATA_INIT
}


void CPickRels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickRels)
	DDX_Check(pDX, IDC_ANC, m_bAncestors);
	DDX_Check(pDX, IDC_ANCFEMALE, m_bAncestorsFemale);
	DDX_Check(pDX, IDC_ANCMALE, m_bAncestorsMale);
	DDX_Check(pDX, IDC_DESC, m_bDescendants);
	DDX_Check(pDX, IDC_DESCFEMALE, m_bDescendantsFemale);
	DDX_Check(pDX, IDC_DESCMALE, m_bDescendantsMale);
	DDX_Check(pDX, IDC_MTDNA, m_bMtdna);
	DDX_Check(pDX, IDC_YCHROM, m_bYchrom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPickRels, CDialog)
	//{{AFX_MSG_MAP(CPickRels)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickRels message handlers
