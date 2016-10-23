// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "gedtree.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_WEB_SITE, OnWebSite)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	// when user presses F1 key:
	ON_COMMAND(ID_HELP, OnWebSite)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
	CWnd* pParentWnd, CCreateContext* pContext)
{
	/*
		Same as CFrameWnd::LoadFrame(), but class of main window
		is title, instead of MCF's weird class name.
	*/
	LPCTSTR lpszID = MAKEINTRESOURCE(nIDResource);
	m_nIDHelp = 0;//nIDResource;
	m_strTitle.LoadString(nIDResource);



	// Register our class (main window's title)
	WNDCLASS wndClass;

	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = ::DefWindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = theApp.m_hInstance;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = lpszID;
	wndClass.lpszClassName = m_strTitle;

	HINSTANCE hInst = AfxFindResourceHandle(lpszID,RT_GROUP_ICON);
	wndClass.hIcon = ::LoadIcon(hInst,lpszID);
	if (!wndClass.hIcon)
		wndClass.hIcon = ::LoadIcon(NULL,IDI_APPLICATION);

	AfxRegisterClass(&wndClass);



	// Create the main window
	if (!Create(m_strTitle, m_strTitle, WS_OVERLAPPEDWINDOW, rectDefault,
	  pParentWnd, lpszID, 0, pContext))
		return FALSE;


	
	m_hMenuDefault = ::GetMenu(m_hWnd);
	LoadAccelTable(lpszID);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CMDIFrameWnd::OnActivateApp(bActive, hTask);
	if (bActive)
	{
		CWnd* pWnd = GetLastActivePopup();
		if (pWnd) pWnd->BringWindowToTop();
	}
}

void CMainFrame::OnWebSite()
{
	::ShellExecute(NULL,L"open",L"http://go.to/gro",NULL,NULL,SW_SHOWNORMAL);
}

void CMainFrame::OnClose() 
{
	theApp.CloseClip();
	theApp.SavePathsToReg();
	CMDIFrameWnd::OnClose();
}
