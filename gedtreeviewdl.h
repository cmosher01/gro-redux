// gedtreeViewDL.h : interface of the CGedtreeViewDL class
//
/////////////////////////////////////////////////////////////////////////////

#include <afxcview.h>
#include "gedtreeview.h"
#include <vector>

class CGedtreeDoc;
class CIndividual;

typedef vector<CIndividual*> rpi_t;

class CGedtreeViewDL : public CGedtreeView
{
protected: // create from serialization only
	CGedtreeViewDL();
	DECLARE_DYNCREATE(CGedtreeViewDL)

// Attributes
public:
	CRect m_rectPrev;
	CPoint m_pointPrev;
	BOOL m_bSelecting;
	BOOL m_bMoving;
	int m_nScale;
	CString m_strLastFind;
	bool m_bCtrl;
	bool m_bShift;
	int m_iLastSel;
	HGDIOBJ m_hdib;
	CDC* m_pdcUse;
	COLORREF m_colorFamily;
	CPen* m_ppen;
	int m_dibWidth;
	int m_dibHeight;
	bool m_bSomeHidden;
	CDate* m_pAsOfDate;

	CIndividual* HitIndiTest(const CPoint& point);
	void SelectHitIndis(const CRect& rect);
	CRect ShiftSelectedIndis(const CSize& sizShift);
	BOOL IndiSelected();
	BOOL OneIndiSelected();

// Operations
public:
	CString GetWindowTitle();
	void DeselectAll();
	void SetTotalSize();
	BOOL IsOut(const CPoint& point);
	void ScrollToward(const CPoint& point);
	void SetScale(int nScale);
	void ScrollToIndi(CIndividual* pIndi);
	BOOL DoFind(BOOL bFromSelection = FALSE);
	void Reset(UINT flagsChanged = -1);
	void ResetAllViews();
	void PostNextMove(UINT nFlags, CPoint point);
	void OldGetUserTpl(CMemFile& tpl, const CString& strFilePath, const char* sTpl);
	void MoveSelectionUp();
	void MoveSelectionDown();
	void MoveSelectionLeft();
	void MoveSelectionRight();
	void MoveSelectionUpF();
	void MoveSelectionDownF();
	void MoveSelectionLeftF();
	void MoveSelectionRightF();
	void CleanAll();
	double GetScale();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGedtreeViewDL)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGedtreeViewDL();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void Scrolltoselection();
	CPoint GetScrollPoint();

protected:
	void MyPrint(CMyDC& dc);
	void PrintPages(CMyDC& dc, const MyRect& draw);

// Generated message map functions
protected:
	//{{AFX_MSG(CGedtreeViewDL)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditAlignTop();
	afx_msg void OnUpdateEditAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnEditClean();
	afx_msg void OnUpdateEditClean(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEditSelectAncestors();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAncestors(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnViewScrolltoselection();
	afx_msg void OnUpdateViewScrolltoselection(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindnext();
	afx_msg void OnUpdateEditFindnext(CCmdUI* pCmdUI);
	afx_msg void OnEditDisconnect();
	afx_msg void OnUpdateEditDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnEditNewindividual();
	afx_msg void OnEditConnect();
	afx_msg void OnUpdateEditConnect(CCmdUI* pCmdUI);
	afx_msg void OnViewOpenpedigree();
	afx_msg void OnUpdateViewOpenpedigree(CCmdUI* pCmdUI);
	afx_msg void OnViewDroplinechart();
	afx_msg void OnViewIndi();
	afx_msg void OnUpdateViewIndi(CCmdUI* pCmdUI);
	afx_msg void OnFileHTML();
	afx_msg void OnFileRTF();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewCensus();
	afx_msg void OnUpdateViewCensus(CCmdUI* pCmdUI);
	afx_msg void OnViewPlace();
	afx_msg void OnUpdateViewPlace(CCmdUI* pCmdUI);
	afx_msg void OnViewAnomalies();
	afx_msg void OnUpdateViewAnomalies(CCmdUI* pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFilePrint();
	afx_msg void OnEditHideunselected();
	afx_msg void OnUpdateEditHideunselected(CCmdUI* pCmdUI);
	afx_msg void OnEditShowall();
	afx_msg void OnUpdateEditShowall(CCmdUI* pCmdUI);
	afx_msg void OnEditShowAsOf();
	afx_msg void OnUpdateEditShowAsOf(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
