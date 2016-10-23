// EditSex.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "EditSex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSex dialog


CEditSex::CEditSex(CWnd* pParent /*=NULL*/)
	: CDialog(CEditSex::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSex)
	m_nSex = -1;
	//}}AFX_DATA_INIT
}


void CEditSex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSex)
	DDX_Radio(pDX, IDC_UNKNOWN, m_nSex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditSex, CDialog)
	//{{AFX_MSG_MAP(CEditSex)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSex message handlers
