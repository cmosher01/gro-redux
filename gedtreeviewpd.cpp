// gedtreeViewPD.cpp : implementation of the CGedtreeViewPD class
//

#include "stdafx.h"
#include "gedtree.h"

#include "gedtreeDoc.h"
#include "gedtreeViewPD.h"
#include "editevent.h"
#include "editname.h"
#include "editsex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CSize MARGIN(10,10);

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD

IMPLEMENT_DYNCREATE(CGedtreeViewPD, CGedtreeView)

BEGIN_MESSAGE_MAP(CGedtreeViewPD, CGedtreeView)
	//{{AFX_MSG_MAP(CGedtreeViewPD)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD construction/destruction

CGedtreeViewPD::CGedtreeViewPD():
	m_iIndi(-1),
	m_bInit(FALSE),
	m_cGen(5)
{
}

CGedtreeViewPD::~CGedtreeViewPD()
{
}

BOOL CGedtreeViewPD::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

CString CGedtreeViewPD::GetWindowTitle()
{
	CString s;
	if (m_iIndi>=0)
	{
		CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
		s = indi.Name()+" ";
	}
	s += "Pedigree";
	return s;
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD drawing

void CGedtreeViewPD::OnDraw(CDC* pDC)
{
	CGedtreeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPen pen(PS_SOLID,1,::GetSysColor(COLOR_3DSHADOW));
	pDC->SelectObject(&pen);

	COLORREF colorText = ::GetSysColor(COLOR_WINDOWTEXT);
	pDC->SetTextColor(colorText);
	COLORREF colorBack = ::GetSysColor(COLOR_WINDOW);
	pDC->SetBkColor(colorBack);

	pDC->SelectObject(&theApp.m_font);

	TEXTMETRIC tm;
	pDC->GetOutputTextMetrics(&tm);
	m_dy = -(tm.tmAscent+1);
	m_dx = 3;

	CRect rectClient;
	GetClientRect(rectClient);

	CRect r;
	r.top = 0;
	r.left = 0;
	r.bottom = r.right = 65535;
	pDC->DrawText("Use + or - to change the number of generations.",r,DT_NOCLIP|DT_NOPREFIX);

	CPoint pt(0,rectClient.bottom/2);
	int dx = rectClient.Width()/(m_cGen+1);
	int dy = rectClient.Height()/4;

	DrawGen(pDC,pt,dx,dy,m_cGen,m_iIndi);
}

void CGedtreeViewPD::DrawGen(CDC* pDC, CPoint ptStart, int dx, int dy, int g, int iIndi)
{
	CIndividual* pIndi = NULL;
	if (iIndi>=0)
		pIndi = &GetDocument()->m_rIndividual[iIndi];

	CPoint pt(ptStart), pt1;

	pDC->MoveTo(pt);

	DrawPerson(pDC,pt,pIndi);

	pt += CSize(dx,0);
	pDC->LineTo(pt);

	if (!g) return;

	int i;

	pt1 = pt-CSize(0,dy);
	pDC->LineTo(pt1);
	if (pIndi)
		i = pIndi->m_iFather;
	else
		i = -1;
	DrawGen(pDC,pt1,dx,dy/2,g-1,i);

	pDC->MoveTo(pt);

	pt1 = pt+CSize(0,dy);
	pDC->LineTo(pt1);
	if (pIndi)
		i = pIndi->m_iMother;
	else
		i = -1;
	DrawGen(pDC,pt1,dx,dy/2,g-1,i);
}

void CGedtreeViewPD::DrawPerson(CDC* pDC, CPoint pt, CIndividual* pIndi)
{
	if (!pIndi) return;

	CRect r;
	r.top = pt.y+m_dy;
	r.left = pt.x+m_dx;
	r.bottom = r.right = 65535;

	pDC->DrawText(pIndi->m_strTreeDisplay,r,DT_NOCLIP|DT_NOPREFIX);
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD printing

BOOL CGedtreeViewPD::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGedtreeViewPD::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGedtreeViewPD::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD diagnostics

#ifdef _DEBUG
void CGedtreeViewPD::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGedtreeViewPD::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewPD message handlers

void CGedtreeViewPD::Init(int iIndi)
{
	m_iIndi = iIndi;
	Reset();
}

void CGedtreeViewPD::OnInitialUpdate() 
{
	CGedtreeView::OnInitialUpdate();

	CRect r;
	GetClientRect(&r);

	if (m_bInit)
		PositionControls(r.right,r.bottom);
	else
		SetScrollSizes(MM_TEXT,r.Size());

	m_bInit= TRUE;
}

void CGedtreeViewPD::PositionControls(int cx, int cy)
{
	if (m_iIndi==-1) return;

	SetRedraw(FALSE);

	SetScrollSizes(MM_TEXT,CSize(cx,cy));

	SetRedraw();
	Invalidate();
}

void CGedtreeViewPD::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)
	{
//		if (m_treePedigree.m_hWnd)
//		{
			PositionControls(cx,cy);
//		}
	}
}

void CGedtreeViewPD::OnDestroy() 
{
	CGedtreeView::OnDestroy();
	
	// TODO: Add your message handler code here

	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	indi.m_pPedigree = NULL;
}

void CGedtreeViewPD::Reset(UINT flagsChanged)
{
	SetRedraw(FALSE);

	SetRedraw();
	Invalidate();
}

void CGedtreeViewPD::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CGedtreeDoc* pDoc = GetDocument();

//	CIndividual& indi = GetDocument()->m_rIndividual[m_iIndi];
	//indi.OpenPed();

	CGedtreeView::OnLButtonDblClk(nFlags, point);
}

void CGedtreeViewPD::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar=='-')
	{
		if (m_cGen)
		{
			m_cGen--;
			Reset();
		}
	}
	else if (nChar=='+')
	{
		m_cGen++;
		Reset();
	}
	CGedtreeView::OnChar(nChar, nRepCnt, nFlags);
}
