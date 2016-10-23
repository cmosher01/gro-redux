#pragma once
// ChooseFont.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseFont dialog

class CChooseFont : public CFontDialog
{
	DECLARE_DYNAMIC(CChooseFont)

public:
	CChooseFont(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	CChooseFont(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#endif

public:
	bool m_bScale;

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
