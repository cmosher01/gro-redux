// gedtree.h : main header file for the GEDTREE application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <string>

#include "resource.h"       // main symbols
#include "appinfo.h"
#include "IndiClip.h"
#include "WebServer.h"

using namespace std;

#define SetDispText(s) item.pszText = (LPTSTR)(LPCTSTR)(s);

/////////////////////////////////////////////////////////////////////////////
// CGedtreeApp:
// See gedtree.cpp for the implementation of this class
//

class CGedtreeApp : public CWinApp
{
public:
	CGedtreeApp();
	virtual ~CGedtreeApp();
	BOOL ConfirmDelete(const CString& strItem);
	void GetReg(const CString& strKey,const CString& strSub,CString& strVal,const CString& strDefault);
	void GetReg(const CString& strKey,const CString& strSub,int& nVal,const int& nDefault);
	void PutReg(const CString& strKey,const CString& strSub,const CString& strVal);
	void PutReg(const CString& strKey,const CString& strSub,const int& nVal);
	void SaveShellFileTypes();
	void RestoreShellFileTypes();
	void PrepareForRegisterShellFileTypes();
	void MoveKey(CDocTemplate* pt, enum CDocTemplate::DocStringIndex idocstr, HKEY hKeySrc, HKEY hKeyDst);
	void RemoveKey(CDocTemplate* pt, enum CDocTemplate::DocStringIndex idocstr, HKEY hKeySrc);
	void CopyKey(HKEY hKeySrc, HKEY hKeyDest, LPCTSTR name);
	void DeleteKey(HKEY hKey, LPCTSTR name);
	void ResetAllDocuments(UINT flagsChanged = -1);
	void ResetFont();
	BOOL SaveAsUnicode();
	CDocument* GetOpenDocument(LPCTSTR lpszFileName);
	BOOL LowMem();
	void Uninstall();
	void RemoveApp();
	void UninstallRegistry();
	void CreateStartMenuShortcut(BOOL bShowMessage = TRUE);
	void DeleteStartMenuShortcut(BOOL bShowMessage = TRUE);
	CString GetStartMenuShortcutFileName();
	void AddTemplates();
	CDC* GetDC();
	void SavePathsToReg();
	void OpenDocsFromReg();
	void OpenDocsFromAppDir();
	bool GetPathFromReg(int i, CString& strPath);
	bool DoPrintDlg(bool bForceReset = false);
	void Copy(CGedtreeDoc* pDocFrom, CGedtreeDoc* pDocTo);
	void CloseClip();
	void GetDocs(wostream& os);
	void htmlFooter(wostream& os);
	CGedtreeDoc* FindDoc(const wstring& sid);
	void writecss(wostringstream& os, wstring& mime, wstring& lastmod, const wstring& ifmod);
	void webFile(const wstring& path, thinsock& sock, wstring& mime);

	CAppInfo m_info;
	HKEY m_hkey;
	CFont m_font;
	CFont m_fontBigBold;
	CFont m_fontSmall;
	double m_fontRescale;
	CDC* m_pDC;
	CSize m_sizePage;
	CString m_strAppDir;
	PRINTDLG m_printdlg;
	bool m_bShowPageBreaks;
	CGedtreeDoc* m_pClip;
	WebServer* m_pWebServer;
	ULARGE_INTEGER m_ulBuildTime;
	wstring m_strBuildTime;
	double m_nMaxIndiWidth;
	double m_nIndiBorderX;
	double m_nIndiBorderY;

	class groch : public WebServer::ContentHandler
	{
	public:
		groch() {}
		virtual wstring getHTML(const wstring& path, wstring& mime, thinsock& sock, wstring& lastmod, const wstring& ifmod);
//		virtual wstring getContype(const wstring& path, wstring& mime, thinsock& sock, wstring& lastmod, const wstring& ifmod);
	};
	groch mgroch;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGedtreeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGedtreeApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFont();
	afx_msg void OnViewSubmitter();
	afx_msg void OnOptionsUnicode();
	afx_msg void OnOptionsPageBreaks();
	afx_msg void OnUpdateOptionsPageBreaks(CCmdUI* pCmdUI);
	afx_msg void OnToolsHoliday();
	afx_msg void OnBirthCalc();
	afx_msg void OnUpdateFileSetup(CCmdUI* pCmdUI);
	afx_msg void OnFileSetup();
	afx_msg void OnFileServe();
	afx_msg void OnFilePageSetup();
	afx_msg void OnUpdateFileRegister(CCmdUI* pCmdUI);
	afx_msg void OnFileRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void GetRegKey();
	void InitFonts(LOGFONT* plf);
	void FontBigBold(LOGFONT* plf);
	void FontSmall(LOGFONT* plf);
	void WriteFontToRegistry(const LOGFONT* plf);
	void ReadFontFromRegistry();
	void WritePageSetupToRegistry();
	void ReadPageSetupFromRegistry();
	BOOL FirstInstance();
	void BuildAppPaths();
	void InitPageSetup();
	void ResetPageSize();
	void CreateClipboard();
};


/////////////////////////////////////////////////////////////////////////////

extern CGedtreeApp theApp;
