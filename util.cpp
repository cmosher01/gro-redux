#include "stdafx.h"
#include "util.h"

CString CUtil::str(const int n)
{
	char s[17];
	_itoa(n,s,10);
	return CString(s);
}

BOOL CUtil::DirectoryExists(const CString& strDir)
{
	DWORD dwAttrib = ::GetFileAttributes(strDir);

	if (dwAttrib==MAXDWORD)
		return FALSE;

	if (dwAttrib&FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;

	return FALSE;
}

CString CUtil::GetWindowsDirectory()
{
	CString str;
	::GetWindowsDirectory(str.GetBuffer(MAX_PATH+1),MAX_PATH+1);
	str.ReleaseBuffer();

	if (!str.IsEmpty())
		if (str.Right(1)!="\\")
			str += "\\";

	return str;
}
