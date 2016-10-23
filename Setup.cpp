// Setup.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Setup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetup dialog


CSetup::CSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetup)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetup, CDialog)
	//{{AFX_MSG_MAP(CSetup)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_UNINSTALL, OnUninstall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetup message handlers

void CSetup::OnAdd() 
{
	theApp.CreateStartMenuShortcut();
}

void CSetup::OnRemove() 
{
	theApp.DeleteStartMenuShortcut();
}

void CSetup::OnUninstall() 
{
	if (AfxMessageBox(
		_T("This will completely and cleanly uninstall the GRO application.\n")
		_T("If you want to use this application in the future, you will\n")
		_T("need to install it from the original CD-ROM, diskette, or download\n")
		_T("it from the World Wide Web.\n\n")
		_T("Are you sure you want to UNINSTALL GRO right now?"),MB_YESNO)==IDYES)
		EndDialog(IDC_UNINSTALL);
}
