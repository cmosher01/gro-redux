#include "stdafx.h"
#include "appinfo.h"
#include "winver.h"

CAppInfo::CAppInfo() :
	m_pVerBuf(0)
{
}

CAppInfo::~CAppInfo()
{
	if (m_pVerBuf) delete [] m_pVerBuf;
}

void CAppInfo::Read()
{
	LPTSTR sApp = m_strAppPath.GetBuffer(MAX_PATH+1);
	DWORD bOK = ::GetModuleFileName(NULL,sApp,MAX_PATH+1);
	m_strAppPath.ReleaseBuffer();
	if (!bOK) throw CError();

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(m_strAppPath,drive,dir,fname,ext);
	m_strAppDrive = drive;
	m_strAppDir = dir;
	m_strAppFName = fname;
	m_strAppExt = ext;

	if (m_strAppDrive.IsEmpty())
		m_nAppDriveType = ::GetDriveType(m_strAppDir);
	else
		m_nAppDriveType = ::GetDriveType(m_strAppDrive+L"\\");

	m_bPermanent =
		m_nAppDriveType==0||
		m_nAppDriveType==1||
		m_nAppDriveType==DRIVE_REMOVABLE||
		m_nAppDriveType==DRIVE_CDROM||
		m_nAppDriveType==DRIVE_REMOTE;

	DWORD h;
	DWORD sizBuf = ::GetFileVersionInfoSize((LPTSTR)(LPCTSTR)m_strAppPath,&h);
	if (!sizBuf) throw CError();

	m_pVerBuf = new char[sizBuf];
	bOK = ::GetFileVersionInfo((LPTSTR)(LPCTSTR)m_strAppPath,h,sizBuf,m_pVerBuf);
	if (!bOK) throw CError();

	m_strDescription = GetString("FileDescription");
	m_strCopyright = GetString("LegalCopyright");
	m_strCompany = GetString("CompanyName");
	m_strComments = GetString("Comments");
	m_strInternalName = GetString("InternalName");
	m_strName = GetString("ProductName");

	UINT nLen;
	VS_FIXEDFILEINFO* pFix;

	bOK = ::VerQueryValue(m_pVerBuf,_T("\\"),(void**)&pFix,&nLen);

	m_nVersion = pFix->dwFileVersionMS;

	short int* rVers = (short int*)&m_nVersion;
	m_strVersion.Format(_T("%d.%d"),rVers[1],rVers[0]);
}

CString CAppInfo::GetString(const CString& strStringField)
{
	CString s;
	UINT nLen;
	LPTSTR prc;

	CString strField = "\\StringFileInfo\\040904B0\\";
	strField += strStringField;

	if (::VerQueryValue(m_pVerBuf,(LPTSTR)(LPCTSTR)strField,(void**)&prc,&nLen))
		s = prc;

	return s;
}
