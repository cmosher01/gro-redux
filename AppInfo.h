#ifndef _appinfo_
#define _appinfo_

class CAppInfo
{
public:
	class CError
	{
	public:
		int nLastError;
		CError() { nLastError = ::GetLastError(); }
	};

	CString m_strAppPath;
	CString m_strAppDrive;
	CString m_strAppDir;
	CString m_strAppFName;
	CString m_strAppExt;
	UINT m_nAppDriveType;
	BOOL m_bPermanent;

	// All info is file info (not product info)
	int m_nVersion; // example: 0x00010002 means version 1.2
	CString m_strVersion; // example: "1.2"
	CString m_strInternalName;
	CString m_strDescription;
	CString m_strCopyright;
	CString m_strCompany;
	CString m_strComments;
	CString m_strName;

	CAppInfo();
	virtual ~CAppInfo();

	void Read();

protected:
	char* m_pVerBuf;
	CString GetString(const CString& strStringField);
};

#endif
