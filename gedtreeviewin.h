// gedtreeViewIN.h : interface of the CGedtreeViewIN class
//
/////////////////////////////////////////////////////////////////////////////

#include <afxcview.h>
#include "gedtreeview.h"

class CGedtreeDoc;
class CIndividual;
class CFamily;

class CGedtreeViewIN : public CGedtreeView
{
protected: // create from serialization only
	CGedtreeViewIN();
	DECLARE_DYNCREATE(CGedtreeViewIN)

// Attributes
public:
	BOOL m_bInit;
	INT m_iIndi;
	CRect m_rectName;
	CRect m_rectSex;
	CTreeCtrl m_treePedigree;
	CListCtrl m_listPartner;
	CListCtrl m_listEvent;
	CListCtrl m_listAttr;
	CButton m_buttonNew;
	CButton m_buttonName;
	CButton m_buttonSex;
	BOOL m_bFontTooHigh;
	BOOL m_bBigFontTooHigh;
	CTabCtrl m_tab;

// Operations
public:
	void Init(int iIndi);
	CString GetWindowTitle();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGedtreeViewIN)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGedtreeViewIN();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void PositionControls(int cx, int cy);
	void InitCtrls();
	void AddParents(HTREEITEM htvi, int iIndi, int cDepth = 1);
	void FillEvents();
	void FillPartners();
	void FillAttributes();
	void CheckColumnWidth(int nCol, const CString& str, BOOL bForce = FALSE);
	void CheckPColumnWidth(int nCol, const CString& str, BOOL bForce = FALSE);
	void CheckAColumnWidth(int nCol, const CString& str, BOOL bForce = FALSE);
	void DeletePartnerData();
	void DeleteEventData();
	void DeleteAttrData();
	void Reset(UINT flagsChanged = -1);
	void EditEvent(CIndividual& indi, int iEvent);
	void EditEvent(CFamily& fami, int iEvent);
	void EditAttr(CIndividual& indi, int iAttr);
	void NewEvent(CIndividual& indi);
	void NewEvent(CFamily& fami);
	void NewAttr(CIndividual& indi);
	void InitButton(CButton& button, const CString& strName, int nID);

// Generated message map functions
protected:
	//{{AFX_MSG(CGedtreeViewIN)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnViewDroplinechart();
	afx_msg void OnViewOpenpedigree();
	afx_msg void OnViewIndi();
	afx_msg void OnUpdateViewIndi(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnExpandPedigree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectPedigree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoEventlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoAttrlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoPartnerlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectPartner(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAttr(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnName();
	afx_msg void OnSex();

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
