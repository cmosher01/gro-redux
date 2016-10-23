// gedtreeView.cpp : implementation of the CGedtreeView class
//

#include "stdafx.h"
#include "gedtree.h"
#include "gedtreeView.h"
#include "gedtreeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGedtreeView

IMPLEMENT_DYNCREATE(CGedtreeView, CScrollView)

BEGIN_MESSAGE_MAP(CGedtreeView, CScrollView)
	//{{AFX_MSG_MAP(CGedtreeView)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGedtreeView construction/destruction

CGedtreeView::CGedtreeView()
{
}

CGedtreeView::~CGedtreeView()
{
}

void CGedtreeView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	UpdateTitle();
}

void CGedtreeView::UpdateTitle()
{
	CString sTitle;
	sTitle =  GetDocument()->GetTitle();
	sTitle += " - ";
	sTitle += GetWindowTitle();

	GetParent()->SetWindowText(sTitle);
}

CString CGedtreeView::GetWindowTitle()
{
	ASSERT(FALSE);
	return CString();
}

void CGedtreeView::Reset(UINT flagsChanged)
{
	ASSERT(FALSE);
}
