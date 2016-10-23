// BigView.h : interface of the CBigScrollView class
//
/////////////////////////////////////////////////////////////////////////////

#define MM_NONE 0	// from MFC viewscrl.cpp

// [vlu] = virtual logical units = logical units + offset

class CBigScrollView : public CScrollView
{
protected: // create from serialization only
	CBigScrollView();
	DECLARE_DYNAMIC(CBigScrollView)

// Attributes
public:
	COLORREF m_colorBackGnd;

protected:
	CSize  m_TotalSize;		// logical BigScrollView size [vlu]
	BOOL   m_bExtendX;		// TRUE if BigScrollSize > ScrollSize
	BOOL   m_bExtendY;		// TRUE if BigScrollSize > ScrollSize
	long   m_Delta;			// to correct position between DP and LP
	CPoint m_Offset;		// document offset [vlu]
	CPoint m_Center;		// document center [vlu]
	CSize  m_Ratio;			// scale factor to restore client center
							// simply using CSize for convenience
// Operations
public:

protected:
// Overrides
	void   SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage = sizeDefault, const SIZE& sizeLine = sizeDefault);
	void   SetScaleToFitSize(SIZE sizeTotal);		// sizeTotal [vlu]
	void   FillOutsideRect(CDC* pDC, CBrush* pBrush);
	CSize  GetTotalSize() const;					// [vlu]
	CPoint GetScrollPosition() const;				// [vlu]
	void   ScrollToPosition(POINT Point);			// Point [vlu]
	void   CenterOnPoint(CPoint Center);			// Center [vlu]

// Helpers
	CPoint GetDeviceOrg() const;					// [du]
	void   GetLogClientRect(CRect *pRect) const;	// [vlu]
	CPoint GetLogPosition(CPoint Point) const;		// [vlu]
	CPoint GetLogClientCenter() const;				// [vlu]

	void SaveClientCenter();
	void RestoreClientCenter();
	
// Overrides
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBigScrollView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBigScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBigScrollView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
