#ifndef header_fami
#define header_fami

#include "gedrecord.h"
#include "event.h"
class CIndividual;
class CMyDC;
#include "myrect.h"
#include <utility>

class CFamily : public CGedRecord
{
public:
	CArray<CEvt,CEvt&> m_revt;
	int m_iHusband;
	int m_iWife;
	CArray<int,int> m_riChild;
	CPoint m_pt1, m_pt2, m_ptP;
	CPoint m_ptC1, m_ptC2;
	CArray<CPoint,CPoint> m_rpointChild;
	int m_nScale;
	CPoint m_ptsc1, m_ptsc2, m_ptscP;
	CPoint m_ptscC1, m_ptscC2;
	CArray<CPoint,CPoint> m_rpointscChild;
	MyRect m_rectBounds;
	MyRect m_rectScaledBounds;
	MyRect m_rectScaledBoundsDraw;
	BOOL m_bHidden;

	CFamily(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	CFamily::CFamily(const CFamily& family);
	CFamily& operator=(const CFamily& o);
	void Calc();
	void SetScale(int nScale);
	double GetScale();
	void CalcScale();
	CPoint CalcParPt(CPoint pt1, CPoint pt2);
	void OnDraw(CMyDC& dc);
	void ScaleForPrint(CRect& rect);
	void ScaleForPrint(CPoint& pt);
	BOOL AddChild(int iChild, BOOL bCheckFirst = FALSE);
	void CalcDraw();
	void CalcBounds();
	void CheckBounds(const CPoint& pt);
	void GetFromTree();
	void AddEvent(HTREEITEM htiEvent, const CString& strTypeTok);
	void Disconnect(int iIndividual);
	void Connect(CIndividual* pIndi0, CIndividual* pIndi1, int nRelation);
	void ConnectParent(CIndividual* pIndi);
	void ConnectChild(CIndividual* pIndi);
	void DebugRelations();
	void GetSortedChildren(CArray<int,int>& riChild);
	int GetSimpleMarriage();
	int GetSimpleMarriageEnd();
//	void writeSVG(std::wostringstream& os);
	void writeApplet(std::wostream& os);
	void showAsOf(CDate* pasof);
};

#endif
