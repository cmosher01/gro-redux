// ChooseFont.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "ChooseFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseFont

IMPLEMENT_DYNAMIC(CChooseFont, CFontDialog)

CChooseFont::CChooseFont(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags|CF_ENABLETEMPLATE, pdcPrinter, pParentWnd),
	m_bScale(true)
{
	m_cf.hInstance = ::AfxGetInstanceHandle();
	m_cf.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSEFONT);
}

BEGIN_MESSAGE_MAP(CChooseFont, CFontDialog)
END_MESSAGE_MAP()

BOOL CChooseFont::OnInitDialog() 
{
	BOOL bRet = CFontDialog::OnInitDialog();
	CheckDlgButton(IDC_SCALE,m_bScale);
	return bRet;
}

void CChooseFont::OnOK() 
{
	m_bScale = !!IsDlgButtonChecked(IDC_SCALE);
	CFontDialog::OnOK();
}
