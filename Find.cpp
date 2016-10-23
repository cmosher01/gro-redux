// Find.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Find.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFind dialog


CFind::CFind(CWnd* pParent /*=NULL*/)
	: CDialog(CFind::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFind)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CFind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFind)
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFind, CDialog)
	//{{AFX_MSG_MAP(CFind)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFind message handlers
