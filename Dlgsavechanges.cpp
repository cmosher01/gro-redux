// DlgSaveChanges.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSaveChanges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveChanges dialog


CDlgSaveChanges::CDlgSaveChanges(CString& strFileName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveChanges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSaveChanges)
	//}}AFX_DATA_INIT
	m_strPrompt = strFileName;
	if (m_strPrompt.IsEmpty())
		m_strPrompt.LoadString(AFX_IDS_UNTITLED);
	m_strPrompt += " ?";
}


void CDlgSaveChanges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSaveChanges)
	DDX_Text(pDX, IDC_FILENAME, m_strPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSaveChanges, CDialog)
	//{{AFX_MSG_MAP(CDlgSaveChanges)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveChanges message handlers

void CDlgSaveChanges::OnYes() 
{
	EndDialog(IDYES);
}

void CDlgSaveChanges::OnNo() 
{
	EndDialog(IDNO);
}
