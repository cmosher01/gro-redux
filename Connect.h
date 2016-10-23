// Connect.h : header file
//

class CIndividual;
class CFamily;
class CGedtreeDoc;

/////////////////////////////////////////////////////////////////////////////
// CConnect dialog

class CConnect : public CDialog
{
// Construction
public:
	CConnect(CGedtreeDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnect)
	enum { IDD = IDD_CONNECT };
	CComboBox	m_comboFamily;
	int		m_nRelation;
	int		m_nFamily;
	CString	m_strName0;
	CString	m_strName1;
	//}}AFX_DATA
	CGedtreeDoc* m_pDoc;
	enum relation { parent, child, spouse, sibling };
	CIndividual* m_pIndi0;
	CIndividual* m_pIndi1;
	CFamily* m_pFamily; // (output) NULL means to create a new family


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillFamilies();
	void AddFamily(int iFamily, int iShow);
	void AddFamilyItem(const CString& str, int iFamily);
	void AddSpouseFamilies(CIndividual* pIndi);
	void AddSiblingFamilies(CIndividual* pIndiSour, CIndividual* pIndiDest);
	void FillParentChild(CIndividual* pIndiParent, CIndividual* pIndiChild);
	void CheckSex(CIndividual* pIndi);

	// Generated message map functions
	//{{AFX_MSG(CConnect)
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg void OnRelation();
	DECLARE_MESSAGE_MAP()
};
