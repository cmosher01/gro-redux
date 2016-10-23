// About.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "About.h"
#include "util.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog


CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbout)
	m_strAbout = _T("");
	//}}AFX_DATA_INIT
}


void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (!pDX->m_bSaveAndValidate)
		m_strAbout = GetAbout();

	//{{AFX_DATA_MAP(CAbout)
	DDX_Control(pDX, IDC_ABOUT, m_staticAbout);
	DDX_Text(pDX, IDC_ABOUT, m_strAbout);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
	{
		LOGFONT lf;
		lf.lfHeight = -12;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = PROOF_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		_tcscpy(lf.lfFaceName,_T("Arial"));
		m_font.CreateFontIndirect(&lf);
		m_staticAbout.SetFont(&m_font);
	}
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
	//{{AFX_MSG_MAP(CAbout)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers

CString CAbout::GetAbout()
{
	CString s;

	s += theApp.m_pszAppName; s+= "\n";
	s += "Version "; s += theApp.m_info.m_strVersion;
	s += "  (released ";
	s += RELEASE_DATE;
	s += ")\n";
	s += theApp.m_info.m_strCopyright; s+= "\n";
	s += "\n";
	s += theApp.m_info.m_strComments;

	s += "\n\nrunning: ";

	UINT nAppDriveType = theApp.m_info.m_nAppDriveType;
	if (nAppDriveType==DRIVE_REMOVABLE)
		s += "(floppy) ";
	else if (nAppDriveType==DRIVE_CDROM)
		s += "(CD-ROM) ";
	else if (nAppDriveType==DRIVE_REMOTE)
		s += "(network) ";

	CString strApp = theApp.m_info.m_strAppPath;
	strApp.MakeLower();
	s += strApp;

	return s;
}
