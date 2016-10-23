// BigView.cpp : implementation of the CBigScrollView class
//

#include "stdafx.h"
#include "gedtree.h"
#include "BigView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView

IMPLEMENT_DYNAMIC(CBigScrollView, CScrollView)

BEGIN_MESSAGE_MAP(CBigScrollView, CScrollView)
	//{{AFX_MSG_MAP(CBigScrollView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView construction/destruction

CBigScrollView::CBigScrollView()
{
	m_bCenter = TRUE;
	m_colorBackGnd = RGB(255, 255, 255);

	m_bExtendX = FALSE;
	m_bExtendY = FALSE;
	m_Delta = 0;
	m_Center = CPoint(0, 0);
	m_Offset = CPoint(0, 0);
	m_Ratio = CSize(1, 1);
	m_TotalSize = CSize(1,1);
}

CBigScrollView::~CBigScrollView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView overrides

void CBigScrollView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrepareDC(pDC, pInfo);

	// avoid drawing in client area
	CPoint pointOrg = GetDeviceOrg();
	if (!pDC->IsPrinting() && pointOrg != CPoint(0, 0))
	{
		// set windows clipping region
		CRect rectClip;
		pDC->GetClipBox(&rectClip);
//		if (pointOrg.x > 0)
//			{ rectClip.left = 0; rectClip.right = m_totalLog.cx; }
//		if (pointOrg.y > 0)
//			{ rectClip.top = 0; rectClip.bottom = -m_totalLog.cy; }
//		pDC->IntersectClipRect(&rectClip);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView drawing

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView diagnostics

#ifdef _DEBUG
void CBigScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBigScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView overrides

void CBigScrollView::SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage, const SIZE& sizeLine)
{
	long cx, cy;

	// save total size
	ASSERT(sizeTotal.cx > 0 && sizeTotal.cy > 0);
	CSize TotalSize = sizeTotal;

	// compute document size in logical unit
	cx = sizeTotal.cx;
	cy = sizeTotal.cy;
	
	// scroll size can't be greater than 32767
//	if (sizeTotal.cx > 32767) sizeTotal.cx = 32767;
//	if (sizeTotal.cy > 32767) sizeTotal.cy = 32767;
	
	// calculate if BigScrollSize greater than ScrollSize
	cx -= sizeTotal.cx; m_bExtendX = (cx != 0);
	cy -= sizeTotal.cy; m_bExtendY = (cy != 0);

	// if first time, init client center and offset
	if (m_nMapMode == MM_NONE)
	{
		ASSERT(nMapMode > 0);
		int y = sizeTotal.cy / 2;
		if (nMapMode != MM_TEXT) y = -y;
		m_Center = CPoint(sizeTotal.cx / 2, y);
		m_Offset = CPoint(cx / 2, cy / 2);
		m_Ratio = TotalSize;
	}
	else SaveClientCenter();
	
	// to correct scroll position between DP and LP
	m_Delta = 0;
	if (nMapMode != MM_TEXT)
	{
		ASSERT(nMapMode > 0);
		CPoint point(1, 1);
		CWindowDC dc(NULL);
		dc.SetMapMode(nMapMode);
		dc.DPtoLP(&point);
		m_Delta = point.x;
	}

	SetRedraw(FALSE);
//	CScrollView::SetScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
////////////////////////////////////////////////////////////////////////////////
	ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
	ASSERT(nMapMode > 0);
	ASSERT(nMapMode != MM_ISOTROPIC && nMapMode != MM_ANISOTROPIC);

	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;

	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(NULL);
		ASSERT(m_nMapMode > 0);
		dc.SetMapMode(m_nMapMode);

		// total size
		m_totalDev = m_totalLog;
//		dc.LPtoDP((LPPOINT)&m_totalDev);
		m_pageDev = sizePage;
//		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = sizeLine;
//		dc.LPtoDP((LPPOINT)&m_lineDev);
		if (m_totalDev.cy < 0)
			m_totalDev.cy = -m_totalDev.cy;
		if (m_pageDev.cy < 0)
			m_pageDev.cy = -m_pageDev.cy;
		if (m_lineDev.cy < 0)
			m_lineDev.cy = -m_lineDev.cy;
	} // release DC here

	// now adjust device specific sizes
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if (m_pageDev.cx == 0)
		m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0)
		m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0)
		m_lineDev.cx = m_pageDev.cx / 10;
	if (m_lineDev.cy == 0)
		m_lineDev.cy = m_pageDev.cy / 10;

	if (m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
////////////////////////////////////////////////////////////////////////////////
	m_TotalSize = TotalSize;
	RestoreClientCenter();
	SetRedraw(TRUE);
	Invalidate(TRUE);
}

void CBigScrollView::SetScaleToFitSize(SIZE sizeTotal)
{
	// do not use CScrollView::SetScaleToFit();
	// this function change the size of the view !
	CSize sizeClient, sizeSb;
	GetTrueClientSize(sizeClient, sizeSb);
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&sizeClient);
	}
	if (sizeClient.cy < MulDiv(sizeTotal.cy, sizeClient.cx, sizeTotal.cx))
	{
		// add 1% to eliminate scroll bars
		sizeTotal.cy = MulDiv(sizeTotal.cy, 101, 100);
		sizeTotal.cx = MulDiv(sizeTotal.cx, sizeClient.cy, sizeTotal.cy);
		sizeTotal.cy = sizeClient.cy;
	}
	else
	{
		// add 1% to eliminate scroll bars
		sizeTotal.cx = MulDiv(sizeTotal.cx, 101, 100);
		sizeTotal.cx = sizeClient.cx;
		sizeTotal.cy = MulDiv(sizeTotal.cy, sizeClient.cx, sizeTotal.cx);
	}
	SetScrollSizes(m_nMapMode, sizeTotal);
}

void CBigScrollView::FillOutsideRect(CDC* pDC, CBrush* pBrush)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	// if client area is larger than total device size
	CPoint pointOrg = GetDeviceOrg();
	if (pointOrg == CPoint(0, 0)) return;
	
	pDC->SaveDC();

	// draw the document background
	CRect rectDoc(pointOrg, m_totalDev);
	pDC->FillRect(rectDoc, pBrush);

	// draw the client area
	CRect draw;
	//CBrush SideBrush(::GetSysColor(COLOR_APPWORKSPACE));
	CBrush SideBrush(RGB(192,192,192));
	draw.SetRect(rectClient.left, rectClient.top, rectDoc.left, rectClient.bottom);
	if (!draw.IsRectEmpty()) pDC->FillRect(draw, &SideBrush);
	draw.SetRect(rectDoc.right, rectClient.top, rectClient.right, rectClient.bottom);
	if (!draw.IsRectEmpty()) pDC->FillRect(draw, &SideBrush);
	draw.SetRect(rectClient.left, rectClient.top, rectClient.right, rectDoc.top);
	if (!draw.IsRectEmpty()) pDC->FillRect(draw, &SideBrush);
	draw.SetRect(rectClient.left, rectDoc.bottom, rectClient.right, rectClient.bottom);
	if (!draw.IsRectEmpty()) pDC->FillRect(draw, &SideBrush);

	// draw the document frame (from ViewScrl.cpp)
	CPen rectPen, shadowPen;
	rectPen.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_WINDOWFRAME));
	shadowPen.CreatePen(PS_SOLID, 3, ::GetSysColor(COLOR_BTNSHADOW));

	pDC->SelectStockObject(HOLLOW_BRUSH);
	pDC->SelectObject(&rectPen);
	rectDoc.InflateRect(1, 1, 2, 2);
	pDC->Rectangle(rectDoc);
    
	pDC->SelectObject(&shadowPen);
	pDC->MoveTo(rectDoc.right + 1, rectDoc.top    + 2);
	pDC->LineTo(rectDoc.right + 1, rectDoc.bottom + 1);
	pDC->MoveTo(rectDoc.left  + 2, rectDoc.bottom + 1);
	pDC->LineTo(rectDoc.right + 1, rectDoc.bottom + 1);

	rectPen.DeleteObject();
	shadowPen.DeleteObject();

	pDC->RestoreDC(-1);
}

CSize CBigScrollView::GetTotalSize() const
{
	// return the true total size (scroll view + offset)
	return m_TotalSize;
}

CPoint CBigScrollView::GetScrollPosition() const
{
	// return the true scroll position (scroll view + offset)
	CPoint point = CScrollView::GetScrollPosition();
	point += m_Offset;
	return point;
}

void CBigScrollView::ScrollToPosition(POINT Point)
{
	CRect rect;
	GetLogClientRect(&rect);
	Point.x += rect.Width() / 2;
	Point.y += rect.Height() / 2;
	// use center on point to scroll to, new offset will be calculated
	CenterOnPoint(Point);
}

void CBigScrollView::CenterOnPoint(CPoint ptCenter)
{
	// save scroll position & offset
	CPoint savePosition = CScrollView::GetScrollPosition();
	CPoint saveOffset = m_Offset;
	
	CRect rect;	GetClientRect(&rect);
	CPoint Center = rect.CenterPoint();
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&Center);
	}

	// reverse direction
	if (m_nMapMode != MM_TEXT)
	{
		ptCenter.y = -ptCenter.y;
		Center.y = -Center.y;
	}

	CPoint Position(ptCenter.x - Center.x, ptCenter.y - Center.y);

	if (m_bExtendX)		// BigScrollSize < ScrollSize
	{
		// window is left over
		if (ptCenter.x < Center.x)
		{
			Position.x = 0;
			m_Offset.x = 0;
		}
		// page is left over
		else if (ptCenter.x < 16384)
		{
			Position.x = (int)(ptCenter.x - Center.x);
			m_Offset.x = 0;
		}
		// window is right over
		else if (ptCenter.x + Center.x > m_TotalSize.cx)
		{
			Position.x = (int)(32768 - 2 * Center.x);
			m_Offset.x = m_TotalSize.cx - 32768;
		}
		// page is right over
		else if (ptCenter.x + 16384 > m_TotalSize.cx)
		{
			Position.x = (int)(32768 - Center.x - m_TotalSize.cx + ptCenter.x);
			m_Offset.x = m_TotalSize.cx - 32768;
		}
		// page is centered
		else
		{
			Position.x = (int)(16384 - Center.x);
			m_Offset.x = ptCenter.x - 16384;
		}
	}
	else m_Offset.x = 0;

	if (m_bExtendY)		// BigScrollSize < ScrollSize
	{
		// window is top over
		if (ptCenter.y < Center.y)
		{
			Position.y = 0;
			m_Offset.y = 0;
		}
		// page is top over
		else if (ptCenter.y < 16384)
		{
			Position.y = (int)(ptCenter.y - Center.y);
			m_Offset.y = 0;
		}
		// window is bottom over
		else if (ptCenter.y + Center.y > m_TotalSize.cy)
		{
			Position.y = (int)(32768 - 2 * Center.y);
			m_Offset.y = m_TotalSize.cy - 32768;
		}
		// page is bottom over
		else if (ptCenter.y + 16384 > m_TotalSize.cy)
		{
			Position.y = (int)(32768 - Center.y - m_TotalSize.cy + ptCenter.y);
			m_Offset.y = m_TotalSize.cy - 32768;
		}
		// page is centered
		else
		{
			Position.y = (int)(16384 - Center.y);
			m_Offset.y = ptCenter.y - 16384;
		}
	}
	else m_Offset.y = 0;

	// reverse direction
	if (m_nMapMode != MM_TEXT)
	{
		ptCenter.y = -ptCenter.y;
		Position.y = -Position.y;
		m_Offset.y = -m_Offset.y;
	}

	// if scroll position change, scroll view
	if (Position != savePosition)
	{
		CScrollView::ScrollToPosition(Position);
	}
	
	// if offset change, scroll window
	if (saveOffset != m_Offset)
	{
		CSize sizeScroll(saveOffset.x - m_Offset.x, saveOffset.y - m_Offset.y);
		if (m_nMapMode != MM_TEXT)
		{
			ASSERT(m_nMapMode > 0); // must be set
			CWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&sizeScroll);
		}
		ScrollWindow(sizeScroll.cx, -sizeScroll.cy);
	}

	SaveClientCenter();
}

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView helpers

CPoint CBigScrollView::GetDeviceOrg() const
{
	// not zero if client area is larger than total device size
	CPoint pointOrg = GetDeviceScrollPosition();
	if (pointOrg.x > 0) pointOrg.x = 0;
	if (pointOrg.y > 0) pointOrg.y = 0;
	pointOrg.x = abs(pointOrg.x);
	pointOrg.y = abs(pointOrg.y);
	return pointOrg;
}
 
void CBigScrollView::GetLogClientRect(CRect *pRect) const
{
	// return the true client rectangle (scroll view + offset)
	GetClientRect(pRect);
	CPoint point = (m_nMapMode != MM_NONE) ? GetDeviceScrollPosition(): CPoint(0, 0);
	pRect->OffsetRect(point.x, point.y);
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(pRect);
	}
	pRect->OffsetRect(m_Offset);
}

CPoint CBigScrollView::GetLogPosition(CPoint Point) const
{
	// return the true point position (scroll view + offset)
	CPoint point = (m_nMapMode != MM_NONE) ? GetDeviceScrollPosition(): CPoint(0, 0);
	Point += point;
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&Point);
	}
	Point += m_Offset;
	return Point;
}

CPoint CBigScrollView::GetLogClientCenter() const
{
	// return the true client center (scroll view + offset)
	CRect rect;
	GetLogClientRect(&rect);
	return rect.CenterPoint();
}

/////////////////////////////////////////////////////////////////////////////

void CBigScrollView::SaveClientCenter()
{
	// save client center and scale factor
	m_Center = GetLogClientCenter();
	m_Ratio  = m_TotalSize;
}

void CBigScrollView::RestoreClientCenter()
{
	// update client center with scale and restore center
	m_Center.x = MulDiv(m_Center.x, m_TotalSize.cx, m_Ratio.cx);
	m_Center.y = MulDiv(m_Center.y, m_TotalSize.cy, m_Ratio.cy);
//	CenterOnPoint(m_Center);
}

/////////////////////////////////////////////////////////////////////////////
// CBigScrollView message handlers

void CBigScrollView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

	// if ScrollView is not initialized, exit
	if (m_nMapMode == MM_NONE) return;

	// if changing size, restore client center
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
		if (IsWindowVisible())
			RestoreClientCenter();
}

BOOL CBigScrollView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClient;
	GetClientRect(&rectClient);
	CBrush BackGround(m_colorBackGnd);
	// if client area is larger than total device size
	CPoint pointOrg = GetDeviceOrg();
	if (pointOrg != CPoint(0, 0))
		FillOutsideRect(pDC, &BackGround);
	else
		pDC->FillRect(rectClient, &BackGround);
	return TRUE;
}

void CBigScrollView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bExtendX && nSBCode != SB_ENDSCROLL)
	{
		CPoint ScrollPos = CScrollView::GetScrollPosition();
		CRect rect; GetLogClientRect(&rect);
		int Width = rect.Width();

		CClientDC dc(this);
		dc.SetMapMode(m_nMapMode);

		SIZE size;
		switch (nSBCode)
		{
			case SB_LINELEFT:
			case SB_LINERIGHT:
				size = m_lineDev;
				break;
			case SB_PAGELEFT:
			case SB_PAGERIGHT:
				size = m_pageDev;
				break;
			default:
				size.cx = size.cy = 0;
		}
		dc.DPtoLP(&size);
    
		// left side
		if (ScrollPos.x == 0)
		{
			m_Offset.x -= size.cx;
			if (m_Offset.x < 0)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				m_Offset.x = 0;
			}
			Invalidate(TRUE);
		}
		// right side
		else if (ScrollPos.x >= 32767 - Width - m_Delta)
		{
			m_Offset.x += size.cx;
			if (m_Offset.x > m_TotalSize.cx - 32767)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				m_Offset.x = m_TotalSize.cx - 32767;
			}
			Invalidate(TRUE);
		}
	}
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	SaveClientCenter();
}

void CBigScrollView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bExtendY && nSBCode != SB_ENDSCROLL)
	{
		CPoint ScrollPos = CScrollView::GetScrollPosition();
		CRect rect; GetLogClientRect(&rect);
		int Height = rect.Height();

		// reverse direction
		if (m_nMapMode != MM_TEXT)
		{
			ScrollPos.y = -ScrollPos.y;
			Height = -Height;
			m_Offset.y = -m_Offset.y;
		}

		CClientDC dc(this);
		dc.SetMapMode(m_nMapMode);

		SIZE size;
		switch (nSBCode)
		{
			case SB_LINEUP:
			case SB_LINEDOWN:
				size = m_lineDev;
				break;
			case SB_PAGEUP:
			case SB_PAGEDOWN:
				size = m_pageDev;
				break;
			default:
				size.cx = size.cy = 0;
		}
		dc.DPtoLP(&size);

		// on top
		if (ScrollPos.y == 0)
		{
			m_Offset.y -= size.cy;
			if (m_Offset.y < 0)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				m_Offset.y = 0;
			}
			Invalidate(TRUE);
		}
		// on bottom
		else if (ScrollPos.y >= 32767 - Height - m_Delta)
		{
			m_Offset.y += size.cy;
			if (m_Offset.y > m_TotalSize.cy - 32767)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				m_Offset.y = m_TotalSize.cy - 32767;
			}
			Invalidate(TRUE);
		}

		// reverse direction
		if (m_nMapMode != MM_TEXT)
			m_Offset.y = -m_Offset.y;
	}
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
	SaveClientCenter();
}

BOOL CBigScrollView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// calc new x position
	int x = GetScrollPos(SB_HORZ);
	int xOrig = x;

	switch (LOBYTE(nScrollCode))
	{
		case SB_TOP:
			x = 0;
			break;
		case SB_BOTTOM:
			x = INT_MAX;
			break;
		case SB_LINEUP:
			x -= m_lineDev.cx;
			break;
		case SB_LINEDOWN:
			x += m_lineDev.cx;
			break;
		case SB_PAGEUP:
			x -= m_pageDev.cx;
			break;
		case SB_PAGEDOWN:
			x += m_pageDev.cx;
			break;
		case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			si.fMask = SIF_TRACKPOS;
			si.cbSize = sizeof(SCROLLINFO);
			GetScrollInfo(SB_HORZ,&si,SIF_TRACKPOS);
			x = si.nTrackPos;
			break;
		}
	}

	// calc new y position
	int y = GetScrollPos(SB_VERT);
	int yOrig = y;

	switch (HIBYTE(nScrollCode))
	{
		case SB_TOP:
			y = 0;
			break;
		case SB_BOTTOM:
			y = INT_MAX;
			break;
		case SB_LINEUP:
			y -= m_lineDev.cy;
			break;
		case SB_LINEDOWN:
			y += m_lineDev.cy;
			break;
		case SB_PAGEUP:
			y -= m_pageDev.cy;
			break;
		case SB_PAGEDOWN:
			y += m_pageDev.cy;
			break;
		case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			si.fMask = SIF_TRACKPOS;
			si.cbSize = sizeof(SCROLLINFO);
			GetScrollInfo(SB_VERT,&si,SIF_TRACKPOS);
			y = si.nTrackPos;
			break;
		}
	}

	BOOL bResult = OnScrollBy(CSize(x - xOrig, y - yOrig), bDoScroll);
	if (bResult && bDoScroll)
		UpdateWindow();

	return bResult;
}
