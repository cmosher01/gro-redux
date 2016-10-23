#if !defined(AFX_DLGWEBSERVER_H__0840558A_ADFC_4135_9CDD_91167486630A__INCLUDED_)
#define AFX_DLGWEBSERVER_H__0840558A_ADFC_4135_9CDD_91167486630A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWebServer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWebServer dialog

class CDlgWebServer : public CDialog
{
// Construction
public:
	CDlgWebServer(CWnd* pParent = NULL, bool bRunning = false);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWebServer)
	enum { IDD = IDD_WEBSERVER };
	CEdit	m_editPort;
	CButton	m_btnStartStop;
	int		m_nPort;
	CString	m_strRunning;
	//}}AFX_DATA
	bool m_bRunning;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWebServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWebServer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWEBSERVER_H__0840558A_ADFC_4135_9CDD_91167486630A__INCLUDED_)
