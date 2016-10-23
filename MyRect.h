#pragma once

class MyRect
{
public:
	int left, right, top, bottom;
	MyRect() : left(0), right(0), top(0), bottom(0) {}
	MyRect(const CRect& r) { SetRect(r.left,r.top,r.right,r.bottom); }

	virtual ~MyRect() {}

	operator const CRect() const { return CRect(left,top,right,bottom); }

	int Width() const { return abs(right-left); }
	int Height() const { return abs(bottom-top); }
	CPoint TopLeft() const { return CPoint(left,top); }
	CPoint BottomRight() const { return CPoint(right,bottom); }
	bool PtInRect(const CPoint& p) const { return left <= p.x && p.x <= right && top <= p.y && p.y <= bottom; }
	bool RectInRect(const MyRect& r) const { return top <= r.bottom && r.top <= bottom && left <= r.right && r.left <= right; }
	CPoint CenterPoint() const { return CPoint((right+left)/2,(bottom+top)/2); }
	CSize Size() const { return CSize(Width(),Height()); }
	bool IsEmpty() const { return !Width() && !Height(); }

	void SetRect(int xleft, int ytop, int xright, int ybottom) { left = xleft; right = xright; top = ytop; bottom = ybottom; }
	void SetRectEmpty() { SetRect(0,0,0,0); }
	void InflateRect( int x, int y ) { left -= x; right += x; top -= y; bottom += y; }
	void InflateRect( const CSize& size ) { left -= size.cx; right += size.cx; top -= size.cy; bottom += size.cy; }
	void InflateRect( int l, int t, int r, int b ) { left -= l; right += r; top -= t; bottom += b; }
	void operator |= (const MyRect& r)
	{
		if (r.IsEmpty())
			return;

		if (IsEmpty())
		{
			left = r.left;
			right = r.right;
			top = r.top;
			bottom = r.bottom;
		}
		else
		{
			left = __min(left,r.left);
			right = __max(right,r.right);
			top = __min(top,r.top);
			bottom = __max(bottom,r.bottom);
		}
	}
	void operator +=(const CSize& s) { left += s.cx; right += s.cx; top += s.cy; bottom += s.cy; }
};
