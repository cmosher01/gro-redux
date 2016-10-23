// gedtree.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "gedtree.h"

#include <winreg.h>
#include <dde.h>
#include <math.h>
#include <afxadv.h>
#include "MainFrm.h"
#include "ChildFrm.h"
#include "gedtreeDoc.h"
#include "gedtreeViewDL.h"
#include "gedtreeViewIN.h"
#include "gedtreeViewPD.h"
#include "gedtreeViewIL.h"
#include "confirmdelete.h"
#include "appinfo.h"
#include "about.h"
#include "editsubm.h"
#include "unicodesel.h"
#include "holiday.h"
#include "setup.h"
#include "gedcmd.h"
#include "util.h"
#include <objbase.h>
#include "birthcalc.h"
#include <initguid.h>
#include "rfor.h"
#include <cderr.h>
#include "reg.h"
#include "register.h"
#include <locale.h>
#include "DlgWebServer.h"
#include "ChooseFont.h"

#include <algorithm>
#include <sstream>
#include <shlguid.h>
#include <comdef.h>
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGedtreeApp

BEGIN_MESSAGE_MAP(CGedtreeApp, CWinApp)
	//{{AFX_MSG_MAP(CGedtreeApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_OPTIONS_FONT, OnFont)
	ON_COMMAND(ID_OPTIONS_SUBMITTER, OnViewSubmitter)
	ON_COMMAND(ID_OPTIONS_UNICODE, OnOptionsUnicode)
	ON_COMMAND(ID_OPTIONS_PAGEBREAKS, OnOptionsPageBreaks)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PAGEBREAKS, OnUpdateOptionsPageBreaks)
	ON_COMMAND(ID_TOOLS_HOLIDAY, OnToolsHoliday)
	ON_COMMAND(ID_BIRTH_CALC, OnBirthCalc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SETUP, OnUpdateFileSetup)
	ON_COMMAND(ID_FILE_SETUP, OnFileSetup)
	ON_COMMAND(ID_FILE_SERVE, OnFileServe)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_REGISTER, OnUpdateFileRegister)
	ON_COMMAND(ID_FILE_REGISTER, OnFileRegister)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/*
static void check(HRESULT hr)
{
	if (FAILED(hr))
		throw _com_error(hr);
}
*/

// gets build time, in local zone, into fielded SYSTEMTIME struct
static void getBuildTime(SYSTEMTIME* st)
{
	//__DATE__
	//Mmm dd yyyy
	//01234567890
	wstring sdat(_T(__DATE__));
	wstring mos(L"JanFebMarAprMayJunJulAugSepOctNovDec");

	//__TIME__
	//hh:mm:ss
	//01234567
	wstring stim(_T(__TIME__));

	st->wYear = ::_wtoi(sdat.substr(7,4).c_str());
	st->wMonth = mos.find(sdat.substr(0,3))/3+1;
	st->wDay = ::_wtoi(sdat.substr(4,2).c_str());
	st->wDayOfWeek = 0; // not set
	st->wHour = ::_wtoi(stim.substr(0,2).c_str());
	st->wMinute = ::_wtoi(stim.substr(3,2).c_str());
	st->wSecond = ::_wtoi(stim.substr(6,2).c_str());
	st->wMilliseconds = 0;
}

// convert local SYSTEMTIME to GMT SYSTEMTIME
static void localToGMT(const SYSTEMTIME* stLocal, SYSTEMTIME* stGMT)
{
	BOOL bOK;

	FILETIME ftLocal;
	bOK = ::SystemTimeToFileTime(stLocal,&ftLocal);
	if (!bOK)
	{
		DWORD e = ::GetLastError();
		return;
	}

	FILETIME ftUTC;
	bOK = ::LocalFileTimeToFileTime(&ftLocal,&ftUTC);
	if (!bOK)
	{
		DWORD e = ::GetLastError();
		return;
	}

	bOK = ::FileTimeToSystemTime(&ftUTC,stGMT);
	if (!bOK)
	{
		DWORD e = ::GetLastError();
		return ;
	}
}

// convert fielded SYSTEMTIME struct (in GMT) to HTTP time string (in GMT)
wstring toHTTPTime(SYSTEMTIME* st)
{
	BOOL bOK(FALSE);
	wstring mos(L"JanFebMarAprMayJunJulAugSepOctNovDec");
	wstring wks(L"SunMonTueWedThuFriSat");

	wstring sdtMo = mos.substr((st->wMonth-1)*3,3);
	wstring sdtWk = wks.substr(st->wDayOfWeek*3,3);

	//Thu, 13 Mar 2003 04:35:00 GMT
	TCHAR ssdt[30];
	::swprintf(ssdt,L"%s, %02d %s %04d %02d:%02d:%02d GMT",
		sdtWk.c_str(),
		st->wDay,
		sdtMo.c_str(),
		st->wYear,
		st->wHour,
		st->wMinute,
		st->wSecond);

	return wstring(ssdt);
}

// convert HTTP time string (in GMT) to fielded SYSTEMTIME struct (in GMT)
//example http header times:
//Thu, 13 Mar 2003 04:35:00 GMT; length=1546
//Tuesday, 09-Dec-97 03:17:50 GMT (...)
void fromHTTPTime(const wstring& s, SYSTEMTIME* st)
{
	st->wDayOfWeek = 0; // not set
	st->wMilliseconds = 0; // not set

	wstring::size_type i, j;

	// find the first number, and that will
	// be the day
	i = s.find_first_of(L"0123456789");
	if (i==-1)
	{
		// no if-mod or bad if-mod
		st->wDay = 1;
		st->wMonth = 1;
		st->wYear = 1971;
		st->wHour = 1;
		st->wMinute = 1;
		st->wSecond = 1;
		return;
	}
	j = s.find_first_not_of(L"0123456789",i);
	st->wDay = ::_wtoi(s.substr(i,j-i).c_str());
	i = j;

	// month
	wstring mos(L"JanFebMarAprMayJunJulAugSepOctNovDec");
	i = s.find_first_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ",i);
	st->wMonth = mos.find(s.substr(i,3))/3+1;
	i += 3;

	// year
	i = s.find_first_of(L"0123456789",i);
	j = s.find_first_not_of(L"0123456789",i);
	st->wYear = ::_wtoi(s.substr(i,j-i).c_str());
	if (st->wYear < 100)
	{
		if (st->wYear <= 60)
			st->wYear += 2000;
		else
			st->wYear += 1900;
	}
	i = j;

	//hour
	i = s.find_first_of(L"0123456789",i);
	j = s.find_first_not_of(L"0123456789",i);
	st->wHour = ::_wtoi(s.substr(i,j-i).c_str());
	i = j;

	//minute
	i = s.find_first_of(L"0123456789",i);
	j = s.find_first_not_of(L"0123456789",i);
	st->wMinute = ::_wtoi(s.substr(i,j-i).c_str());
	i = j;

	//second
	i = s.find_first_of(L"0123456789",i);
	j = s.find_first_not_of(L"0123456789",i);
	st->wSecond = ::_wtoi(s.substr(i,j-i).c_str());
	i = j;

	// ignore rest of input string
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeApp construction

CGedtreeApp::CGedtreeApp():
	m_pDC(NULL),
	m_bShowPageBreaks(false),
	m_pWebServer(NULL),
	m_fontRescale(1),
	m_nMaxIndiWidth(150),
	m_nIndiBorderX(5),
	m_nIndiBorderY(3)
{
	SYSTEMTIME st;
	getBuildTime(&st);
	localToGMT(&st,&st);

	FILETIME ft;
	::SystemTimeToFileTime(&st,&ft);
	m_ulBuildTime.LowPart = ft.dwLowDateTime;
	m_ulBuildTime.HighPart = ft.dwHighDateTime;

	m_strBuildTime = toHTTPTime(&st);
}

CGedtreeApp::~CGedtreeApp()
{
	if (m_pWebServer)
	{
		m_pWebServer->quit();
		delete m_pWebServer;
	}

	if (m_pDC) delete m_pDC;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGedtreeApp object

CGedtreeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGedtreeApp initialization

BOOL CGedtreeApp::InitInstance()
{
	//if (!FirstInstance()) return FALSE;

	m_info.Read();

	::AfxEnableControlContainer();

	SetRegistryKey(m_info.m_strCompany);
	GetRegKey();
	if (m_info.m_bPermanent)
		LoadStdProfileSettings(0);
	else
		LoadStdProfileSettings(10);
	ReadFontFromRegistry();
	ReadPageSetupFromRegistry();

	BuildAppPaths();

	HRESULT hr = ::CoInitialize(0);

	_wsetlocale(LC_COLLATE,L"english");

	AddTemplates();

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	if (!m_info.m_bPermanent)
	{
		EnableShellOpen();
		SaveShellFileTypes();
		PrepareForRegisterShellFileTypes();
		RegisterShellFileTypes();
		::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,0,0);
	}

	// Parse command line for standard shell commands, DDE, file open
	CGedCmd cmdInfo;
	if (!cmdInfo.Process())
		return FALSE;

	pMainFrame->DragAcceptFiles();

	InitPageSetup();

	reg_t reg = GetRegistration();
/*
	::SystemParametersInfo(SPI_SETFONTSMOOTHING,
                     TRUE,
                     0,
                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	::SystemParametersInfo(0x200B,
                     2,
                     0,
                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE); 
*/
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	if (!cmdInfo.PostProcess())
		return FALSE;

	if (!m_info.m_bPermanent)
		CreateStartMenuShortcut(FALSE);

	CreateClipboard();

	if (m_info.m_bPermanent)
		OpenDocsFromAppDir();
	else
		OpenDocsFromReg();

	if (reg==registered_bad)
	{
		AfxMessageBox(_T("CAUTION! You will NOT BE ABLE TO SAVE any changes you make,\n")
			_T("because this program has been registered INCORRECTLY. Please wait 10 seconds...."));
		::Sleep(9991);
	}
	else if (reg==not_registered && !m_info.m_bPermanent)
	{
		AfxMessageBox(_T("CAUTION! You will NOT BE ABLE TO SAVE any changes you make,\n")
			_T("because this program has not been REGISTERED."));
	}

	return TRUE;
}

BOOL CGedtreeApp::FirstInstance()
{
	CString strTitle;
	strTitle.LoadString(IDR_MAINFRAME);

	CWnd* pWnd = CWnd::FindWindow(strTitle,NULL);
	if (!pWnd) return TRUE; // no previous instance

	pWnd->SetForegroundWindow();

	if (pWnd->IsIconic())
		pWnd->ShowWindow(SW_RESTORE);

	HGLOBAL hCommand(NULL);
	LPTSTR lpCommand(NULL);
	LPARAM lParam(0);

	try
	{
		TCHAR szCommandString[1024];
		_tcscpy(szCommandString,::GetCommandLine());

		hCommand = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(szCommandString)+1);
		if (!hCommand) throw FALSE;

		lpCommand = (LPTSTR)::GlobalLock(hCommand);
		if (!lpCommand) throw FALSE;

		_tcscpy(lpCommand,szCommandString);

		::GlobalUnlock(hCommand);

		lParam = ::PackDDElParam(WM_DDE_EXECUTE,0,(UINT)hCommand);
		if (!lParam) throw FALSE;

		BOOL bOK = pWnd->PostMessage(WM_DDE_EXECUTE,(WPARAM)AfxGetInstanceHandle(),lParam);
		if (!bOK) throw FALSE;

		throw FALSE;
	}
	catch (BOOL b)
	{
		if (hCommand) ::GlobalFree(hCommand);
		if (lParam) ::FreeDDElParam(WM_DDE_EXECUTE, lParam);
		return b;
	}

	return FALSE;
}

BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

CDocument* CGedtreeApp::GetOpenDocument(LPCTSTR lpszFileName)
{
	CDocument* pOpenDocument = NULL;

	TCHAR szPath[_MAX_PATH];
	AfxFullPath(szPath,lpszFileName);

	CDocTemplate::Confidence match;
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = GetNextDocTemplate(pos);
	match = pTemplate->MatchDocType(szPath,pOpenDocument);

	if (match != CDocTemplate::yesAlreadyOpen)
		pOpenDocument = NULL; // just in case

	return pOpenDocument;
}

void CGedtreeApp::AddTemplates()
{
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_GEDTRETYPE,
		RUNTIME_CLASS(CGedtreeDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGedtreeViewDL)); // drop line
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_GEDINTYPE,
		RUNTIME_CLASS(CGedtreeDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGedtreeViewIN)); // individual sheet
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_GEDPDTYPE,
		RUNTIME_CLASS(CGedtreeDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGedtreeViewPD)); // pedigree
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_GEDILTYPE,
		RUNTIME_CLASS(CGedtreeDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGedtreeViewIL)); // list of individuals
	AddDocTemplate(pDocTemplate);
}

void CGedtreeApp::GetRegKey()
{
	if (m_info.m_bPermanent)
		return;

	CString strKey = "Software\\";
	strKey += m_info.m_strCompany;
	strKey += "\\";
	strKey += m_pszAppName;
	strKey += "\\Settings";

	if (::RegOpenKey(HKEY_CURRENT_USER,strKey,&m_hkey)!=ERROR_SUCCESS)
		if (::RegCreateKey(HKEY_CURRENT_USER,strKey,&m_hkey)!=ERROR_SUCCESS)
			m_hkey = NULL;
	if (!m_hkey) AfxMessageBox(_T("Error creating entry in registry."));
}

const int ccRegBuf(512);

void CGedtreeApp::GetReg(
	const CString& strKey,
	const CString& strSub,
	CString& strVal,
	const CString& strDefault)
{
	HKEY hkey;
	if (m_info.m_bPermanent || ::RegOpenKey(m_hkey,strKey,&hkey)!=ERROR_SUCCESS)
		strVal = strDefault;
	else
	{
		_TCHAR s[ccRegBuf];
		LONG cc(ccRegBuf);
		if (::RegQueryValue(hkey,strSub,s,&cc)!=ERROR_SUCCESS)
			strVal = strDefault;
		else
			strVal = s;
		::RegCloseKey(hkey);
	}
}

void CGedtreeApp::GetReg(
	const CString& strKey,
	const CString& strSub,
	int& nVal,
	const int& nDefault)
{
	CString strVal;
	char sDefault[17];
	_itoa(nDefault,sDefault,10);
	GetReg(strKey,strSub,strVal,sDefault);
	nVal = _ttoi(strVal);
}

void CGedtreeApp::PutReg(
	const CString& strKey,
	const CString& strSub,
	const CString& strVal)
{
	HKEY hkey;
	int e = ::RegOpenKey(m_hkey,strKey,&hkey);
	if (e!=ERROR_SUCCESS)
		e = ::RegCreateKey(m_hkey,strKey,&hkey);

	if (e==ERROR_SUCCESS)
	{
		e = ::RegSetValue(hkey,strSub,REG_SZ,strVal,strVal.GetLength());
		::RegCloseKey(hkey);
	}

	if (e!=ERROR_SUCCESS)
		AfxMessageBox(_T("Error writing to registry."));
}

void CGedtreeApp::PutReg(
	const CString& strKey,
	const CString& strSub,
	const int& nVal)
{
	char sVal[17];
	_itoa(nVal,sVal,10);
	PutReg(strKey,strSub,sVal);
}

void CGedtreeApp::SaveShellFileTypes()
{
	// save (and remove) classes reg keys that will be
	// set later by RegisterShellFileTypes

	HKEY hKeyStor;
	LONG nErr = ::RegOpenKey(m_hkey,_T("OldClasses"),&hKeyStor);
	if (nErr==ERROR_SUCCESS)
	{
		// we've already done the save on a previous run
		::RegCloseKey(hKeyStor);
		return;
	}

	nErr = ::RegCreateKey(m_hkey,_T("OldClasses"),&hKeyStor);
	if (nErr!=ERROR_SUCCESS) return;

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pt = GetNextDocTemplate(pos);

		CString strFileTypeID, strFilterExt, strFileTypeName;

		// file type id (e.g., "Gedcom") key under classes root
		MoveKey(pt,CDocTemplate::regFileTypeId,HKEY_CLASSES_ROOT,hKeyStor);

		// file type (e.g., ".ged") key under classes root
		MoveKey(pt,CDocTemplate::filterExt,HKEY_CLASSES_ROOT,hKeyStor);
	}

	::RegCloseKey(hKeyStor);
}

void CGedtreeApp::RestoreShellFileTypes()
{
	HKEY hKeyStor;
	LONG nErr = ::RegOpenKey(m_hkey,_T("OldClasses"),&hKeyStor);
	if (nErr!=ERROR_SUCCESS) return;

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pt = GetNextDocTemplate(pos);

		// file type id (e.g., "Gedcom") key under classes root
		MoveKey(pt,CDocTemplate::regFileTypeId,hKeyStor,HKEY_CLASSES_ROOT);

		// file type (e.g., ".ged") key under classes root
		MoveKey(pt,CDocTemplate::filterExt,hKeyStor,HKEY_CLASSES_ROOT);
	}

	::RegCloseKey(hKeyStor);

	::SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,0,0);
}

void CGedtreeApp::PrepareForRegisterShellFileTypes()
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pt = GetNextDocTemplate(pos);

		// file type id (e.g., "Gedcom") key under classes root
		RemoveKey(pt,CDocTemplate::regFileTypeId,HKEY_CLASSES_ROOT);

		// file type (e.g., ".ged") key under classes root
		RemoveKey(pt,CDocTemplate::filterExt,HKEY_CLASSES_ROOT);
	}
}

void CGedtreeApp::MoveKey(CDocTemplate* pt, enum CDocTemplate::DocStringIndex idocstr, HKEY hKeySrc, HKEY hKeyDst)
{
	BOOL bOK;
	CString str;

	bOK = pt->GetDocString(str,idocstr);
	if (!bOK) return;
	if (str.IsEmpty()) return;

	DeleteKey(hKeyDst,str);
	CopyKey(hKeySrc,hKeyDst,str);
	DeleteKey(hKeySrc,str);
}

void CGedtreeApp::RemoveKey(CDocTemplate* pt, enum CDocTemplate::DocStringIndex idocstr, HKEY hKeySrc)
{
	BOOL bOK;
	CString str;

	bOK = pt->GetDocString(str,idocstr);
	if (!bOK) return;
	if (str.IsEmpty()) return;

	DeleteKey(hKeySrc,str);
}

const DWORD nMaxRegName(MAX_PATH+1);

void CGedtreeApp::CopyKey(HKEY hKeySrc, HKEY hKeyDest, LPCTSTR name)
{
	HKEY hKeySubSrc;
	LONG nErr = ::RegOpenKey(hKeySrc,name,&hKeySubSrc);
	if (nErr!=ERROR_SUCCESS) return;

	HKEY hKeySubDst;
	nErr = ::RegCreateKey(hKeyDest,name,&hKeySubDst);
	if (nErr!=ERROR_SUCCESS) return;

	TCHAR sValueName[nMaxRegName];
	DWORD nValueNameSiz = nMaxRegName;
	DWORD nType;
	BYTE *pData = new BYTE [nMaxRegName];
	DWORD nDataSiz = nMaxRegName;
	int i(0);
	while (::RegEnumValue(hKeySubSrc,i++,sValueName,&nValueNameSiz,NULL,&nType,pData,&nDataSiz)==ERROR_SUCCESS)
	{
		nErr = ::RegSetValueEx(hKeySubDst,sValueName,0,nType,pData,nDataSiz);
		if (nErr!=ERROR_SUCCESS) {}//???
		nValueNameSiz = nMaxRegName;
		nDataSiz = nMaxRegName;
	}
	delete [] pData;

	i = 0;
	while (::RegEnumKey(hKeySubSrc,i++,sValueName,nMaxRegName)==ERROR_SUCCESS)
		CopyKey(hKeySubSrc,hKeySubDst,sValueName);

	::RegCloseKey(hKeySubDst);
	::RegCloseKey(hKeySubSrc);
}

void CGedtreeApp::DeleteKey(HKEY hKey, LPCTSTR name)
{
	// delete all subkeys first

	HKEY hKeySub;
	LONG nErr = ::RegOpenKey(hKey,name,&hKeySub);
	if (nErr!=ERROR_SUCCESS) return;

	// always pass index zero, because we will be deleting keys as we go through them
	TCHAR sValueName[nMaxRegName];
	while (::RegEnumKey(hKeySub,0,sValueName,nMaxRegName)==ERROR_SUCCESS)
		DeleteKey(hKeySub,sValueName);

	::RegCloseKey(hKeySub);

	::RegDeleteKey(hKey,name);
}

BOOL CGedtreeApp::ConfirmDelete(const CString& strItem)
{
	CConfirmDelete d(strItem);
	return d.DoModal()==IDC_DELETE;
}

void CGedtreeApp::InitFonts(LOGFONT* plf)
{
	plf->lfQuality = PROOF_QUALITY;

	m_font.CreateFontIndirect(plf);

	FontBigBold(plf);
	m_fontBigBold.CreateFontIndirect(plf);

	FontSmall(plf);
	m_fontSmall.CreateFontIndirect(plf);
}

void CGedtreeApp::FontBigBold(LOGFONT* plf)
{
	plf->lfHeight *= 2;
	plf->lfWeight = FW_BOLD;
}

void CGedtreeApp::FontSmall(LOGFONT* plf)
{
	plf->lfHeight = 16;
	plf->lfWeight = FW_NORMAL;
}

void CGedtreeApp::ResetAllDocuments(UINT flagsChanged) 
{
	ResetFont();
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* pt = GetNextDocTemplate(pos); // DL tmpl
		POSITION posDoc = pt->GetFirstDocPosition();
		while (posDoc)
		{
			CGedtreeDoc* pDoc = (CGedtreeDoc*)pt->GetNextDoc(posDoc); // DL tmpl
			for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
			{
				pDoc->m_rIndividual[i].ResetWidth();
				pDoc->m_rIndividual[i].Calc();
			}
			for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
			{
				pDoc->m_rFamily[i].Calc();
			}
			pDoc->ResetAllViews(flagsChanged);
		}
	}
}

CDC* CGedtreeApp::GetDC()
{
	if (!m_pDC)
	{
		m_pDC = new CDC;
		VERIFY(m_pDC->CreateCompatibleDC(NULL));
	}
	return m_pDC;
}

void CGedtreeApp::ResetFont()
{
	GetDC()->SelectObject(&m_font);
}

BOOL CGedtreeApp::LowMem()
{
	MEMORYSTATUS mem;
	mem.dwLength = sizeof(MEMORYSTATUS);
	::GlobalMemoryStatus(&mem);

	BOOL bLow = mem.dwMemoryLoad>90; // if 90% or more

	if (bLow)
		AfxMessageBox(_T("Warning: your system is running low on memory.\n")
		_T("The command may not work correctly right now."));

	return bLow;
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeApp commands

void CGedtreeApp::OnAppAbout()
{
	CAbout d;
	d.DoModal();
}

void CGedtreeApp::OnFileNew() 
{
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* p = GetNextDocTemplate(pos); // DL tmpl
	p->OpenDocumentFile(NULL);
}

void CGedtreeApp::CreateClipboard()
{
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* p = GetNextDocTemplate(pos); // DL tmpl
	m_pClip = (CGedtreeDoc*)(p->OpenDocumentFile(NULL,FALSE));
	p->RemoveDocument(m_pClip);
	m_pClip->ResetSubValue(TVI_ROOT,"HEAD","",TRUE,TRUE);
}

void CGedtreeApp::OnFont() 
{
	LOGFONT lf;
	m_font.GetLogFont(&lf);
	theApp.ResetFont();
	TEXTMETRIC tm;
	::GetTextMetrics(theApp.GetDC()->m_hDC,&tm);
	int oldheight = tm.tmAveCharWidth+tm.tmOverhang;

	CChooseFont d(&lf,CF_BOTH|CF_INITTOLOGFONTSTRUCT);
	if (d.DoModal()==IDOK)
	{
		WriteFontToRegistry(d.m_cf.lpLogFont);
		m_font.DeleteObject();
		m_fontBigBold.DeleteObject();
		m_fontSmall.DeleteObject();
		InitFonts(d.m_cf.lpLogFont);

		theApp.ResetFont();
		TEXTMETRIC tm;
		::GetTextMetrics(theApp.GetDC()->m_hDC,&tm);
		int newheight = tm.tmAveCharWidth+tm.tmOverhang;
		if (d.m_bScale)
			m_fontRescale = (double)newheight/(double)oldheight;
		else
			m_fontRescale = 1;
		m_nMaxIndiWidth *= m_fontRescale;
		m_nIndiBorderX *= m_fontRescale;
		m_nIndiBorderY *= m_fontRescale;

		ResetAllDocuments(CGedtreeView::font);

		POSITION pos = GetFirstDocTemplatePosition();
		while (pos)
		{
			CDocTemplate* pt = GetNextDocTemplate(pos);
			POSITION posDoc = pt->GetFirstDocPosition();
			while (posDoc)
			{
				CGedtreeDoc* pDoc = (CGedtreeDoc*)pt->GetNextDoc(posDoc);
				pDoc->Normalize();
			}
		}
	}
}
//???
//#define PutRegPsd(f) PutReg("PageSetup",#f,(int)(psd->f))
//#define GetRegPsd(f) GetReg("PageSetup",#f,(int)(psd->f))

void CGedtreeApp::WritePageSetupToRegistry()
{
//	if (theApp.m_info.m_bPermanent)
//	PAGESETUPDLG& psd(m_dlgPageSetup.m_psd);
//	PutReg("PageSetup","",psd.
/*
	DWORD           lStructSize;
	HWND            hwndOwner;
	HGLOBAL         hDevMode;
	HGLOBAL         hDevNames;
	DWORD           Flags;
	POINT           ptPaperSize;
	RECT            rtMinMargin;
	RECT            rtMargin;
	HINSTANCE       hInstance;
	LPARAM          lCustData;
	LPPAGESETUPHOOK lpfnPageSetupHook;
	LPPAGEPAINTHOOK lpfnPagePaintHook;
	LPCWSTR         lpPageSetupTemplateName;
	HGLOBAL         hPageSetupTemplate;
*/
}

void CGedtreeApp::ReadPageSetupFromRegistry()
{
//	PAGESETUPDLG psd(m_dlgPageSetup.m_psd);
}

#define PutRegFont(f) PutReg("Font",#f,(int)(plf->f))

void CGedtreeApp::WriteFontToRegistry(const LOGFONT* plf)
{
	if (m_info.m_bPermanent)
		return;

	PutRegFont(lfHeight);
	PutRegFont(lfWidth);
	PutRegFont(lfEscapement);
	PutRegFont(lfOrientation);
	PutRegFont(lfWeight);
	PutRegFont(lfItalic);
	PutRegFont(lfUnderline);
	PutRegFont(lfStrikeOut);
	PutRegFont(lfCharSet);
	PutRegFont(lfOutPrecision);
	PutRegFont(lfClipPrecision);
	PutRegFont(lfQuality);
	PutRegFont(lfPitchAndFamily);

	PutReg("Font","lfFaceName",plf->lfFaceName);
}

#define GetRegFont(f) GetReg("Font",#f,*(int*)&lf.f,lfDef.f)

void CGedtreeApp::ReadFontFromRegistry()
{
	LOGFONT lfDef;
	::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&lfDef);

	LOGFONT lf;
	GetRegFont(lfHeight);
	GetRegFont(lfWidth);
	GetRegFont(lfEscapement);
	GetRegFont(lfOrientation);
	GetRegFont(lfWeight);
	GetRegFont(lfItalic);
	GetRegFont(lfUnderline);
	GetRegFont(lfStrikeOut);
	GetRegFont(lfCharSet);
	GetRegFont(lfOutPrecision);
	GetRegFont(lfClipPrecision);
	GetRegFont(lfQuality);
	GetRegFont(lfPitchAndFamily);

	CString strFaceName;
	GetReg("Font","lfFaceName",strFaceName,lfDef.lfFaceName);
	_tcsncpy(lf.lfFaceName,strFaceName,LF_FACESIZE);

	InitFonts(&lf);
}

void CGedtreeApp::OnViewSubmitter() 
{
	CEditSubm d;
	d.DoModal();
}

void CGedtreeApp::OnOptionsUnicode() 
{
	CUnicodeSel d;
	d.DoModal();
}

void CGedtreeApp::OnOptionsPageBreaks()
{
	m_bShowPageBreaks = !m_bShowPageBreaks;
	ResetAllDocuments();
}

void CGedtreeApp::OnUpdateOptionsPageBreaks(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowPageBreaks);
}

BOOL CGedtreeApp::SaveAsUnicode()
{
	CUnicodeSel d;
	return d.m_bUnicode;
}

void CGedtreeApp::OnToolsHoliday() 
{
	CHoliday d;
	d.DoModal();
}

void CGedtreeApp::OnBirthCalc()
{
	CBirthCalc d;
	d.DoModal();
}

struct query_params
{
	int level;
	int base;
	int indi;
	int sour;
	query_params() : level(-1), base(-1), indi(-1), sour(-1) {}
};

static void parse_param(const wstring& p, query_params& qp)
{
	wstring::size_type eq = p.find(L'=');
	wstring var = p.substr(0,eq);
	wstring val;
	if (eq != wstring::npos)
		val = p.substr(eq+1);

	if (var==L"level")
		qp.level = ::_wtoi(val.c_str());
	else if (var==L"base")
		qp.base = ::_wtoi(val.c_str());
	else if (var==L"indi")
		qp.indi = ::_wtoi(val.c_str());
	else if (var==L"sour")
		qp.sour = ::_wtoi(val.c_str());
	else if (var==L"chart")
		qp.level = -9999;
	else if (var==L"chartdata")
		qp.level = -9998;
}

static query_params parse_params(const wstring& ps)
{
	query_params qp;
	wstring::size_type lamp(-1);
	do
	{
		wstring::size_type amp = ps.find(L'&',lamp+1);
		wstring p = ps.substr(lamp+1,amp-(lamp+1));
		parse_param(p,qp);
		lamp = amp;
	}
	while (lamp != wstring::npos);
	return qp;
}

void CGedtreeApp::writecss(wostringstream& os, wstring& mime, wstring& lastmod, const wstring& ifmod)
{
	SYSTEMTIME stIfMod;
	fromHTTPTime(ifmod,&stIfMod);
	FILETIME ftIfMod;
	::SystemTimeToFileTime(&stIfMod,&ftIfMod);
	ULARGE_INTEGER ulIfMod;
	ulIfMod.LowPart = ftIfMod.dwLowDateTime;
	ulIfMod.HighPart = ftIfMod.dwHighDateTime;

	if (m_ulBuildTime.QuadPart <= ulIfMod.QuadPart)
		throw 304;

#define L(x) os << _T(x) << endl;
#include "css.h"
	mime = _T("text/css");
	lastmod = theApp.m_strBuildTime;
}

void static sendstream(const wostringstream& os, const wstring& mime, thinsock& sock)
{
	wstring strdoc = os.str();
	int cb = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),0,0,0,0);
	if (!cb)
	{
		::OutputDebugString(CString(L"sendstream, WideCharToMultiByte (1)"));
		throw 404;
	}

	char* pb = new char[cb];
	int ok = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),pb,cb,0,0);
	if (!ok)
	{
		delete [] pb;
		::OutputDebugString(CString(L"sendstream, WideCharToMultiByte (2)"));
		throw 404;
	}

	SYSTEMTIME st;
	::GetSystemTime(&st);
	wstring lmodtime = toHTTPTime(&st);

	wostringstream os2;
	os2 << L"HTTP/1.0 200 OK" << endl;
	os2 << L"Content-Type: " << mime << endl;
	os2 << L"Last-Modified: " << lmodtime << endl;
	os2 << L"Content-Length: " << cb << endl;
	os2 << L"Connection: close" << endl;
	os2 << endl;

	sock.send(os2.str().c_str(),os2.str().length(),0);
	sock.sendbinary(pb,cb,0);

	delete [] pb;
}
/*
wstring CGedtreeApp::groch::getContype(const wstring& path_params, wstring& mime, thinsock& sock, wstring& lastmod, const wstring& ifmod)
{
	wostringstream os2;
	os2 << L"HTTP/1.0 200 OK" << endl;
	os2 << L"Content-Type: image/svg+xml" << endl;
	os2 << L"Content-Length: 0" << endl;
	os2 << L"Connection: Keep-Alive" << endl;
	os2 << endl;

	sock.send(os2.str().c_str(),os2.str().length(),0);

	return wstring();
}
*/

/*
path_params will be in one of these formats:

	/                                   [list of documents page]
	/doc.ged/?level=v&base=b            [index page]
	/doc.ged/?indi=n                    [individual page]
	/doc.ged/?sour=s                    [source page]
	/doc.ged/d1/.../some_file_path.xyz  [other file, (e.g., image) with
	                                    path relative to ged file's folder]
	/doc.ged/?chart                     [drop-line chart html page]
	/doc.ged/?chartdata                 [drop-line chart data]
	/doc.ged/groaplt.jar                [applet jar file]
*/

wstring CGedtreeApp::groch::getHTML(const wstring& path_params, wstring& mime, thinsock& sock, wstring& lastmod, const wstring& ifmod)
{
	mime = L"text/html; charset=UTF-8";
	lastmod = L"";

	wstring path(path_params); // local copy

	// remove leading slash
	if (path.length())
	{
		if (path[0] != '/')
			throw 404;
		path = path.substr(1);
	}

	// trim off doc id (up to first slash)
	wstring::size_type slash = path.find('/');
	wstring docid = path.substr(0,slash);
	if (slash != wstring::npos)
		path = path.substr(slash+1);

	wostringstream os;

	if (docid.empty())
	{
//		os << L"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>" << endl;
		os << L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl;
		os << L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">" << endl;

		os << L"<head>" << endl;
		os << L"<title>Genealogy Research Organizer</title>" << endl;
		os << L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
		os << L"<link rel=\"stylesheet\" href=\"/css\" type=\"text/css\" media=\"all\" />" << endl;
		os << L"</head>" << endl;
		os << L"<body>" << endl;
		theApp.GetDocs(os);
		theApp.htmlFooter(os);
		os << L"</body>" << endl;

		os << L"</html>" << endl;
		sendstream(os,L"text/html; charset=UTF-8",sock);
		return wstring();
	}
	else if (docid==L"css")
	{
		theApp.writecss(os,mime,lastmod,ifmod);
	}
	else
	{
		// find doc object
		CGedtreeDoc* pDoc = theApp.FindDoc(docid);
		if (!pDoc)
		{
			::OutputDebugString(CString(L"getHTML can't find doc"));
			throw 404;
		}

		query_params qp;
		if (path.length() && path[0]=='?')
		{
			path = path.substr(1);
			qp = parse_params(path);
/*
				if (ifmod.empty())
				{
					//pDoc->svgChart(docid,sock);
				}
				else
				{
					wostringstream os;
					os << L"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl;
					os << L"<svg width=\"100%\" height=\"100%\">" << endl;
					os << L"</svg>" << endl;
					sendstream(os,L"image/svg+xml",sock);
					return wstring();
				}
*/
			if (qp.level==-9999)//chart
			{
				os << L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl;
				os << L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">" << endl;
				os << L"<head>" << endl;
				os << L"<title>" << endl;
				os << docid << endl;
				os << L"</title>" << endl;
				os << L"</head>" << endl;
				os << L"<body>" << endl;
				os << L"<div>" << endl;
				os << L"<a href=\"/\">home</a> <a href=\"?level=0&base=0\">index</a><br />" << endl;
				os << L"Click on a person to go to his or her page." << endl;
				os << L"<!-- Microsoft Internet Explorer -->" << endl;
				os << L"<object classid=\"clsid:8AD9C840-044E-11D1-B3E9-00805F499D93\" width=\"100%\" height=\"100%\" codebase=\"http://java.sun.com/products/plugin/autodl/jinstall-1_4-windows-i586.cab#Version=1,4,0,0\">" << endl;
					os << L"<param name=\"code\" value=\"to.go.gro.groapplet.GROapplet\" />" << endl;
					os << L"<param name=\"archive\" value=\"groaplt.jar\" />" << endl;
					os << L"<param name=\"type\" value=\"application/x-java-applet;version=1.4\" />" << endl;
					os << L"<param name=\"scriptable\" value=\"false\" />" << endl;
					os << L"<!-- Mozilla and Netscape -->" << endl;
					os << L"<object classid=\"java:to.go.gro.groapplet.GROapplet\" width=\"100%\" height=\"100%\">" << endl;
						os << L"<param name=\"archive\" value=\"groaplt.jar\" />" << endl;
						os << L"<param name=\"type\" value=\"application/x-java-applet;version=1.4\" />" << endl;
						os << L"<param name=\"scriptable\" value=\"false\" />" << endl;
						os << L"<!-- needed by Internet Explorer (see http://ww2.cs.fsu.edu/~steele/XHTML/appletObject.html) -->" << endl;
					os << L"</object>" << endl;
				os << L"</object>" << endl;
				os << L"</div>" << endl;
				os << L"</body>" << endl;
				os << L"</html>" << endl;
				sendstream(os,L"text/html; charset=UTF-8",sock);
				return wstring();
			}
			else if (qp.level==-9998)//chartdata
				pDoc->appletChart(docid,sock);
			else
			{
	//			os << L"<?xml version=\"1.0\" encoding=\"iso-8859-1\" standalone=\"no\"?>" << endl;
				os << L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">" << endl;
				os << L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">" << endl;

				if (qp.level >= 0)
				{
					pDoc->htmlIndex(qp.level,qp.base,docid,os);
				}
				else if (qp.indi >= 0)
				{
					pDoc->htmlIndi(qp.indi,docid,os);
				}
				else if (qp.sour >= 0)
				{
					pDoc->htmlSour(qp.sour,docid,os);
				}
				else
				{
					::OutputDebugString(CString(L"getHTML not index, indi, or source"));
					throw 404;
				}

				os << L"</html>" << endl;
				sendstream(os,L"text/html; charset=UTF-8",sock);
				return wstring();
			}
		}
		else if (path==L"groaplt.jar")
		{
			wstring mime = L"application/java-archive";
			theApp.webFile(path,sock,mime);
		}
		else
		{
			// This is the case of the "other file".
			// path is now of the form [ d1 / [ d2 / [...] ] ] filename.typ
			mime = L""; // ???
			pDoc->webFile(path,sock,mime);
		}
	}

	return os.str();
}

void CGedtreeApp::webFile(const wstring& spath, thinsock& sock, wstring& mime)
{
	wstring path = theApp.m_info.m_strAppDrive+theApp.m_info.m_strAppDir+spath.c_str();
	// get file
	HANDLE f(INVALID_HANDLE_VALUE);
	try
	{
		wstring p2(path);
		std::replace(p2.begin(),p2.end(),'/','\\');
		HANDLE f = ::CreateFile(p2.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (f==INVALID_HANDLE_VALUE)
		{
			DWORD e = ::GetLastError();
			e = e;
			::OutputDebugString(CString(L"webFile can't find file"));
			throw 404;
		}

		DWORD dwSize = ::GetFileSize(f,0);
		if (dwSize==INVALID_FILE_SIZE)
		{
			::OutputDebugString(CString(L"webFile invalid file size"));
			throw 404;
		}

		{
			wostringstream os;
			os << L"HTTP/1.0 200 OK" << endl;
			os << L"Content-Length: " << dwSize << endl;
			os << L"Connection: close" << endl;
			os << endl;
			sock.send(os.str().c_str(),os.str().length(),0);
		}

		DWORD cr(0);
		char buf[100*1024];
		BOOL bok = ::ReadFile(f,buf,100*1024,&cr,0);
		if (!bok)
		{
			::OutputDebugString(CString(L"webFile error reading file"));
			throw 404;
		}

		while (cr)
		{
			try
			{
				sock.sendbinary(buf,cr,0);
			}
			catch (...)
			{
				::OutputDebugString(CString(L"webFile error calling sendbinary"));
				throw 404;
			}

			bok = ::ReadFile(f,buf,100*1024,&cr,0);
			if (!bok)
			{
				::OutputDebugString(CString(L"webFile error reading file (2)"));
				throw 404;
			}
		}

	}
	catch (...)
	{
		if (f!=INVALID_HANDLE_VALUE)
			::CloseHandle(f);

		throw;
	}

	if (f!=INVALID_HANDLE_VALUE)
		::CloseHandle(f);
}

void CGedtreeApp::htmlFooter(wostream& os)
{
	os << L"<hr />" << endl;
	os << L"<address>" << endl;
//	os << L"<a href=\"http://validator.w3.org/p3p/20020128/p3p.pl?uri=http://mosher.mine.nu/">Valid P3P</a>
	os << L"<a href=\"http://validator.w3.org/check/referer\"><img style=\"border:0;width:88px;height:31px\" src=\"http://www.w3.org/Icons/valid-xhtml11\" alt=\"Valid XHTML 1.1!\" height=\"31\" width=\"88\" /></a>" << endl;
	os << L"<a href=\"http://jigsaw.w3.org/css-validator/check/referer\"><img style=\"border:0;width:88px;height:31px\" src=\"http://jigsaw.w3.org/css-validator/images/vcss\" alt=\"Valid CSS!\" /></a><br />" << endl;
	os << L"This page was generated by <a href=\"http://go.to/gro\">Genealogy Research Organizer</a>." << endl;
	os << L"</address>" << endl;
}

CGedtreeDoc* CGedtreeApp::FindDoc(const wstring& sid)
{
	CGedtreeDoc* pDoc = 0;
	if (!sid.empty())
	{
		POSITION pos = GetFirstDocTemplatePosition();
		CDocTemplate* pt = GetNextDocTemplate(pos);
		pos = pt->GetFirstDocPosition();
		while (pos && !pDoc)
		{
			CGedtreeDoc* pDocTest = (CGedtreeDoc*)pt->GetNextDoc(pos);
			if (pDocTest->GetTitle().CompareNoCase(sid.c_str())==0)
				pDoc = pDocTest;
		}
	}
	return pDoc;
}

void CGedtreeApp::GetDocs(wostream& os)
{
	CGedtreeDoc* pDoc = 0;
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pt = GetNextDocTemplate(pos);
	pos = pt->GetFirstDocPosition();
	bool found(false);
	while (pos)
	{
		if (!found)
		{
			found = true;
			os << L"<p>" << endl;
			os << L"The following GEDCOM documents are currently available on this GRO server." << endl;
//			os << L"Please choose one to go to the index of its people.<br />" << endl;
//			os << L"To view the charts, you can download <a href=\"http://www.adobe.com/svg/viewer/install/\">Adobe SVG Viewer</a>." << endl;
			os << L"</p><hr />" << endl;
		}

		pDoc = (CGedtreeDoc*)pt->GetNextDoc(pos);

		os << L"<p>" << endl;

		os << L"<span class=\"filename\"><a href=\"/";
		os << (LPCTSTR)pDoc->GetTitle();
		os << L"/?chart\">";
		os << (LPCTSTR)pDoc->GetTitle();
		os << L"</a> " << endl;

		os << L"(<a href=\"/";
		os << (LPCTSTR)pDoc->GetTitle();
		os << L"/?level=0&amp;base=0\">index</a>)</span><br />" << endl;

		CString sn = pDoc->m_head.m_strNote;
		if (sn.IsEmpty())
			sn = "[No description for this GEDCOM file is available.]";
		os << L"<span class=\"filedesc\">" << (LPCTSTR)sn << L"</span>" << endl;

		os << L"</p>" << endl;
	}
	if (!found)
	{
		os << L"<p>" << endl;
		os << L"There are no GEDCOM documents currently available on this GRO server." << endl;
		os << L"Please check back soon." << endl;
		os << L"</p>" << endl;
	}
}

void CGedtreeApp::OnFileServe()
{
	CDlgWebServer d(m_pMainWnd,!!m_pWebServer);
	if (d.DoModal()==IDOK)
	{
		if (m_pWebServer)
		{
			m_pWebServer->quit();
			delete m_pWebServer;
			m_pWebServer = 0;
		}
		else
		{
			m_pWebServer = new WebServer(d.m_nPort,mgroch);
		}
	}
}

void CGedtreeApp::OnUpdateFileSetup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_info.m_bPermanent);
}

void CGedtreeApp::OnFileSetup() 
{
	int nRet;

	{
		CSetup d;
		nRet = d.DoModal();
	}

	if (nRet==IDC_UNINSTALL) Uninstall();
}

void CGedtreeApp::CreateStartMenuShortcut(BOOL bShowMessage) 
{
	CString strMsg;

	try
	{
		// get the shortcut file's name
		CString strShortcut = GetStartMenuShortcutFileName();

		FILE* lnkfile = _wfopen(strShortcut,L"r");
		if (lnkfile)
		{
			// shortcut already exists, do nothing
			fclose(lnkfile);
			strMsg = theApp.m_pszAppName;
			strMsg = " is already in the Start menu.";
		}
		else
		{
			// create the shortcut file
			IShellLinkWPtr sh(CLSID_ShellLink);
 			sh->SetPath(m_info.m_strAppPath);
			sh->SetDescription(m_info.m_strComments);
			IPersistFilePtr pf = sh;
			pf->Save(strShortcut,TRUE);

			strMsg = theApp.m_pszAppName;
			strMsg += " has been added to the Start menu.";
		}
	}
	catch (...)
	{
		strMsg = "ERROR: Could not add item to the Start menu.";
	}

	if (bShowMessage)
		::AfxMessageBox(strMsg);
}

void CGedtreeApp::DeleteStartMenuShortcut(BOOL bShowMessage) 
{
	BOOL bOK = ::DeleteFile(GetStartMenuShortcutFileName());

	CString strMsg;
	if (bOK)
	{
		strMsg = theApp.m_pszAppName;
		strMsg += " has been removed from the Start menu.";
	}
	else
	{
		strMsg = "ERROR: Could not remove ";
		strMsg += theApp.m_pszAppName;
		strMsg += " from the Start menu.";
	}

	if (bShowMessage)
		::AfxMessageBox(strMsg);
}

CString CGedtreeApp::GetStartMenuShortcutFileName()
{
	LPITEMIDLIST pidl;
	::SHGetSpecialFolderLocation(m_pMainWnd->GetSafeHwnd(),CSIDL_PROGRAMS,&pidl);
	TCHAR sPath[MAX_PATH+1];
	::SHGetPathFromIDList(pidl,sPath);
	wstring wsPath = sPath;
	wsPath += L"\\";
	wsPath += m_info.m_strName;
	wsPath += L".lnk";

	return CString(wsPath.c_str());
}

void CGedtreeApp::Uninstall() 
{
	UninstallRegistry();
	DeleteStartMenuShortcut(FALSE);
	RemoveApp();
	OnAppExit();
}


void CGedtreeApp::BuildAppPaths() 
{
	CString drive = m_info.m_strAppDrive;
	CString dir = m_info.m_strAppDir;
	CString fname = m_info.m_strAppFName;
	CString ext = m_info.m_strAppExt;

	m_strAppDir = drive+dir;
	if (m_strAppDir.Right(1)=='\\')
		m_strAppDir = m_strAppDir.Left(m_strAppDir.GetLength()-1);
}

void CGedtreeApp::RemoveApp() 
{
	CString strApp = theApp.m_info.m_strAppPath;

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(strApp,drive,dir,fname,ext);

	TCHAR path_buffer[_MAX_PATH];

	_tmakepath(path_buffer,drive,dir,L"",L"");
	CString strDir = path_buffer;
	ReplaceFileOnReboot(strDir,0);

	ReplaceFileOnReboot(strApp,0);

	_tmakepath(path_buffer,drive,dir,L"groaplt",L".jar");
	ReplaceFileOnReboot(CString(path_buffer),0);

	_tmakepath(path_buffer,drive,dir,L"unicows",L".dll");
	ReplaceFileOnReboot(CString(path_buffer),0);

	_tmakepath(path_buffer,drive,dir,L"mfc42lu",L".dll");
	ReplaceFileOnReboot(CString(path_buffer),0);

	_tmakepath(path_buffer,drive,dir,L"msluirt",L".dll");
	ReplaceFileOnReboot(CString(path_buffer),0);

	_tmakepath(path_buffer,drive,dir,L"mslup60",L".dll");
	ReplaceFileOnReboot(CString(path_buffer),0);

	_tmakepath(path_buffer,drive,dir,L"mslurt",L".dll");
	ReplaceFileOnReboot(CString(path_buffer),0);

	AfxMessageBox(_T("The next time you reboot your system, the uninstallation will finish. Visit http://go.to/gro to re-download GRO if you want to re-install."));
}

void CGedtreeApp::UninstallRegistry() 
{
	if (m_pRecentFileList)
	{
		delete m_pRecentFileList;
		m_pRecentFileList = NULL;
	}

	RestoreShellFileTypes();

	// delete our app key

	::RegCloseKey(m_hkey);

	CString strKey = "Software\\";
	strKey += m_info.m_strCompany;
	strKey += "\\";
	strKey += m_pszAppName;

	DeleteKey(HKEY_CURRENT_USER,strKey);

	// for company key, delete it if it has no subkeys

	strKey = "Software\\";
	strKey += m_info.m_strCompany;

	LONG nErr;
	HKEY hKeyComp;
	nErr = ::RegOpenKey(HKEY_CURRENT_USER,strKey,&hKeyComp);
	if (nErr!=ERROR_SUCCESS) return;

	DWORD nSubKeys;
	nErr = ::RegQueryInfoKey(hKeyComp,NULL,NULL,NULL,&nSubKeys,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	::RegCloseKey(hKeyComp);

	if (nErr!=ERROR_SUCCESS) return;
	if (nSubKeys) return;

	DeleteKey(HKEY_CURRENT_USER,strKey);

	// now pretend we're permanent, so nothing else
	// gets written to the registry (like open docs list would have).
	m_info.m_bPermanent = true;
}

int CGedtreeApp::ExitInstance() 
{
	::CoUninitialize();
	return CWinApp::ExitInstance();
}

void CGedtreeApp::SavePathsToReg() 
{
	if (m_info.m_bPermanent)
		return;

	DeleteKey(m_hkey,L"OpenDocuments");

	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate* pt = GetNextDocTemplate(pos); // DL
	pos = pt->GetFirstDocPosition();
	int i(0);
	while (pos)
	{
		CGedtreeDoc* pDoc = (CGedtreeDoc*)pt->GetNextDoc(pos);
		pDoc->SavePathToReg(++i);
	}
}

void CGedtreeApp::OpenDocsFromReg() 
{
	int i(0);
	CString s;
	while (GetPathFromReg(++i,s))
		OpenDocumentFile(s);
}

void CGedtreeApp::OpenDocsFromAppDir()
{
	CString sDir = m_strAppDir+"\\";
	CString sFile = sDir+"*.ged";

	WIN32_FIND_DATA ff;
	HANDLE hFind = ::FindFirstFile(sFile,&ff);
	if (hFind == INVALID_HANDLE_VALUE) 
		return;

	bool done(false);
	while (!done)
	{
		OpenDocumentFile(sDir+ff.cFileName);
		done = !::FindNextFile(hFind,&ff);
	}

	::FindClose(hFind);
}

bool CGedtreeApp::GetPathFromReg(int i, CString& strPath) 
{
	CString s;
	s.Format(L"%d",i);
	theApp.GetReg("OpenDocuments",s,strPath,"");
	return !strPath.IsEmpty();
}

void CGedtreeApp::InitPageSetup()
{
	m_printdlg.lStructSize = sizeof(PRINTDLG);
	m_printdlg.hwndOwner = GetMainWnd()->GetSafeHwnd();
	m_printdlg.hDevMode = NULL;
	m_printdlg.hDevNames = NULL;
	m_printdlg.hDC = NULL;
	m_printdlg.Flags = 0
		|PD_RETURNDEFAULT
		|PD_RETURNIC
		|PD_NOWARNING
		|PD_USEDEVMODECOPIESANDCOLLATE
		;

	DoPrintDlg(true);
}

void CGedtreeApp::ResetPageSize()
{
	try
	{
		if (!m_printdlg.hDC)
			throw 0;

		CDC dc;
		if (!dc.Attach(m_printdlg.hDC))
			throw 0;

		m_sizePage.cx = dc.GetDeviceCaps(HORZRES)/4;
		m_sizePage.cy = dc.GetDeviceCaps(VERTRES)/4;

		dc.Detach();
	}
	catch (int)
	{
		m_sizePage.cx = 32767;
		m_sizePage.cy = 32767;
	}
}

void CGedtreeApp::OnFilePageSetup()
{
//???
//	if (m_dlgPageSetup.DoModal()==IDOK)
//		WritePageSetupToRegistry();

	m_printdlg.Flags = 0
		|PD_PRINTSETUP
		|PD_RETURNIC
		|PD_DISABLEPRINTTOFILE
		|PD_HIDEPRINTTOFILE
		|PD_NOWARNING
		|PD_USEDEVMODECOPIESANDCOLLATE
		;

	DoPrintDlg();
}

bool CGedtreeApp::DoPrintDlg(bool bForceReset)
{
	bool ok;
	ok = !!::PrintDlg(&m_printdlg);

	if (! (ok || bForceReset))
	{
		DWORD e = ::CommDlgExtendedError();
		if (e)
		{
			if (e==PDERR_NODEFAULTPRN)
				AfxMessageBox(
				L"Before you can print, you need to install a printer.\n"
				L"To do this, click Start, point to Settings, click Printers, and then double-click Add Printer.");
			else
				AfxMessageBox(L"An ERROR occurred while trying to access the PRINTER.");
			bForceReset = true;
		}
	}

	if (ok || bForceReset)
	{
		ResetPageSize();
		ResetAllDocuments();
	}

	return ok;
}

void CGedtreeApp::OnUpdateFileRegister(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_info.m_bPermanent);
}

void CGedtreeApp::OnFileRegister()
{
	if (m_info.m_bPermanent)
	{
		AfxMessageBox(L"The program must be saved on your hard drive before you register it.");
		return;
	}

	reg_t reg = GetRegistration();
	if (reg==registered_good)
	{
		AfxMessageBox(L"You have already registered the program.");
	}
	else
	{
		CRegister d;
		d.DoModal();
	}
}

void CGedtreeApp::Copy(CGedtreeDoc* pDocFrom, CGedtreeDoc* pDocTo)
{
	// maps of old items' indexes to their new indexes
	// Initially, these maps are simply a set of the items
	// to be copied (and map to -1), because we haven't
	// inserted them into the destination document yet so
	// we don't know their new index.
	CMap<int,int,int,int> mapIndi;
	CMap<int,int,int,int> mapFami;
	CMap<int,int,int,int> mapSour;
	CMap<int,int,int,int> mapRepo;
	CMap<int,int,int,int> mapNote;

	POSITION pos;
	int i;

	// find all indis to add
	for (i = 0; i<pDocFrom->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDocFrom->m_rIndividual[i];
		if (indi.m_bSelected)
			mapIndi[i] = -1;
	}

	// look for things the indis *point to*, and copy
	// *those* things into the new doc as well
	pos = mapIndi.GetStartPosition();
	while (pos)
	{
		int x; // ignored here (it's always -1)
		mapIndi.GetNextAssoc(pos,i,x);

		CIndividual& indi = pDocFrom->m_rIndividual[i];

		int j,k;

		// add this indi's name source
		if ((j = indi.m_name.m_cita.m_iSource) >= 0)
			mapSour[j] = -1;

		// add indi's spouse-to-families and child-to-family
		if ((k = indi.m_iChildToFamily) >= 0)
			mapFami[k] = -1;
		for (j = 0; j<indi.m_riSpouseToFamily.GetSize(); ++j)
			mapFami[indi.m_riSpouseToFamily[j]] = -1;

		// add indi's events' sources and notes
		for (j = 0; j<indi.m_revt.GetSize(); ++j)
		{
			CEvt& evt = indi.m_revt[j];
			if ((k = evt.m_cita.m_iSource) >= 0)
				mapSour[k] = -1;
			if ((k = evt.m_iNote) >= 0)
				mapNote[k] = -1;
		}
		// add indi's attributes' sources and notes
		for (j = 0; j<indi.m_rattr.GetSize(); ++j)
		{
			CEvt& evt = indi.m_rattr[j].m_evt;
			if ((k = evt.m_cita.m_iSource) >= 0)
				mapSour[k] = -1;
			if ((k = evt.m_iNote) >= 0)
				mapNote[k] = -1;
		}
	}

	// now that we have the complete list of families that
	// we are going to be pasting, go thru them and get their
	// events' sources and notes.
	pos = mapFami.GetStartPosition();
	while (pos)
	{
		int x;
		mapFami.GetNextAssoc(pos,i,x);

		CFamily& fami = pDocFrom->m_rFamily[i];
		for (int j(0); j<fami.m_revt.GetSize(); ++j)
		{
			int k;
			CEvt& evt = fami.m_revt[j];
			if ((k = evt.m_cita.m_iSource) >= 0)
				mapSour[k] = -1;
			if ((k = evt.m_iNote) >= 0)
				mapNote[k] = -1;
		}
	}

	// now that we have all sources, go through them and
	// get all their repositories
	pos = mapSour.GetStartPosition();
	while (pos)
	{
		int x;
		mapSour.GetNextAssoc(pos,i,x);

		CSource& sour = pDocFrom->m_rSource[i];
		int k;
		if ((k = sour.m_iRepository) >= 0)
			mapRepo[k] = -1;
	}



	// now we actually put the items into the destination
	pos = mapRepo.GetStartPosition();
	while (pos)
	{
		int x;
		mapRepo.GetNextAssoc(pos,i,x);

		CRepository& repo = pDocFrom->m_rRepository[i];
		// repositories have no links to fix up
		int j = pDocTo->PasteRepo(repo);
		mapRepo[i] = j;
	}

	pos = mapNote.GetStartPosition();
	while (pos)
	{
		int x;
		mapNote.GetNextAssoc(pos,i,x);

		CNote& note = pDocFrom->m_rNote[i];
		// notes have no lnks to fix up
		int j = pDocTo->PasteNote(note);
		mapNote[i] = j;
	}

	pos = mapSour.GetStartPosition();
	while (pos)
	{
		int x;
		mapSour.GetNextAssoc(pos,i,x);

		CSource sour = pDocFrom->m_rSource[i]; // COPY of orig sour

		// fix up links: repository
		if (sour.m_iRepository >= 0)
		{
			BOOL bFound = mapRepo.Lookup(sour.m_iRepository,x);
			ASSERT(bFound); // assert that we actually did add the repo.
			sour.m_iRepository = x;
		}

		int j = pDocTo->PasteSour(sour);
		mapSour[i] = j;
	}

	// deselect all indis in the target doc
	// (so we can have just the newly pasted indis selected)
	for (i = 0; i<pDocTo->m_rIndividual.GetSize(); i++)
		pDocTo->m_rIndividual[i].Select(FALSE);

	// paste the indis
	pos = mapIndi.GetStartPosition();
	while (pos)
	{
		int x;
		mapIndi.GetNextAssoc(pos,i,x);

		CIndividual& indi = pDocFrom->m_rIndividual[i];

		// Problem: since indis have links to themselves, we
		// cannot fix them up here, because we may not
		// yet have added an indi that is pointed to (and so
		// we wouldn't yet know what to change it to).
		// Also, we haven't inserted any families yet, so
		// we don't know what to change those links to. either.
		// So we don't do fix-ups here, we do them after
		// inserting all indis and families (note, these initial
		// indi insertions will have bad links).

		int j = pDocTo->PasteIndi(indi);
		// j is the indi's (new) index in the clipboard

		mapIndi[i] = j; // map old indi index to new indi index
	}

	// paste families
	pos = mapFami.GetStartPosition();
	while (pos)
	{
		int x;
		mapFami.GetNextAssoc(pos,i,x);

		CFamily fami = pDocFrom->m_rFamily[i]; // COPY of orig family

		// fix up links to indis (remove children/spouses that
		// we are not copying, and change indexes of the
		// ones we are)
		int n = fami.m_riChild.GetSize();
		for (int k(0); k<n; ++k)
		{
			x = fami.m_riChild[k];
			if (mapIndi.Lookup(fami.m_riChild[k],x))
			{
				fami.m_riChild[k] = x;
			}
			else
			{
				fami.m_riChild.RemoveAt(k);
				--k;
				--n;
			}
		}
		if (fami.m_iHusband >= 0)
		{
			if (mapIndi.Lookup(fami.m_iHusband,x))
				fami.m_iHusband = x;
			else
				fami.m_iHusband = -1;
		}
		if (fami.m_iWife >= 0)
		{
			if (mapIndi.Lookup(fami.m_iWife,x))
				fami.m_iWife = x;
			else
				fami.m_iWife = -1;
		}

		int j;
		BOOL bFound;
		for (j = 0; j<fami.m_revt.GetSize(); ++j)
		{
			CEvt& evt = fami.m_revt[j];
			if (evt.m_cita.m_iSource >= 0)
			{
				bFound = mapSour.Lookup(evt.m_cita.m_iSource,x);
				ASSERT(bFound);
				evt.m_cita.m_iSource = x;
			}
			if (evt.m_iNote >= 0)
			{
				bFound = mapNote.Lookup(evt.m_iNote,x);
				ASSERT(bFound);
				evt.m_iNote = x;
			}
		}

		j = pDocTo->PasteFami(fami);
		mapFami[i] = j;
	}

	// now we fix up the inids (we couldn't
	// fix them earlier)
	pos = mapIndi.GetStartPosition();
	while (pos)
	{
		int xindi;
		mapIndi.GetNextAssoc(pos,i,xindi);

		// get the indi from the NEW doc (using NEW index)
		CIndividual& indi = pDocTo->m_rIndividual[xindi];

		int x;
		BOOL bFound;

		if (indi.m_iFather >= 0)
		{
			if (mapIndi.Lookup(indi.m_iFather,x))
				indi.m_iFather = x;
			else
				indi.m_iFather = -1;
		}
		if (indi.m_iMother >= 0)
		{
			if (mapIndi.Lookup(indi.m_iMother,x))
				indi.m_iMother = x;
			else
				indi.m_iMother = -1;
		}

		int k;
		int n;
		n = indi.m_riSpouse.GetSize();
		for (k = 0; k<n; ++k)
		{
			if (mapIndi.Lookup(indi.m_riSpouse[k],x))
			{
				indi.m_riSpouse[k] = x;
			}
			else
			{
				indi.m_riSpouse.RemoveAt(k);
				--k;
				--n;
			}
		}
		n = indi.m_riChild.GetSize();
		for (k = 0; k<n; ++k)
		{
			if (mapIndi.Lookup(indi.m_riChild[k],x))
			{
				indi.m_riChild[k] = x;
			}
			else
			{
				indi.m_riChild.RemoveAt(k);
				--k;
				--n;
			}
		}
		n = indi.m_riSpouseToFamily.GetSize();
		for (k = 0; k<n; ++k)
		{
			bFound = mapFami.Lookup(indi.m_riSpouseToFamily[k],x);
			ASSERT(bFound);
			indi.m_riSpouseToFamily[k] = x;
		}
		if (indi.m_iChildToFamily >= 0)
		{
			bFound = mapFami.Lookup(indi.m_iChildToFamily,x);
			ASSERT(bFound);
			indi.m_iChildToFamily = x;
		}

		if (indi.m_name.m_cita.m_iSource >= 0)
		{
			bFound = mapSour.Lookup(indi.m_name.m_cita.m_iSource,x);
			ASSERT(bFound);
			indi.m_name.m_cita.m_iSource = x;
		}

		int j;
		for (j = 0; j<indi.m_revt.GetSize(); ++j)
		{
			CEvt& evt = indi.m_revt[j];
			if (evt.m_cita.m_iSource >= 0)
			{
				bFound = mapSour.Lookup(evt.m_cita.m_iSource,x);
				ASSERT(bFound);
				evt.m_cita.m_iSource = x;
			}
			if (evt.m_iNote >= 0)
			{
				bFound = mapNote.Lookup(evt.m_iNote,x);
				ASSERT(bFound);
				evt.m_iNote = x;
			}
		}
		for (j = 0; j<indi.m_rattr.GetSize(); ++j)
		{
			CEvt& evt = indi.m_rattr[j].m_evt;
			if (evt.m_cita.m_iSource >= 0)
			{
				bFound = mapSour.Lookup(evt.m_cita.m_iSource,x);
				ASSERT(bFound);
				evt.m_cita.m_iSource = x;
			}
			if (evt.m_iNote >= 0)
			{
				bFound = mapNote.Lookup(evt.m_iNote,x);
				ASSERT(bFound);
				evt.m_iNote = x;
			}
		}

		pDocTo->FixUpPastedIndi(xindi);
	}
	pDocTo->ResetAllViews();
	m_pClip->SetModifiedFlag(FALSE);
	pDocTo->Normalize();
	CPoint pt = pDocTo->m_pDL->GetScrollPoint();
	CSize szShift = pt-CPoint(0,0);
	CRect rectIndis = pDocTo->m_pDL->ShiftSelectedIndis(szShift);
	pDocTo->Normalize();
}

void CGedtreeApp::CloseClip() // close documents before exiting
{
	if (m_pClip)
	{
		m_pClip->OnCloseDocument();
		m_pClip = 0;
	}
}
