// gedtreeDoc.h : interface of the CGedtreeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "indi.h"
#include "fami.h"
#include "sour.h"
#include "repo.h"
#include "head.h"
#include "note.h"

class CGedtreeViewDL;
class CProgress;

const CSize MAX_BOUNDS(0x7fffffff,0x7fffffff);
const int VIEW_BORDER(30);

class CGedtreeDoc : public CDocument
{
protected: // create from serialization only
	CGedtreeDoc();
	DECLARE_DYNCREATE(CGedtreeDoc)

// Attributes
public:
	CString m_strUuid; // uuid created at doc open time
	UUID m_uuid; // uuid created at doc open time
	CWnd m_wndTree;
	CTreeCtrl m_tree;
	int m_iSourceLast;
	bool m_bHadXY;

	CHeader m_head;
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapschemaIndiTagToLabel;
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapschemaFamiTagToLabel;

	CArray<CIndividual,CIndividual&> m_rIndividual;
	CArray<int,int> m_rsortIndi;
	bool m_bsortIndiNeedsUpdate;
	CMap<CString,LPCTSTR,int,int> m_mapIDToIndividual;
	int LookupIndividual(const CString& strID);

	CArray<CFamily,CFamily&> m_rFamily;
	CMap<CString,LPCTSTR,int,int> m_mapIDToFamily;
	int LookupFamily(const CString& strID);

	CArray<CSource,CSource&> m_rSource;
	CMap<CString,LPCTSTR,int,int> m_mapIDToSource;
	int LookupSource(const CString& strID);

	CArray<CRepository,CRepository&> m_rRepository;
	CMap<CString,LPCTSTR,int,int> m_mapIDToRepository;
	int LookupRepository(const CString& strID);

	CArray<CNote,CNote&> m_rNote;
	CMap<CString,LPCTSTR,int,int> m_mapIDToNote;
	int LookupNote(const CString& strID);

	int mcLevel; // and it's count of levels

	CIndividual* Individual(int i);
	CFamily* Family(int i);
	CRect GetBounds();
	CRect GetUnscaledBounds();
	CRect GetUnscaledBoundsNoBorder();
	void CalcScale();
	void CalcNormalBounds();
	void CalcBounds();
	CSize Normalize();
	HTREEITEM ResetSubValue(HTREEITEM hTreeItem, const CString& strTok, const CString& strVal,
		BOOL bAlwaysAdd = FALSE, BOOL bEvenIfEmpty = FALSE, BOOL bBreakIntoCont = FALSE);
	void DeleteSubValue(HTREEITEM hTreeItem, const CString& strTok);
	void DeleteItem(HTREEITEM hTreeItem);
	void ResetToken(HTREEITEM hTreeItem, const CString& strTok, const CString& strVal = "");
	void GetContLines(HTREEITEM htiParent, CString& strValue);
	void BreakContLines(const CString& strValue, CArray<CString,LPCTSTR>& rstrTok, CArray<CString,LPCTSTR>& rstrVal);
	HTREEITEM InsertChild(const CString& strTok, HTREEITEM hTreeItem = TVI_ROOT, int cLev = 0, const CString& strVal = "");
	BOOL HasChildren(const CGedRecord& recParent);
	BOOL HitsIndi(CIndividual* pIndiTest);

// Operations
public:
	void ShiftAllIndividuals(const CSize& sizShift);
	void ClearAllIndividuals();
	void AutoClean();
	void Init();
	void OpenIndi(int iIndi);
	CPoint GetNextPos();
	void ResetAllViews(UINT flagsChanged = -1);
	void UpdateTitle();
	void SetInputCharSet(const CString& str);
	CArray<int,int>* GetSortedIndis();
	void GetSortedSours(CArray<int,int>& rSour);
	void SavePathToReg(int i);
	int PasteIndi(CIndividual& indiFrom);
	void FixUpPastedIndi(int i);
	int PasteFami(CFamily& famiFrom);
	int PasteSour(CSource& sourFrom);
	int PasteRepo(CRepository& repoFrom);
	int PasteNote(CNote& noteFrom);
	void ClearContents();
	void htmlIndex(int iLevel, int iBase, const wstring& docid, wostream& os);
//	void svgChart(const wstring& docid, thinsock& sock);
	void appletChart(const wstring& docid, thinsock& sock);
	void clearAppletChart();
	void htmlIndi(int iIndi, const wstring& docid, wostream& os);
	void htmlSour(int iSour, const wstring& docid, wostream& os);
	void webFile(const wstring& path, thinsock& os, wstring& mime);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGedtreeDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGedtreeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CChildFrame* m_pIndiList;
	CGedtreeViewDL* m_pDL;

protected:
	CPoint m_pointNext;
	MyRect m_rectUnscaledBounds;
	MyRect m_rectBounds;
	MyRect m_rectUnscaledBoundsNoBorder;
	MyRect m_rectBoundsNoBorder;
	int m_cRootRecord;
	CProgress* m_pPrg;
	int m_nPrg;
	BOOL m_bAnsel;
	BOOL m_bUnicode;
	BOOL m_bUnicodeFlagInFile;
	BOOL m_bReverse;
	BOOL m_bUseLast;
	BYTE m_byteLast;
	WORD m_tucharLast;
	BOOL m_bBad;
	int m_cIndi;
	int m_cFami;
	int m_cSour;
	int m_cRepo;
	int m_cNote;
	char* m_pAppletChart;
	int m_cAppletChart;

// Generated message map functions
protected:
	//{{AFX_MSG(CGedtreeDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnEditResizedocument();
	afx_msg void OnViewSources();
	afx_msg void OnViewRepositories();
	afx_msg void OnViewIndexofindividuals();
	afx_msg void OnViewNotes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CGedtreeApp* App() { return (CGedtreeApp*)AfxGetApp(); }
	void ReadFromArchive(CArchive& ar);
	void WriteToArchive(CArchive& ar);
	void GetFromTree();
	void WriteItem(CArchive& ar, HTREEITEM hTreeItem);
	void WriteRecords(CArchive& ar, const CString& strTok = "");
	BOOL GetNextTraverse(HTREEITEM& htvi, int* cLev = NULL);
	void DeleteGedLine(HTREEITEM hTreeItem);
	void BreakConcLines(const CString& strValue, CArray<CString,LPCTSTR>& rstrTok, CArray<CString,LPCTSTR>& rstrVal);
	void ResetIDs();
	void TestUnicode(CFile* pFile);
	BOOL GetLine(CArchive& ar, CString& str);
	void PutLine(CArchive& ar, const CString& str);
	wchar_t FilterReadChar(BYTE c);
	void DisconnectFile();
	void writeApplet(std::wostream& os);
};

/////////////////////////////////////////////////////////////////////////////
