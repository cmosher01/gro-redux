// EditEvent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditEvent dialog

#include "event.h"
#include "attr.h"

class CEditEvent : public CDialog
{
// Construction
public:
	CEditEvent(BOOL bFamily = FALSE, BOOL bAttr = FALSE, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CEvt m_evt;
	CAttr m_attr;
	BOOL m_bFamily;
	BOOL m_bAttr;
	CComboBox	m_comboType;
	//{{AFX_DATA(CEditEvent)
	enum { IDD = IDD_EVENT };
	CEdit	m_editPlace;
	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CButton	m_buttonDelete;
	CEdit	m_editNote;
	CStatic	m_staticSource;
	CStatic	m_staticDate;
	CEdit	m_editType;
	CString	m_strPlace;
	CString	m_strType;
	int		m_nType;
	CString	m_strNote;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DataToValue();
	void ValueToData();
	void SetStaticTitles();
	void Enable();
	void FillEvents();
	void PositionControls(int cx, int cy);

	// Generated message map functions
	//{{AFX_MSG(CEditEvent)
	afx_msg void OnChangedate();
	afx_msg void OnSelchangeType();
	afx_msg void OnChangesource();
	afx_msg void OnDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
