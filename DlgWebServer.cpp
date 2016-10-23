// DlgWebServer.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "DlgWebServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWebServer dialog


CDlgWebServer::CDlgWebServer(CWnd* pParent /*=NULL*/, bool bRunning)
	: CDialog(CDlgWebServer::IDD, pParent),
	m_bRunning(bRunning)
{
	//{{AFX_DATA_INIT(CDlgWebServer)
	m_nPort = 80;
	m_strRunning = _T("");
	//}}AFX_DATA_INIT
	if (m_bRunning)
		m_strRunning = _T("The server is currently RUNNING.");
	else
		m_strRunning = _T("The server is currently NOT running.");
}


void CDlgWebServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDlgWebServer)
	DDX_Control(pDX, IDC_PORT, m_editPort);
	DDX_Control(pDX, IDOK, m_btnStartStop);
	DDX_Text(pDX, IDC_PORT, m_nPort);
	DDX_Text(pDX, IDC_RUNNING, m_strRunning);
	//}}AFX_DATA_MAP
	if (!!pDX->m_bSaveAndValidate)
	{
	}
	if (m_nPort <= 0 || 65536 <= m_nPort)
	{
		AfxMessageBox(_T("Port must be between 1 and 65535. The default HTTP (web server) port is 80."));
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CDlgWebServer, CDialog)
	//{{AFX_MSG_MAP(CDlgWebServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWebServer message handlers

BOOL CDlgWebServer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bRunning)
	{
		SetDlgItemText(IDOK,_T("Stop Server"));
		m_editPort.SetReadOnly();
	}
	else
	{
		SetDlgItemText(IDOK,_T("Start Server"));
		m_editPort.SetReadOnly(false);
	}

	return TRUE;
}
