#if !defined(AFX_PLACES_H__8002B102_1392_11D4_8C2C_00A0C95999E9__INCLUDED_)
#define AFX_PLACES_H__8002B102_1392_11D4_8C2C_00A0C95999E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Places.h : header file
//
#include "DateValue.h"

/////////////////////////////////////////////////////////////////////////////
// Places dialog
struct evtattr
{
	int iIndi;
	int iEvt;
	int iAttr;
	int iFam;
	int iFamEvt;
	evtattr() {}
	evtattr(
		int xiIndi,
		int xiEvt = -1,
		int xiAttr = -1,
		int xiFam = -1,
		int xiFamEvt = -1):
		iIndi(xiIndi),
		iEvt(xiEvt),
		iAttr(xiAttr),
		iFam(xiFam),
		iFamEvt(xiFamEvt)
		{}
};

typedef CArray<evtattr,evtattr&> place_set;

struct data
{
	int iIndi;
	CDateValue dDate;
	CString strEvt;
	CString strPlace;
	data() {}
	data(int i, CDateValue d, const CString& s, const CString& sp):
		iIndi(i), dDate(d), strEvt(s), strPlace(sp) { }
};

class Places : public CDialog
{
// Construction
public:
	Places(CGedtreeDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Places)
	enum { IDD = IDD_PLACE };
	CListCtrl	m_listEvent;
	CTreeCtrl	m_treePlace;
	//}}AFX_DATA

	CGedtreeDoc* m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Places)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddPlaces();
	void AddPlace(const CString& strPlace, int xiIndi, int xiEvt, int xiAttr, int xiFam = -1, int xiFamEvt = -1);
	HTREEITEM FindPlace(HTREEITEM hParent, const CString& strPlace);
	bool SplitPlace(CString& strConglom, CString& strPart);
	void FillEventList();
	void InitList();
	void CheckColumnWidth(int nCol, const CString& str);
	void DeleteData();

	CArray<place_set*,place_set*> m_rEvtattr;
	CArray<data,const data&> m_rpData;
	bool m_bFirst;

	// Generated message map functions
	//{{AFX_MSG(Places)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangedPlace(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLACES_H__8002B102_1392_11D4_8C2C_00A0C95999E9__INCLUDED_)
