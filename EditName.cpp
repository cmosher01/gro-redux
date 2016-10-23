// EditName.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "EditName.h"
#include "editcitation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditName dialog


CEditName::CEditName(CWnd* pParent /*=NULL*/)
	: CDialog(CEditName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditName)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CEditName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
	{
		ValueToData();
	}

	//{{AFX_DATA_MAP(CEditName)
	DDX_Control(pDX, IDC_NAME, m_editName);
	DDX_Control(pDX, IDC_SOURCE, m_staticSource);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DataToValue();
	}
	SetStaticTitles();
}


void CEditName::DataToValue()
{
	m_name.m_strDisplay = m_strName;
	m_name.Calc();
}

void CEditName::ValueToData()
{
	m_strName = m_name.m_strDisplay;
}

void CEditName::SetStaticTitles()
{
	m_staticSource.SetWindowText(m_name.m_cita.Display());
	m_editName.SetFont(&theApp.m_fontSmall);
}

BEGIN_MESSAGE_MAP(CEditName, CDialog)
	//{{AFX_MSG_MAP(CEditName)
	ON_BN_CLICKED(IDC_CHANGESOURCE, OnChangesource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditName message handlers

void CEditName::OnChangesource() 
{
	UpdateData();
	CEditCitation d;
	d.m_cita = m_name.m_cita;
	if (d.DoModal()==IDOK)
		m_name.m_cita = d.m_cita;
	UpdateData(FALSE);
}
