// gedtreeView.h : interface of the CGedtreeView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef header_gedtreeview
#define header_gedtreeview

#include <afxcview.h>
#include "bigview.h"

class CGedtreeDoc;

class CGedtreeView : public CBigScrollView
{
protected: // create from serialization only
	CGedtreeView();
	DECLARE_DYNCREATE(CGedtreeView)

// Attributes
public:
	CGedtreeDoc* GetDocument() { return (CGedtreeDoc*)m_pDocument; }

// Operations
public:
	void UpdateTitle();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGedtreeView)
	public:
	virtual void OnDraw(CDC* pDC) { ASSERT(FALSE); }  // overridden to draw this view
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGedtreeView();
	virtual CString GetWindowTitle();
	virtual void Reset(UINT flagsChanged = -1);
	enum // flagsChanged
	{
		position,
		font,
		lists
	};

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGedtreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
