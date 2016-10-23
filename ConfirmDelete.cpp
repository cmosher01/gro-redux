// ConfirmDelete.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "ConfirmDelete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmDelete dialog


CConfirmDelete::CConfirmDelete(const CString& strItem, CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDelete::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmDelete)
	//}}AFX_DATA_INIT
	m_strMessage = 
		"Are you sure you want to permanently delete this "+
		strItem+"?";
}

void CConfirmDelete::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDelete)
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDelete, CDialog)
	//{{AFX_MSG_MAP(CConfirmDelete)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmDelete message handlers

void CConfirmDelete::OnDelete() 
{
	EndDialog(IDC_DELETE);
}
