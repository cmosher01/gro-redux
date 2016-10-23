// GedCmd.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "GedCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGedCmd class

CGedCmd::CGedCmd()
{
}

BOOL CGedCmd::Process()
{
	theApp.ParseCommandLine(*this);

	if (m_nShellCommand==FileNew)
		m_nShellCommand = FileNothing;

	// Dispatch standard commands
	if (!theApp.ProcessShellCommand(*this))
		return FALSE;

	return TRUE;
}

#define Flag(s) strParam.CompareNoCase(_T(s))==0

void CGedCmd::ParseParam(const char* pszParam,BOOL bFlag,BOOL bLast)
{
	bool bHandled(false);

/*
	CString strParam(pszParam);
	if (bFlag)
	{
		if (Flag("uninstall"))
		{
			m_bUninstall = true;
			bHandled = true;
		}
	}
	else
	{
		if (m_bUninstall)
		{
			bHandled = true;
			if (bLast)
				m_strFile = strParam;
			else
				m_h = (HANDLE)atoi(strParam);
		}
	}
*/

	if (!bHandled)
		CCommandLineInfo::ParseParam(pszParam,bFlag,bLast);
}

BOOL CGedCmd::PostProcess()
{
	return true;
}
