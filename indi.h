#ifndef header_indi
#define header_indi

#include "name.h"
#include "event.h"
#include "attr.h"
#include "datevalue.h"
#include "gedrecord.h"
#include "myrect.h"
#include <list>
#include <utility>

class CChildFrame;
class CFamily;
class CMyDC;

class CIndividual : public CGedRecord
{
public:
	CName m_name;
	int m_nSex; //0=unknown,1=male,2=female
	CArray<CEvt,CEvt&> m_revt;
	int m_iBirth;
	int m_iDeath;
	CArray<CAttr,CAttr&> m_rattr;
	CString m_strTreeDisplay;
	CString m_strBirthDisplay;
	CString m_strDeathDisplay;
//	CString m_strEllipsisDisplay;
//	CString m_strBirthXML;
//	CString m_strDeathXML;
	MyRect m_rectFrame;
	MyRect m_rectText;
	int m_nScale;
	MyRect m_rectScaledFrame;
	MyRect m_rectScaledFrameDraw;
	MyRect m_rectScaledText;
	BOOL m_bSelected;
	BOOL m_bHidden;
	BOOL m_bMark;
	int m_iFather;
	int m_iMother;
	CArray<int,int> m_riSpouse;
	CArray<int,int> m_riChild;
	CArray<int,int> m_riSpouseToFamily;
	int m_iChildToFamily;
	CChildFrame* m_pFrame;
	CChildFrame* m_pPedigree;
	int m_nGrid;
	int m_nLevel;
	CIndividual* m_pMale;
	int m_maxmale;

	CIndividual(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	~CIndividual();
	CIndividual(const CIndividual& o);
	CIndividual& operator=(const CIndividual& o);
	CString Name();
	CString Ident();
	void Calc();
	void CalcFull();
	void PlaceAt(const CPoint& point);
	void ResetWidth();
	void GridRect(CRect& rect);
	BOOL MoveToIsInBounds(CPoint& pt);
	void MoveTo(const CPoint& point);
	int SetScale(int nScale);
	double GetScale();
	void CalcScale();
	void OnDraw(CMyDC& dc);
	CRect GetFrameRect();
	CRect GetUnscaledFrameRect();
	BOOL HitTest(const CPoint& point);
	BOOL HitTest(const CRect& rect);
	BOOL Select(BOOL bSelect = TRUE);
	void Shift(const CSize& sizShift);
	void ShiftUnscaled(const CSize& sizShift);
	BOOL AddSpouse(int iSpouse, BOOL bCheckFirst = FALSE);
	BOOL AddChild(int iChild, BOOL bCheckFirst = FALSE);
	BOOL AddSpouseToFamily(int iSpouseToFamily, BOOL bCheckFirst = FALSE);
	void Clean(int nDepth);
	void CalcFamilies();
	void OpenView();
	void OpenPedigree();
	void AddEvent(HTREEITEM htiEvent, const CString& strTypeTok);
	void AddAttr(HTREEITEM htiAttr, const CString& strTypeTok, const CString& strValue);
	BOOL GetFromTree();
	CPoint GetXY(const CString& str);
	CString GetXY(const CPoint& pt);
	void ScaleForPrint(CRect& rect);
	void Disconnect();
	void SetSex(int nSex);
	CString Sex();
	void DebugRelations();
	CString GetWebPage(CMemFile& tpl, const CString& sDocID = "");
	CString GetRTFPage(CMemFile& tpl);
	CString GetIndiLink(int iIndi, const CString& sDocID = "");
	CString GetIndiRTF(int iIndi);
	CString GetIndiPageRef(int iIndi);
	CString GetSourLink(int iSour, const CString& strCita, const CString& sDocID = "");
	CString Cell(const CString& str);
	BOOL Private();
	virtual CString GetLinkText() { return Name(); }
	void GetSortedEvents(CArray<CEvt,CEvt&>& revt);
	void GetSortedEvents(CFamily& fami, CArray<CEvt,CEvt&>& revt);
	void GetSortedAttrs(CArray<CAttr,CAttr&>& rattr);
	CString Census(const CArray<CDate,CDate>& rdateCensusDay);
	CString AllPlaces();
	CString GetAnomalies();
	int GetSimpleBirth();
	void GetSortedSpouseFamilies(CArray<int,int>& riSpouseToFamily);
	void setLevel(int i = 0);
	void setSeqWithSpouses(double& seq, std::pair<double,double> lev_bounds[], bool left, std::list<CIndividual*>& rptoclean = std::list<CIndividual*>());
	bool setMaxMaleIf(int n) { bool is = n>m_maxmale; if (is) m_maxmale = n; return is; }
	void markWithSpouses();
	void setRootWithSpouses(CIndividual* proot);
	CIndividual* GetLeftSpouse();
	CIndividual* GetRightSpouse();
//	void writeSVG(std::wostringstream& os, int pt);
	void writeApplet(std::wostream& os);
	bool wasMarriedBy(int ymd);
	int MostRecentSpouse(int ymd);
	void showAsOf(CDate* pasof);
	void WriteDescent(CStdioFile& f, int indiNum, std::list<CIndividual*>& todo);
};

#endif
