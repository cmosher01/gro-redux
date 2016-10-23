// Register.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "Register.h"
#include "reg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegister dialog


CRegister::CRegister(CWnd* pParent /*=NULL*/)
	: CDialog(CRegister::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegister)
	m_strName = _T("");
	m_strKey = _T("");
	//}}AFX_DATA_INIT
}


void CRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegister)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_KEY, m_strKey);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		theApp.PutReg("Reg","Name",m_strName);
		theApp.PutReg("Reg","Key",m_strKey);

		reg_t reg = GetRegistration();
		if (reg==registered_good)
		{
			AfxMessageBox(L"Thank you for registering Genealogy Reasearch Organizer. You can now SAVE your changes.");
		}
		else
		{
			AfxMessageBox(L"Invalid registration. Please click OK, then wait 10 seconds and try again....");
			// pause for 10 seconds
			::Sleep(9989);
		}
	}
}


BEGIN_MESSAGE_MAP(CRegister, CDialog)
	//{{AFX_MSG_MAP(CRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegister message handlers
