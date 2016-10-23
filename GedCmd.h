// GedCmd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGedCmd class

class CGedCmd : public CCommandLineInfo
{
// Construction
public:
	CGedCmd();   // standard constructor
	virtual ~CGedCmd() { }

	BOOL Process();
	BOOL PostProcess();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSaveChanges)
	protected:
	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);
	//}}AFX_VIRTUAL

// Implementation

};
