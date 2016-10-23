#include "stdafx.h"
#include "mydc.h"
#include  <math.h>

const UINT nDrawFormat(DT_WORDBREAK|DT_NOCLIP|DT_NOPREFIX);
const double dim_off(.8);

void CMyDC::MakeDraw(MyRect& r)
{
	r += -m_draw.TopLeft();
}

void CMyDC::MakeDraw(CPoint& p)
{
	p.x -= m_draw.left;
	p.y -= m_draw.top;
}

static int rnd(double d)
{
	int i;
	if (-1e-8<=d && d<=1e-8) // if d == 0
	{
		i = 0;
	}
	else if (d>0)
	{
		i = (int)(floor(d+.50000001)+1e-8);
	}
	else
	{
		i = (int)(ceil(d-.50000001)-1e-8);
	}
	return i;
}

bool CMyDC::FixXY(CPoint& p, const CPoint& q)
{
	CPoint po(p);

	int newx1(p.x), newy1(p.y);
	int dx(0), dy(0);
	if (p.x < m_draw.left)
	{
		newx1 = m_draw.left;
		dy = rnd((double)(q.y-p.y)*(m_draw.left-p.x)/(q.x-p.x));
		newy1 = p.y + dy;
	}
	else if (p.x > m_draw.right)
	{
		newx1 = m_draw.right;
		dy = rnd((double)(q.y-p.y)*(m_draw.right-p.x)/(q.x-p.x));
		newy1 = p.y + dy;
	}
	p.x = newx1;
	p.y = newy1;

	if (p.y < m_draw.top)
	{
		newy1 = m_draw.top;
		dx = rnd((double)(q.x-p.x)*(m_draw.top-p.y)/(q.y-p.y));
		newx1 = p.x + dx;
	}
	else if (p.y > m_draw.bottom)
	{
		newy1 = m_draw.bottom;
		dx = rnd((double)(q.x-p.x)*(m_draw.bottom-p.y)/(q.y-p.y));
		newx1 = p.x + dx;
	}

	p.x = newx1;
	p.y = newy1;

	bool bInX = (m_draw.left <= p.x && p.x <= m_draw.right);
	bool bInY = (m_draw.top <= p.y && p.y <= m_draw.bottom);

	bool bInLineX;
	bool bInLineY;
	if (po.x < q.x)
		bInLineX = (po.x <= p.x && p.x <= q.x);
	else
		bInLineX = (q.x <= p.x && p.x <= po.x);
	if (po.y < q.y)
		bInLineY = (po.y <= p.y && p.y <= q.y);
	else
		bInLineY = (q.y <= p.y && p.y <= po.y);

	return bInX && bInY && bInLineX && bInLineY;
}

bool CMyDC::FixPoint(CPoint& p, const CPoint& q)
{
	bool bDraw = FixXY(p,q);
	MakeDraw(p);
	return bDraw;
}

void CMyDC::DrawLine(const CPoint& p1, const CPoint& p2, bool bHidden)
{
	CPoint pd1(p1);
	bool bIn1 = FixPoint(pd1,p2);
	CPoint pd2(p2);
	bool bIn2 = FixPoint(pd2,p1);

	if (!bIn1 && !bIn2)
		return;

	m_pDC->MoveTo(pd1);
	if (bHidden)
	{
		CPen* pcur = m_pDC->GetCurrentPen();
		LOGPEN logpen;
		pcur->GetLogPen(&logpen);
		COLORREF color = logpen.lopnColor;
		COLORREF colorBack = ::GetSysColor(COLOR_WINDOW);
		int c1 = abs(GetRValue(color)-GetRValue(colorBack));
		int c2 = abs(GetGValue(color)-GetGValue(colorBack));
		int c3 = abs(GetBValue(color)-GetBValue(colorBack));
		int cd = (int)floor(min(c1,min(c2,c3))*dim_off);
		if (GetRValue(color)>GetRValue(colorBack))
			cd = -cd;
		color = RGB(GetRValue(color)+cd,GetGValue(color)+cd,GetBValue(color)+cd);

		CPen pen(PS_SOLID,1,color);
		CPen* pOld = m_pDC->SelectObject(&pen);

		m_pDC->LineTo(pd2);

		m_pDC->SelectObject(pOld);
	}
	else
		m_pDC->LineTo(pd2);
}

void CMyDC::ClearBackground()
{
	MyRect r(m_draw);
	MakeDraw(r);
	CRect rect(r);
	m_pDC->FillSolidRect(&rect,::GetSysColor(COLOR_WINDOW));
}

void CMyDC::FrameRectGray(const MyRect& rect)
{
	CPen pen(PS_SOLID,1,RGB(128,128,128));
	CPen* pOld = m_pDC->SelectObject(&pen);

	FrameRect(rect);

	m_pDC->SelectObject(pOld);
}

void CMyDC::FrameRect(const MyRect& rect, bool bHidden)
{
	MyRect r(rect);
	CPoint p1(rect.left,rect.top);
	CPoint p2(rect.right,rect.top);
	CPoint p3(rect.right,rect.bottom);
	CPoint p4(rect.left,rect.bottom);

	if (bHidden)
	{
		CPen* pcur = m_pDC->GetCurrentPen();
		LOGPEN logpen;
		pcur->GetLogPen(&logpen);
		COLORREF color = logpen.lopnColor;

		static COLORREF colorBackPrev = 0xffffffff;
		static COLORREF colorPrev;
		static COLORREF colorLight; // cache ligher color
		COLORREF colorBack = ::GetSysColor(COLOR_WINDOW);
		if (colorBackPrev!=colorBack || color!=colorPrev)
		{
			colorBackPrev = colorBack;
			colorPrev = color;
			int c1 = max(abs(GetRValue(color)-GetRValue(colorBack)),26);
			int c2 = max(abs(GetGValue(color)-GetGValue(colorBack)),26);
			int c3 = max(abs(GetBValue(color)-GetBValue(colorBack)),26);
			int cd = (int)floor(min(c1,min(c2,c3))*dim_off);
			if (GetRValue(color)>GetRValue(colorBack))
				cd = -cd;
			colorLight = RGB(GetRValue(color)+cd,GetGValue(color)+cd,GetBValue(color)+cd);
		}
		color = colorLight;

		CPen pen(PS_SOLID,1,color);
		CPen* pOld = m_pDC->SelectObject(&pen);

		DrawLine(p1,p2);
		DrawLine(p2,p3);
		DrawLine(p3,p4);
		DrawLine(p4,p1);

		m_pDC->SelectObject(pOld);
	}
	else
	{
		DrawLine(p1,p2);
		DrawLine(p2,p3);
		DrawLine(p3,p4);
		DrawLine(p4,p1);
	}
}

void CMyDC::DrawText(const CString& s, const MyRect& rect)
{
	MyRect r(rect);
	if (!Visible(r))
		return;
	MakeDraw(r);

	CRect rectx(r);
	m_pDC->DrawText(s,&rectx,nDrawFormat);
}

void CMyDC::DrawTextSel(const CString& s, const MyRect& rect, bool bSelected, bool bHidden)
{
	if (bHidden)
	{
		COLORREF color = ::GetSysColor(COLOR_WINDOWTEXT);
		COLORREF colorBack = ::GetSysColor(COLOR_WINDOW);

		static COLORREF colorBackPrev = 0xffffffff;
		static COLORREF colorPrev = 0xffffffff;
		static COLORREF colorLight; // cache ligher color
		if (colorBackPrev!=colorBack || color!=colorPrev)
		{
			colorBackPrev = colorBack;
			colorPrev = color;
			int c1 = max(abs(GetRValue(color)-GetRValue(colorBack)),26);
			int c2 = max(abs(GetGValue(color)-GetGValue(colorBack)),26);
			int c3 = max(abs(GetBValue(color)-GetBValue(colorBack)),26);
			int cd = (int)floor(min(c1,min(c2,c3))*dim_off);
			if (GetRValue(color)>GetRValue(colorBack))
				cd = -cd;
			colorLight = RGB(GetRValue(color)+cd,GetGValue(color)+cd,GetBValue(color)+cd);
		}
		color = colorLight;
		COLORREF colorOld = m_pDC->SetTextColor(color);

		DrawText(s,rect);

		m_pDC->SetTextColor(colorOld);
	}
	else
	{
		COLORREF color = ::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT);
		COLORREF colorOld = m_pDC->SetTextColor(color);

		DrawText(s,rect);

		m_pDC->SetTextColor(colorOld);
	}
}

void CMyDC::Drag( const MyRect& rect)
{
	FrameRectGray(rect);
}

void CMyDC::DrawRectSel(const MyRect& rect, bool bSelected)
{
	MyRect r(rect);
	if (!Visible(r))
		return;
	MakeDraw(r);

	COLORREF colorFill = ::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	CRect rectx(r);
	m_pDC->FillSolidRect(&rectx,colorFill);
}

void CMyDC::FrameRect3D(const MyRect& rect, bool bHidden)
{
	MyRect r(rect);
	if (!Visible(r))
		return;
	MakeDraw(r);
	CRect rectx(r);
	if (bHidden)
		FrameRect(rect,bHidden);
	else
		m_pDC->DrawEdge(&rectx,EDGE_RAISED,BF_RECT);
}

bool CMyDC::Visible(const MyRect& rect)
{
	return
		m_draw.top <= rect.bottom && rect.top <= m_draw.bottom &&
		m_draw.left <= rect.right && rect.left <= m_draw.right;
}
