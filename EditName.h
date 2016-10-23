// EditName.h : header file
//

#include "name.h"

/////////////////////////////////////////////////////////////////////////////
// CEditName dialog

class CEditName : public CDialog
{
// Construction
public:
	CEditName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CName m_name;
	//{{AFX_DATA(CEditName)
	enum { IDD = IDD_NAME };
	CEdit	m_editName;
	CStatic	m_staticSource;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DataToValue();
	void ValueToData();
	void SetStaticTitles();

	// Generated message map functions
	//{{AFX_MSG(CEditName)
	afx_msg void OnChangesource();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
