#pragma once
// PickRels.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPickRels dialog

class CPickRels : public CDialog
{
// Construction
public:
	CPickRels(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPickRels)
	enum { IDD = IDD_RELATIVES };
	BOOL	m_bAncestors;
	BOOL	m_bAncestorsFemale;
	BOOL	m_bAncestorsMale;
	BOOL	m_bDescendants;
	BOOL	m_bDescendantsFemale;
	BOOL	m_bDescendantsMale;
	BOOL	m_bMtdna;
	BOOL	m_bYchrom;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickRels)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickRels)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
