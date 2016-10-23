// gedtreeViewDL.cpp : implementation of the CGedtreeViewDL class
//

#include "stdafx.h"
#include "gedtree.h"

#include "gedtreeDoc.h"
#include "gedtreeViewDL.h"
#include "EditDate.h"
#include "zoom.h"
#include "pickrels.h"
#include "find.h"
#include "connect.h"
#include "util.h"
#include "progress.h"
#include "places.h"
#include "mydc.h"
#include "reg.h"
#include <math.h>
#include <time.h>
#include <iterator>
#include <iostream>
#include <list>
#include <set>
#include <algorithm>

//#include "anydec.h"
//#include "winbmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CSize SIZ_SCROLL_LINE(80,80);

extern char *sTplIndi;
extern char *sTplSour;
extern char *sTplHeadrtf;
extern char *sTplIndirtf;
extern char *sTplSourrtf;
extern char *sTplBibrtf;


/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL

IMPLEMENT_DYNCREATE(CGedtreeViewDL, CGedtreeView)

BEGIN_MESSAGE_MAP(CGedtreeViewDL, CGedtreeView)
	//{{AFX_MSG_MAP(CGedtreeViewDL)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_EDIT_ALIGNTOP, OnEditAlignTop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ALIGNTOP, OnUpdateEditAlignTop)
	ON_COMMAND(ID_EDIT_CLEAN, OnEditClean)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAN, OnUpdateEditClean)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_SELECTANCESTORS, OnEditSelectAncestors)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTANCESTORS, OnUpdateEditSelectAncestors)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	ON_COMMAND(ID_VIEW_SCROLLTOSELECTION, OnViewScrolltoselection)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCROLLTOSELECTION, OnUpdateViewScrolltoselection)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FINDNEXT, OnEditFindnext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FINDNEXT, OnUpdateEditFindnext)
	ON_COMMAND(ID_EDIT_DISCONNECT, OnEditDisconnect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DISCONNECT, OnUpdateEditDisconnect)
	ON_COMMAND(ID_EDIT_NEWINDIVIDUAL, OnEditNewindividual)
	ON_COMMAND(ID_EDIT_CONNECT, OnEditConnect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CONNECT, OnUpdateEditConnect)
	ON_COMMAND(ID_VIEW_OPENPEDIGREE, OnViewOpenpedigree)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPENPEDIGREE, OnUpdateViewOpenpedigree)
	ON_COMMAND(ID_VIEW_DROPLINECHART, OnViewDroplinechart)
	ON_COMMAND(ID_VIEW_INDI, OnViewIndi)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INDI, OnUpdateViewIndi)
	ON_COMMAND(ID_FILE_HTML, OnFileHTML)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_CENSUS, OnViewCensus)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CENSUS, OnUpdateViewCensus)
	ON_COMMAND(ID_VIEW_PLACE, OnViewPlace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLACE, OnUpdateViewPlace)
	ON_COMMAND(ID_VIEW_ANOMALIES, OnViewAnomalies)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANOMALIES, OnUpdateViewAnomalies)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_RTF, OnFileRTF)
	ON_COMMAND(ID_EDIT_HIDEUNSELECTED, OnEditHideunselected)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HIDEUNSELECTED, OnUpdateEditHideunselected)
	ON_COMMAND(ID_EDIT_SHOWALL, OnEditShowall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOWALL, OnUpdateEditShowall)
	ON_COMMAND(ID_EDIT_SHOWASOF, OnEditShowAsOf)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOWASOF, OnUpdateEditShowAsOf)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL construction/destruction

CGedtreeViewDL::CGedtreeViewDL() :
	m_bMoving(FALSE),
	m_bSelecting(FALSE),
	m_nScale(1),
	m_bCtrl(false),
	m_bShift(false),
	m_iLastSel(-1),
	m_hdib(0),
	m_pdcUse(0),
	m_ppen(0),
	m_dibWidth(-1),
	m_dibHeight(-1),
	m_bSomeHidden(false),
	m_pAsOfDate(0)
{
	m_colorFamily = (COLORREF)::GetSysColor(COLOR_3DSHADOW);
	m_ppen = new CPen(PS_SOLID,1,m_colorFamily);
}

CGedtreeViewDL::~CGedtreeViewDL()
{
	delete m_ppen;
	delete m_pdcUse;
	if (m_hdib)
		::DeleteObject(m_hdib);

	delete m_pAsOfDate;
}

BOOL CGedtreeViewDL::PreCreateWindow(CREATESTRUCT& cs)
{
	return CBigScrollView::PreCreateWindow(cs);
}

CString CGedtreeViewDL::GetWindowTitle()
{
	return "Drop-Line Chart";
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL drawing

void CGedtreeViewDL::OnDraw(CDC* pDC)
{
	int i;

	CGedtreeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ASSERT(!pDC->IsPrinting());

	CRect rectClient;
	GetClientRect(rectClient);
	MyRect rectDraw(rectClient);
	rectDraw += GetScrollPosition();

	// Draw into a bitmap in memory, then transfer to screen.
	// This reduces flashing or flickering on the screen.

	if (rectDraw.Width() != m_dibWidth || rectDraw.Height() != m_dibHeight)
	{
		// Create memory DIB
		m_dibWidth = rectDraw.Width();
		m_dibHeight = rectDraw.Height();

		BITMAPINFO bim;
		bim.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bim.bmiHeader.biWidth = m_dibWidth;
		bim.bmiHeader.biHeight = m_dibHeight;
		bim.bmiHeader.biPlanes  = 1;
		bim.bmiHeader.biBitCount = 24;
		bim.bmiHeader.biCompression = BI_RGB;
		bim.bmiHeader.biSizeImage = 0;
		bim.bmiHeader.biXPelsPerMeter = 0;
		bim.bmiHeader.biYPelsPerMeter = 0;
		bim.bmiHeader.biClrUsed = 0;
		bim.bmiHeader.biClrImportant = 0;
		bim.bmiColors[0].rgbBlue = 0;
		bim.bmiColors[0].rgbGreen = 0;
		bim.bmiColors[0].rgbRed = 0;
		bim.bmiColors[0].rgbReserved = 0;

		if (m_hdib)
			::DeleteObject(m_hdib);
		void* pBits;
		m_hdib = ::CreateDIBSection(theApp.GetDC()->m_hDC,&bim,DIB_RGB_COLORS,&pBits,0,0);
		if (!m_hdib)
		{
			DWORD e = ::GetLastError();
			::AfxMessageBox(L"Cannot create DIB: %d",e);
		}

		delete m_pdcUse;
		m_pdcUse = new CDC();
		BOOL bOK = m_pdcUse->CreateCompatibleDC(pDC);
		if (!bOK)
		{
			::AfxMessageBox(L"Cannot create device context.");
		}
		HGDIOBJ olddib = ::SelectObject(m_pdcUse->m_hDC,m_hdib);
		m_pdcUse->SetTextColor(RGB(0,0,0));
		m_pdcUse->SelectObject(m_ppen);
	}
	if (!m_pdcUse) return;
//	theApp.m_font.GetLogFont();
	m_pdcUse->SelectObject(&theApp.m_font);

	CMyDC dc(m_pdcUse,rectDraw);
	dc.ClearBackground();
	dc.FrameRectGray(pDoc->GetBounds());

	// Draw the printed page boundaries
	if (theApp.m_bShowPageBreaks)
	{
		COLORREF colorPrint(RGB(192,192,192));
		CPen pen2(PS_SOLID,1,colorPrint);
		CGdiObject* poldpen = m_pdcUse->SelectObject(&pen2);
		COLORREF oldcolor = m_pdcUse->GetTextColor();
		m_pdcUse->SetTextColor(colorPrint);

		PrintPages(dc,rectDraw);
		m_pdcUse->SelectObject(poldpen);
		m_pdcUse->SetTextColor(oldcolor);
	}

	// Draw the selection rectangle, if any
	if (m_bSelecting)
		dc.Drag(m_rectPrev);

	// draw hidden families, then individuals
	if (m_bSomeHidden || m_pAsOfDate)
	{
		for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
		{
			CFamily& fami = pDoc->m_rFamily[i];
			if (fami.m_bHidden)
				fami.OnDraw(dc);
		}
		for (i = 0; i<pDoc->m_rIndividual.GetSize(); i++)
		{
			CIndividual& indi = pDoc->m_rIndividual[i];
			if (indi.m_bHidden)
				indi.OnDraw(dc);
		}
	}
	// draw non-hidden families, then individuals
	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		if (!fami.m_bHidden)
			fami.OnDraw(dc);
	}
	for (i = 0; i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (!indi.m_bHidden)
			indi.OnDraw(dc);
	}

	// Transfer the bitmap (which is in memory) to the screen.
	CPoint org = pDC->GetViewportOrg();
	pDC->SetViewportOrg(0,0);
	BOOL bOK = pDC->BitBlt(0,0,rectDraw.Width(),rectDraw.Height(),m_pdcUse,0,0,SRCCOPY);

	pDC->SetViewportOrg(org);
}

void CGedtreeViewDL::PrintPages(CMyDC& dc, const MyRect& draw)
{
	if (!theApp.m_bShowPageBreaks)
		return;

	CGedtreeDoc* pDoc = GetDocument();
	CRect rect(pDoc->GetBounds());
	int x = theApp.m_sizePage.cx/GetScale();
	int y = theApp.m_sizePage.cy/GetScale();
	int pageswide = (rect.Width()+x-1)/x;

	CRect drawdoc;
	drawdoc.left = __max(draw.left,rect.left);
	drawdoc.right = __min(draw.right,rect.right);
	drawdoc.top = __max(draw.top,rect.top);
	drawdoc.bottom = __min(draw.bottom,rect.bottom);

	for (int vertline = (drawdoc.left+x-1)/x*x; vertline < (drawdoc.right+x-1)/x*x; vertline += x)
	{
		dc.DrawLine(CPoint(vertline,rect.top),CPoint(vertline,rect.bottom));
	}
	for (int horzline = (drawdoc.top+y-1)/y*y; horzline < (drawdoc.bottom+y-1)/y*y; horzline += y)
	{
		dc.DrawLine(CPoint(rect.left,horzline),CPoint(rect.right,horzline));
	}

	int pageFirstOfRow(drawdoc.left/x+drawdoc.top/y*pageswide);
	int pageLastOfRow(drawdoc.right/x+drawdoc.top/y*pageswide);
	int pageLast(drawdoc.right/x+drawdoc.bottom/y*pageswide);
	int f(__max(1,5/GetScale()));
	CSize offset(f,f);
	CString s;
	while (pageLastOfRow <= pageLast)
	{
		for (int page(pageFirstOfRow); page <= pageLastOfRow; page++)
		{
			int left = page%pageswide*x;
			int top = page/pageswide*y;
			int right = left+x;
			int bottom = top+y;
			CRect r(left+offset.cx,top+offset.cy,right,bottom);
			if (m_nScale==1)
				s.Format(L"page %d",page+1);
			else
				s.Format(L"%d",page+1);
			dc.DrawText(s,r);
		}
		pageFirstOfRow += pageswide;
		pageLastOfRow += pageswide;
	}
}

CIndividual* CGedtreeViewDL::HitIndiTest(const CPoint& point)
{
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(pDoc->m_rIndividual.GetSize()-1); i>=0; i--)
	{
		if (pDoc->m_rIndividual[i].HitTest(point))
			return &pDoc->m_rIndividual[i];
	}
	return NULL;
}

void CGedtreeViewDL::SelectHitIndis(const CRect& rect)
{
	CGedtreeDoc* pDoc = GetDocument();
	int cHit(0);
	CIndividual* pIndi = NULL;
	for (int i(pDoc->m_rIndividual.GetSize()-1); i>=0; i--)
	{
		CIndividual* pIndiCurr = &pDoc->m_rIndividual[i];
		if (!pIndiCurr->m_bHidden && pIndiCurr->HitTest(rect))
		{
			pIndiCurr->Select();
			cHit++;
			pIndi = pIndiCurr;
		}
	}
	CString sMsg;
	if (cHit==1)
	{
		sMsg = pIndi->Name();
	}
	((CFrameWnd*)AfxGetMainWnd())->SetMessageText(sMsg);
}

CRect CGedtreeViewDL::ShiftSelectedIndis(const CSize& sizShift)
{
	MyRect rectBounds; // bounds of new pos of indis
	rectBounds.SetRectEmpty();

	if (sizShift!=CSize(0,0))
	{
		CGedtreeDoc* pDoc = GetDocument();
		for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
		{
			CIndividual& indi = pDoc->m_rIndividual[i];
			if (indi.m_bSelected)
			{
				indi.Shift(sizShift);
				rectBounds |= indi.GetUnscaledFrameRect();
			}
		}
	}

	return rectBounds;
}

BOOL CGedtreeViewDL::IndiSelected()
{
	BOOL bSelected(FALSE);
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); !bSelected && i<pDoc->m_rIndividual.GetSize(); i++)
	{
		if (pDoc->m_rIndividual[i].m_bSelected)
			bSelected = TRUE;
	}
	return bSelected;
}

BOOL CGedtreeViewDL::OneIndiSelected()
{
	BOOL bSelected(FALSE);
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		if (pDoc->m_rIndividual[i].m_bSelected)
		{
			if (bSelected)
				return FALSE;
			else
				bSelected = TRUE;
		}
	}
	return bSelected;
}

void CGedtreeViewDL::DeselectAll()
{
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		pDoc->m_rIndividual[i].Select(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL printing

void CGedtreeViewDL::OnFilePrint()
{
	// calculate total number of pages
	CRect rect(GetDocument()->GetBounds());
	int x = theApp.m_sizePage.cx;
	int y = theApp.m_sizePage.cy;
	CSize sizePages = CSize((rect.Width()+x-1)/x,(rect.Height()+y-1)/y);
	int pageswide = sizePages.cx;
	int pageshigh = sizePages.cy;


	theApp.m_printdlg.nFromPage = theApp.m_printdlg.nMinPage = 1;
	theApp.m_printdlg.nToPage = theApp.m_printdlg.nMaxPage = (WORD)(pageshigh*pageswide);

	theApp.m_printdlg.Flags = 0
		|PD_RETURNDC
		|PD_DISABLEPRINTTOFILE
		|PD_HIDEPRINTTOFILE
		|PD_NOWARNING
		|PD_USEDEVMODECOPIESANDCOLLATE
		;

	if (theApp.DoPrintDlg())
	{
		CDC dc;
		dc.Attach(theApp.m_printdlg.hDC);
		dc.m_bPrinting = true;

		DOCINFO docinfo;
		memset(&docinfo, 0, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDocName = L"drop-line chart";

		if (dc.StartDoc(&docinfo) < 0)
			throw 0;

		// print the whole document here
		for (int page(theApp.m_printdlg.nFromPage-1); page<=theApp.m_printdlg.nToPage-1; page++)
		{
			if (dc.StartPage() < 0)
				throw 0;

			int row = page/pageswide;
			int col = page%pageswide;
			ASSERT(row < pageshigh);
			ASSERT(col < pageswide);

			int x = col*theApp.m_sizePage.cx*4;
			int y = row*theApp.m_sizePage.cy*4;

			CRect r(x,y,x+theApp.m_sizePage.cx*4,y+theApp.m_sizePage.cy*4);
			CMyDC mydc(&dc,r);
			MyPrint(mydc);

			dc.EndPage();
		}

		dc.EndDoc();
		dc.Detach();
	}
}

void CGedtreeViewDL::MyPrint(CMyDC& dc)
{
/*
	CSize sizWndOrg = pDCUse->GetWindowOrg();
	CSize sizWndExt = pDCUse->GetWindowExt();
	CSize sizVwpOrg = pDCUse->GetViewportOrg();
	CSize sizVwpExt = pDCUse->GetViewportExt();
//	pDCUse->SetWindowExt(32767, 32767); 
	CString s;
	s.Format(L"w=(%d,%d)(%d,%d)   v=(%d,%d)(%d,%d)\n",
		sizWndOrg.cx,sizWndOrg.cy,sizWndExt.cx,sizWndExt.cy,
		sizVwpOrg.cx,sizVwpOrg.cy,sizVwpExt.cx,sizVwpExt.cy);
	::OutputDebugString(s);
//	CRect r;
//	GetClientRect(&r);
//	pDCUse->SetViewportOrg(0,0);
//	pDCUse->SetViewportExt(r.Width()/2,r.Height()/2);
*/
	CGedtreeDoc* pDoc = GetDocument();

	CDC* pDCUse = dc.GetDC();

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	pDCUse->SelectObject(&pen);
	for (int i(0); i<pDoc->m_rFamily.GetSize(); i++)
		pDoc->m_rFamily[i].OnDraw(dc);

	LOGFONT lf;
	theApp.m_font.GetLogFont(&lf);
	CFont fontUse;
	lf.lfHeight *= 4;
	lf.lfHeight += 4;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	fontUse.CreateFontIndirect(&lf);
	pDCUse->SelectObject(&fontUse);

	for (i = 0; i<pDoc->m_rIndividual.GetSize(); i++)
		pDoc->m_rIndividual[i].OnDraw(dc);
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL diagnostics

#ifdef _DEBUG
void CGedtreeViewDL::AssertValid() const
{
	CBigScrollView::AssertValid();
}

void CGedtreeViewDL::Dump(CDumpContext& dc) const
{
	CBigScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGedtreeViewDL message handlers

void CGedtreeViewDL::OnInitialUpdate() 
{
	CGedtreeView::OnInitialUpdate();
	SetTotalSize();

	if (!IndiSelected())
	{
		CGedtreeDoc* pDoc = GetDocument();
		if (pDoc->m_rIndividual.GetSize()>0)
		{
			CIndividual& indi = pDoc->m_rIndividual[0];
			indi.Select();
			OnViewScrolltoselection();
		}
	}
}

void CGedtreeViewDL::SetTotalSize()
{
	CSize sizBounds = GetDocument()->GetBounds().Size();

	CRect rectClient;
	GetClientRect(rectClient);
	CSize sizClient(rectClient.right,rectClient.bottom);

	SetScrollSizes(MM_TEXT,sizBounds,sizClient,SIZ_SCROLL_LINE);
}

void CGedtreeViewDL::OnSize(UINT nType, int cx, int cy) 
{
	CBigScrollView::OnSize(nType, cx, cy);
}

BOOL CGedtreeViewDL::IsOut(const CPoint& point)
{
	CRect rectc;
	GetClientRect(rectc);
	MyRect rectClient(rectc);

	return !rectClient.PtInRect(point);
}

void CGedtreeViewDL::ScrollToward(const CPoint& point)
{
	CRect rectClient;
	GetClientRect(rectClient);

	CSize szScroll(0,0);
	if (point.x>=rectClient.right)
		szScroll.cx = SIZ_SCROLL_LINE.cx;
	if (point.x<=rectClient.left)
		szScroll.cx = -SIZ_SCROLL_LINE.cx;
	if (point.y>=rectClient.bottom)
		szScroll.cy = SIZ_SCROLL_LINE.cy;
	if (point.y<=rectClient.top)
		szScroll.cy = -SIZ_SCROLL_LINE.cy;

	ScrollToPosition(GetScrollPosition()+szScroll);
}

void CGedtreeViewDL::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pointDoc(point);
	pointDoc += GetScrollPosition();

	BOOL bExtended = (MK_SHIFT|MK_CONTROL) & nFlags;

	CIndividual* pIndi = HitIndiTest(pointDoc);
	if (pIndi)
	{
		m_bMoving = TRUE;

		if (!pIndi->m_bSelected)
		{
			if (!bExtended)
				DeselectAll();
			if (!pIndi->m_bHidden)
			{
				pIndi->Select();
				m_iLastSel = -1;
			}
			((CFrameWnd*)AfxGetMainWnd())->SetMessageText(pIndi->Name());
		}
		else
		{
			if (bExtended)
				pIndi->Select(FALSE);
		}

		m_pointPrev = pointDoc;

		SetCapture();
		ResetAllViews();
	}
	else
	{
		m_bSelecting = TRUE;

		if (!bExtended)
			DeselectAll();

		m_pointPrev = pointDoc;
		m_rectPrev = CRect(pointDoc,pointDoc);

		SetCapture();
		ResetAllViews();
	}

	CBigScrollView::OnLButtonDown(nFlags, point);
}

void CGedtreeViewDL::PostNextMove(UINT nFlags, CPoint point) 
{
	::Sleep(30);
	MSG msg;
	if (!::PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_NOREMOVE))
		PostMessage(WM_MOUSEMOVE,nFlags,MAKELPARAM(point.x,point.y));
}

double CGedtreeViewDL::GetScale()
{
	return (double)m_nScale;
}

void CGedtreeViewDL::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bExtended = (MK_SHIFT|MK_CONTROL) & nFlags;

	if (m_bMoving)
	{
		if (IsOut(point)) ScrollToward(point);

		CPoint pointDoc(point);
		pointDoc += GetScrollPosition();

		CGedtreeDoc* pDoc = GetDocument();

		CSize szShift = pointDoc-m_pointPrev;
		CRect rectIndis = ShiftSelectedIndis(szShift);

		CSize szUnshift(0,0);
		if (!rectIndis.IsRectNull())
		{
			// Now that we moved them, check to see if they
			// went out of bounds (i.e., off any of the four edges
			// of the document bounds). If so, move them back!
			CRect rectBounds = pDoc->GetUnscaledBoundsNoBorder();

			if (rectIndis.top<rectBounds.top)
				szUnshift.cy = rectBounds.top-rectIndis.top;
			if (rectIndis.bottom>rectBounds.bottom)
				szUnshift.cy = rectBounds.bottom-rectIndis.bottom;

			if (rectIndis.left<rectBounds.left)
				szUnshift.cx = rectBounds.left-rectIndis.left;
			if (rectIndis.right>rectBounds.right)
				szUnshift.cx = rectBounds.right-rectIndis.right;

			szUnshift.cx /= GetScale();
			szUnshift.cy /= GetScale();


			ShiftSelectedIndis(szUnshift);
		}

		m_pointPrev += szShift+szUnshift;

		ResetAllViews();

		if (IsOut(point))
		{
			RedrawWindow();
			PostNextMove(nFlags,point);
		}
	}
	else if (m_bSelecting)
	{
		if (IsOut(point)) ScrollToward(point);

		CPoint pointDoc(point);
		pointDoc += GetScrollPosition();

		m_rectPrev = CRect(m_pointPrev,pointDoc);
		m_rectPrev.NormalizeRect();

		if (!bExtended)
			DeselectAll();

		SelectHitIndis(m_rectPrev);

		ResetAllViews();

		if (IsOut(point))
		{
			RedrawWindow();
			PostNextMove(nFlags,point);
		}
	}

	CBigScrollView::OnMouseMove(nFlags, point);
}

void CGedtreeViewDL::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bMoving)
	{
		m_bMoving = FALSE;
		ReleaseCapture();
		ResetAllViews();
	}
	else if (m_bSelecting)
	{
		m_bSelecting = FALSE;
		ReleaseCapture();
		ResetAllViews();
	}

	CBigScrollView::OnLButtonUp(nFlags, point);
}

void CGedtreeViewDL::OnEditAlignTop() 
{
	// TODO: Add your command handler code here

	int nTop(INT_MAX);

	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			int nTopCur = indi.GetUnscaledFrameRect().top;
			if (nTopCur<nTop)
				nTop = nTopCur;
		}
	}
	ASSERT(nTop<INT_MAX);

	MyRect rectBounds; // bounds of new pos of indis
	rectBounds.SetRectEmpty();

	for (i = 0; i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			CPoint pt = indi.GetUnscaledFrameRect().TopLeft();
			pt.y = nTop;
			indi.MoveTo(pt);

			rectBounds |= indi.GetUnscaledFrameRect();
		}
	}

	GetDocument()->Normalize();
	ResetAllViews();
}

void CGedtreeViewDL::OnUpdateEditAlignTop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(IndiSelected());
}

int childdx(18);
static const int familydx(childdx*4);

typedef pair<int,CIndividual*> t_ma;

void CGedtreeViewDL::OnEditClean() 
{
	if (AfxMessageBox(L"This will REARRANGE EVERY INDIVIDUAL in the entire tree. "
		L"You should have a back up copy of your tree before you perform this action, "
		L"because it cannot be undone."
		L"\n\nIs this OK?",MB_OKCANCEL)==IDCANCEL)
		return;

	CleanAll();
}

void CGedtreeViewDL::CleanAll()
{
	// preliminary stuff
	CGedtreeDoc* pDoc = GetDocument();

	double cIndi = pDoc->m_rIndividual.GetSize();
	if (cIndi <= 1)
		return;



	// set generation levels (also sets position on y-axis)
	{
		pDoc->ClearAllIndividuals();

		int batch(0);
		bool someleft(true);
		while (someleft)
		{
			someleft = false;
			for (int i = 0; i < cIndi; ++i)
			{
				CIndividual& indi = pDoc->m_rIndividual[i];
				if (!indi.m_bMark)
				{
					someleft = true;
					indi.setLevel(batch++*20);
				}
			}
		}
	}

	int i;
	CProgress prg;

	// find number of levels, and normalize indis' level nums
	int nlev;
	{
		prg.Reset("Counting generations.",cIndi*2);
		int maxlev(0x80000000), minlev(0x7fffffff);
		for (i = 0; i < cIndi; ++i)
		{
			CIndividual& indi = pDoc->m_rIndividual[i];
			prg.Set(i+1,indi.Name());
			int lev = indi.m_nLevel;
			if (lev > maxlev)
				maxlev = lev;
			if (lev < minlev)
				minlev = lev;
		}
		//count of levels
		nlev = maxlev-minlev+1;
		for (i = 0; i < cIndi; ++i)
		{
			CIndividual& indi = pDoc->m_rIndividual[i];
			prg.Set(cIndi+i+1,indi.Name());
			indi.m_nLevel -= minlev;
		}
	}

	// calc max male-branch-descendant-generations size for all indis
	pDoc->ClearAllIndividuals();
	{
		prg.Reset("Finding branches.",cIndi);
		for (i = 0; i < cIndi; ++i)
		{
			CIndividual* pindi = &pDoc->m_rIndividual[i];
			prg.Set(i+1,pindi->Name());
			CIndividual* pfath = pindi;
			int c;
			if (pindi->m_nSex==1)
				c = 1;
			else
				c = 0;
			set<int> setindi;// guard against loops
			int f;
			while (((f = pfath->m_iFather) >= 0) && setindi.find(f)==setindi.end())
			{
				setindi.insert(f);
				++c;
				pfath = &pDoc->m_rIndividual[f];
			}
			pfath->setMaxMaleIf(c);
			if (pfath->m_iMother >= 0)
				pDoc->m_rIndividual[pfath->m_iMother].m_maxmale = c+1;
		}
	}

	list<CIndividual*> rptoclean;
	{
		// build vector of indis (w/male descent size)
		vector<t_ma> rma;
		for (i = 0; i < cIndi; ++i)
		{
			CIndividual* pindi = &pDoc->m_rIndividual[i];
			if (pindi->m_maxmale)
				rma.push_back(t_ma(pindi->m_maxmale,pindi));
		}
		// sort
		prg.Reset("Sorting branches.",rma.size()-1);
		int xxx = rma.size()-1;
		for (int s1(0); s1<xxx; ++s1)
		{
			prg.Set(s1+1);
			for (int s2(s1+1); s2<rma.size(); ++s2)
			{
				if (rma[s1].first < rma[s2].first)
				{
					t_ma tmp = rma[s2];
					rma[s2] = rma[s1];
					rma[s1] = tmp;
				}
				else if (rma[s1].first == rma[s2].first)
				{
					if (rma[s1].second->m_nLevel < rma[s2].second->m_nLevel)
					{
						t_ma tmp = rma[s2];
						rma[s2] = rma[s1];
						rma[s1] = tmp;
					}
					else if (rma[s1].second->m_nLevel == rma[s2].second->m_nLevel)
					{
						if (rma[s1].second->m_nSex==2 && rma[s2].second->m_nSex==1)
						{
							t_ma tmp = rma[s2];
							rma[s2] = rma[s1];
							rma[s1] = tmp;
						}
					}
				}
			}
		}

		// put indis on rptoclean list in order of processing
		vector<t_ma>::iterator ima = rma.begin();
		while (ima != rma.end())
		{
			CIndividual* psec = (*ima).second;
			rptoclean.push_back(psec);
			++ima;
		}
	}


	prg.Reset("Labeling branches.",rptoclean.size());
	int iProg(0);
	pDoc->ClearAllIndividuals();
	list<CIndividual*>::iterator iptoclean = rptoclean.begin();
	while (iptoclean != rptoclean.end())
	{

		CIndividual* pindi = *iptoclean;
		prg.Set(++iProg,pindi->Name());
		list<CIndividual*> todo;

		pindi->setRootWithSpouses(pindi);
		todo.push_back(pindi);
		while (!todo.empty())
		{
			CIndividual* pgmi = todo.front(); todo.pop_front();
			prg.Set(iProg,pindi->Name()+": "+pgmi->Name());
			for (int j = 0; j < pgmi->m_riSpouseToFamily.GetSize(); ++j)
			{
				CFamily& fami = pDoc->m_rFamily[pgmi->m_riSpouseToFamily[j]];
				for (int k = 0; k < fami.m_riChild.GetSize(); ++k)
				{
					CIndividual* pchil = &pDoc->m_rIndividual[fami.m_riChild[k]];
					if (!pchil->m_bMark)
					{
						pchil->setRootWithSpouses(pindi);
						if (pchil->m_nSex==1)
							todo.push_back(pchil);
					}
				}
			}
		}
		++iptoclean;
	}








	// build new list with only house heads
	list<CIndividual*> rptoclean2;
	set<CIndividual*> settoclean2;
	{
		prg.Reset("Finding progenitors.",(cIndi+rptoclean.size()));
		int iProg(0);
		//make a list of all house heads
		set<int> setheads;
		for (i = 0; i < cIndi; ++i)
		{
			CIndividual* pindi = &pDoc->m_rIndividual[i];
			prg.Set(++iProg,pindi->Name());
			if (pindi->m_pMale)
				setheads.insert(pindi->m_pMale->m_i);
		}

		// put house heads on rptoclean2 list in order of processing
		list<CIndividual*>::iterator itc2 = rptoclean.begin();
		while (itc2 != rptoclean.end())
		{
			CIndividual* psec = *itc2;
			prg.Set(++iProg,psec->Name());
			if (setheads.find(psec->m_i)!=setheads.end())
			{
				rptoclean2.push_back(psec);
				settoclean2.insert(psec);
			}
			++itc2;
		}
	}


	CIndividual* firsthead;
	if (rptoclean2.size() > 0)
		firsthead = rptoclean2.front();
	else
		firsthead = 0;





	pair<double,double>* lev_bounds = new pair<double,double>[nlev];
	for (i = 0; i < nlev; ++i)
		lev_bounds[i] = pair<double,double>(1,1);

	pDoc->ClearAllIndividuals();
	prg.Reset("Moving branches.",rptoclean2.size());
	iProg = 0;
	while (!rptoclean2.empty())
	{
		CIndividual* psec = rptoclean2.front(); rptoclean2.pop_front(); settoclean2.erase(settoclean2.find(psec));
		prg.Set(++iProg,psec->Name());

		list<CIndividual*> nexthouse;

		set<CIndividual*> guard;
		list<CIndividual*> todo;
		todo.push_back(psec);
		guard.insert(psec);
		while (!todo.empty())
		{
			CIndividual* pgmi = todo.front(); todo.pop_front();
			prg.Set(iProg,psec->Name()+": "+pgmi->Name());
			double x(0);
			list<CIndividual*> cleannext;
			pgmi->setSeqWithSpouses(x,lev_bounds,false,cleannext);
			if (!cleannext.empty())
			{
				list<CIndividual*>::iterator inext = cleannext.begin();
				while (inext != cleannext.end())
				{
					CIndividual* pindi = *inext;
					nexthouse.push_front(pindi);
					++inext;
				}
			}

			for (int i = 0; i < pgmi->m_riSpouseToFamily.GetSize(); ++i)
			{
				CFamily& fami = pDoc->m_rFamily[pgmi->m_riSpouseToFamily[i]];
				CArray<int,int> riChild;
				fami.GetSortedChildren(riChild);
				int nch = riChild.GetSize();
				if (nch)
				{
					//put the (first two) children with spouses on the outside edges
					int sp1(-1), sp2(-1);
					for (int ch = 0; ch < nch; ++ch)
					{
						CIndividual& chil = pDoc->m_rIndividual[riChild[ch]];
						bool has = chil.m_riSpouse.GetSize() > 0;
						if (has)
						{
							if (sp1 < 0)
								sp1 = ch;
							else if (sp2 < 0)
								sp2 = ch;
						}
					}

					CArray<int,int> riChild2;
					if (sp1 >= 0)
						riChild2.Add(riChild[sp1]);
					for (ch = 0; ch < nch; ++ch)
					{
						if (ch != sp1 && ch != sp2)
							riChild2.Add(riChild[ch]);
					}
					if (sp2 >= 0)
						riChild2.Add(riChild[sp2]);
					nch = riChild2.GetSize();

					bool left = (nch > 1);
					for (int i = 0; i < nch; ++i)
					{
						CIndividual* pchil = &pDoc->m_rIndividual[riChild2[i]];
						list<CIndividual*> cleannext;
						pchil->setSeqWithSpouses(x,lev_bounds,left,cleannext);
						if (!cleannext.empty())
						{
							list<CIndividual*>::iterator inext = cleannext.begin();
							while (inext != cleannext.end())
							{
								CIndividual* pindi = *inext;
								nexthouse.push_front(pindi);
								++inext;
							}
						}
						lev_bounds[pchil->m_nLevel].second += childdx;
						left = false;
						if (pchil->m_nSex==1 && guard.find(pchil)==guard.end())
						{
							todo.push_back(pchil);
							guard.insert(pchil);
						}
					}
				}
			}
		}
		double maxx(-10000);
		bool any(false);
		for (i = 0; i < nlev; ++i)
		{
			if (lev_bounds[i].second > maxx)
				maxx = lev_bounds[i].second;
			//klugde to see if any people in this house
			if (i > 0 && lev_bounds[i].second != lev_bounds[i-1].second)
				any = true;
		}
		if (any)
		{
			maxx += familydx;
			for (i = 0; i < nlev; ++i)
				lev_bounds[i].second = maxx;
		}

		list<CIndividual*>::iterator inext = nexthouse.begin();
		while (inext != nexthouse.end())
		{
			CIndividual* pindi = *inext;
			if (settoclean2.find(pindi)!=settoclean2.end())
			{
				rptoclean2.remove(pindi);
				rptoclean2.push_front(pindi);
			}
			++inext;
		}
	}

	delete [] lev_bounds;

	for (i = 0; i < cIndi; ++i)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		indi.Calc();
	}
	for (i = 0; i < pDoc->m_rFamily.GetSize(); ++i)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		fami.Calc();
	}

	GetDocument()->Normalize();

	DeselectAll();
	if (firsthead)
		firsthead->Select();
	OnViewScrolltoselection();

	ResetAllViews();
}

void CGedtreeViewDL::OnUpdateEditClean(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IndiSelected());
}

BOOL CGedtreeViewDL::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CGedtreeViewDL::OnEditSelectAncestors() 
{
	CPickRels dlg;
	if (dlg.DoModal()==IDOK)
	{
		CGedtreeDoc* pDoc = GetDocument();

		int iindi(-1);
		for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
			if (pDoc->m_rIndividual[i].m_bSelected)
				iindi = i;
		if (iindi==-1) // shouldn't happen
			return;

		CIndividual* pindi = &pDoc->m_rIndividual[iindi];

		if (dlg.m_bAncestorsMale)
		{
			// direct paternal ancestor branch
			CIndividual* pfath = pindi;
			set<int> setindi;// guard against loops
			int f;
			while (((f = pfath->m_iFather) >= 0) && setindi.find(f)==setindi.end())
			{
				setindi.insert(f);
				pfath = &pDoc->m_rIndividual[f];
				pfath->Select();
			}
		}

		if (dlg.m_bAncestorsFemale)
		{
			// direct maternal ancestor branch
			CIndividual* pmoth = pindi;
			set<int> setindi;// guard against loops
			int f;
			while (((f = pmoth->m_iMother) >= 0) && setindi.find(f)==setindi.end())
			{
				setindi.insert(f);
				pmoth = &pDoc->m_rIndividual[f];
				pmoth->Select();
			}
		}

		if (dlg.m_bAncestors)
		{
			// all direct ancestors (pedigree)
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			rpi.push_back(pindi);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				p->Select();
				setindi.insert(p);
				if (p->m_iFather >= 0)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_iFather];
					if (setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
				if (p->m_iMother >= 0)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_iMother];
					if (setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
			}
		}

		if (dlg.m_bDescendants)
		{
			// direct descendants
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			rpi.push_back(pindi);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				p->Select();
				setindi.insert(p);
				for (int i(0); i<p->m_riChild.GetSize(); ++i)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_riChild[i]];
					if (setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
			}
		}

		if (dlg.m_bDescendantsMale)
		{
			// direct male descendants
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			rpi.push_back(pindi);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				p->Select();
				setindi.insert(p);
				for (int i(0); i<p->m_riChild.GetSize(); ++i)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_riChild[i]];
					if (pp->m_nSex==1 && setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
			}
		}

		if (dlg.m_bDescendantsFemale)
		{
			// direct female descendants
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			rpi.push_back(pindi);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				p->Select();
				setindi.insert(p);
				for (int i(0); i<p->m_riChild.GetSize(); ++i)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_riChild[i]];
					if (pp->m_nSex==2 && setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
			}
		}

		if (dlg.m_bYchrom)
		{
			// common Y-chromosome
			if (pindi->m_nSex != 1) // only males have a Y-chromosome
				return;
			CIndividual* pfath = pindi;
			set<int> setindii;// guard against loops
			int f;
			while (((f = pfath->m_iFather) >= 0) && setindii.find(f)==setindii.end())
			{
				setindii.insert(f);
				pfath = &pDoc->m_rIndividual[f];
			}
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			rpi.push_back(pfath);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				p->Select();
				setindi.insert(p);
				for (int i(0); i<p->m_riChild.GetSize(); ++i)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_riChild[i]];
					if (pp->m_nSex==1 && setindi.find(pp)==setindi.end())
						rpi.push_back(pp);
				}
			}
		}

		if (dlg.m_bMtdna)
		{
			// common mitochondrial DNA
			CIndividual* pfath = pindi;
			set<int> setindii;// guard against loops
			int f;
			while (((f = pfath->m_iMother) >= 0) && setindii.find(f)==setindii.end())
			{
				setindii.insert(f);
				pfath = &pDoc->m_rIndividual[f];
			}
			set<CIndividual*> setindi;// guard against loops
			list<CIndividual*> rpi;
			pfath->Select();
			setindi.insert(pfath);
			rpi.push_back(pfath);
			while (!rpi.empty())
			{
				CIndividual* p = rpi.front(); rpi.pop_front();
				for (int i(0); i<p->m_riChild.GetSize(); ++i)
				{
					CIndividual* pp = &pDoc->m_rIndividual[p->m_riChild[i]];
					pp->Select();
					if (pp->m_nSex==2 && setindi.find(pp)==setindi.end())
					{
						setindi.insert(pp);
						rpi.push_back(pp);
					}
				}
			}
		}

		ResetAllViews();
	}
}

void CGedtreeViewDL::OnUpdateEditSelectAncestors(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(OneIndiSelected());
}

void CGedtreeViewDL::OnEditSelectAll() 
{
	// TODO: Add your command handler code here
	CGedtreeDoc* pDoc = GetDocument();

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (!indi.m_bHidden)
			indi.Select();
	}
	ResetAllViews();
}

void CGedtreeViewDL::OnViewZoomOut() 
{
	CZoom dlg;
	dlg.m_nScale = m_nScale;
	if (dlg.DoModal()==IDOK)
		SetScale(dlg.m_nScale);
	OnViewScrolltoselection();
}

void CGedtreeViewDL::OnViewZoomNormal()
{
	SetScale(1);
	OnViewScrolltoselection();
}

void CGedtreeViewDL::SetScale(int nScale)
{
	CRect rectc;
	GetClientRect(rectc);
	MyRect rectClient(rectc);
	rectClient += GetScrollPosition();

	CPoint pointScroll = rectClient.CenterPoint();
	pointScroll.x *= GetScale();
	pointScroll.y *= GetScale();

	m_nScale = nScale;

	CGedtreeDoc* pDoc = GetDocument();

	pDoc->CalcScale();

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		indi.SetScale(m_nScale);
	}

	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& family = pDoc->m_rFamily[i];
		family.SetScale(m_nScale);
	}

	SetTotalSize();

	pointScroll.x /= GetScale();
	pointScroll.y /= GetScale();

	CSize szHalfClient(rectClient.Width()/2,rectClient.Height()/2);

	pointScroll -= szHalfClient;

	CPoint pointMax(pDoc->GetBounds().BottomRight()-rectClient.Size());
	if (pointScroll.x>pointMax.x)
		pointScroll.x = pointMax.x;
	if (pointScroll.y>pointMax.y)
		pointScroll.y = pointMax.y;

	ScrollToPosition(pointScroll);

	ResetAllViews();
}

void CGedtreeViewDL::OnViewScrolltoselection() 
{
	// TODO: Add your command handler code here
	
	CGedtreeDoc* pDoc = GetDocument();

	CIndividual* pIndi = NULL;

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		pIndi = &pDoc->m_rIndividual[i];
		if (pIndi->m_bSelected)
		{
			break;
		}
	}
	if (i<pDoc->m_rIndividual.GetSize() && pIndi)
		ScrollToIndi(pIndi);
}

void CGedtreeViewDL::ScrollToIndi(CIndividual* pIndi)
{
	CRect rectc;
	GetClientRect(rectc);
	MyRect rectClient(rectc);

	MyRect rectIndi = pIndi->GetFrameRect();
	CPoint pointScroll = rectIndi.CenterPoint();

	pointScroll.x -= rectClient.Width()/2;
	pointScroll.y -= rectClient.Height()/2;

	ScrollToPosition(pointScroll);
}

void CGedtreeViewDL::Scrolltoselection()
{
	OnViewScrolltoselection();
}

void CGedtreeViewDL::OnUpdateViewScrolltoselection(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	OnViewIndi();

	CGedtreeView::OnLButtonDblClk(nFlags, point);
}

void CGedtreeViewDL::OnEditFind() 
{
	// TODO: Add your command handler code here
	CFind d;
	d.m_strName = m_strLastFind;
	if (d.DoModal()==IDOK)
	{
		m_strLastFind = d.m_strName;
		m_strLastFind.TrimLeft();
		m_strLastFind.TrimRight();
		m_strLastFind.MakeUpper();

		if (!DoFind())
			AfxMessageBox(_T("Cannot find individual"));
	}
}

void CGedtreeViewDL::OnEditFindnext() 
{
	// TODO: Add your command handler code here
	if (!DoFind(TRUE))
		if (!DoFind())
			AfxMessageBox(_T("Cannot find individual"));
}

void CGedtreeViewDL::OnEditHideunselected()
{
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (!indi.m_bSelected)
		{
			indi.m_bHidden = TRUE;
			m_bSomeHidden = true;
		}
	}
	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		fami.Calc();
	}
	Reset();
}

void CGedtreeViewDL::OnUpdateEditHideunselected(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CGedtreeViewDL::OnEditShowall()
{
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		indi.m_bHidden = FALSE;
	}
	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		fami.Calc();
	}
	m_bSomeHidden = false;
	delete m_pAsOfDate;
	m_pAsOfDate = 0;
	Reset();
}

void CGedtreeViewDL::OnUpdateEditShowall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CGedtreeViewDL::OnEditShowAsOf()
{
	CEditDate d;
	if (d.DoModal()!=IDOK)
	{
		return;
	}
	delete m_pAsOfDate;
	if (!d.m_dv.IsExact())
	{
		return;
	}
	int ymd = d.m_dv.GetSimpleYMD();
	m_pAsOfDate = new CDate();
	m_pAsOfDate->Set(ymd/10000,ymd%10000/100,ymd%100);

	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		indi.showAsOf(m_pAsOfDate);
	}
	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		fami.showAsOf(m_pAsOfDate);
	}
	Reset();
}

void CGedtreeViewDL::OnUpdateEditShowAsOf(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

BOOL CGedtreeViewDL::DoFind(BOOL bFromSelection)
{
	CGedtreeDoc* pDoc = GetDocument();
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (bFromSelection)
		{
			if (indi.m_bSelected)
				bFromSelection = FALSE;
		}
		else
		{
			CString sTest(indi.Name());
			sTest.MakeUpper();
			if (sTest.Find(m_strLastFind)>=0)
			{
				DeselectAll();
				if (indi.m_bHidden)
				{
					indi.m_bHidden = FALSE;
					for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
					{
						CFamily& fami = pDoc->m_rFamily[i];
						fami.Calc();
					}
				}
				indi.Select();
				ResetAllViews();
				OnViewScrolltoselection();
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CGedtreeViewDL::OnUpdateEditFindnext(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(!m_strLastFind.IsEmpty());
}

void CGedtreeViewDL::Reset(UINT flagsChanged)
{
//	Invalidate();
	RedrawWindow();
}

void CGedtreeViewDL::ResetAllViews()
{
	GetDocument()->ResetAllViews(position);
}

void CGedtreeViewDL::OnEditDisconnect() 
{
	CGedtreeDoc* pDoc = GetDocument();

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			indi.Disconnect();
		}
	}
	ResetAllViews();
}

void CGedtreeViewDL::OnUpdateEditDisconnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(IndiSelected());
}

CPoint CGedtreeViewDL::GetScrollPoint()
{
	CRect rectc;
	GetClientRect(rectc);

	MyRect rectClient(rectc);
	rectClient += GetScrollPosition();

	CPoint pointScroll = rectClient.CenterPoint();
	pointScroll.x *= GetScale();
	pointScroll.y *= GetScale();

	return pointScroll;
}

void CGedtreeViewDL::OnEditNewindividual() 
{
	reg_t reg = GetRegistration();
	if (reg==registered_bad)
	{
		AfxMessageBox(_T("You cannot save, because this program has not been properly REGISTERED yet. Please click OK, then wait 10 seconds...."));
		::Sleep(9997);
		return;
	}

	CGedtreeDoc* pDoc = GetDocument();
	HTREEITEM hti = pDoc->InsertChild(_T("INDI"));
	CIndividual indi(pDoc,hti);
	int i = pDoc->m_rIndividual.Add(indi);
	pDoc->m_rIndividual[i].SetIndex(i);
	pDoc->m_rIndividual[i].CalcID();

	CPoint pointScroll = GetScrollPoint();

	CIndividual& indid = pDoc->m_rIndividual[i];

	indid.PlaceAt(pointScroll);
	indid.SetScale(m_nScale);
	indid.Calc();
	indid.MoveTo(pointScroll);

	indid.m_name.PutToTree(indid.m_hTreeItem);
	pDoc->m_bsortIndiNeedsUpdate = true;

	pDoc->ResetAllViews();

	indid.Select();
	indid.OpenView();
}

void CGedtreeViewDL::OnEditConnect() 
{
	// TODO: Add your command handler code here
	
	CGedtreeDoc* pDoc = GetDocument();

	int iiIndi(0);
	int riIndi[2];
	for (int i(pDoc->m_rIndividual.GetSize()-1); i>=0; --i)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			riIndi[iiIndi++] = i;
		}
	}

	ASSERT(iiIndi==2);

	CConnect d(pDoc);
	d.m_pIndi0 = &pDoc->m_rIndividual[riIndi[0]];
	d.m_pIndi1 = &pDoc->m_rIndividual[riIndi[1]];
	if (d.DoModal()==IDOK)
	{
		CFamily* pFami;
		if (d.m_nFamily<0)
		{
			// create new family
			HTREEITEM hti = pDoc->InsertChild("FAM");
			CFamily fami(pDoc,hti);
			int i = pDoc->m_rFamily.Add(fami);

			pFami = &pDoc->m_rFamily[i];

			pFami->SetIndex(i);
			pFami->CalcID();
			pFami->SetScale(m_nScale);
		}
		else
			pFami = &pDoc->m_rFamily[d.m_nFamily];

		pFami->Connect(d.m_pIndi0,d.m_pIndi1,d.m_nRelation);
		pFami->Calc();

		pDoc->ResetAllViews();
	}
}

void CGedtreeViewDL::OnUpdateEditConnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	CGedtreeDoc* pDoc = GetDocument();

	pCmdUI->Enable(FALSE);

	int iiIndi(0);
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			iiIndi++;
			if (iiIndi==2)
			{
				pCmdUI->Enable(TRUE);
			}
			else if (iiIndi>2)
			{
				pCmdUI->Enable(FALSE);
				return;
			}
		}
	}
}

void CGedtreeViewDL::OnViewOpenpedigree() 
{
	// TODO: Add your command handler code here
	
	CGedtreeDoc* pDoc = GetDocument();

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			indi.OpenPedigree();
		}
	}
}

void CGedtreeViewDL::OnUpdateViewOpenpedigree(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnViewDroplinechart() 
{
	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
	CFrameWnd* pFrame = p->CreateNewFrame(GetDocument(),NULL);
	p->InitialUpdateFrame(pFrame,GetDocument());
//	((CGedtreeViewDL*)m_pFrame->GetActiveView())->Init(m_i);
//	p->InitialUpdateFrame(m_pFrame,m_pDoc);
}

void CGedtreeViewDL::OnViewIndi() 
{
	// TODO: Add your command handler code here
	
	CGedtreeDoc* pDoc = GetDocument();

	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			indi.OpenView();
		}
	}
}

void CGedtreeViewDL::OnUpdateViewIndi(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(IndiSelected());
}

template<class C>
C tmin(C a, C b)
{
	return a>b ? b : a;
}

void CGedtreeViewDL::OnFileHTML() 
{
	// TODO: Add your command handler code here

	CGedtreeDoc* pDoc = GetDocument();

	int cIndi = pDoc->m_rIndividual.GetSize();
	if (!cIndi)
	{
		AfxMessageBox(L"There are no individuals to export. Please choose Edit/New Individual to add a person.");
		return;
	}

	CString sPath = pDoc->GetPathName();
	if (sPath.IsEmpty())
	{
		AfxMessageBox(L"Please save this GEDCOM document before using this command.");
		return;
	}

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(sPath,drive,dir,fname,ext);
	sPath = CString(drive)+CString(dir)+CString(fname)+"\\";

	CString sPathIndi = CString(drive)+CString(dir)+"indi.tpl";
	CString sPathSour = CString(drive)+CString(dir)+"sour.tpl";

	CString sMsg;
	sMsg.Format(L"This will create an HTML file for every individual. Is this OK?\n\n"
		L"The title page will be\n%sindex.html\n\n",(LPCTSTR)sPath);

	if (AfxMessageBox(sMsg,MB_YESNO)!=IDYES)
		return;

	if (!CUtil::DirectoryExists(sPath))
	{
		SECURITY_ATTRIBUTES sec;
		sec.nLength = sizeof(sec);
		sec.lpSecurityDescriptor = NULL;
		sec.bInheritHandle = FALSE;

		::CreateDirectory(sPath,&sec);
	}

//	CStdioFile tplIndi, tplSour;
//	if (!GetUserTpl(tplIndi,"indi.tpl"))
//		return;
//
//	if (!GetUserTpl(tplSour,"sour.tpl"))
//		return;

	CMemFile tplIndi, tplSour;
	OldGetUserTpl(tplIndi,sPathIndi,sTplIndi);
	OldGetUserTpl(tplSour,sPathSour,sTplSour);

	CArray<int,int>* rSrtIndi = pDoc->GetSortedIndis();

	AfxGetMainWnd()->SetForegroundWindow();

	{
		CProgress prg;
		prg.Reset("Exporting individuals.",pDoc->m_rIndividual.GetSize());

		for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
		{
			CIndividual& indi = pDoc->m_rIndividual[i];
			prg.Set(i+1,indi.Name());
			CStdioFile f(sPath+indi.GetWebFilePath(),CFile::modeCreate|CFile::modeWrite);
			f.WriteString(indi.GetWebPage(tplIndi));
		}

		prg.Reset("Exporting sources.",pDoc->m_rSource.GetSize());
		for (i = 0; i<pDoc->m_rSource.GetSize(); i++)
		{
			CSource& sour = pDoc->m_rSource[i];
			prg.Set(i+1,sour.m_strTitle);
			CStdioFile f(sPath+sour.GetWebFilePath(),CFile::modeCreate|CFile::modeWrite);
			f.WriteString(sour.GetWebPage(tplSour));
		}

		pDoc->GetSortedIndis();

		static const int siz(10);

		int cLevel = pDoc->mcLevel;

		prg.Reset("Exporting index.",cIndi);
		for (int iFile(0); iFile<cIndi; iFile += siz)
		{
			prg.Set(iFile+1,"");
			CString sFile;
			sFile.Format(L"%sx%dx%d.html",(LPCTSTR)sPath,0,iFile);
			CStdioFile f(sFile,CFile::modeCreate|CFile::modeWrite);
			f.WriteString(L"<HTML><BODY style=\"{font-family: \"microsoft sans serif\", arial, helvetica, sans-serif; background-color: #FFFFCC;}\"><TABLE><TR>\n");

			CString s;

			if (cLevel>1)
			{
				s.Format(L"<TD valign=\"center\">(<A HREF=\"x%dx%d.html\">home</A>)&nbsp;&nbsp;</TD>\n",
					cLevel-1,0);
				f.WriteString(s);
			}
			if (cLevel>2)
			{
				s.Format(L"<TD valign=\"center\">(<A HREF=\"x%dx%d.html\">up one level</A>)&nbsp;&nbsp;</TD>\n",
					1,iFile/siz/siz*siz*siz);
				f.WriteString(s);
			}
			f.WriteString(L"<TD valign=\"center\">\n<TABLE>\n");
			if (iFile-siz>=0)
				s.Format(L"<TR><TD>(<A HREF=\"x%dx%d.html\">previous</A>)</TD></TR>\n",
					0,iFile-siz);
			else
				s.Format(L"<TR><TD colspan=6><HR></TD></TR>\n");
			f.WriteString(s);
			for (int i(iFile); i<tmin(cIndi,iFile+siz); i++)
			{
				CIndividual& indi = pDoc->m_rIndividual[rSrtIndi->GetAt(i)];
				int birth(0), death(0);
				if (indi.m_iBirth>=0)
					birth = indi.m_revt[indi.m_iBirth].m_dvDate.GetSimpleYear();
				if (indi.m_iDeath>=0)
					death = indi.m_revt[indi.m_iDeath].m_dvDate.GetSimpleYear();
				CString sBirth("?"), sDeath("?");
				if (birth)
					sBirth.Format(L"%04d",birth);
				if (death)
					sDeath.Format(L"%04d",death);

				CString s;
				s.Format(
					L"<TR>"
						L"<TD><A HREF=\"i%d.html\"><B>%s</B>, %s</A></TD>"
						L"<TD>(</TD>"
						L"<TD align=\"center\">%s</TD>"
						L"<TD>-</TD>"
						L"<TD align=\"center\">%s</TD>"
						L"<TD>)</TD>"
					L"</TR>\n",
					indi.m_i,indi.m_name.Surname(),indi.m_name.GivenName(),sBirth,sDeath);
				f.WriteString(s);
			}
			if (iFile+siz<cIndi)
				s.Format(L"<TR><TD>(<A HREF=\"x%dx%d.html\">next</A>)</TD></TR>\n",
					0,iFile+siz);
			else
				s.Format(L"<TR><TD colspan=6><HR></TD></TR>\n");
			f.WriteString(s);

			f.WriteString(L"</TABLE>\n</TD>\n");
			f.WriteString(L"</TR></TABLE></BODY></HTML>\n");
		}

		int levval(1);
		prg.Reset("Exporting index tree.",cLevel*cIndi);
		for (int lev(1); lev<cLevel; lev++)
		{
			levval *= 10;
			for (int iFile(0); iFile<cIndi; iFile += levval*siz)
			{
				prg.Set(iFile+(lev-1)*cLevel+1,"");
				CString sFile;
				sFile.Format(L"%sx%dx%d.html",(LPCTSTR)sPath,lev,iFile);
				CStdioFile f(sFile,CFile::modeCreate|CFile::modeWrite);
				f.WriteString(L"<HTML><BODY style=\"{font-family: \"microsoft sans serif\", arial, helvetica, sans-serif; background-color: #FFFFCC;}\"><TABLE><TR>\n");

				CString s;

				if (cLevel-1>1 && !(iFile==0&&lev==cLevel-1))
				{
					s.Format(L"<TD valign=\"center\">(<A HREF=\"x%dx%d.html\">home</A>)&nbsp;&nbsp;</TD>\n",
						cLevel-1,0);
					f.WriteString(s);
				}
				int iUp(iFile/(levval*siz)/(siz)*(levval*siz)*(siz));
				if (lev+1<cLevel && !(lev+1==cLevel-1&&iUp==0))
				{
					s.Format(L"<TD valign=\"center\">(<A HREF=\"x%dx%d.html\">up one level</A>)&nbsp;&nbsp;</TD>\n",
						lev+1,iUp);
					f.WriteString(s);
				}

				f.WriteString(L"<TD valign=\"center\">\n<TABLE>\n");

				if (iFile-levval*siz>=0)
					s.Format(L"<TR><TD>(<A HREF=\"x%dx%d.html\">previous</A>)</TD></TR>\n",
						lev,iFile-levval*siz);
				else
					s.Format(L"<TR><TD colspan=3><HR></TD></TR>\n");
				f.WriteString(s);

				for (int i(iFile); i<tmin(cIndi,iFile+levval*siz); i += levval)
				{
					CIndividual& indi1 = pDoc->m_rIndividual[rSrtIndi->GetAt(i)];
					CIndividual& indi2 = pDoc->m_rIndividual[rSrtIndi->GetAt(tmin(cIndi-1,i+(levval/10)*siz-1))];
					CString s;
					s.Format(
						L"<TR>"
							L"<TD valign=\"bottom\"><A HREF=\"x%dx%d.html\"><B>%s</B>, %s</A></TD>"
							L"<TD valign=\"bottom\"><A HREF=\"x%dx%d.html\"><FONT size=-2>through</FONT></A></TD>"
							L"<TD valign=\"bottom\"><A HREF=\"x%dx%d.html\"><B>%s</B>, %s</A></TD>"
						L"</TR>\n",
						lev-1,i,
						indi1.m_name.Surname(),indi1.m_name.GivenName(),
						lev-1,i,
						lev-1,i,
						indi2.m_name.Surname(),indi2.m_name.GivenName());
					f.WriteString(s);
				}
				if (iFile+levval*siz<cIndi)
					s.Format(L"<TR><TD>(<A HREF=\"x%dx%d.html\">next</A>)</TD></TR>\n",
						lev,iFile+levval*siz);
				else
					s.Format(L"<TR><TD colspan=3><HR></TD></TR>\n");
				f.WriteString(s);

				f.WriteString(L"</TABLE>\n</TD>\n");
				f.WriteString(L"</TR></TABLE>\n");
				if (lev==cLevel-1)
					f.WriteString(L"<A HREF=\"index.html\">Title page</A>\n");
				f.WriteString(L"</BODY></HTML>\n");
			}
		}

		{
			CStdioFile f(sPath+"index.html",CFile::modeCreate|CFile::modeWrite);
			f.WriteString(L"<HTML>\n<HEAD>\n<TITLE>");
			f.WriteString(pDoc->GetTitle());
			f.WriteString(L"</TITLE>\n</HEAD>\n<BODY>\n");
			f.WriteString(L"<FONT size=7><B>");
			f.WriteString(pDoc->GetTitle());
			f.WriteString(L"</B></FONT><P><P>\n");
			f.WriteString(L"These pages were created from the GEDCOM file\n");
			f.WriteString(pDoc->GetTitle());
			f.WriteString(L"\nwith the Genealogy Research Organizer program\n");
			f.WriteString(L"(<A HREF=\"http://go.to/gro\">GRO</A>).\n");
			f.WriteString(L"<P>\n");
			CString s;
			s.Format(L"<FONT size=5><A HREF=\"x%dx%d.html\">Index</A></FONT>\n",cLevel-1,0);
			f.WriteString(s);
			f.WriteString(L"<P>\n");
			f.WriteString(L"Click on the Index link above to navigate through\n");
			f.WriteString(L"the index of individuals in this family tree.\n");
			f.WriteString(L"Use the index to \"drill down\" to the individual you want to see.\n");
			f.WriteString(L"</BODY>\n</HTML>\n");
		}

	}
	AfxGetMainWnd()->SetForegroundWindow();

	if (AfxMessageBox(L"Would you like to open the title page in your Web browser?",MB_YESNO)!=IDYES)
		return;

	::ShellExecute(NULL,L"open",sPath+"index.html",NULL,NULL,SW_SHOWNORMAL);
}

void CGedtreeViewDL::OnFileRTF() 
{
	if (AfxMessageBox(L"This will generate a word-processor document for every individual. "
		L"Is this OK?",MB_OKCANCEL)==IDCANCEL)
		return;

	CGedtreeDoc* pDoc = GetDocument();

	int cIndi = pDoc->m_rIndividual.GetSize();
	if (!cIndi)
	{
		AfxMessageBox(L"There are no individuals to export. Please choose Edit/New Individual to add a person.");
		return;
	}

	CString sPath = pDoc->GetPathName();
	if (sPath.IsEmpty())
	{
		AfxMessageBox(L"Please save this GEDCOM document before using this command.");
		return;
	}

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(sPath,drive,dir,fname,ext);
/*
taking out rtf code just to test the "text descent book" code
	sPath = CString(drive)+CString(dir)+CString(fname)+".rtf";

	CString sPathHead = CString(drive)+CString(dir)+"head.rtf";
	CString sPathIndi = CString(drive)+CString(dir)+"indi.rtf";
	CString sPathSour = CString(drive)+CString(dir)+"sour.rtf";
	CString sPathBib = CString(drive)+CString(dir)+"bib.rtf";

	CMemFile tplHead, tplIndi, tplSour, tplBib;
	OldGetUserTpl(tplHead,sPathHead,sTplHeadrtf);
	OldGetUserTpl(tplIndi,sPathIndi,sTplIndirtf);
	OldGetUserTpl(tplSour,sPathSour,sTplSourrtf);
	OldGetUserTpl(tplBib,sPathBib,sTplBibrtf);

	CArray<int,int>* rSrtIndi = pDoc->GetSortedIndis();

	AfxGetMainWnd()->SetForegroundWindow();

	{
		CProgress prg;
		prg.Reset("Exporting individuals.",pDoc->m_rIndividual.GetSize());

		CStdioFile f(sPath,CFile::modeCreate|CFile::modeWrite);

		tplHead.SeekToBegin();
		char* pc = new char[1024];
		UINT n;
		while ((n = tplHead.Read(pc,1024)) > 0)
			f.Write(pc,n);
		delete [] pc;

		for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
		{
			CIndividual& indi = pDoc->m_rIndividual[rSrtIndi->GetAt(i)];
			prg.Set(i+1,indi.Name());
			f.WriteString(indi.GetRTFPage(tplIndi));
		}

		CArray<int,int> rSour;
		pDoc->GetSortedSours(rSour);
		prg.Reset("Exporting source text.",pDoc->m_rSource.GetSize());
		for (i = 0; i<pDoc->m_rSource.GetSize(); i++)
		{
			CSource& sour = pDoc->m_rSource[rSour[i]];
			prg.Set(i+1,sour.m_strTitle);
			if (sour.m_strText.GetLength())
				f.WriteString(sour.GetRTF(tplSour));
		}

		prg.Reset("Exporting bibliography.",pDoc->m_rSource.GetSize());
		f.WriteString(L"{\\fs32 \\b Bibliography} \\par \\par\n");
		for (i = 0; i<pDoc->m_rSource.GetSize(); i++)
		{
			CSource& sour = pDoc->m_rSource[rSour[i]];
			prg.Set(i+1,sour.m_strTitle);
			f.WriteString(sour.GetRTF(tplBib));
		}

		f.WriteString(L"}");
	}

	AfxGetMainWnd()->SetForegroundWindow();

	if (AfxMessageBox(L"Would you like to open it with your word processor?",MB_YESNO)!=IDYES)
		return;

	::ShellExecute(NULL,L"open",sPath,NULL,NULL,SW_SHOWNORMAL);
*/
	sPath = CString(drive)+CString(dir)+CString(fname)+".html";

	for (int iIndi(0); iIndi<pDoc->m_rIndividual.GetSize(); iIndi++)
	{
		CIndividual* pindi = &pDoc->m_rIndividual[iIndi];
		if (pindi->m_bSelected)
		{
			CStdioFile f(sPath,CFile::modeCreate|CFile::modeWrite);

			f.WriteString(L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n");
			f.WriteString(L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n");
			f.WriteString(L"<head>\n");
			f.WriteString(L"<title>Descent</title>\n");
			f.WriteString(L"<link rel=\"stylesheet\" href=\"descent.css\" type=\"text/css\" media=\"all\" />\n");
			f.WriteString(L"</head>\n");
			f.WriteString(L"<body>\n");
			f.WriteString(L"\n\n\n\n\n");

			list<CIndividual*> todo;
			pindi->m_nLevel = 1;

			todo.push_back(pindi);
			int indiNum = 1;
			while (!todo.empty())
			{
				f.WriteString(L"\n\n\n");
				CIndividual* pi = todo.front(); todo.pop_front();
				pi->WriteDescent(f,indiNum,todo);
				++indiNum;
			}

			f.WriteString(L"</body>\n");
			f.WriteString(L"</html>\n");
		}
	}

}

void CGedtreeViewDL::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	GetParentFrame()->ActivateFrame();

	CPoint pointLocal(point);
	ScreenToClient(&pointLocal);
	OnLButtonDown(0,pointLocal);
	OnLButtonUp(0,pointLocal);

	CMenu menu;
	if (!menu.LoadMenu(IDRC_GEDTRETYPE))
		return;

	CMenu* pPopup = menu.GetSubMenu(0);
	if (!pPopup)
		return;

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
}

void CGedtreeViewDL::OnViewCensus() 
{
	CGedtreeDoc* pDoc = GetDocument();

	CString sPath = pDoc->GetPathName();
	if (sPath.IsEmpty())
	{
		AfxMessageBox(L"Please save this GEDCOM document before using this command.");
		return;
	}

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(sPath,drive,dir,fname,ext);
	sPath = CString(drive)+CString(dir);
	CString sFilePath = sPath + CString(fname) + "census.html";

	CString sMsg;
	sMsg.Format(L"This will create a HTML page showing each person's probable "
		L"location for each US Census year. Is this OK?\n\n"
		L"The HTML page will be\n%s\n\n",(LPCTSTR)sFilePath);

	if (AfxMessageBox(sMsg,MB_YESNO)!=IDYES)
		return;

	CArray<CDate,CDate> rdateCensusDay;
	for (int nYear = 1790; nYear <= CTime::GetCurrentTime().GetYear(); nYear += 10)
	{
		CDate date;
		date.SetHoliday(nYear,CDate::Census);
		rdateCensusDay.Add(date);
	}

	CString sFile;
	sFile =
		"<html><body><p>This table lists people (rows) and US Census years (columns).<br />\n"
		"Each cell shows the most likely location of a person in a census year.<br />\n"
		"? means the place is unknown.<br />\n"
		"? is also shown if the person's age needs to be estimated.<br />\n"
		"* before the place name means that the census event is actually present \n"
		"in the GEDCOM document.<br /><table border=\"1\">\n";
	sFile += "<tr><th>surname</th><th>given name</th>";

	for (int i(0); i<rdateCensusDay.GetSize(); i++)
	{
		CString s;
		s.Format(L"<th>%d</th>",rdateCensusDay[i].GetSimpleYear());
		sFile += s;
	}
	sFile += "</tr>\n";

	for (int iIndi(0); iIndi<pDoc->m_rIndividual.GetSize(); iIndi++)
	{
		CIndividual& indi = pDoc->m_rIndividual[iIndi];
		if (indi.m_bSelected)
		{
			sFile += indi.Census(rdateCensusDay);
		}
	}

	sFile += "</table></body></html>\n";

	CStdioFile f(sFilePath,CFile::modeCreate|CFile::modeWrite);
	f.WriteString(sFile);
	f.Close();

	if (AfxMessageBox(L"Would you like to open the HTML page in your browser?",MB_YESNO)!=IDYES)
		return;

	::ShellExecute(NULL,L"open",sFilePath,NULL,NULL,SW_SHOWNORMAL);
}

void CGedtreeViewDL::OnUpdateViewCensus(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnViewPlace() 
{
	Places dlg(GetDocument());
	dlg.DoModal();
}

void CGedtreeViewDL::OnUpdateViewPlace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnUpdateViewAnomalies(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IndiSelected());
}

void CGedtreeViewDL::OnEditCopy() 
{
	theApp.m_pClip->ClearContents();
	theApp.Copy(GetDocument(),theApp.m_pClip);
}

void CGedtreeViewDL::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_pClip->m_rIndividual.GetSize()>0);
}

void CGedtreeViewDL::OnEditPaste() 
{
	theApp.Copy(theApp.m_pClip,GetDocument());
}

void CGedtreeViewDL::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == '\r') // Enter
	{
		OnViewIndi();
	}

	CGedtreeView::OnChar(nChar, nRepCnt, nFlags);
}

void CGedtreeViewDL::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CString s;
//	s.Format(L"nChar==%d, nRepCnt==%d, nFlags==%d\n",nChar,nRepCnt,nFlags);
//	OutputDebugString(s);
	if (nChar==VK_CONTROL)
		m_bCtrl = true;
	else if (nChar==VK_SHIFT)
		m_bShift = true;
	else if (nChar==VK_LEFT)
	{
		if (m_bShift && m_bCtrl)
		{
			MoveSelectionLeftF();
			ResetAllViews();
		}
		else if (m_bShift)
		{
			MoveSelectionLeft();
			ResetAllViews();
		}
		else if (m_bCtrl)
		{
			ShiftSelectedIndis(CSize(-1,0));
			ResetAllViews();
		}
		else
			OnScroll(MAKEWORD(SB_LINEUP, -1), 0);
	}
	else if (nChar==VK_RIGHT)
	{
		if (m_bShift && m_bCtrl)
		{
			MoveSelectionRightF();
			ResetAllViews();
		}
		else if (m_bShift)
		{
			MoveSelectionRight();
			ResetAllViews();
		}
		else if (m_bCtrl)
		{
			ShiftSelectedIndis(CSize(1,0));
			ResetAllViews();
		}
		else
			OnScroll(MAKEWORD(SB_LINEDOWN, -1), 0);
	}
	else if (nChar==VK_UP)
	{
		if (m_bShift && m_bCtrl)
		{
			MoveSelectionUpF();
			ResetAllViews();
		}
		else if (m_bShift)
		{
			MoveSelectionUp();
			ResetAllViews();
		}
		else if (m_bCtrl)
		{
			ShiftSelectedIndis(CSize(0,-1));
			ResetAllViews();
		}
		else
			OnScroll(MAKEWORD(-1, SB_LINEUP), 0);
	}
	else if (nChar==VK_DOWN)
	{
		if (m_bShift && m_bCtrl)
		{
			MoveSelectionDownF();
			ResetAllViews();
		}
		else if (m_bShift)
		{
			MoveSelectionDown();
			ResetAllViews();
		}
		else if (m_bCtrl)
		{
			ShiftSelectedIndis(CSize(0,1));
			ResetAllViews();
		}
		else
			OnScroll(MAKEWORD(-1, SB_LINEDOWN), 0);
	}
	CGedtreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGedtreeViewDL::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar==VK_CONTROL)
		m_bCtrl = false;
	else if (nChar==VK_SHIFT)
		m_bShift = false;
	CGedtreeView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGedtreeViewDL::OldGetUserTpl(CMemFile& tpl, const CString& strFilePath, const char* sTpl)
{
	CStdioFile tplUser;
	if (tplUser.Open(strFilePath,CFile::modeRead))
	{
		CString s;
		while (tplUser.ReadString(s))
		{
			s += "\n";
			tpl.Write(s,s.GetLength());
		}
	}
	else
	{
		tpl.Write(sTpl,strlen(sTpl));
	}
}

void CGedtreeViewDL::OnViewAnomalies() 
{
	CGedtreeDoc* pDoc = GetDocument();

/*
	// add UK to end of England, Northern Ireland, and Scotland places
	for (int i(0); i<pDoc->m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		CArray<CEvt,CEvt&>& revt = indi.m_revt;
		UKPlaceEvents(revt);
		CoPlaceEvents(revt);
		CArray<CAttr,CAttr&>& rattr = indi.m_rattr;
		UKPlaceAttrs(rattr);
		CoPlaceAttrs(rattr);
	}
	for (i = 0; i<pDoc->m_rFamily.GetSize(); i++)
	{
		CFamily& fami = pDoc->m_rFamily[i];
		CArray<CEvt,CEvt&>& revt = fami.m_revt;
		UKPlaceEvents(revt);
		CoPlaceEvents(revt);
	}
	return;

//convert source text to HTML
//before releasing this, change it so it first checks to see
//if the text already has HTML tags before adding them
	for (int i(0); i<pDoc->m_rSource.GetSize(); i++)
	{
		CSource& sour = pDoc->m_rSource[i];
		sour.ConvertToHTML();
		sour.PutToTree();
	}
	return;
*/

	CString sPath = pDoc->GetPathName();
	if (sPath.IsEmpty())
	{
		AfxMessageBox(L"Please save this GEDCOM document before using this command.");
		return;
	}

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(sPath,drive,dir,fname,ext);
	sPath = CString(drive)+CString(dir);
	CString sFilePath = sPath + CString(fname) + "anomaly.html";

	CString sMsg;
	sMsg.Format(L"This will create a HTML page showing a report of each person's anomalies. "
		L"Is this OK?\n\nThe HTML page will be\n%s\n\n",(LPCTSTR)sFilePath);

	if (AfxMessageBox(sMsg,MB_YESNO)!=IDYES)
		return;

	CString sFile;
	sFile =
		"<HTML><BODY>This is a report of anomalies found in the GEDCOM document.<BR>\n";

	for (int iIndi(0); iIndi<pDoc->m_rIndividual.GetSize(); iIndi++)
	{
		CIndividual& indi = pDoc->m_rIndividual[iIndi];
		if (indi.m_bSelected)
		{
			sFile += indi.GetAnomalies();
		}
	}

	sFile += "</BODY></HTML>\n";

	{
		CStdioFile f(sFilePath,CFile::modeCreate|CFile::modeWrite);
		f.WriteString(sFile);
	}

	if (AfxMessageBox(L"Would you like to open the HTML page in your browser?",MB_YESNO)!=IDYES)
		return;

	::ShellExecute(NULL,L"open",sFilePath,NULL,NULL,SW_SHOWNORMAL);
}

void CGedtreeViewDL::MoveSelectionUp()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			int f = indi.m_iFather;
			if (f >= 0)
			{
				indi.Select(false);
				m_iLastSel = i;
				pDoc->m_rIndividual[f].Select();
				OnViewScrolltoselection();
			}

			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionDown()
{
	CGedtreeDoc* pDoc = GetDocument();
	if (m_iLastSel >= 0)
	{
		DeselectAll();
		pDoc->m_rIndividual[m_iLastSel].Select();
		OnViewScrolltoselection();
		m_iLastSel = -1;
		return;
	}

	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			if (indi.m_riChild.GetSize())
			{
				indi.Select(false);
				int fx(0x7fffffff);
				int ich(0);
				for (int j(0); j < indi.m_riChild.GetSize(); ++j)
				{
					CIndividual& ch = pDoc->m_rIndividual[indi.m_riChild[j]];
					if (ch.m_rectFrame.left < fx)
					{
						fx = ch.m_rectFrame.left;
						ich = indi.m_riChild[j];
					}
				}
				pDoc->m_rIndividual[ich].Select();
				OnViewScrolltoselection();
			}
			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionLeft()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			if (indi.m_iChildToFamily >= 0)
			{
				CFamily& fam = pDoc->m_rFamily[indi.m_iChildToFamily];
				int fx(0x80000000);
				int ich(-1);
				for (int j(0); j < fam.m_riChild.GetSize(); ++j)
				{
					CIndividual& sib = pDoc->m_rIndividual[fam.m_riChild[j]];
					if (sib.m_rectFrame.left < indi.m_rectFrame.left)
					{
						if (sib.m_rectFrame.left > fx)
						{
							fx = sib.m_rectFrame.left;
							ich = fam.m_riChild[j];
						}
					}
				}
				if (ich >= 0)
				{
					indi.Select(false);
					m_iLastSel = -1;
					pDoc->m_rIndividual[ich].Select();
					OnViewScrolltoselection();
				}
			}
			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionRight()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			if (indi.m_iChildToFamily >= 0)
			{
				CFamily& fam = pDoc->m_rFamily[indi.m_iChildToFamily];
				int fx(0x7fffffff);
				int ich(-1);
				for (int j(0); j < fam.m_riChild.GetSize(); ++j)
				{
					CIndividual& sib = pDoc->m_rIndividual[fam.m_riChild[j]];
					if (sib.m_rectFrame.left > indi.m_rectFrame.left)
					{
						if (sib.m_rectFrame.left < fx)
						{
							fx = sib.m_rectFrame.left;
							ich = fam.m_riChild[j];
						}
					}
				}
				if (ich >= 0)
				{
					indi.Select(false);
					m_iLastSel = -1;
					pDoc->m_rIndividual[ich].Select();
					OnViewScrolltoselection();
				}
			}
			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionUpF()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			int f = indi.m_iMother;
			if (f >= 0)
			{
				indi.Select(false);
				m_iLastSel = i;
				pDoc->m_rIndividual[f].Select();
				OnViewScrolltoselection();
			}

			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionDownF()
{
	CGedtreeDoc* pDoc = GetDocument();
	if (m_iLastSel >= 0)
	{
		DeselectAll();
		pDoc->m_rIndividual[m_iLastSel].Select();
		OnViewScrolltoselection();
		m_iLastSel = -1;
		return;
	}

	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			if (indi.m_riChild.GetSize())
			{
				indi.Select(false);
				int fx(0x80000000);
				int ich(0);
				for (int j(0); j < indi.m_riChild.GetSize(); ++j)
				{
					CIndividual& ch = pDoc->m_rIndividual[indi.m_riChild[j]];
					if (ch.m_rectFrame.left > fx)
					{
						fx = ch.m_rectFrame.left;
						ich = indi.m_riChild[j];
					}
				}
				pDoc->m_rIndividual[ich].Select();
				OnViewScrolltoselection();
			}
			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionLeftF()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			CIndividual* pi = indi.GetLeftSpouse();
			if (pi)
			{
				indi.Select(false);
				pi->Select();
				OnViewScrolltoselection();
				if (m_iLastSel >= 0)
				{
					CIndividual& last = pDoc->m_rIndividual[m_iLastSel];
					if (last.m_iFather!=pi->m_i && last.m_iMother!=pi->m_i)
					{
						m_iLastSel = -1;
					}
				}
			}
			done = true;
		}
	}
}

void CGedtreeViewDL::MoveSelectionRightF()
{
	CGedtreeDoc* pDoc = GetDocument();
	bool done(false);
	for (int i(0); i<pDoc->m_rIndividual.GetSize() && !done; i++)
	{
		CIndividual& indi = pDoc->m_rIndividual[i];
		if (indi.m_bSelected)
		{
			CIndividual* pi = indi.GetRightSpouse();
			if (pi)
			{
				indi.Select(false);
				pi->Select();
				OnViewScrolltoselection();
				if (m_iLastSel >= 0)
				{
					CIndividual& last = pDoc->m_rIndividual[m_iLastSel];
					if (last.m_iFather!=pi->m_i && last.m_iMother!=pi->m_i)
					{
						m_iLastSel = -1;
					}
				}
			}
			done = true;
		}
	}
}
