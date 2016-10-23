#pragma once
#include "myrect.h"

class CMyDC
{
	CDC* m_pDC;
	MyRect m_draw;

public:
	CMyDC(CDC* pDC, const MyRect& draw) : m_pDC(pDC), m_draw(draw) {}
	virtual ~CMyDC() {}

	void DrawLine(const CPoint& p1, const CPoint& p2, bool bHidden = false);
	bool FixXY(CPoint& p, const CPoint& q);
	bool FixPoint(CPoint& p, const CPoint& q);
	void ClearBackground();
	void FrameRectGray(const MyRect& rect);
	void FrameRect(const MyRect& rect, bool bHidden = false);
	void DrawRectSel(const MyRect& rect, bool bSelected);
	void FrameRect3D(const MyRect& rect, bool bHidden = false);
	void DrawText(const CString& s, const MyRect& rect);
	void DrawTextSel(const CString& s, const MyRect& rect, bool bSelected, bool bHidden = false);
	void Drag(const MyRect& rect);
	void MakeDraw(MyRect& r);
	void MakeDraw(CPoint& p);
	bool Print() { return !!m_pDC->IsPrinting(); }
	CDC* GetDC() { return m_pDC; }
	bool Visible(const MyRect& rect);
};
