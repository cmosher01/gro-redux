#include "stdafx.h"
#include <math.h>
#include "fami.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"
#include "connect.h"
#include "mydc.h"
#include <sstream>
using namespace std;

const int BAR_HEIGHT(4);
const int CHILD_LINE_DISTANCE(10);
const int CHILD_HEIGHT(10);
const int MARRIAGE_SPACING(20);
const int CHILD_VSPACING(150);

CFamily::CFamily(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem),
	m_iHusband(-1),
	m_iWife(-1),
	m_nScale(1),
	m_bHidden(FALSE)
{
}

CFamily& CFamily::operator=(const CFamily& o)
{
	CGedRecord::operator=(o);
	m_revt.Copy(o.m_revt);
	m_iHusband = o.m_iHusband;
	m_iWife = o.m_iWife;
	m_riChild.Copy(o.m_riChild);
	m_pt1 = o.m_pt1;
	m_pt2 = o.m_pt2;
	m_ptP = o.m_ptP;
	m_ptC1 = o.m_ptC1;
	m_ptC2 = o.m_ptC2;
	m_rpointChild.Copy(o.m_rpointChild);
	m_nScale = o.m_nScale;
	m_ptsc1 = o.m_ptsc1;
	m_ptsc2 = o.m_ptsc2;
	m_ptscP = o.m_ptscP;
	m_ptscC1 = o.m_ptscC1;
	m_ptscC2 = o.m_ptscC2;
	m_rpointscChild.Copy(o.m_rpointscChild);
	m_rectBounds = o.m_rectBounds;
	m_rectScaledBounds = o.m_rectScaledBounds;
	m_rectScaledBoundsDraw = o.m_rectScaledBoundsDraw;
	m_bHidden = o.m_bHidden;
	return *this;
}

CFamily::CFamily(const CFamily& o):
	m_iHusband(o.m_iHusband),
	m_iWife(o.m_iWife),
	m_pt1(o.m_pt1),
	m_pt2(o.m_pt2),
	m_ptP(o.m_ptP),
	m_ptC1(o.m_ptC1),
	m_ptC2(o.m_ptC2),
	m_nScale(o.m_nScale),
	m_ptsc1(o.m_ptsc1),
	m_ptsc2(o.m_ptsc2),
	m_ptscP(o.m_ptscP),
	m_ptscC1(o.m_ptscC1),
	m_ptscC2(o.m_ptscC2),
	m_rectBounds(o.m_rectBounds),
	m_rectScaledBounds(o.m_rectScaledBounds),
	m_rectScaledBoundsDraw(o.m_rectScaledBoundsDraw),
	m_bHidden(o.m_bHidden)
{
	m_revt.Copy(o.m_revt);
	m_riChild.Copy(o.m_riChild);
	m_rpointChild.Copy(o.m_rpointChild);
	m_rpointscChild.Copy(o.m_rpointscChild);
}

void CFamily::Calc()
{
	for (int i(0); i<m_revt.GetSize(); i++)
	{
		CEvt& evt = m_revt[i];
		evt.Calc();
	}
	CalcDraw();
}

void CFamily::CalcDraw()
{
	ASSERT(Exists());

	// assume either a husband, or a wife, or at least one child
	if (!(m_iHusband>=0||m_iWife>=0||m_riChild.GetSize()>0)) return;

	CRect rect1, rect2;
	if (m_iHusband<0&&m_iWife<0) // no husband or wife
	{
		/*
		rect1 = m_pDoc->Individual(m_riChild[0])->GetUnscaledFrameRect();
		rect1 -= CSize(0,4*BAR_HEIGHT);
		rect2 = rect1;
		rect2.left = rect2.right = rect1.right+MARRIAGE_SPACING;
		*/
		rect1.SetRectEmpty();
		rect2.SetRectEmpty();
	}
	else if (m_iHusband<0) // wife with no husband
	{
		rect1 = rect2 = m_pDoc->Individual(m_iWife)->GetUnscaledFrameRect();
		rect1.left = rect1.right = rect2.left-MARRIAGE_SPACING;
	}
	else if (m_iWife<0) // husband with no wife
	{
		rect1 = rect2 = m_pDoc->Individual(m_iHusband)->GetUnscaledFrameRect();
		rect2.left = rect2.right = rect1.right+MARRIAGE_SPACING;
	}
	else // both husband and wife
	{
		rect1 = m_pDoc->Individual(m_iHusband)->GetUnscaledFrameRect();
		rect2 = m_pDoc->Individual(m_iWife)->GetUnscaledFrameRect();
	}

	BOOL bHusbandOnRight = (rect1.left>rect2.left);
	if (bHusbandOnRight)
	{
		CRect rectTemp = rect1;
		rect1 = rect2;
		rect2 = rectTemp;
	}

	CPoint pt1, pt2;
	m_pt1.x = rect1.right;
	m_pt1.y = (rect1.top+rect1.bottom)/2;
	m_pt2.x = rect2.left;
	m_pt2.y = (rect2.top+rect2.bottom)/2;

	if (bHusbandOnRight)
		m_ptP = CalcParPt(
			CPoint(m_pt2.x,m_pt2.y+BAR_HEIGHT/2),
			CPoint(m_pt1.x,m_pt1.y+BAR_HEIGHT/2));
	else
		m_ptP = CalcParPt(
			CPoint(m_pt1.x,m_pt1.y+BAR_HEIGHT/2),
			CPoint(m_pt2.x,m_pt2.y+BAR_HEIGHT/2));

	if (m_riChild.GetSize())
	{
		CRect rect = m_pDoc->Individual(m_riChild[0])->GetUnscaledFrameRect();
		int nTop = rect.top;
		int nLeft, nRight;
		nLeft = nRight = (rect.right+rect.left)/2;
		m_rpointChild.RemoveAll();
		m_rpointChild.Add(CPoint((rect.right+rect.left)/2,rect.top));
		for (int i(1); i<m_riChild.GetSize(); i++)
		{
			rect = m_pDoc->Individual(m_riChild[i])->GetUnscaledFrameRect();

			m_rpointChild.Add(CPoint((rect.right+rect.left)/2,rect.top));

			if (rect.top<nTop) nTop = rect.top;

			int nTest = (rect.right+rect.left)/2;
			if (nTest<nLeft) nLeft = nTest;
			if (nTest>nRight) nRight = nTest;
		}
		m_ptC1.y = m_ptC2.y = nTop-CHILD_HEIGHT;
		m_ptC1.x = nLeft;
		m_ptC2.x = nRight;
	}
	CalcBounds();
	CalcScale();
	int cShown(0);
	if (m_iHusband>=0 && !m_pDoc->Individual(m_iHusband)->m_bHidden)
		cShown++;
	if (m_iWife>=0 && !m_pDoc->Individual(m_iWife)->m_bHidden)
		cShown++;
	for (int i(0); i<m_riChild.GetSize() && cShown<2; i++)
	{
		if (!m_pDoc->Individual(m_riChild[i])->m_bHidden)
			cShown++;
	}
	m_bHidden = cShown==0;
}

CPoint CFamily::CalcParPt(CPoint pt1, CPoint pt2)
{
	CSize d(pt2-pt1);
	double dist = sqrt(d.cx*d.cx+d.cy*d.cy);

	return pt1+CSize(
		(int)(CHILD_LINE_DISTANCE*d.cx/dist),
		(int)(CHILD_LINE_DISTANCE*d.cy/dist));
}

void CFamily::SetScale(int nScale)
{
	m_nScale = nScale;
	CalcScale();
}

double CFamily::GetScale()
{
	return (double)m_nScale;
}

void CFamily::CalcScale()
{
	double scale = GetScale();
	m_ptsc1.x = m_pt1.x/scale;
	m_ptsc1.y = m_pt1.y/scale;
	m_ptsc2.x = m_pt2.x/scale;
	m_ptsc2.y = m_pt2.y/scale;
	m_ptscP.x = m_ptP.x/scale;
	m_ptscP.y = m_ptP.y/scale;
	m_ptscC1.x = m_ptC1.x/scale;
	m_ptscC1.y = m_ptC1.y/scale;
	m_ptscC2.x = m_ptC2.x/scale;
	m_ptscC2.y = m_ptC2.y/scale;
	m_rpointscChild.RemoveAll();
	for (int i(0); i<m_rpointChild.GetSize(); i++)
	{
		CPoint pt(m_rpointChild[i].x/scale,m_rpointChild[i].y/scale);
		m_rpointscChild.Add(pt);
	}
	m_rectScaledBounds.top = m_rectBounds.top/scale;
	m_rectScaledBounds.left = m_rectBounds.left/scale;
	m_rectScaledBounds.bottom = m_rectBounds.bottom/scale;
	m_rectScaledBounds.right = m_rectBounds.right/scale;

	m_rectScaledBoundsDraw.top = m_rectScaledBounds.top-1;
	m_rectScaledBoundsDraw.left = m_rectScaledBounds.left-1;
	m_rectScaledBoundsDraw.bottom = m_rectScaledBounds.bottom+1;
	m_rectScaledBoundsDraw.right = m_rectScaledBounds.right+1;
}

void CFamily::CalcBounds()
{
	m_rectBounds.SetRect(INT_MAX,INT_MAX,INT_MIN,INT_MIN);
	CheckBounds(m_pt1-CSize(0,BAR_HEIGHT/2));
	CheckBounds(m_pt2-CSize(0,BAR_HEIGHT/2));
	CheckBounds(m_ptC1);
	CheckBounds(m_ptC2);
	for (int i(0); i<m_rpointChild.GetSize(); i++)
		CheckBounds(m_rpointChild[i]);
	ASSERT(m_rectBounds.left<INT_MAX);
	ASSERT(m_rectBounds.top<INT_MAX);
	ASSERT(m_rectBounds.right>INT_MIN);
	ASSERT(m_rectBounds.bottom>INT_MIN);

	int nInflate = CHILD_LINE_DISTANCE;
	if (CHILD_HEIGHT>nInflate)
		nInflate = CHILD_HEIGHT;
	m_rectBounds.InflateRect(0,nInflate);
}

void CFamily::CheckBounds(const CPoint& pt)
{
	if (pt.y<m_rectBounds.top)
		m_rectBounds.top = pt.y;
	if (pt.x<m_rectBounds.left)
		m_rectBounds.left = pt.x;
	if (pt.y>m_rectBounds.bottom)
		m_rectBounds.bottom = pt.y;
	if (pt.x>m_rectBounds.right)
		m_rectBounds.right = pt.x;
}

void CFamily::OnDraw(CMyDC& dc)
{
	if (!Exists()) return;

	// assume either a husband, or a wife, or at least one child
	if (!(m_iHusband>=0||m_iWife>=0||m_riChild.GetSize()>0)) return;

	if (m_pt1.x||m_pt1.y||m_pt2.x||m_pt2.y)
	{
		CPoint p1(m_ptsc1);
		CPoint p2(m_ptsc2);
		int nBarHeight(BAR_HEIGHT/GetScale());
		if (dc.Print())
		{
			ScaleForPrint(p1);
			ScaleForPrint(p2);
			nBarHeight *= 4;
		}
		dc.DrawLine(CPoint(p1.x,p1.y-nBarHeight/2),CPoint(p2.x,p2.y-nBarHeight/2),!!m_bHidden);
		dc.DrawLine(CPoint(p1.x,p1.y+nBarHeight/2),CPoint(p2.x,p2.y+nBarHeight/2),!!m_bHidden);
	}

	if (m_riChild.GetSize())
	{
		CPoint p1(m_ptscC1);
		CPoint p2(m_ptscC2);

		if (dc.Print())
		{
			ScaleForPrint(p1);
			ScaleForPrint(p2);
		}

		dc.DrawLine(p1,p2,!!m_bHidden);
		for (int i(0); i<m_rpointscChild.GetSize(); i++)
		{
			CPoint pt(m_rpointscChild[i]);
			CPoint pt2(pt.x,m_ptscC1.y);
			if (dc.Print())
			{
				ScaleForPrint(pt);
				ScaleForPrint(pt2);
			}

			dc.DrawLine(pt2,pt,!!m_bHidden);
		}
		if (m_pt1.x||m_pt1.y||m_pt2.x||m_pt2.y)
		{
			CPoint pt(m_ptscP);
			if (dc.Print())
				ScaleForPrint(pt);

			if (p1.x<pt.x && pt.x<p2.x)
			{
				dc.DrawLine(pt,CPoint(pt.x,p1.y),!!m_bHidden);
			}
			else
			{
				int nChildHeight(CHILD_HEIGHT/GetScale());
				if (dc.Print())
					nChildHeight *= 4;
				dc.DrawLine(pt,CPoint(pt.x,p1.y-nChildHeight),!!m_bHidden);
				dc.DrawLine(CPoint(pt.x,p1.y-nChildHeight),CPoint((p1.x+p2.x)/2,p1.y-nChildHeight),!!m_bHidden);
				dc.DrawLine(CPoint((p1.x+p2.x)/2,p1.y-nChildHeight),CPoint((p1.x+p2.x)/2,p1.y),!!m_bHidden);
			}
		}
	}
}

void CFamily::ScaleForPrint(CRect& rect)
{
	int nFactor(4);
	rect.top *= nFactor;
	rect.left *= nFactor;
	rect.bottom *= nFactor;
	rect.right *= nFactor;
}

void CFamily::ScaleForPrint(CPoint& pt)
{
	int nFactor(4);
	pt.x *= nFactor;
	pt.y *= nFactor;
}

BOOL CFamily::AddChild(int iChild, BOOL bCheckFirst)
{
	if (iChild<0) return FALSE;

	BOOL bAdd(TRUE);
	if (bCheckFirst)
	{
		int i(0);
		while (i<m_riChild.GetSize()&&bAdd)
			if (m_riChild[i++]==iChild)
				bAdd = FALSE;
	}
	if (bAdd)
		m_riChild.Add(iChild);

	return bAdd;
}

void CFamily::AddEvent(HTREEITEM htiEvent, const CString& strTypeTok)
{
	CEvt evt(m_pDoc,htiEvent);
	evt.GetFromTree(strTypeTok);
	m_revt.Add(evt);
}

void CFamily::Disconnect(int iIndividual)
{
	int bHusb(FALSE), bWife(FALSE), iChil(-1);

	if (m_iHusband==iIndividual)
	{
		m_iHusband = -1;
		bHusb = TRUE;
	}
	else if (m_iWife==iIndividual)
	{
		m_iWife = -1;
		bWife = TRUE;
	}
	else
	{
		for (int i(0); i<m_riChild.GetSize() && iChil<0; i++)
		{
			if (m_riChild[i]==iIndividual)
			{
				m_riChild.RemoveAt(i);
				iChil = i;
			}
		}
	}
	ASSERT(bHusb||bWife||(iChil>=0));

	if (bHusb||bWife)
	{
		// clear children's father;
		for (int i(0); i<m_riChild.GetSize(); i++)
		{
			CIndividual& indi = m_pDoc->m_rIndividual[m_riChild[i]];
			if (bHusb)
				indi.m_iFather = -1;
			else
				indi.m_iMother = -1;
		}
		// remove from individual's spouses' list of spouses
		CIndividual& indi = m_pDoc->m_rIndividual[iIndividual];
		for (i = 0; i<indi.m_riSpouse.GetSize(); i++)
		{
			CIndividual& indiSpouse = m_pDoc->m_rIndividual[indi.m_riSpouse[i]];
			int j = 0;
			while (j<indiSpouse.m_riSpouse.GetSize())
			{
				if (indiSpouse.m_riSpouse[j]==iIndividual)
					indiSpouse.m_riSpouse.RemoveAt(j);
				else
					j++;
			}
		}
	}
	else
	{
		// remove child from father's and mother's children list
		if (m_iHusband>=0)
		{
			CIndividual& indi = m_pDoc->m_rIndividual[m_iHusband];
			int i = 0;
			while (i<indi.m_riChild.GetSize())
			{
				if (indi.m_riChild[i]==iIndividual)
					indi.m_riChild.RemoveAt(i);
				else
					i++;
			}
		}
		if (m_iWife>=0)
		{
			CIndividual& indi = m_pDoc->m_rIndividual[m_iWife];
			int i = 0;
			while (i<indi.m_riChild.GetSize())
			{
				if (indi.m_riChild[i]==iIndividual)
					indi.m_riChild.RemoveAt(i);
				else
					i++;
			}
		}
	}

	CTreeCtrl& tree = m_pDoc->m_tree;

	int cChil(-1);
	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		HTREEITEM htiNext = tree.GetNextSiblingItem(htiSub);

		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);
		if (pglChild->m_strTok=="HUSB" && bHusb)
			m_pDoc->DeleteItem(htiSub);
		else if (pglChild->m_strTok=="WIFE" && bWife)
			m_pDoc->DeleteItem(htiSub);
		else if (pglChild->m_strTok=="CHIL" && iChil>=0)
		{
			cChil++;
			if (cChil==iChil)
				m_pDoc->DeleteItem(htiSub);
		}

		htiSub = htiNext;
	}
	if (!(m_iHusband>=0||m_iWife>=0||m_riChild.GetSize()>0))
		Delete();
	else
		CalcDraw();
}

void CFamily::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);
		if (pglChild->m_strTok=="HUSB")
			m_iHusband = m_pDoc->LookupIndividual(pglChild->m_strValAsID);
		else if (pglChild->m_strTok=="WIFE")
			m_iWife = m_pDoc->LookupIndividual(pglChild->m_strValAsID);
		else if (pglChild->m_strTok=="CHIL")
			AddChild(m_pDoc->LookupIndividual(pglChild->m_strValAsID));
		else if (
			pglChild->m_strTok=="ANUL"||
			pglChild->m_strTok=="CENS"||
			pglChild->m_strTok=="DIV"||
			pglChild->m_strTok=="DIVF"||
			pglChild->m_strTok=="ENGA"||
			pglChild->m_strTok=="MARR"||
			pglChild->m_strTok=="MARB"||
			pglChild->m_strTok=="MARC"||
			pglChild->m_strTok=="MARL"||
			pglChild->m_strTok=="MARS"||
			pglChild->m_strTok=="EVEN"||
			FALSE)
		{
			AddEvent(htiSub,pglChild->m_strTok);
		}
		htiSub = tree.GetNextSiblingItem(htiSub);
	}

	if (m_iHusband>=0)
	{
		m_pDoc->m_rIndividual[m_iHusband].AddSpouse(m_iWife);
		m_pDoc->m_rIndividual[m_iHusband].AddSpouseToFamily(m_i);
	}
	if (m_iWife>=0)
	{
		m_pDoc->m_rIndividual[m_iWife].AddSpouse(m_iHusband);
		m_pDoc->m_rIndividual[m_iWife].AddSpouseToFamily(m_i);
	}
	for (int i(0); i<m_riChild.GetSize(); i++)
	{
		if (m_iHusband>=0)
			m_pDoc->m_rIndividual[m_iHusband].AddChild(m_riChild[i]);
		if (m_iWife>=0)
			m_pDoc->m_rIndividual[m_iWife].AddChild(m_riChild[i]);
		m_pDoc->m_rIndividual[m_riChild[i]].m_iFather = m_iHusband;
		m_pDoc->m_rIndividual[m_riChild[i]].m_iMother = m_iWife;
		m_pDoc->m_rIndividual[m_riChild[i]].m_iChildToFamily = m_i;
		m_pDoc->ResetSubValue(m_pDoc->m_rIndividual[m_riChild[i]].m_hTreeItem,"FAMC",GetID(),FALSE);
	}
}

void CFamily::Connect(CIndividual* pIndi0, CIndividual* pIndi1, int nRelation)
{
	if (nRelation==CConnect::parent)
	{
		ConnectParent(pIndi0);
		ConnectChild(pIndi1);
	}
	else if (nRelation==CConnect::child)
	{
		ConnectChild(pIndi0);
		ConnectParent(pIndi1);
	}
	else if (nRelation==CConnect::spouse)
	{
		ConnectParent(pIndi0);
		ConnectParent(pIndi1);
	}
	else if (nRelation==CConnect::sibling)
	{
		ConnectChild(pIndi0);
		ConnectChild(pIndi1);
	}
}

void CFamily::ConnectParent(CIndividual* pIndi)
{
	if (!pIndi->m_nSex) //???
	{
		AfxMessageBox(_T("Can't connect: don't know gender."));
		return;
	}

	BOOL bWife = pIndi->m_nSex==2;

	int* piSpouse, iOtherSpouse;
	CString strTag;
	if (bWife)
	{
		piSpouse = &m_iWife;
		iOtherSpouse = m_iHusband;
		strTag = "WIFE";
	}
	else
	{
		piSpouse = &m_iHusband;
		iOtherSpouse = m_iWife;
		strTag = "HUSB";
	}

	if (*piSpouse<0)
	{
		*piSpouse = pIndi->m_i;
		m_pDoc->ResetSubValue(m_hTreeItem,strTag,pIndi->GetID());

		CIndividual& indi = m_pDoc->m_rIndividual[*piSpouse];
		indi.AddSpouse(iOtherSpouse,TRUE);
		indi.AddSpouseToFamily(m_i,TRUE);

		for (int i(0); i<m_riChild.GetSize(); i++)
		{
			indi.AddChild(m_riChild[i],TRUE);
			if (bWife)
				m_pDoc->m_rIndividual[m_riChild[i]].m_iMother = *piSpouse;
			else
				m_pDoc->m_rIndividual[m_riChild[i]].m_iFather = *piSpouse;
		}
		if (iOtherSpouse>=0)
			m_pDoc->m_rIndividual[iOtherSpouse].AddSpouse(*piSpouse,TRUE);
	}
	else
	{
		ASSERT(*piSpouse==pIndi->m_i);
	}
}

void CFamily::ConnectChild(CIndividual* pIndi)
{
	if (AddChild(pIndi->m_i,TRUE))
		m_pDoc->ResetSubValue(m_hTreeItem,"CHIL",pIndi->GetID(),TRUE);

	for (int i(0); i<m_riChild.GetSize(); i++)
	{
		if (m_iWife>=0)
			m_pDoc->m_rIndividual[m_iWife].AddChild(m_riChild[i],TRUE);
		if (m_iHusband>=0)
			m_pDoc->m_rIndividual[m_iHusband].AddChild(m_riChild[i],TRUE);
		CIndividual& indi = m_pDoc->m_rIndividual[m_riChild[i]];
		indi.m_iFather = m_iHusband;
		indi.m_iMother = m_iWife;
		indi.m_iChildToFamily = m_i;
		m_pDoc->ResetSubValue(indi.m_hTreeItem,"FAMC",GetID(),FALSE);
	}
}

void CFamily::DebugRelations()
{
	return;
	CString s, s2;

	s.Format(_T("f==%d: "),m_i);
	if (m_iHusband>=0)
	{
		s2.Format(_T("h==%d "),m_iHusband);
		s += s2;
	}
	if (m_iWife>=0)
	{
		s2.Format(_T("w==%d "),m_iWife);
		s += s2;
	}
	s += "\n";
	OutputDebugString(s);

	for (int i(0); i<m_riChild.GetSize(); i++)
	{
		s.Format(_T("   c==%d\n"),m_riChild[i]);
		OutputDebugString(s);
	}
}

int CFamily::GetSimpleMarriage()
{
	int d(99999999);

	// get date of (earliest) MARR event in the partnership
	for (int i(0); i<m_revt.GetSize(); i++)
	{
		CEvt& evt = m_revt[i];
		if (evt.m_dvDate.GetSimpleYMD()<d && evt.m_strTypeTok == "MARR")
			d = evt.m_dvDate.GetSimpleYMD();
	}

	return d==99999999?0:d;
}

int CFamily::GetSimpleMarriageEnd()
{
	int d(99999999);

	// get date of (earliest) DIV or ANUL event in the partnership
	for (int i(0); i<m_revt.GetSize(); i++)
	{
		CEvt& evt = m_revt[i];
		if (evt.m_dvDate.GetSimpleYMD()<d && (evt.m_strTypeTok == "DIV" || evt.m_strTypeTok == "ANUL"))
			d = evt.m_dvDate.GetSimpleYMD();
	}

	if (d==99999999)
	{
		// no div, so get earlier of husb death or wife death
		if (m_iHusband >= 0 || m_iWife >= 0)
		{
			if (m_iHusband >= 0)
			{
				CIndividual& indi = *m_pDoc->Individual(m_iHusband);
				int nBirth(0), nDeath(0);
				if (indi.m_iBirth>=0)
					nBirth = indi.m_revt[indi.m_iBirth].m_dvDate.GetSimpleYMD();
				if (indi.m_iDeath>=0)
					nDeath = indi.m_revt[indi.m_iDeath].m_dvDate.GetSimpleYMD();

				if (nBirth||nDeath)
				{
					// 60 years est. life if death date missing
					if (!nDeath)
						nDeath = nBirth+600000;
					d = nDeath;
				}
			}
			if (m_iWife >= 0)
			{
				CIndividual& indi = *m_pDoc->Individual(m_iWife);
				int nBirth(0), nDeath(0);
				if (indi.m_iBirth>=0)
					nBirth = indi.m_revt[indi.m_iBirth].m_dvDate.GetSimpleYMD();
				if (indi.m_iDeath>=0)
					nDeath = indi.m_revt[indi.m_iDeath].m_dvDate.GetSimpleYMD();

				if (nBirth||nDeath)
				{
					// 60 years est. life if death date missing
					if (!nDeath)
						nDeath = nBirth+600000;
					if (nDeath < d)
					{
						d = nDeath;
					}
				}
			}

			if (d==99999999)
			{
				// no death dates of spouses, so use marriage length est.
				int mar = GetSimpleMarriage();
				if (mar >= 0)
				{
					// assume 40 years est. marriage length
					d = mar+400000;
				}
				else
				{
					// no idea
					d = 0;
				}
			}
		}
	}
	return d;
}

void CFamily::GetSortedChildren(CArray<int,int>& riChild)
{
	riChild.SetSize(m_riChild.GetSize());
	for (int i(0); i<m_riChild.GetSize(); i++)
	{
		riChild[i] = m_riChild[i];
	}
	for (i = 0; i<riChild.GetSize()-1; i++)
	{
		for (int j(i+1); j<riChild.GetSize(); j++)
		{
			CIndividual& indi1 = m_pDoc->m_rIndividual[riChild[i]];
			CIndividual& indi2 = m_pDoc->m_rIndividual[riChild[j]];
			int d1 = indi1.GetSimpleBirth();
			int d2 = indi2.GetSimpleBirth();
			if (d2<d1)
			{
				int t = riChild[i];
				riChild[i] = riChild[j];
				riChild[j] = t;
			}
		}
	}
}

static inline basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl endl2(basic_ostream<wchar_t, char_traits<wchar_t> >& _O)
{
	_O.put(L'\r');
	_O.put(L'\n');
	_O.flush();
	return (_O);
}
/*
void static svgLine(const CPoint& p1, const CPoint& p2, wostringstream& os)
{
	CString s;
	s.Format(L"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" />",
		p1.x, p1.y, p2.x, p2.y);
	os << (LPCTSTR)s << endl2;
}

void CFamily::writeSVG(wostringstream& os)
{
	if (!Exists()) return;

	if (!(m_iHusband>=0||m_iWife>=0||m_riChild.GetSize()>0)) return;

	if (m_pt1.x||m_pt1.y||m_pt2.x||m_pt2.y)
	{
		CPoint p1(m_ptsc1);
		CPoint p2(m_ptsc2);
		int nBarHeight(BAR_HEIGHT/GetScale());
		svgLine(CPoint(p1.x,p1.y-nBarHeight/2),CPoint(p2.x,p2.y-nBarHeight/2),os);
		svgLine(CPoint(p1.x,p1.y+nBarHeight/2),CPoint(p2.x,p2.y+nBarHeight/2),os);
	}

	if (m_riChild.GetSize())
	{
		CPoint p1(m_ptscC1);
		CPoint p2(m_ptscC2);

		svgLine(p1,p2,os);
		for (int i(0); i<m_rpointscChild.GetSize(); i++)
		{
			CPoint pt(m_rpointscChild[i]);
			CPoint pt2(pt.x,m_ptscC1.y);
			svgLine(pt2,pt,os);
		}
		if (m_pt1.x||m_pt1.y||m_pt2.x||m_pt2.y)
		{
			CPoint pt(m_ptscP);
			if (p1.x<pt.x && pt.x<p2.x)
			{
				svgLine(pt,CPoint(pt.x,p1.y),os);
			}
			else
			{
				int nChildHeight(CHILD_HEIGHT/GetScale());
				svgLine(pt,CPoint(pt.x,p1.y-nChildHeight),os);
				svgLine(CPoint(pt.x,p1.y-nChildHeight),CPoint((p1.x+p2.x)/2,p1.y-nChildHeight),os);
				svgLine(CPoint((p1.x+p2.x)/2,p1.y-nChildHeight),CPoint((p1.x+p2.x)/2,p1.y),os);
			}
		}
	}
}
*/
void CFamily::writeApplet(std::wostream& os)
{
	if (m_iHusband >= 0)
		os << (LPCTSTR)m_pDoc->m_rIndividual[m_iHusband].m_strID;
	os << L",";

	if (m_iWife >= 0)
		os << (LPCTSTR)m_pDoc->m_rIndividual[m_iWife].m_strID;
	os << L",";

	os << m_riChild.GetSize();
	for (int i(0); i < m_riChild.GetSize(); ++i)
		os << L"," << (LPCTSTR)m_pDoc->m_rIndividual[m_riChild[i]].m_strID;

	os << endl2;
}


void CFamily::showAsOf(CDate* pasof)
{
	m_bHidden = true;
	for (int i(0); i<m_riChild.GetSize(); ++i)
	{
		CIndividual& child = m_pDoc->m_rIndividual[m_riChild[i]];
		if (!child.m_bHidden)
		{
			m_bHidden = false;
			return;
		}
	}
	int d = pasof->GetSimpleYMD();
	int mar = GetSimpleMarriage();
	int div = GetSimpleMarriageEnd();
	if (0 < mar && mar <= d && d <= div)
	{
		m_bHidden = false;
	}
}
