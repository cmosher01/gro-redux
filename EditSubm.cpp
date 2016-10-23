// EditSubm.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "EditSubm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSubm dialog


CEditSubm::CEditSubm(CWnd* pParent /*=NULL*/)
	: CDialog(CEditSubm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSubm)
	m_strName = _T("");
	m_strAddr = _T("");
	//}}AFX_DATA_INIT
	ReadFromRegistry();
}


void CEditSubm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSubm)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_ADDR, m_strAddr);
	//}}AFX_DATA_MAP
}

void CEditSubm::ReadFromRegistry()
{
	theApp.GetReg("Submitter","Name",m_strName,"");
	theApp.GetReg("Submitter","Address",m_strAddr,"");
}

void CEditSubm::WriteToRegistry()
{
	if (theApp.m_info.m_bPermanent)
		return;
	theApp.PutReg("Submitter","Name",m_strName);
	theApp.PutReg("Submitter","Address",m_strAddr);
}

BEGIN_MESSAGE_MAP(CEditSubm, CDialog)
	//{{AFX_MSG_MAP(CEditSubm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSubm message handlers

void CEditSubm::OnOK() 
{
	CDialog::OnOK();
	WriteToRegistry();
}
