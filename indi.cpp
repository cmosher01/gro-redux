#include "stdafx.h"
#include "indi.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "childfrm.h"
#include "gedtreeviewin.h"
#include "gedtreeviewpd.h"
#include "gedline.h"
#include "gedrecord.h"
#include "mydc.h"
#include <set>
#include <queue>
#include <list>
#include <sstream>

const CSize MIN_INDI_SIZE3D(8,8);
const CSize MIN_INDI_SIZE(6,4);

const UINT nDrawFormat(DT_WORDBREAK|DT_NOCLIP|DT_NOPREFIX);

CIndividual::CIndividual(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem),
	m_nScale(1),
	m_bSelected(FALSE),
	m_bHidden(FALSE),
	m_iFather(-1),
	m_iMother(-1),
	m_iChildToFamily(-1),
	m_bMark(FALSE),
	m_pFrame(NULL),
	m_pPedigree(NULL),
	m_nGrid(1),
	m_name(pDoc,NULL),
	m_nSex(0),
	m_nLevel(0x7fffffff),
	m_maxmale(0),
	m_pMale(NULL)
{
}

CIndividual::~CIndividual()
{
}

CIndividual& CIndividual::operator=(const CIndividual& o)
{
	CGedRecord::operator=(o);
	m_name = o.m_name;
	m_nSex = o.m_nSex;
	m_iBirth = o.m_iBirth;
	m_iDeath = o.m_iDeath;
	m_strTreeDisplay = o.m_strTreeDisplay;
	m_rectFrame = o.m_rectFrame;
	m_rectText = o.m_rectText;
	m_nScale = o.m_nScale;
	m_rectScaledFrame = o.m_rectScaledFrame;
	m_rectScaledFrameDraw = o.m_rectScaledFrameDraw;
	m_rectScaledText = o.m_rectScaledText;
	m_bSelected = o.m_bSelected;
	m_bHidden = o.m_bHidden;
	m_bMark = o.m_bMark;
	m_iFather = o.m_iFather;
	m_iMother = o.m_iMother;
	m_iChildToFamily = o.m_iChildToFamily;
	m_pFrame = o.m_pFrame;
	m_pPedigree = o.m_pPedigree;
	m_nGrid = o.m_nGrid;
	m_nLevel = o.m_nLevel;

	m_revt.Copy(o.m_revt);
	m_rattr.Copy(o.m_rattr);
	m_riSpouse.Copy(o.m_riSpouse);
	m_riChild.Copy(o.m_riChild);
	m_riSpouseToFamily.Copy(o.m_riSpouseToFamily);

	return *this;
}

CIndividual::CIndividual(const CIndividual& o):
	m_name(o.m_name),
	m_nSex(o.m_nSex),
	m_iBirth(o.m_iBirth),
	m_iDeath(o.m_iDeath),
	m_strTreeDisplay(o.m_strTreeDisplay),
	m_rectFrame(o.m_rectFrame),
	m_rectText(o.m_rectText),
	m_nScale(o.m_nScale),
	m_rectScaledFrame(o.m_rectScaledFrame),
	m_rectScaledFrameDraw(o.m_rectScaledFrameDraw),
	m_rectScaledText(o.m_rectScaledText),
	m_bSelected(o.m_bSelected),
	m_bHidden(o.m_bHidden),
	m_bMark(o.m_bMark),
	m_iFather(o.m_iFather),
	m_iMother(o.m_iMother),
	m_iChildToFamily(o.m_iChildToFamily),
	m_pFrame(o.m_pFrame),
	m_pPedigree(o.m_pPedigree),
	m_nGrid(o.m_nGrid),
	m_nLevel(o.m_nLevel)
{
	m_revt.Copy(o.m_revt);
	m_rattr.Copy(o.m_rattr);
	m_riSpouse.Copy(o.m_riSpouse);
	m_riChild.Copy(o.m_riChild);
	m_riSpouseToFamily.Copy(o.m_riSpouseToFamily);
}

CString CIndividual::Name()
{
	return m_name.Name();
}

BOOL CIndividual::AddSpouse(int iSpouse, BOOL bCheckFirst)
{
	if (iSpouse<0) return FALSE;

	BOOL bAdd(TRUE);
	if (bCheckFirst)
	{
		int i(0);
		while (i<m_riSpouse.GetSize()&&bAdd)
			if (m_riSpouse[i++]==iSpouse)
				bAdd = FALSE;
	}
	if (bAdd)
		m_riSpouse.Add(iSpouse);

	return bAdd;
}

BOOL CIndividual::AddSpouseToFamily(int iSpouseToFamily, BOOL bCheckFirst)
{
	if (iSpouseToFamily<0) return FALSE;

	BOOL bAdd(TRUE);
	if (bCheckFirst)
	{
		int i(0);
		while (i<m_riSpouseToFamily.GetSize()&&bAdd)
			if (m_riSpouseToFamily[i++]==iSpouseToFamily)
				bAdd = FALSE;
	}
	if (bAdd)
	{
		m_riSpouseToFamily.Add(iSpouseToFamily);
		CFamily& fami = m_pDoc->m_rFamily[iSpouseToFamily];
		fami.CalcID();
		m_pDoc->ResetSubValue(m_hTreeItem,"FAMS",fami.GetID(),TRUE);
	}

	return bAdd;
}

BOOL CIndividual::AddChild(int iChild, BOOL bCheckFirst)
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

void CIndividual::PlaceAt(const CPoint& point)
{
	CPoint pointText = point;
	pointText.x += theApp.m_nIndiBorderX;
	pointText.y += theApp.m_nIndiBorderY;
	m_rectText = CRect(pointText,CSize(theApp.m_nMaxIndiWidth,0));
}

void CIndividual::ResetWidth()
{
	m_rectText = CRect(m_rectText.TopLeft(),CSize(theApp.m_nMaxIndiWidth,0));
}

void CIndividual::CalcFull()
{
	ResetWidth();
	Calc();
	CalcFamilies();
	m_pDoc->ResetAllViews();
}

/**
 *	Replace these characters, for use in XML documents:
 *	&lt; <  less than 
 *	&gt; >  greater than 
 *	&amp;  & ampersand  
 *	&apos; '  apostrophe 
 *	&quot; " quotation mark 
static void FixXML(CString& str)
{
	str.Replace(L"&",L"&amp;");
	str.Replace(L"<",L"&lt;");
	str.Replace(L">",L"&gt;");
	str.Replace(L"\'",L"&apos;");
	str.Replace(L"\"",L"&quot;");
}
 */

void CIndividual::Calc()
{
	ASSERT(Exists());

	m_strTreeDisplay.Empty();

	m_strTreeDisplay += Name();


//TEST BEGIN
//	m_strTreeDisplay.Format("lev:%d\nseq:%f\npat:%d\n",m_nLevel,m_seq,m_maxmale);
//	m_strTreeDisplay.Format("M:%d\n",m_maxmale);
/*
	m_strTreeDisplay += "\n----------------------\n";
	CString smaxm;
	smaxm.Format(L"maxmale:%d\n",m_maxmale);
	m_strTreeDisplay += smaxm;
	if (!m_bMark)
		m_strTreeDisplay += "NOT MARKED\n";
	if (m_pMale)
	{
		m_strTreeDisplay += "[from ";
		m_strTreeDisplay += m_pMale->m_name.m_strDisplay;
		m_strTreeDisplay += "]";
	}
*/
//TEST END



	MyRect r(m_rectText);
	r += -r.TopLeft();
	CRect rrr = r;

	theApp.ResetFont();
/*
	m_strEllipsisDisplay = m_strTreeDisplay;

	TEXTMETRIC textmet;
	theApp.GetDC()->GetTextMetrics(&textmet);
	if (textmet.tmPitchAndFamily&TMPF_TRUETYPE)
	{
		int elen = m_strTreeDisplay.GetLength()+4;
		LPTSTR sElip = m_strEllipsisDisplay.GetBuffer(elen);
		int tw(0);
		for (int i(0); i<elen-4; ++i)
		{
			int c = sElip[i];
			ABC abc;
			BOOL bOK = ::GetCharABCWidths(theApp.GetDC()->m_hDC,c,c,&abc);
			if (!bOK)
				break;
			int w = abc.abcA+abc.abcB+abc.abcC;
			tw += w;
			if (tw >= rrr.Width()+5)
			{
				if (i >= 3)
				{
					sElip[i-3] = '.';
					sElip[i-2] = '.';
					sElip[i-1] = '.';
					sElip[i] = '\0';
				}
				break;
			}
		}
		m_strEllipsisDisplay.ReleaseBuffer();
	}
	else
	{
		int elen = m_strTreeDisplay.GetLength()+4;
		LPTSTR sElip = m_strEllipsisDisplay.GetBuffer(elen);
		int tw(0);
		for (int i(0); i<elen-4; ++i)
		{
			int c = sElip[i];
			int w(0);
			BOOL bOK = ::GetCharWidth32(theApp.GetDC()->m_hDC,c,c,&w);
			if (!bOK)
				break;
			tw += w;
			if (tw >= rrr.Width()+5)
			{
				if (i >= 3)
				{
					sElip[i-3] = '.';
					sElip[i-2] = '.';
					sElip[i-1] = '.';
					sElip[i] = '\0';
				}
				break;
			}
		}
		m_strEllipsisDisplay.ReleaseBuffer();
	}
*/
	m_iBirth = -1;
	m_iDeath = -1;

	for (int i(0); i<m_revt.GetSize(); i++)
	{
		CEvt& evt = m_revt[i];
		evt.Calc();
		if (evt.m_strTypeTok=="BIRT" && m_iBirth<0)
			m_iBirth = i;
		if (evt.m_strTypeTok=="DEAT" && m_iDeath<0)
			m_iDeath = i;
	}

	if (m_iBirth>=0)
	{
		CEvt& evt = m_revt[m_iBirth];
		CString strDate = evt.m_dvDate.Display(DATE_SHORTDATE);
		if (strDate.IsEmpty())
		{
			m_strBirthDisplay.Empty();
		}
		else
		{
			m_strBirthDisplay = "b: "+strDate;
			m_strTreeDisplay += "\n";
			m_strTreeDisplay += m_strBirthDisplay;
		}
	}
	if (m_iDeath>=0)
	{
		CEvt& evt = m_revt[m_iDeath];
		CString strDate = evt.m_dvDate.Display(DATE_SHORTDATE);
		if (strDate.IsEmpty())
		{
			m_strDeathDisplay.Empty();
		}
		else
		{
			m_strDeathDisplay = "d: "+strDate;
			m_strTreeDisplay += "\n";
			m_strTreeDisplay += m_strDeathDisplay;
		}
	}

	for (i = 0; i<m_rattr.GetSize(); i++)
	{
		CAttr& attr = m_rattr[i];
		attr.Calc();
	}

	theApp.GetDC()->DrawText(m_strTreeDisplay,&rrr,DT_CALCRECT|nDrawFormat);
	r = rrr;
	m_rectText.top *= theApp.m_fontRescale;
	m_rectText.left *= theApp.m_fontRescale;
	m_rectText.right = m_rectText.left+r.Width();
	m_rectText.bottom = m_rectText.top+r.Height();
	m_rectFrame = m_rectText;
	m_rectFrame.InflateRect(theApp.m_nIndiBorderX,theApp.m_nIndiBorderY);
/*
	m_strBirthXML = m_strBirthDisplay;
	m_strDeathXML = m_strDeathDisplay;
	FixXML(m_strEllipsisDisplay);
	FixXML(m_strBirthXML);
	FixXML(m_strDeathXML);
*/
	CalcScale();
	m_pDoc->clearAppletChart();
}

void CIndividual::GridRect(CRect& rect)
{
	rect.top = rect.top/m_nGrid*m_nGrid;
	rect.left = rect.left/m_nGrid*m_nGrid;
	rect.bottom = (rect.bottom+m_nGrid-1)/m_nGrid*m_nGrid;
	rect.right = (rect.right+m_nGrid-1)/m_nGrid*m_nGrid;
}

void CIndividual::MoveTo(const CPoint& point)
{
	Shift(point-m_rectFrame.TopLeft());
}

void CIndividual::Shift(const CSize& sizShift)
{
	double scale = GetScale();
	ShiftUnscaled(CSize(sizShift.cx*scale,sizShift.cy*scale));
}

void CIndividual::ShiftUnscaled(const CSize& sizShift)
{
	m_rectFrame += sizShift;
	m_rectText += sizShift;
	CalcScale();
	CalcFamilies();
	m_pDoc->ResetSubValue(m_hTreeItem,"_XY",GetXY(m_rectFrame.TopLeft()));
	m_pDoc->clearAppletChart();
}

int CIndividual::SetScale(int nScale)
{
	int nOldScale = m_nScale;
	m_nScale = nScale;
	CalcScale();
	return nOldScale;
}

double CIndividual::GetScale()
{
	return (double)m_nScale;
}

void CIndividual::CalcScale()
{
	double scale = GetScale();
	m_rectScaledFrame.top = m_rectFrame.top/scale;
	m_rectScaledFrame.left = m_rectFrame.left/scale;
	m_rectScaledFrame.bottom = m_rectFrame.bottom/scale;
	m_rectScaledFrame.right = m_rectFrame.right/scale;
	m_rectScaledText.top = m_rectText.top/scale;
	m_rectScaledText.left = m_rectText.left/scale;
	m_rectScaledText.bottom = m_rectText.bottom/scale;
	m_rectScaledText.right = m_rectText.right/scale;
	if (m_rectScaledFrame.Width()<MIN_INDI_SIZE.cx)
		m_rectScaledFrame.right = m_rectScaledFrame.left+MIN_INDI_SIZE.cx;
	if (m_rectScaledFrame.Height()<MIN_INDI_SIZE.cy)
		m_rectScaledFrame.bottom = m_rectScaledFrame.top+MIN_INDI_SIZE.cy;
	m_rectScaledFrameDraw.top = m_rectScaledFrame.top-1;
	m_rectScaledFrameDraw.left = m_rectScaledFrame.left-1;
	m_rectScaledFrameDraw.bottom = m_rectScaledFrame.bottom+1;
	m_rectScaledFrameDraw.right = m_rectScaledFrame.right+1;
}

void CIndividual::Disconnect()
{
	if (m_iChildToFamily>=0)
	{
		m_pDoc->Family(m_iChildToFamily)->Disconnect(m_i);
		m_iChildToFamily = -1;
	}
	m_pDoc->DeleteSubValue(m_hTreeItem,"FAMC");
	for (int i(0); i<m_riSpouseToFamily.GetSize(); i++)
	{
		m_pDoc->Family(m_riSpouseToFamily[i])->Disconnect(m_i);
	}
	m_pDoc->DeleteSubValue(m_hTreeItem,"FAMS");
	m_riSpouseToFamily.RemoveAll();
	m_riSpouse.RemoveAll();
	m_riChild.RemoveAll();
	m_iFather = -1;
	m_iMother = -1;
	m_pDoc->ResetAllViews();
}

void CIndividual::CalcFamilies()
{
	if (m_iChildToFamily>=0)
		m_pDoc->Family(m_iChildToFamily)->CalcDraw();
	for (int i(0); i<m_riSpouseToFamily.GetSize(); i++)
		m_pDoc->Family(m_riSpouseToFamily[i])->CalcDraw();
}

void CIndividual::OnDraw(CMyDC& dc)
{
	if (!Exists()) return;
	//if (m_bHidden) return;

	CRect rectScaledFrame(m_rectScaledFrame);
	CRect rectScaledText(m_rectScaledText);

/*
	if (m_nScale==1)
	{
		GridRect(rectScaledFrame);
		GridRect(rectScaledText);
	}
*/

	if (dc.Print())
	{
		ScaleForPrint(rectScaledFrame);
		ScaleForPrint(rectScaledText);
		dc.DrawRectSel(rectScaledFrame,false);
		dc.FrameRect(rectScaledFrame);
		dc.DrawText(m_strTreeDisplay,rectScaledText);
	}
	else
	{
		if (rectScaledFrame.Width()<=MIN_INDI_SIZE3D.cx||
			rectScaledFrame.Height()<=MIN_INDI_SIZE3D.cy)
		{
			// if rect is too small to show details, don't draw text or 3D outline
			dc.DrawRectSel(rectScaledFrame,!!m_bSelected);
			dc.FrameRect(rectScaledFrame,!!m_bHidden);
		}
		else
		{
			// draw the background rectangle (highlighted if selected)
			dc.DrawRectSel(rectScaledFrame,!!m_bSelected);

			// draw the text (use highlight color if selected) unless zoomed-out
			if (m_nScale==1)
				dc.DrawTextSel(m_strTreeDisplay,rectScaledText,!!m_bSelected,!!m_bHidden);

			// draw the 3D rectangle edge
			dc.FrameRect3D(rectScaledFrame,!!m_bHidden);
		}
	}
}

void CIndividual::ScaleForPrint(CRect& rect)
{
	int nFactor(4);
	rect.top *= nFactor;
	rect.left *= nFactor;
	rect.bottom *= nFactor;
	rect.right *= nFactor;
}

CRect CIndividual::GetFrameRect()
{
	return m_rectScaledFrame;
}

CRect CIndividual::GetUnscaledFrameRect()
{
	return m_rectFrame;
}

BOOL CIndividual::HitTest(const CPoint& point)
{
	return m_rectScaledFrame.PtInRect(point);
}

BOOL CIndividual::HitTest(const CRect& rect)
{
	return m_rectScaledFrame.RectInRect(rect);
}

BOOL CIndividual::Select(BOOL bSelect)
{
	BOOL bWas = m_bSelected;
	m_bSelected = bSelect;
	return bWas;
}

BOOL CIndividual::MoveToIsInBounds(CPoint& pt)
{
	CSize shift = pt-m_rectFrame.TopLeft();
	shift.cx *= GetScale();
	shift.cy *= GetScale();
	MyRect r = m_rectFrame;
	r += shift;

	CRect rectBounds(CPoint(0,0),CPoint(MAX_BOUNDS-CSize(VIEW_BORDER,VIEW_BORDER)));// = m_pDoc->GetUnscaledBoundsNoBorder();

	CSize szUnshift(0,0);
	if (r.top<rectBounds.top)
		szUnshift.cy = rectBounds.top-r.top;
	if (r.bottom>rectBounds.bottom)
		szUnshift.cy = rectBounds.bottom-r.bottom;

	if (r.left<rectBounds.left)
		szUnshift.cx = rectBounds.left-r.left;
	if (r.right>rectBounds.right)
		szUnshift.cx = rectBounds.right-r.right;

	szUnshift.cx /= GetScale();
	szUnshift.cy /= GetScale();

	BOOL bOut = szUnshift.cx||szUnshift.cy;
	if (bOut)
		pt += szUnshift;

	return !bOut;
}

void CIndividual::Clean(int nDepth)
{
	static const int MARRIAGE_SPACING(20);
	static const int CHILD_VSPACING(50);
	static const int CHILD_HSPACING(155);
	CPoint pt;
	int cTry;

	if (nDepth>1 && m_iFather>=0)
	{
		CIndividual* pFather = m_pDoc->Individual(m_iFather);
		if (!pFather->m_bMark)
		{
			pt = CPoint(m_rectFrame.left,m_rectFrame.top-CHILD_VSPACING);
			int nOldScale = pFather->SetScale(1);
			pFather->MoveToIsInBounds(pt);
			pFather->MoveTo(pt);
			pFather->SetScale(nOldScale);
			cTry = 0;
			while (m_pDoc->HitsIndi(pFather) && ++cTry<5)
			{
				pt -= CSize(30,5);
				int nOldScale = pFather->SetScale(1);
				pFather->MoveToIsInBounds(pt);
				pFather->MoveTo(pt);
				pFather->SetScale(nOldScale);
			}
			pFather->m_bMark = TRUE;
			pFather->Clean(nDepth-1);
		}
	}

	m_bMark = TRUE;

	pt = CPoint(
		m_rectFrame.right+MARRIAGE_SPACING,
		(m_rectFrame.top+m_rectFrame.bottom)/2+CHILD_VSPACING/2);
	CPoint pt2 = CPoint(
		m_rectFrame.left-MARRIAGE_SPACING,
		(m_rectFrame.top+m_rectFrame.bottom)/2);
	for (int i(0); i<m_riSpouse.GetSize(); i++)
	{
		if (m_riSpouse[i]>=0)
		{
			CIndividual* pSpouse = m_pDoc->Individual(m_riSpouse[i]);
			if (!pSpouse->m_bMark)
			{
				CRect rectSpouse = pSpouse->GetFrameRect();
				CPoint ptTopLeft = (i%2) ? pt : pt2;
				ptTopLeft = CPoint(ptTopLeft.x,ptTopLeft.y-(rectSpouse.bottom-rectSpouse.top)/2);
				if (!(i%2)) ptTopLeft.x -= rectSpouse.Width();
				int nOldScale = pSpouse->SetScale(1);
				pSpouse->MoveToIsInBounds(pt);
				pSpouse->MoveTo(ptTopLeft);
				pSpouse->m_bMark = TRUE;
				pSpouse->SetScale(nOldScale);
				if (nDepth>1)
				{
					pSpouse->Clean(nDepth-2);
				}
				if (i%2)
					pt += CSize(MARRIAGE_SPACING,CHILD_VSPACING/*/2*/);
				else
					pt2 -= CSize(MARRIAGE_SPACING,-CHILD_VSPACING/*/2*/);
			}
		}
	}

	if (nDepth<1) return;

	pt = CPoint(
		m_rectFrame.left+MARRIAGE_SPACING-((m_riChild.GetSize()-1)*CHILD_HSPACING)/2,
		m_rectFrame.bottom+CHILD_VSPACING);
	for (i = 0; i<m_riChild.GetSize(); i++)
	{
		CIndividual* pChild = m_pDoc->Individual(m_riChild[i]);
		if (!pChild->m_bMark)
		{
			int nOldScale = pChild->SetScale(1);
			pChild->MoveToIsInBounds(pt);
			pChild->MoveTo(pt);
			pChild->SetScale(nOldScale);
			if (i==0) // see if first child overlaps
			{
				cTry = 0;
				while (m_pDoc->HitsIndi(pChild) && ++cTry<5)
				{
					pt += CSize(30,5);
					int nOldScale = pChild->SetScale(1);
					pChild->MoveToIsInBounds(pt);
					pChild->MoveTo(pt);
					pChild->SetScale(nOldScale);
				}
			}
			pChild->m_bMark = TRUE;
			if (nDepth>0)
			{
				pChild->Clean(nDepth-1);
			}
			pt.x += CHILD_HSPACING;
		}
	}
}

void CIndividual::OpenView()
{
	if (m_pFrame)
	{
		m_pFrame->ActivateFrame(SW_SHOW);
	}
	else
	{
		POSITION pos = theApp.GetFirstDocTemplatePosition();
		CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
		p = theApp.GetNextDocTemplate(pos); // IN tmpl
		m_pFrame = (CChildFrame*)p->CreateNewFrame(m_pDoc,NULL);
		p->InitialUpdateFrame(m_pFrame,m_pDoc);
		((CGedtreeViewIN*)m_pFrame->GetActiveView())->Init(m_i);
		p->InitialUpdateFrame(m_pFrame,m_pDoc);
	}
}

void CIndividual::OpenPedigree()
{
	if (m_pPedigree)
	{
		m_pPedigree->ActivateFrame(SW_SHOW);
	}
	else
	{
		POSITION pos = theApp.GetFirstDocTemplatePosition();
		CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
		p = theApp.GetNextDocTemplate(pos); // IN tmpl
		p = theApp.GetNextDocTemplate(pos); // PD tmpl
		m_pPedigree = (CChildFrame*)p->CreateNewFrame(m_pDoc,NULL);
		p->InitialUpdateFrame(m_pPedigree,m_pDoc);
		((CGedtreeViewPD*)m_pPedigree->GetActiveView())->Init(m_i);
		p->InitialUpdateFrame(m_pPedigree,m_pDoc);
	}
}

void CIndividual::AddEvent(HTREEITEM htiEvent, const CString& strTypeTok)
{
	CEvt evt(m_pDoc,htiEvent);
	evt.GetFromTree(strTypeTok);
	m_revt.Add(evt);
}

void CIndividual::AddAttr(HTREEITEM htiAttr, const CString& strTypeTok, const CString& strValue)
{
	CAttr attr(m_pDoc,htiAttr);
	attr.GetFromTree(strTypeTok,strValue);
	m_rattr.Add(attr);
}

struct tSchemaItem
{
	CString sType;
	HTREEITEM hti;
};
BOOL CIndividual::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;
	BOOL bXY(FALSE);

	CString strNote;
	CArray<HTREEITEM,HTREEITEM> rhtiNote;
	CArray<tSchemaItem,tSchemaItem&> rSchema;
	CString strLabel;

	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		HTREEITEM htiNext = tree.GetNextSiblingItem(htiSub);

		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);
		if (pglChild->m_strTok=="NAME")
		{
			m_name.Set(m_pDoc,htiSub);
			m_name.GetFromTree(pglChild->m_strVal);
		}
		else if (pglChild->m_strTok=="_XY")
		{
			bXY = TRUE;
			PlaceAt(GetXY(pglChild->m_strVal));
		}
		else if (pglChild->m_strTok=="FAMS")
			m_pDoc->DeleteItem(htiSub);
		else if (pglChild->m_strTok=="FAMC")
			m_pDoc->DeleteItem(htiSub);
		else if (
			pglChild->m_strTok=="EVEN"||
			pglChild->m_strTok=="BIRT"||
			pglChild->m_strTok=="DEAT"||
			pglChild->m_strTok=="CHR"||
			pglChild->m_strTok=="BURI"||
			pglChild->m_strTok=="CREM"||
			pglChild->m_strTok=="ADOP"||
			pglChild->m_strTok=="BAPM"||
			pglChild->m_strTok=="BARM"||
			pglChild->m_strTok=="BASM"||
			pglChild->m_strTok=="BLES"||
			pglChild->m_strTok=="CHRA"||
			pglChild->m_strTok=="CONF"||
			pglChild->m_strTok=="FCOM"||
			pglChild->m_strTok=="ORDN"||
			pglChild->m_strTok=="NATU"||
			pglChild->m_strTok=="EMIG"||
			pglChild->m_strTok=="IMMI"||
			pglChild->m_strTok=="CENS"||
			pglChild->m_strTok=="PROB"||
			pglChild->m_strTok=="WILL"||
			pglChild->m_strTok=="GRAD"||
			pglChild->m_strTok=="RETI"||
			pglChild->m_strTok=="RESI"||
			FALSE)
		{
			AddEvent(htiSub,pglChild->m_strTok);
		}
		else if (
			pglChild->m_strTok=="CAST"||
			pglChild->m_strTok=="DSCR"||
			pglChild->m_strTok=="EDUC"||
			pglChild->m_strTok=="IDNO"||
			pglChild->m_strTok=="NATI"||
			pglChild->m_strTok=="NCHI"||
			pglChild->m_strTok=="NMR"||
			pglChild->m_strTok=="OCCU"||
			pglChild->m_strTok=="PROP"||
			pglChild->m_strTok=="RELI"||
			pglChild->m_strTok=="SSN"||
			pglChild->m_strTok=="TITL"||
			FALSE)
		{
			AddAttr(htiSub,pglChild->m_strTok,pglChild->m_strVal);
		}
		else if (pglChild->m_strTok=="SEX")
		{
			CString strSex = pglChild->GetCleanValue(CGedLine::COLLAPSE|CGedLine::UPCASE);
			if (strSex=="M")
				m_nSex = 1;
			else if (strSex=="F")
				m_nSex = 2;
		}
		else if (pglChild->m_strTok=="NOTE")
		{
			strNote += pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,strNote);
			rhtiNote.Add(htiSub);
		}
		else if (m_pDoc->m_mapschemaIndiTagToLabel.Lookup(pglChild->m_strTok,strLabel))
		{
			tSchemaItem sci;
			sci.sType = strLabel;
			sci.hti = htiSub;
			rSchema.Add(sci);
		}
		htiSub = htiNext;
	}

	if (!strNote.IsEmpty())
	{
		HTREEITEM hti = m_pDoc->InsertChild(_T("EVEN"),m_hTreeItem,1);
		CEvt evt(m_pDoc,hti);
		evt.m_strNote = strNote;
		evt.m_strType = "note";
		evt.m_strTypeTok = "EVEN";
		evt.PutToTree();
		m_revt.Add(evt);
		for (int i(0); i<rhtiNote.GetSize(); i++)
			m_pDoc->DeleteItem(rhtiNote[i]);
	}

	for (int i(0); i < rSchema.GetSize(); ++i)
	{
		tSchemaItem& sci = rSchema[i];
		HTREEITEM hti = m_pDoc->InsertChild(_T("EVEN"),m_hTreeItem,1);
		CEvt evt(m_pDoc,hti);

		CString sNote;
		HTREEITEM htiSub = tree.GetChildItem(sci.hti);
		while (htiSub && !sNote.GetLength())
		{
			CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);
			if (pglChild->m_strTok=="PLAC")
			{
				sNote = pglChild->m_strVal;
				m_pDoc->DeleteItem(htiSub);
			}
			htiSub = tree.GetNextSiblingItem(htiSub);
		}

		evt.m_strNote = sNote;
		evt.m_strType = sci.sType;
		evt.m_strTypeTok = "EVEN";
		evt.m_hTreeItem = sci.hti;
		evt.GetFromTree("EVEN");
		evt.m_hTreeItem = hti;
		evt.PutToTree();
		m_revt.Add(evt);

		m_pDoc->DeleteItem(sci.hti);
	}

	if (!bXY)
		PlaceAt(m_pDoc->GetNextPos());

	return bXY;
}

CPoint CIndividual::GetXY(const CString& str)
{
	CPoint pt(0,0);
	CString s(str);
	int i = s.Find(cDelim);
	if (i>=0)
	{
		pt.x = _ttoi(s.Left(i));
		pt.y = _ttoi(s.Mid(i+1));
	}
	return pt;
}

CString CIndividual::GetXY(const CPoint& pt)
{
	CString str;
	str.Format(_T("%d %d"),pt.x,pt.y);
	return str;
}

void CIndividual::SetSex(int nSex)
{
	if (m_riSpouseToFamily.GetSize())
	{
		AfxMessageBox(_T("You must disconnect this individual from its relatives ")
			_T("before you change genders."));
	}
	else
	{
		m_nSex = nSex;

		CString strSex;
		if (m_nSex==0)
		{
		}
		else if (m_nSex==1)
			strSex = "M";
		else if (m_nSex==2)
			strSex = "F";
		else
			ASSERT(FALSE);

		if (m_nSex)
			m_pDoc->ResetSubValue(m_hTreeItem,"SEX",strSex);
		else
			m_pDoc->DeleteSubValue(m_hTreeItem,"SEX");

		CalcFull();
	}
}

CString CIndividual::Sex()
{
	switch (m_nSex)
	{
	case 0:
		return "unknown";
	case 1:
		return "male";
	case 2:
		return "female";
	};
	ASSERT(FALSE);
	return "";
}

void CIndividual::DebugRelations()
{
	return;
	CString s, s2;

	s.Format(_T("i==%d: "),m_i);
	if (m_iFather>=0)
	{
		s2.Format(_T("f==%d "),m_iFather);
		s += s2;
	}
	if (m_iMother>=0)
	{
		s2.Format(_T("m==%d "),m_iMother);
		s += s2;
	}
	s += _T("\n");
	OutputDebugString(s);

	for (int i(0); i<m_riSpouse.GetSize(); i++)
	{
		s.Format(_T("   s==%d\n"),m_riSpouse[i]);
		OutputDebugString(s);
	}
	for (i = 0; i<m_riChild.GetSize(); i++)
	{
		s.Format(_T("   c==%d\n"),m_riChild[i]);
		OutputDebugString(s);
	}

	if (m_iChildToFamily>=0)
	{
		s.Format(_T("   ct==%d\n"),m_iChildToFamily);
		OutputDebugString(s);
	}
	for (i = 0; i<m_riSpouseToFamily.GetSize(); i++)
	{
		s.Format(_T("   st==%d\n"),m_riSpouseToFamily[i]);
		OutputDebugString(s);
	}
}

CString CIndividual::GetWebPage(CMemFile& fTmpl, const CString& sDocID)
{
	CArray<CEvt,CEvt&> revt;
	GetSortedEvents(revt);
	CString str;

	CArray<CAttr,CAttr&> rattr;
	GetSortedAttrs(rattr);

	fTmpl.SeekToBegin();
	char c;
	while (fTmpl.Read(&c,1))
	{
		if (c=='%')
		{
			CString sTok;
			if (!fTmpl.Read(&c,1)) return str;
			while (c!='%')
			{
				sTok += c;
				if (!fTmpl.Read(&c,1)) return str;
			}
			if (sTok.IsEmpty())
			{
				str += '%';
			}
			else
			{
				if (!sTok.CompareNoCase(_T("name")))
				{
					str += Name();
				}
				else if (!sTok.CompareNoCase(_T("events")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (revt.GetSize()&&!Private())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/events")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("event")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<revt.GetSize()&&!Private(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									CEvt& evt = revt[i];

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/event")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("type")))
												str += Cell(evt.m_strType);
											else if (!sTok.CompareNoCase(_T("date")))
												str += Cell(evt.m_dvDate.Display(DATE_SHORTDATE));
											else if (!sTok.CompareNoCase(_T("place")))
												str += Cell(evt.m_strPlace);
											else if (!sTok.CompareNoCase(_T("source")))
											{
												str += GetSourLink(evt.m_cita.m_iSource,Cell(evt.m_cita.Display()),sDocID);
											}
											else if (!sTok.CompareNoCase(_T("citation")))
											{
												if (evt.m_cita.m_strText.GetLength())
													str += "\""+evt.m_cita.m_strText+"\"";
											}
											else if (!sTok.CompareNoCase(_T("note")))
											{
												if (evt.m_iNote>=0)
													str += m_pDoc->m_rNote[evt.m_iNote].m_strNote;
												else
													str += "&nbsp;";
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (revt.GetSize()&&!Private())
								str += c;
						}
					}
				}
				else if (!sTok.CompareNoCase(_T("attributes")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (rattr.GetSize()&&!Private())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/attributes")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("attribute")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<rattr.GetSize()&&!Private(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									CAttr& attr = rattr[i];

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/attribute")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("type")))
												str += Cell(attr.m_strType);
											else if (!sTok.CompareNoCase(_T("value")))
												str += Cell(attr.m_strValue);
											else if (!sTok.CompareNoCase(_T("date")))
												str += Cell(attr.m_evt.m_dvDate.Display(DATE_SHORTDATE));
											else if (!sTok.CompareNoCase(_T("place")))
												str += Cell(attr.m_evt.m_strPlace);
											else if (!sTok.CompareNoCase(_T("source")))
											{
												str += GetSourLink(attr.m_evt.m_cita.m_iSource,Cell(attr.m_evt.m_cita.Display()),sDocID);
											}
											else if (!sTok.CompareNoCase(_T("citation")))
											{
												if (attr.m_evt.m_cita.m_strText.GetLength())
													str += "\""+attr.m_evt.m_cita.m_strText+"\"";
											}
											else if (!sTok.CompareNoCase(_T("note")))
											{
												if (attr.m_evt.m_iNote>=0)
													str += m_pDoc->m_rNote[attr.m_evt.m_iNote].m_strNote;
												else
													str += "&nbsp;";
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (rattr.GetSize()&&!Private())
								str += c;
						}
					}
				}
				else if (!sTok.CompareNoCase(_T("father")))
				{
					str += GetIndiLink(m_iFather,sDocID);
				}
				else if (!sTok.CompareNoCase(_T("mother")))
				{
					str += GetIndiLink(m_iMother,sDocID);
				}
				else if (!sTok.CompareNoCase(_T("partnerships")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (m_riSpouseToFamily.GetSize())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/partnerships")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("partnership")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<m_riSpouseToFamily.GetSize(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									int iFami = m_riSpouseToFamily[i];
									CFamily& fami = m_pDoc->m_rFamily[iFami];

									int iSpouse;
									if (fami.m_iHusband==m_i)
										iSpouse = fami.m_iWife;
									else
									{
										ASSERT(fami.m_iWife==m_i);
										iSpouse = fami.m_iHusband;
									}

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/partnership")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("name")))
												str += GetIndiLink(iSpouse,sDocID);
											else if (!sTok.CompareNoCase(_T("events")))
											{
												CArray<CEvt,CEvt&> revt;
												GetSortedEvents(fami,revt);
												BOOL bEnd(FALSE);
												while (!bEnd)
												{
													if (!fTmpl.Read(&c,1)) return str;
													if (c=='%')
													{
														CString sTok;
														if (!fTmpl.Read(&c,1)) return str;
														while (c!='%')
														{
															sTok += c;
															if (!fTmpl.Read(&c,1)) return str;
														}
														if (sTok.IsEmpty())
														{
															if (revt.GetSize()&&!Private())
																str += '%';
														}
														else if (!sTok.CompareNoCase(_T("/events")))
															bEnd = TRUE;
														else if (!sTok.CompareNoCase(_T("event")))
														{
															DWORD start = fTmpl.GetPosition();
															for (int i(0); i<revt.GetSize()&&!Private(); i++)
															{
																fTmpl.Seek(start,CFile::begin);
																CEvt& evt = revt[i];

																BOOL bEnd(FALSE);
																while (!bEnd)
																{
																	if (!fTmpl.Read(&c,1)) return str;
																	if (c=='%')
																	{
																		CString sTok;
																		if (!fTmpl.Read(&c,1)) return str;
																		while (c!='%')
																		{
																			sTok += c;
																			if (!fTmpl.Read(&c,1)) return str;
																		}
																		if (sTok.IsEmpty())
																			str += '%';
																		else if (!sTok.CompareNoCase(_T("/event")))
																			bEnd = TRUE;
																		else if (!sTok.CompareNoCase(_T("type")))
																			str += Cell(evt.m_strType);
																		else if (!sTok.CompareNoCase(_T("date")))
																			str += Cell(evt.m_dvDate.Display(DATE_SHORTDATE));
																		else if (!sTok.CompareNoCase(_T("place")))
																			str += Cell(evt.m_strPlace);
																		else if (!sTok.CompareNoCase(_T("source")))
																		{
																			str += GetSourLink(evt.m_cita.m_iSource,Cell(evt.m_cita.Display()),sDocID);
																		}
																		else if (!sTok.CompareNoCase(_T("citation")))
																		{
																			if (evt.m_cita.m_strText.GetLength())
																				str += "\""+evt.m_cita.m_strText+"\"";
																		}
																		else if (!sTok.CompareNoCase(_T("note")))
																		{
																			if (evt.m_iNote>=0)
																				str += m_pDoc->m_rNote[evt.m_iNote].m_strNote;
																			else
																				str += "&nbsp;";
																		}
																	}
																	else
																	{
																		str += c;
																	}
																}
															}
														}
													}
													else
													{
														if (revt.GetSize()&&!Private())
															str += c;
													}
												}
											}
											else if (!sTok.CompareNoCase(_T("children")))
											{
												BOOL bEnd(FALSE);
												while (!bEnd)
												{
													if (!fTmpl.Read(&c,1)) return str;
													if (c=='%')
													{
														CString sTok;
														if (!fTmpl.Read(&c,1)) return str;
														while (c!='%')
														{
															sTok += c;
															if (!fTmpl.Read(&c,1)) return str;
														}
														if (sTok.IsEmpty())
														{
															if (fami.m_riChild.GetSize())
																str += '%';
														}
														else if (!sTok.CompareNoCase(_T("/children")))
															bEnd = TRUE;
														else if (!sTok.CompareNoCase(_T("child")))
														{
															DWORD start = fTmpl.GetPosition();
															CArray<int,int> riChild;
															fami.GetSortedChildren(riChild);
															for (int j(0); j<riChild.GetSize(); j++)
															{
																fTmpl.Seek(start,CFile::begin);
																int iChild = riChild[j];

																BOOL bEnd(FALSE);
																while (!bEnd)
																{
																	if (!fTmpl.Read(&c,1)) return str;
																	if (c=='%')
																	{
																		CString sTok;
																		if (!fTmpl.Read(&c,1)) return str;
																		while (c!='%')
																		{
																			sTok += c;
																			if (!fTmpl.Read(&c,1)) return str;
																		}
																		if (sTok.IsEmpty())
																			str += '%';
																		else if (!sTok.CompareNoCase(_T("/child")))
																			bEnd = TRUE;
																		else if (!sTok.CompareNoCase(_T("name")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			bool bDescendants(!!pIndi->m_riChild.GetSize());
																			if (bDescendants)
																				str += "<b>";
																			str += GetIndiLink(iChild,sDocID);
																			if (bDescendants)
																				str += "</b>";
																		}
																		else if (!sTok.CompareNoCase(_T("birthdate")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			if (pIndi->m_iBirth>=0&&!pIndi->Private())
																				str += pIndi->m_revt[pIndi->m_iBirth].m_dvDate.Display(DATE_SHORTDATE);
																			else
																				str += "&nbsp;";
																		}
																		else if (!sTok.CompareNoCase(_T("birthplace")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			if (pIndi->m_iBirth>=0&&!pIndi->Private())
																				str += pIndi->m_revt[pIndi->m_iBirth].m_strPlace;
																			else
																				str += "&nbsp;";
																		}
																		else if (!sTok.CompareNoCase(_T("deathdate")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			if (pIndi->m_iDeath>=0&&!pIndi->Private())
																				str += pIndi->m_revt[pIndi->m_iDeath].m_dvDate.Display(DATE_SHORTDATE);
																			else
																				str += "&nbsp;";
																		}
																		else if (!sTok.CompareNoCase(_T("deathplace")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			if (pIndi->m_iDeath>=0&&!pIndi->Private())
																				str += pIndi->m_revt[pIndi->m_iDeath].m_strPlace;
																			else
																				str += "&nbsp;";
																		}
																	}
																	else
																	{
																		str += c;
																	}
																}
															}
														}
													}
													else
													{
														if (fami.m_riChild.GetSize())
															str += c;
													}
												}
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (m_riSpouseToFamily.GetSize())
								str += c;
						}
					}
				}
			}
		}
		else
		{
			str += c;
		}
	}
	return str;
}

CString CIndividual::GetIndiLink(int iIndi, const CString& sDocID)
{
	CString str;
	if (iIndi>=0)
	{
		CIndividual* pIndi = m_pDoc->Individual(iIndi);
		str = pIndi->GetLink("",sDocID);
	}
	else
	{
		str = "[unknown]";
	}
	return str;
}

CString CIndividual::GetIndiRTF(int iIndi)
{
	CString str;
	if (iIndi>=0)
	{
		CIndividual* pIndi = m_pDoc->Individual(iIndi);
		str = pIndi->Name();
	}
	else
	{
		str = "[unknown]";
	}
	return str;
}

CString CIndividual::GetIndiPageRef(int iIndi)
{
	CString str;
	if (iIndi>=0)
	{
		CIndividual* pIndi = m_pDoc->Individual(iIndi);
		str = " (p. {\\field {\\*\\fldinst { PAGEREF "+pIndi->m_strID+" \\\\h }}{\\fldrslt <type Ctrl-A F9>}})";
	}
	else
	{
		str = "";
	}
	return str;
}

CString CIndividual::GetSourLink(int iSour, const CString& strCita, const CString& sDocID)
{
	CString str;
	if (iSour>=0)
	{
		str = m_pDoc->m_rSource[iSour].GetLink(strCita,sDocID);
	}
	else
	{
		str = strCita;
	}
	return str;
}

CString CIndividual::Cell(const CString& str)
{
	if (str.IsEmpty())
		return "&nbsp;";
	else
		return str;
}

BOOL CIndividual::Private()
{
	if (m_iBirth<0)
		return FALSE;

	CEvt& evt = m_revt[m_iBirth];
	int nYearTest = evt.m_dvDate.GetSimpleYear();
	if (nYearTest==0)
		return FALSE;

	int nYearNow = CTime::GetCurrentTime().GetYear();
	return nYearNow-nYearTest < 72;
}

int GetRank(const CString& tok)
{
	if (tok=="DEAT")
		return 1;
	if (tok=="CREM")
		return 2;
	if (tok=="BURI")
		return 3;
	if (tok=="PROB")
		return 4;

	return 0;
}

bool TestEmpty(const CEvt& eEmpty, const CEvt& eDated)
{
	// if eEmpty <= eDated, OK
	// if eEmpty > eDated, out of order

	if (eEmpty.m_strTypeTok=="BIRT")
		return false;

	if (eDated.m_strTypeTok=="BIRT")
		return true;

	int r1 = GetRank(eEmpty.m_strTypeTok);
	int r2 = GetRank(eDated.m_strTypeTok);

	return r1>r2;
}
void CIndividual::GetSortedEvents(CArray<CEvt,CEvt&>& revt)
{
	int n = m_revt.GetSize();
	CArray<int,int> rx;
	rx.SetSize(n);
	for (int i(0); i<n; i++)
		rx[i] = i;

	for (i = 0; i<n-1; i++)
	{
		for (int j(i+1); j<n; j++)
		{
			CEvt& evt1 = m_revt[rx[i]];
			CEvt& evt2 = m_revt[rx[j]];
			const CString& d1 = evt1.m_dvDate.Sort();
			const CString& d2 = evt2.m_dvDate.Sort();

			bool out_of_order;
			if (d1.IsEmpty())
				out_of_order = TestEmpty(evt1,evt2);
			else if (d2.IsEmpty())
				out_of_order = !TestEmpty(evt2,evt1);
			else
				out_of_order = d1>d2;

			if (out_of_order)
			{
				int t = rx[i];
				rx[i] = rx[j];
				rx[j] = t;
			}
		}
	}

	revt.SetSize(n);
	for (i = 0; i<n; i++)
		revt[i] = m_revt[rx[i]];
}

void CIndividual::GetSortedEvents(CFamily& fami, CArray<CEvt,CEvt&>& revt)
{
	int n = fami.m_revt.GetSize();
	CArray<int,int> rx;
	rx.SetSize(n);
	for (int i(0); i<n; i++)
		rx[i] = i;

	for (i = 0; i<n-1; i++)
		for (int j(i+1); j<n; j++)
			if (fami.m_revt[rx[i]].m_dvDate.Sort()>fami.m_revt[rx[j]].m_dvDate.Sort())
			{
				int t = rx[i];
				rx[i] = rx[j];
				rx[j] = t;
			}

	revt.SetSize(n);
	for (i = 0; i<n; i++)
		revt[i] = fami.m_revt[rx[i]];
}

void CIndividual::GetSortedAttrs(CArray<CAttr,CAttr&>& rattr)
{
	int n = m_rattr.GetSize();
	CArray<int,int> rx;
	rx.SetSize(n);
	for (int i(0); i<n; i++)
		rx[i] = i;

	for (i = 0; i<n-1; i++)
		for (int j(i+1); j<n; j++)
			if (m_rattr[rx[i]].m_evt.m_dvDate.Sort()>m_rattr[rx[j]].m_evt.m_dvDate.Sort())
			{
				int t = rx[i];
				rx[i] = rx[j];
				rx[j] = t;
			}

	rattr.SetSize(n);
	for (i = 0; i<n; i++)
		rattr[i] = m_rattr[rx[i]];
}

CString CIndividual::Census(const CArray<CDate,CDate>& rdateCensusDay)
{
	CString sLine = "<tr><td>";
	sLine += m_name.Surname();
	sLine += "</td><td>";
	sLine += m_name.GivenName();
	sLine += "</td>";

	int nBirth(0), nDeath(0);
	if (m_iBirth>=0)
		nBirth = m_revt[m_iBirth].m_dvDate.GetSimpleYMD();
	if (m_iDeath>=0)
		nDeath = m_revt[m_iDeath].m_dvDate.GetSimpleYMD();

	if (nBirth||nDeath)
	{
		// 60 years est. life if one date missing
		if (!nBirth)
			nBirth = nDeath-600000;
		if (!nDeath)
			nDeath = nBirth+600000;


		for (int i(0); i<rdateCensusDay.GetSize(); i++)
		{
			int nCensusDay = rdateCensusDay[i].GetSimpleYMD();

			int nLastEvent(0), iLastEvent(-1), iCensusEvent(-1);
			for (int iEvt(0); iEvt<m_revt.GetSize(); iEvt++)
			{
				CEvt& evt = m_revt[iEvt];
				bool bHasPlace = !evt.m_strPlace.IsEmpty();
				int nDate = evt.m_dvDate.GetSimpleYMD();
				if (bHasPlace && nLastEvent<nDate && nDate<=nCensusDay)
				{
					nLastEvent = nDate;
					iLastEvent = iEvt;
				}
				if (nCensusDay/10000==nDate/10000 && evt.m_strTypeTok=="CENS")
				{
					iCensusEvent = iEvt;
				}
			}
			CString strPlace;
			if (iCensusEvent>=0)
			{
				strPlace = m_revt[iCensusEvent].m_strPlace;
			}
			else if (iLastEvent>=0)
			{
				strPlace = m_revt[iLastEvent].m_strPlace;
			}
			else
			{
				strPlace = "?";
			}
			CString sField;
			if (nCensusDay<nBirth)
			{
				if (!(nBirth%10000) && nCensusDay>=nBirth-100000)
				{
					sField += "[";
					sField += strPlace;
					sField += "?]";
				}
			}
			else if (nDeath<nCensusDay)
			{
				if (!(nDeath%10000) && nCensusDay<=nDeath+100000)
				{
					sField += "[";
					sField += strPlace;
					sField += "?]";
				}
			}
			else if (iCensusEvent>=0)
			{
				sField += "*";
				sField += strPlace;
			}
			else
			{
				sField += strPlace;
			}

			if (sField.IsEmpty())
			{
				sField = "&nbsp;";
			}
			else
			{
				sField += "<br />(";
				if (m_nSex==1)
				{
					sField += "m";
				}
				else
				{
					sField += "f";
				}
				int age = (nCensusDay-nBirth)/10000;
				if (age >= 0)
				{
					sField += ", ";

					CString sAge;
					if (age >= 1)
					{
						sAge.Format(L"%d",age);
					}
					else
					{
						int nBirthMonths = (nBirth/10000*12)+(nBirth-nBirth/10000*10000)/100;
						int nCensusMonths = rdateCensusDay[i].GetSimpleMonthCount();
						sAge.Format(L"%d/12",nCensusMonths-nBirthMonths);
					}
					sField += sAge+" yrs";
					if (wasMarriedBy(nCensusDay))
					{
						if (m_nSex==1)
						{
							sField += ", hh";
						}
						else
						{
							int spfam = MostRecentSpouse(nCensusDay);
							if (spfam < 0)
							{
								sField += "??? BUG ???";
							}
							else
							{
								CFamily& fami = m_pDoc->m_rFamily[spfam];
								int ih = fami.m_iHusband;
								if (ih < 0)
								{
									sField += ", hh: [unknown]";
								}
								else
								{
									CIndividual& hus = m_pDoc->m_rIndividual[ih];
									int dh = 99999999;
									if (hus.m_iDeath>=0)
									{
										dh = hus.m_revt[hus.m_iDeath].m_dvDate.GetSimpleYMD();
									}
									if (dh <= nCensusDay)
									{
										sField += ", (widow)";
									}
									else
									{
										sField += ", hh: "+hus.m_name.GivenName()+" "+hus.m_name.Surname();
									}
								}
							}
						}
					}
				}
				sField += ")";
			}

			sLine += "<td>" + sField + "</td>";
		}
	}
	sLine += "</tr>\n";
	return sLine;
}

CString CIndividual::AllPlaces()
{
	CString s;
	for (int iEvt(0); iEvt<m_revt.GetSize(); iEvt++)
	{
		CEvt& evt = m_revt[iEvt];
		CString sPlace = evt.m_strPlace;
		if (!sPlace.IsEmpty())
		{
			s += sPlace + "\n";
		}
	}
	for (int iAttr(0); iAttr<m_revt.GetSize(); iAttr++)
	{
		CAttr& attr = m_rattr[iAttr];
		CString sPlace = attr.m_evt.m_strPlace;
		if (!sPlace.IsEmpty())
		{
			s += sPlace + "\n";
		}
	}
	return s;
}

CString CIndividual::GetRTFPage(CMemFile& fTmpl)
{
	CArray<CEvt,CEvt&> revt;
	GetSortedEvents(revt);
	CString str;

	CArray<CAttr,CAttr&> rattr;
	GetSortedAttrs(rattr);

	fTmpl.SeekToBegin();
	char c;
	while (fTmpl.Read(&c,1))
	{
		if (c=='%')
		{
			CString sTok;
			if (!fTmpl.Read(&c,1)) return str;
			while (c!='%')
			{
				sTok += c;
				if (!fTmpl.Read(&c,1)) return str;
			}
			if (sTok.IsEmpty())
			{
				str += '%';
			}
			else
			{
				if (!sTok.CompareNoCase(_T("name")))
				{
					str += Name();
				}
				else if (!sTok.CompareNoCase(_T("events")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (revt.GetSize())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/events")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("event")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<revt.GetSize(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									CEvt& evt = revt[i];

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/event")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("type")))
												str += evt.m_strType;
											else if (!sTok.CompareNoCase(_T("date")))
												str += evt.m_dvDate.Display(DATE_SHORTDATE);
											else if (!sTok.CompareNoCase(_T("place")))
												str += evt.m_strPlace;
											else if (!sTok.CompareNoCase(_T("citation")))
											{
												str += evt.m_cita.Display();
												if (evt.m_cita.m_strText.GetLength())
													str += " \""+evt.m_cita.m_strText+"\"";
											}
											else if (!sTok.CompareNoCase(_T("note")))
											{
												if (evt.m_iNote>=0)
													str += m_pDoc->m_rNote[evt.m_iNote].m_strNote;
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (revt.GetSize())
								str += c;
						}
					}
				}
				else if (!sTok.CompareNoCase(_T("attributes")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (rattr.GetSize())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/attributes")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("attribute")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<rattr.GetSize(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									CAttr& attr = rattr[i];

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/attribute")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("type")))
												str += attr.m_strType;
											else if (!sTok.CompareNoCase(_T("value")))
												str += attr.m_strValue;
											else if (!sTok.CompareNoCase(_T("date")))
												str += attr.m_evt.m_dvDate.Display(DATE_SHORTDATE);
											else if (!sTok.CompareNoCase(_T("place")))
												str += attr.m_evt.m_strPlace;
											else if (!sTok.CompareNoCase(_T("citation")))
											{
												str += attr.m_evt.m_cita.Display();
												if (attr.m_evt.m_cita.m_strText.GetLength())
													str += " \""+attr.m_evt.m_cita.m_strText+"\"";
											}
											else if (!sTok.CompareNoCase(_T("note")))
											{
												if (attr.m_evt.m_iNote>=0)
													str += m_pDoc->m_rNote[attr.m_evt.m_iNote].m_strNote;
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (rattr.GetSize())
								str += c;
						}
					}
				}
				else if (!sTok.CompareNoCase(_T("father")))
				{
					str += GetIndiRTF(m_iFather);
				}
				else if (!sTok.CompareNoCase(_T("fatherid")))
				{
					str += GetIndiPageRef(m_iFather);
				}
				else if (!sTok.CompareNoCase(_T("mother")))
				{
					str += GetIndiRTF(m_iMother);
				}
				else if (!sTok.CompareNoCase(_T("motherid")))
				{
					str += GetIndiPageRef(m_iMother);
				}
				else if (!sTok.CompareNoCase(_T("id")))
				{
					str += m_strID;
				}
				else if (!sTok.CompareNoCase(_T("partnerships")))
				{
					BOOL bEnd(FALSE);
					while (!bEnd)
					{
						if (!fTmpl.Read(&c,1)) return str;
						if (c=='%')
						{
							CString sTok;
							if (!fTmpl.Read(&c,1)) return str;
							while (c!='%')
							{
								sTok += c;
								if (!fTmpl.Read(&c,1)) return str;
							}
							if (sTok.IsEmpty())
							{
								if (m_riSpouseToFamily.GetSize())
									str += '%';
							}
							else if (!sTok.CompareNoCase(_T("/partnerships")))
								bEnd = TRUE;
							else if (!sTok.CompareNoCase(_T("partnership")))
							{
								DWORD start = fTmpl.GetPosition();
								for (int i(0); i<m_riSpouseToFamily.GetSize(); i++)
								{
									fTmpl.Seek(start,CFile::begin);
									int iFami = m_riSpouseToFamily[i];
									CFamily& fami = m_pDoc->m_rFamily[iFami];

									int iSpouse;
									if (fami.m_iHusband==m_i)
										iSpouse = fami.m_iWife;
									else
									{
										ASSERT(fami.m_iWife==m_i);
										iSpouse = fami.m_iHusband;
									}

									BOOL bEnd(FALSE);
									while (!bEnd)
									{
										if (!fTmpl.Read(&c,1)) return str;
										if (c=='%')
										{
											CString sTok;
											if (!fTmpl.Read(&c,1)) return str;
											while (c!='%')
											{
												sTok += c;
												if (!fTmpl.Read(&c,1)) return str;
											}
											if (sTok.IsEmpty())
												str += '%';
											else if (!sTok.CompareNoCase(_T("/partnership")))
												bEnd = TRUE;
											else if (!sTok.CompareNoCase(_T("name")))
												str += GetIndiRTF(iSpouse);
											else if (!sTok.CompareNoCase(_T("id")))
												str += GetIndiPageRef(iSpouse);
											else if (!sTok.CompareNoCase(_T("events")))
											{
												CArray<CEvt,CEvt&> revt;
												GetSortedEvents(fami,revt);
												BOOL bEnd(FALSE);
												while (!bEnd)
												{
													if (!fTmpl.Read(&c,1)) return str;
													if (c=='%')
													{
														CString sTok;
														if (!fTmpl.Read(&c,1)) return str;
														while (c!='%')
														{
															sTok += c;
															if (!fTmpl.Read(&c,1)) return str;
														}
														if (sTok.IsEmpty())
														{
															if (revt.GetSize())
																str += '%';
														}
														else if (!sTok.CompareNoCase(_T("/events")))
															bEnd = TRUE;
														else if (!sTok.CompareNoCase(_T("event")))
														{
															DWORD start = fTmpl.GetPosition();
															for (int i(0); i<revt.GetSize(); i++)
															{
																fTmpl.Seek(start,CFile::begin);
																CEvt& evt = revt[i];

																BOOL bEnd(FALSE);
																while (!bEnd)
																{
																	if (!fTmpl.Read(&c,1)) return str;
																	if (c=='%')
																	{
																		CString sTok;
																		if (!fTmpl.Read(&c,1)) return str;
																		while (c!='%')
																		{
																			sTok += c;
																			if (!fTmpl.Read(&c,1)) return str;
																		}
																		if (sTok.IsEmpty())
																			str += '%';
																		else if (!sTok.CompareNoCase(_T("/event")))
																			bEnd = TRUE;
																		else if (!sTok.CompareNoCase(_T("type")))
																			str += evt.m_strType;
																		else if (!sTok.CompareNoCase(_T("date")))
																			str += evt.m_dvDate.Display(DATE_SHORTDATE);
																		else if (!sTok.CompareNoCase(_T("place")))
																			str += evt.m_strPlace;
																		else if (!sTok.CompareNoCase(_T("citation")))
																		{
																			str += evt.m_cita.Display();
																			if (evt.m_cita.m_strText.GetLength())
																				str += " \""+evt.m_cita.m_strText+"\"";
																		}
																		else if (!sTok.CompareNoCase(_T("note")))
																		{
																			if (evt.m_iNote>=0)
																				str += m_pDoc->m_rNote[evt.m_iNote].m_strNote;
																		}
																	}
																	else
																	{
																		str += c;
																	}
																}
															}
														}
													}
													else
													{
														if (revt.GetSize())
															str += c;
													}
												}
											}
											else if (!sTok.CompareNoCase(_T("children")))
											{
												BOOL bEnd(FALSE);
												while (!bEnd)
												{
													if (!fTmpl.Read(&c,1)) return str;
													if (c=='%')
													{
														CString sTok;
														if (!fTmpl.Read(&c,1)) return str;
														while (c!='%')
														{
															sTok += c;
															if (!fTmpl.Read(&c,1)) return str;
														}
														if (sTok.IsEmpty())
														{
															if (fami.m_riChild.GetSize())
																str += '%';
														}
														else if (!sTok.CompareNoCase(_T("/children")))
															bEnd = TRUE;
														else if (!sTok.CompareNoCase(_T("child")))
														{
															DWORD start = fTmpl.GetPosition();
															for (int j(0); j<fami.m_riChild.GetSize(); j++)
															{
																fTmpl.Seek(start,CFile::begin);
																int iChild = fami.m_riChild[j];

																BOOL bEnd(FALSE);
																while (!bEnd)
																{
																	if (!fTmpl.Read(&c,1)) return str;
																	if (c=='%')
																	{
																		CString sTok;
																		if (!fTmpl.Read(&c,1)) return str;
																		while (c!='%')
																		{
																			sTok += c;
																			if (!fTmpl.Read(&c,1)) return str;
																		}
																		if (sTok.IsEmpty())
																			str += '%';
																		else if (!sTok.CompareNoCase(_T("/child")))
																			bEnd = TRUE;
																		else if (!sTok.CompareNoCase(_T("name")))
																		{
																			CIndividual* pIndi = m_pDoc->Individual(iChild);
																			bool bDescendants(!!pIndi->m_riChild.GetSize());
																			if (bDescendants)
																				str += "{\\b ";
																			str += GetIndiRTF(iChild);
																			if (bDescendants)
																				str += "}";
																		}
																		else if (!sTok.CompareNoCase(_T("id")))
																			str += GetIndiPageRef(iChild);
																	}
																	else
																	{
																		str += c;
																	}
																}
															}
														}
													}
													else
													{
														if (fami.m_riChild.GetSize())
															str += c;
													}
												}
											}
										}
										else
										{
											str += c;
										}
									}
								}
							}
						}
						else
						{
							if (m_riSpouseToFamily.GetSize())
								str += c;
						}
					}
				}
			}
		}
		else
		{
			str += c;
		}
	}
	return str;
}

CString CIndividual::Ident()
{
	long nBirth(0), nDeath(0);
	if (m_iBirth>=0)
		nBirth = m_revt[m_iBirth].m_dvDate.GetSimpleYear();
	if (m_iDeath>=0)
		nDeath = m_revt[m_iDeath].m_dvDate.GetSimpleYear();

	CString s = Name();
	s += " (";
	if (nBirth)
	{
		CString s2;
		s2.Format(L"%d",nBirth);
		s += s2;
	}
	else
	{
		s += "?";
	}
	s += "-";
	if (nDeath)
	{
		CString s2;
		s2.Format(L"%d",nDeath);
		s += s2;
	}
	else
	{
		s += "?";
	}
	s += ")";

	return s;
}

CString CIndividual::GetAnomalies()
{
	CString s;

	int nBirth(0);
	if (m_iBirth>=0)
		nBirth = m_revt[m_iBirth].m_dvDate.GetSimpleYMD();

	if (m_iFather>=0)
	{
		CIndividual* pFather = m_pDoc->Individual(m_iFather);
		int nBirthFather(0);
		if (pFather->m_iBirth>=0)
			nBirthFather = pFather->m_revt[pFather->m_iBirth].m_dvDate.GetSimpleYMD();
		if (nBirth && nBirthFather)
		{
			if (nBirth < nBirthFather+160000)
			{
				s += Ident();
				s += " was born when father, ";
				s += pFather->Ident();
				s += ", was under 16.<BR>\n";
			}
		}
		int nDeathFather(0);
		if (pFather->m_iDeath>=0)
			nDeathFather = pFather->m_revt[pFather->m_iDeath].m_dvDate.GetSimpleYMD();
		if (nBirth && nDeathFather)
		{
			if (nDeathFather < nBirth)
			{
				s += Ident();
				s += " was born after father, ";
				s += pFather->Ident();
				s += ", died.<BR>\n";
			}
		}
	}

	if (m_iMother>=0)
	{
		CIndividual* pMother = m_pDoc->Individual(m_iMother);
		int nBirthMother(0);
		if (pMother->m_iBirth>=0)
			nBirthMother = pMother->m_revt[pMother->m_iBirth].m_dvDate.GetSimpleYMD();
		if (nBirth && nBirthMother)
		{
			if (nBirth < nBirthMother+160000)
			{
				s += Ident();
				s += " was born when mother, ";
				s += pMother->Ident();
				s += ", was under 16.<BR>\n";
			}
		}
		int nDeathMother(0);
		if (pMother->m_iDeath>=0)
			nDeathMother = pMother->m_revt[pMother->m_iDeath].m_dvDate.GetSimpleYMD();
		if (nBirth && nDeathMother)
		{
			if (nDeathMother < nBirth)
			{
				s += Ident();
				s += " was born after mother, ";
				s += pMother->Ident();
				s += ", died.<BR>\n";
			}
		}
	}

	int nDeath(0);
	if (m_iDeath>=0)
		nDeath = m_revt[m_iDeath].m_dvDate.GetSimpleYMD();

	if (nDeath && nBirth)
	{
		if (nDeath<nBirth)
		{
			s += Ident();
			s += " has birth date after death date.<BR>\n";
		}
		if (nBirth+1000000<nDeath)
		{
			s += Ident();
			s += " is over 100 years old.<BR>\n";
		}
	}

	int i, j;

	if (nBirth)
	{
		for (i = 0; i<m_riSpouseToFamily.GetSize(); ++i)
		{
			int iFami = m_riSpouseToFamily[i];
			CFamily& fami = m_pDoc->m_rFamily[iFami];

			int iSpouse(-1);
			if (fami.m_iHusband==m_i)
				iSpouse = fami.m_iWife;
			else
			{
				ASSERT(fami.m_iWife==m_i);
				iSpouse = fami.m_iHusband;
			}
			CIndividual* pSpouse = 0;
			if (iSpouse>=0)
				pSpouse = m_pDoc->Individual(iSpouse);

			for (j = 0; j<fami.m_revt.GetSize(); ++j)
			{
				CEvt& evt = fami.m_revt[j];
				int nEvent = evt.m_dvDate.GetSimpleYMD();
				if (nEvent && nEvent < nBirth+160000)
				{
					s += Ident();
					s += " had a partnership event when under 16.<BR>\n";
				}
			}

			int nBirthSpouse(0);
			if (pSpouse && pSpouse->m_iBirth>=0)
				nBirthSpouse = pSpouse->m_revt[pSpouse->m_iBirth].m_dvDate.GetSimpleYMD();
			if (nBirthSpouse)
				if (abs(nBirth-nBirthSpouse) > 200000)
				{
					s += Ident();
					s += " and partner were born over 20 years apart.<BR>\n";
				}
		}

		if (m_iChildToFamily>=0 && m_revt[m_iBirth].m_dvDate.GetSimpleYMD()%10000)
		{
			int nBirthMonths = m_revt[m_iBirth].m_dvDate.GetSimpleMonthCount();

			CFamily& fami = m_pDoc->m_rFamily[m_iChildToFamily];
			for (i = 0; i<fami.m_riChild.GetSize(); ++i)
			{
				CIndividual* pSib = m_pDoc->Individual(fami.m_riChild[i]);
				if (pSib->m_i != m_i)
				{
					int nSibBirthMonths(0);
					if (pSib->m_iBirth>=0)
						nSibBirthMonths = pSib->m_revt[pSib->m_iBirth].m_dvDate.GetSimpleMonthCount();
					if (nSibBirthMonths && pSib->m_revt[pSib->m_iBirth].m_dvDate.GetSimpleYMD()%10000)
					{
						if (abs(nSibBirthMonths-nBirthMonths)<9)
						{
							s += Ident();
							s += " and sibling were born less than nine months apart.<BR>\n";
						}
					}
				}
			}
		}
	}

	return s;
}

int CIndividual::GetSimpleBirth()
{
	int d(0);

	if (m_iBirth > -1)
	{
		CEvt& evt = m_revt[m_iBirth];
		d = evt.m_dvDate.GetSimpleYMD();
	}

	return d;
}

void CIndividual::GetSortedSpouseFamilies(CArray<int,int>& riSpouseToFamily)
{
	riSpouseToFamily.SetSize(m_riSpouseToFamily.GetSize());
	for (int i(0); i<m_riSpouseToFamily.GetSize(); i++)
	{
		riSpouseToFamily[i] = m_riSpouseToFamily[i];
	}
	for (i = 0; i<riSpouseToFamily.GetSize()-1; i++)
	{
		for (int j(i+1); j<riSpouseToFamily.GetSize(); j++)
		{
			CFamily& fami1 = m_pDoc->m_rFamily[riSpouseToFamily[i]];
			CFamily& fami2 = m_pDoc->m_rFamily[riSpouseToFamily[j]];
			int d1 = fami1.GetSimpleMarriage();
			int d2 = fami2.GetSimpleMarriage();
			if (d2<d1)
			{
				int t = riSpouseToFamily[i];
				riSpouseToFamily[i] = riSpouseToFamily[j];
				riSpouseToFamily[j] = t;
			}
		}
	}
}

void CIndividual::setLevel(int lev)
{
	//already done
	if (m_bMark)
		return;

	//doing
	m_bMark = true;

	//self
	m_nLevel = lev;

	// position along y axis
	static const int levdy(100);
	int nOldScale = SetScale(1);
	CPoint pt(0,(5000-m_nLevel)*levdy-m_rectFrame.Height()/2);
	MoveTo(pt);
	SetScale(nOldScale);

	int i;

	//father
	if (m_iFather >= 0)
	{
		CIndividual& fath = m_pDoc->m_rIndividual[m_iFather];
		fath.setLevel(lev+1);
	}
	//mother (only if no father)
	else if (m_iMother >= 0)
	{
		CIndividual& moth = m_pDoc->m_rIndividual[m_iMother];
		moth.setLevel(lev+1);
	}

	//siblings
	if (m_iChildToFamily >= 0)
	{
		CFamily& fami = m_pDoc->m_rFamily[m_iChildToFamily];
		for (i = 0; i < fami.m_riChild.GetSize(); ++i)
		{
			CIndividual& sib = m_pDoc->m_rIndividual[fami.m_riChild[i]];
			sib.setLevel(lev);
		}
	}

	//children
	for (i = 0; i < m_riChild.GetSize(); ++i)
	{
		CIndividual& chil = m_pDoc->m_rIndividual[m_riChild[i]];
		chil.setLevel(lev-1);
	}

	//spouses
	for (i = 0; i < m_riSpouse.GetSize(); ++i)
	{
		CIndividual& spou = m_pDoc->m_rIndividual[m_riSpouse[i]];
		spou.setLevel(lev);
	}
}


template<class T>
bool list_contains(list<T> lst, const T& e)
{
	return find(lst.begin(),lst.end(),e) != lst.end();
}

extern int childdx;

void CIndividual::setSeqWithSpouses(double& /*seq*/, pair<double,double> lev_bounds[], bool left, list<CIndividual*>& cleannext)
{
//	if (m_bMark)
//		return;

	list<CIndividual*> all_sps;
	// build list of all spouses in this spouse group
	{
		list<CIndividual*> todo;
		if (!m_bMark)
			todo.push_back(this);
		while (!todo.empty())
		{
			CIndividual* pthis = todo.front(); todo.pop_front();
			all_sps.push_back(pthis);
			for (int sp(0); sp < pthis->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pthis->m_riSpouse[sp]];
				if (/*pspou->m_nLevel == m_nLevel &&*/!pspou->m_bMark && !list_contains(all_sps,pspou))
				{
					todo.push_back(pspou);
				}
			}
		}
	}

	list<CIndividual*>::reverse_iterator i = all_sps.rbegin();
	while (i != all_sps.rend())
	{
		CIndividual* pindi = *i;

		if (pindi->m_iFather>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iFather];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
				cleannext.push_back(indiparent->m_pMale);
		}
		if (pindi->m_iMother>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iMother];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
				cleannext.push_back(indiparent->m_pMale);
		}
		++i;
	}

	if (m_bMark)
		return;

	list<CIndividual*> left_sps;
	// build list of spouses to be displayed off to the LEFT of the indi
	{
		left_sps.push_back(this);
		CIndividual* pindi = this;
		while (pindi)
		{
			bool found(false);
			for (int sp(0); !found && sp < pindi->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pindi->m_riSpouse[sp]];
				if (!pspou->m_bMark && pspou->m_iFather < 0 && pspou != this && !list_contains(left_sps,pspou))
				{
					found = true;
					pindi = pspou;
					left_sps.push_back(pspou);
				}
			}
			if (!found)
				pindi = 0;
		}
	}

	list<CIndividual*> right_sps;
	// build list of spouses to be displayed off to the RIGHT of the indi
	{
		CIndividual* pindi = this;
		while (pindi)
		{
			bool found(false);
			for (int sp(0); !found && sp < pindi->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pindi->m_riSpouse[sp]];
				if (!pspou->m_bMark && pspou->m_iFather < 0 && pspou != this && !list_contains(left_sps,pspou) && !list_contains(right_sps,pspou))
				{
					found = true;
					pindi = pspou;
					right_sps.push_back(pspou);
				}
			}
			if (!found)
				pindi = 0;
		}
	}
	//add (to the right) all remaining spouses
	list<CIndividual*>::const_iterator iter = all_sps.begin();
	while (iter != all_sps.end())
	{
		CIndividual* pspou = *iter;
		if (!pspou->m_bMark && pspou->m_iFather < 0 && !list_contains(left_sps,pspou) && !list_contains(right_sps,pspou))
			right_sps.push_back(pspou);
		++iter;
	}

	if (!left)
		swap(right_sps,left_sps);

	// display spouses from the LEFT
	list<CIndividual*>::reverse_iterator left_iter = left_sps.rbegin();
	while (left_iter != left_sps.rend())
	{
		CIndividual* pindi = *left_iter;

		int nOldScale = pindi->SetScale(1);
		CPoint pt(lev_bounds[pindi->m_nLevel].second,pindi->m_rectFrame.top);
		pindi->MoveTo(pt);
		pindi->m_bMark = true;
		lev_bounds[pindi->m_nLevel].second = pindi->m_rectFrame.right+childdx;
		pindi->SetScale(nOldScale);
/*
		if (pindi->m_iFather>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iFather];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
			{
				cleannext.push_back(indiparent->m_pMale);
			}
		}
		if (pindi->m_iMother>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iMother];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
			{
				cleannext.push_back(indiparent->m_pMale);
			}
		}
*/
		++left_iter;
	}
	// display spouses to the RIGHT
	list<CIndividual*>::iterator right_iter = right_sps.begin();
	while (right_iter != right_sps.end())
	{
		CIndividual* pindi = *right_iter;

		int nOldScale = pindi->SetScale(1);
		CPoint pt(lev_bounds[pindi->m_nLevel].second,pindi->m_rectFrame.top);
		pindi->MoveTo(pt);
		pindi->m_bMark = true;;
		lev_bounds[pindi->m_nLevel].second = pindi->m_rectFrame.right+childdx;
		pindi->SetScale(nOldScale);
/*
		if (pindi->m_iFather>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iFather];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
			{
				cleannext.push_back(indiparent->m_pMale);
			}
		}
		if (pindi->m_iMother>=0)
		{
			CIndividual* indiparent = &m_pDoc->m_rIndividual[pindi->m_iMother];
			if (indiparent->m_pMale && indiparent->m_pMale != m_pMale)
			{
				cleannext.push_back(indiparent->m_pMale);
			}
		}
*/
		++right_iter;
	}

}

void CIndividual::markWithSpouses()
{
	if (m_bMark)
		return;

	list<CIndividual*> all_sps;
	// build list of all spouses in this spouse group
	{
		list<CIndividual*> todo;
		todo.push_back(this);
		while (!todo.empty())
		{
			CIndividual* pthis = todo.front(); todo.pop_front();
			all_sps.push_back(pthis);
			for (int sp(0); sp < pthis->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pthis->m_riSpouse[sp]];
				if (/*pspou->m_nLevel == m_nLevel &&*/!pspou->m_bMark && !list_contains(all_sps,pspou))
				{
					todo.push_back(pspou);
				}
			}
		}
	}

	list<CIndividual*>::const_iterator iter = all_sps.begin();
	while (iter != all_sps.end())
	{
		CIndividual* pindi = *iter;
		if (pindi==this || pindi->m_iFather < 0)
		{
			pindi->m_bMark = true;
		}
		++iter;
	}
}

void CIndividual::setRootWithSpouses(CIndividual* proot)
{
	if (m_bMark)
		return;

	list<CIndividual*> all_sps;
	// build list of all spouses in this spouse group
	{
		list<CIndividual*> todo;
		todo.push_back(this);
		while (!todo.empty())
		{
			CIndividual* pthis = todo.front(); todo.pop_front();
			all_sps.push_back(pthis);
			for (int sp(0); sp < pthis->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pthis->m_riSpouse[sp]];
				if (/*pspou->m_nLevel == m_nLevel &&*/!pspou->m_bMark && !list_contains(all_sps,pspou))
				{
					todo.push_back(pspou);
				}
			}
		}
	}

	list<CIndividual*>::const_iterator iter = all_sps.begin();
	while (iter != all_sps.end())
	{
		CIndividual* pindi = *iter;
		if (pindi==this || pindi->m_iFather < 0)
		{
			pindi->m_pMale = proot;
			pindi->m_bMark = true;
		}
		++iter;
	}
}

CIndividual* CIndividual::GetLeftSpouse()
{
	list<CIndividual*> all_sps;
	// build list of all spouses in this spouse group
	{
		list<CIndividual*> todo;
		todo.push_back(this);
		while (!todo.empty())
		{
			CIndividual* pthis = todo.front(); todo.pop_front();
			all_sps.push_back(pthis);
			for (int sp(0); sp < pthis->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pthis->m_riSpouse[sp]];
				if (!list_contains(all_sps,pspou))
				{
					todo.push_back(pspou);
				}
			}
		}
	}

	CIndividual* pi = 0;
	int x(0x80000000);
	list<CIndividual*>::const_iterator iter = all_sps.begin();
	while (iter != all_sps.end())
	{
		CIndividual* pindi = *iter;
		if (pindi->m_rectFrame.left < m_rectFrame.left)
		{
			if (pindi->m_rectFrame.left > x)
			{
				x = pindi->m_rectFrame.left;
				pi = pindi;
			}
		}
		++iter;
	}
	return pi;
}

CIndividual* CIndividual::GetRightSpouse()
{
	list<CIndividual*> all_sps;
	// build list of all spouses in this spouse group
	{
		list<CIndividual*> todo;
		todo.push_back(this);
		while (!todo.empty())
		{
			CIndividual* pthis = todo.front(); todo.pop_front();
			all_sps.push_back(pthis);
			for (int sp(0); sp < pthis->m_riSpouse.GetSize(); ++sp)
			{
				CIndividual* pspou = &m_pDoc->m_rIndividual[pthis->m_riSpouse[sp]];
				if (!list_contains(all_sps,pspou))
				{
					todo.push_back(pspou);
				}
			}
		}
	}

	CIndividual* pi = 0;
	int x(0x7fffffff);
	list<CIndividual*>::const_iterator iter = all_sps.begin();
	while (iter != all_sps.end())
	{
		CIndividual* pindi = *iter;
		if (pindi->m_rectFrame.left > m_rectFrame.left)
		{
			if (pindi->m_rectFrame.left < x)
			{
				x = pindi->m_rectFrame.left;
				pi = pindi;
			}
		}
		++iter;
	}
	return pi;
}

static inline basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl endl2(basic_ostream<wchar_t, char_traits<wchar_t> >& _O)
{
	_O.put(L'\r');
	_O.put(L'\n');
	_O.flush();
	return (_O);
}

/*
void CIndividual::writeSVG(wostringstream& os, int pt)
{
	CString s;


	s.Format(L"<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"white\" stroke=\"black\" stroke-width=\"1\" />",
		m_rectFrame.left,m_rectFrame.top,m_rectFrame.Width()-1,m_rectFrame.Height()-1);
	os << (LPCTSTR)s << endl2;

	LOGFONT lf;
	theApp.m_font.GetLogFont(&lf);

	int x = m_rectFrame.left+2;
	int y = m_rectFrame.top+pt;

	s.Format(L"<text x=\"%d\" y=\"%d\" textlength=\"%d\" text-decoration=\"underline\">",
		x,y,m_rectFrame.Width());
	os << (LPCTSTR)s << endl2;

		os << L"<a xlink:href=\"" << L"?indi=" << (LPCTSTR)m_strID.Mid(1) << L"\">" << (LPCTSTR)m_strEllipsisDisplay << L"</a>" << endl2;

	os << L"</text>" << endl2;

	if (m_strBirthDisplay.GetLength())
	{
		y += pt;
		s.Format(L"<text x=\"%d\" y=\"%d\">%s</text>",x,y,(LPCTSTR)m_strBirthXML);
		os << (LPCTSTR)s << endl2;
	}
	if (m_strDeathDisplay.GetLength())
	{
		y += pt;
		s.Format(L"<text x=\"%d\" y=\"%d\">%s</text>",x,y,(LPCTSTR)m_strDeathXML);
		os << (LPCTSTR)s << endl2;
	}
}
*/
void CIndividual::writeApplet(std::wostream& os)
{
	os << (LPCTSTR)m_strID << L",";
	CString name = Name();
	name.Replace(L',',L' ');
	os << (LPCTSTR)name << L",";
	if (!Private() && m_strBirthDisplay.GetLength())
		os << (LPCTSTR)m_strBirthDisplay;
	os << L",";
	if (!Private() && m_strDeathDisplay.GetLength())
		os << (LPCTSTR)m_strDeathDisplay;
	os << L",";
	os << m_rectFrame.left << L",";
	os << m_rectFrame.top << endl2;
}

bool CIndividual::wasMarriedBy(int ymd)
{
	int nMarr = 99999999;
	for (int i(0); i<m_riSpouseToFamily.GetSize(); ++i)
	{
		int iFami = m_riSpouseToFamily[i];
		CFamily& fami = m_pDoc->m_rFamily[iFami];
		nMarr = fami.GetSimpleMarriage();
	}
	return (nMarr <= ymd);
}

int CIndividual::MostRecentSpouse(int ymd) // returns the most recent family as of ymd
{
	int nLastMar = 0;
	int iLastFam = -1;
	for (int i(0); i<m_riSpouseToFamily.GetSize(); ++i)
	{
		int iFami = m_riSpouseToFamily[i];
		CFamily& fami = m_pDoc->m_rFamily[iFami];
		for (int j(0); j<fami.m_revt.GetSize(); ++j)
		{
			CEvt& evt = fami.m_revt[j];
			if (evt.m_strTypeTok=="MARR")
			{
				int nEvent = evt.m_dvDate.GetSimpleYMD();
				if (nEvent <= ymd)
				{
					if (nLastMar < nEvent)
					{
						nLastMar = nEvent;
						iLastFam = iFami;
					}
				}
			}
		}
	}
	return iLastFam;
}

void CIndividual::showAsOf(CDate* pasof)
{
	int d = pasof->GetSimpleYMD();

	int nBirth(0), nDeath(0);
	if (m_iBirth>=0)
		nBirth = m_revt[m_iBirth].m_dvDate.GetSimpleYMD();
	if (m_iDeath>=0)
		nDeath = m_revt[m_iDeath].m_dvDate.GetSimpleYMD();

	if (nBirth||nDeath)
	{
		// 60 years est. life if one date missing
		if (!nBirth)
			nBirth = nDeath-600000;
		if (!nDeath)
			nDeath = nBirth+600000;

		if (nBirth <= d && d <= nDeath)
		{
		}
		else
		{
			m_bHidden = true;
		}
	}
	else
	{
		m_bHidden = true;
	}
}

CString filterUnknown(const CString& s)
{
	if (s.IsEmpty())
	{
		return CString("[unknown]");
	}
	return s;
}

void CIndividual::WriteDescent(CStdioFile& f, int indiNum, list<CIndividual*>& todo)
{
	char rc[17];

	set<int> riSrc;

	_itoa(indiNum,rc,10);
	CString sInd(rc);

	_itoa(m_nLevel,rc,10);
	CString sGen(rc);

	f.WriteString(L"<!-- ********************************* generation ");
	f.WriteString(sGen);
	f.WriteString(L": ");
	f.WriteString(filterUnknown(m_name.m_strGiven));
	f.WriteString(L" ");
	f.WriteString(filterUnknown(m_name.m_strSurname));
	f.WriteString(m_name.m_strSuffix);
	f.WriteString(L" ********************************* -->\n");

	f.WriteString(L"<p class=\"indiParHead\">\n");

	f.WriteString(L"<span class=\"indiNumber\">");
		f.WriteString(sInd);
	f.WriteString(L"</span>.\n");

	f.WriteString(L"<span class=\"indiName\">");
		f.WriteString(filterUnknown(m_name.m_strGiven));
		f.WriteString(L"<span class=\"gen\">");
			f.WriteString(sGen);
		f.WriteString(L"</span>\n");
		f.WriteString(L"<span class=\"surname\">");
			f.WriteString(filterUnknown(m_name.m_strSurname));
		f.WriteString(L"</span>");
		f.WriteString(m_name.m_strSuffix);
	f.WriteString(L"</span>");
	riSrc.insert(m_name.m_cita.m_iSource);

	int deslev = m_nLevel;
	if (deslev > 1)
	{
		f.WriteString(L"\n(");
		CIndividual* panc = this;
		while (--deslev)
		{
			panc = &m_pDoc->m_rIndividual[panc->m_iFather];
			f.WriteString(filterUnknown(panc->m_name.m_strGiven));
			_itoa(deslev,rc,10);
			CString sLev(rc);
			f.WriteString(L"<span class=\"gen\">");
				f.WriteString(sLev);
			f.WriteString(L"</span>");
			if (deslev > 1)
			{
				f.WriteString(L", ");
			}
		}
		f.WriteString(L")");
	}

	f.WriteString(L",\n");
	bool first(true);

	if (m_iBirth>=0)
	{
		if (!first)
		{
			f.WriteString(L",\n");
		}
		first = false;
		f.WriteString(L"b. ");
		f.WriteString(m_revt[m_iBirth].m_dvDate.Display(DATE_SHORTDATE));
		CString s = m_revt[m_iBirth].m_strPlace;
		riSrc.insert(m_revt[m_iBirth].m_cita.m_iSource);
		if (!s.IsEmpty())
		{
			f.WriteString(L", "+s);
		}
	}

	if (m_iDeath>=0)
	{
		if (!first)
		{
			f.WriteString(L",\n");
		}
		first = false;
		f.WriteString(L"d. ");
		f.WriteString(m_revt[m_iDeath].m_dvDate.Display(DATE_SHORTDATE));
		CString s = m_revt[m_iDeath].m_strPlace;
		riSrc.insert(m_revt[m_iDeath].m_cita.m_iSource);
		if (!s.IsEmpty())
		{
			f.WriteString(L", "+s);
		}
	}

	CArray<CEvt,CEvt&> revt;
	GetSortedEvents(revt);
	for (int i(0); i < revt.GetSize(); ++i)
	{
		CEvt& evt = revt[i];
		if (evt.m_strTypeTok=="CENS")
		{
			if (!first)
			{
				f.WriteString(L",\n");
			}
			first = false;
			_itoa(evt.m_dvDate.GetSimpleYear(),rc,10);
			CString sYear(rc);
			f.WriteString(sYear);
			f.WriteString(L" Census ");
			f.WriteString(evt.m_strPlace);
			riSrc.insert(evt.m_cita.m_iSource);
		}
	}
	// other events?

	CArray<int,int> riSpouseToFamily;
	GetSortedSpouseFamilies(riSpouseToFamily);

	for (int sp(0); sp < riSpouseToFamily.GetSize(); ++sp)
	{
		int iFami = riSpouseToFamily[sp];
		CFamily& fami = m_pDoc->m_rFamily[iFami];
		if (!first)
		{
			f.WriteString(L",\n");
		}
		first = false;
		f.WriteString(L"m. ");
		for (int ev(0); ev < fami.m_revt.GetSize(); ++ev)
		{
			CEvt evt = fami.m_revt[ev];
			if (evt.m_strTypeTok=="MARR")
			{
				f.WriteString(evt.m_dvDate.Display(DATE_SHORTDATE));
				CString s = evt.m_strPlace;
				riSrc.insert(evt.m_cita.m_iSource);
				if (!s.IsEmpty())
				{
					f.WriteString(L", "+s);
				}
			}
			f.WriteString(L", ");
		}
		int iSpouse;
		if (fami.m_iHusband==m_i)
			iSpouse = fami.m_iWife;
		else
		{
			iSpouse = fami.m_iHusband;
		}
		if (iSpouse >= 0)
		{
			CIndividual indiSp(m_pDoc->m_rIndividual[iSpouse]);
			f.WriteString(L"<span class=\"indiName\">");
				f.WriteString(filterUnknown(indiSp.m_name.m_strGiven));
				f.WriteString(L" <span class=\"surname\">");
					f.WriteString(filterUnknown(indiSp.m_name.m_strSurname));
				f.WriteString(L"</span>");
				f.WriteString(indiSp.m_name.m_strSuffix);
			f.WriteString(L"</span>");
			if (indiSp.m_iFather >= 0 || indiSp.m_iMother >= 0)
			{
				f.WriteString(L" (");
				if (indiSp.m_nSex == 0)
				{
					f.WriteString(L"child");
				}
				else if (indiSp.m_nSex == 1)
				{
					f.WriteString(L"son");
				}
				else if (indiSp.m_nSex == 2)
				{
					f.WriteString(L"daughter");
				}
				f.WriteString(L" of ");
				if (indiSp.m_iFather >= 0)
				{
					CIndividual indiFa(m_pDoc->m_rIndividual[indiSp.m_iFather]);
					f.WriteString(L"<span class=\"indiName\">");
						f.WriteString(filterUnknown(indiFa.m_name.m_strGiven));
						f.WriteString(L" <span class=\"surname\">");
							f.WriteString(filterUnknown(indiFa.m_name.m_strSurname));
						f.WriteString(L"</span>");
						f.WriteString(indiFa.m_name.m_strSuffix);
					f.WriteString(L"</span>");
				}
				if (indiSp.m_iFather >= 0 && indiSp.m_iMother >= 0)
				{
					f.WriteString(L" and ");
				}
				if (indiSp.m_iMother >= 0)
				{
					CIndividual indiMo(m_pDoc->m_rIndividual[indiSp.m_iMother]);
					f.WriteString(L"<span class=\"indiName\">");
						f.WriteString(filterUnknown(indiMo.m_name.m_strGiven));
						f.WriteString(L" <span class=\"surname\">");
							f.WriteString(filterUnknown(indiMo.m_name.m_strSurname));
						f.WriteString(L"</span>");
						f.WriteString(indiMo.m_name.m_strSuffix);
					f.WriteString(L"</span>");
				}
				f.WriteString(L")");
			}
		}
		else
		{
			f.WriteString(L"unknown partner");
		}
	}



	f.WriteString(L".\n</p>\n");



	// children

	for (sp = 0; sp < riSpouseToFamily.GetSize(); ++sp)
	{
		int iFami = riSpouseToFamily[sp];
		CFamily& fami = m_pDoc->m_rFamily[iFami];
		if (fami.m_riChild.GetSize())
		{
			f.WriteString(L"\n<p>\nChildren (by ");

			int iSpouse;
			if (fami.m_iHusband==m_i)
				iSpouse = fami.m_iWife;
			else
			{
				iSpouse = fami.m_iHusband;
			}
			if (iSpouse >= 0)
			{
				CIndividual indiSp(m_pDoc->m_rIndividual[iSpouse]);
				f.WriteString(L"<span class=\"indiName\">");
					f.WriteString(filterUnknown(indiSp.m_name.m_strGiven));
					f.WriteString(L" <span class=\"surname\">");
						f.WriteString(filterUnknown(indiSp.m_name.m_strSurname));
					f.WriteString(L"</span>");
					f.WriteString(indiSp.m_name.m_strSuffix);
				f.WriteString(L"</span>");
			}
			else
			{
				f.WriteString(L"unknown partner");
			}

			f.WriteString(L"):\n<ol>\n");

			CArray<int,int> riChild;
			fami.GetSortedChildren(riChild);
			for (int ch(0); ch < riChild.GetSize(); ++ch)
			{
				f.WriteString(L"<li>\n");
				CIndividual* pchild = &m_pDoc->m_rIndividual[riChild[ch]];
				bool bDescendants(!!pchild->m_riChild.GetSize() && pchild->m_nSex == 1);
				if (bDescendants)
				{
					pchild->m_nLevel = m_nLevel+1;
					todo.push_back(pchild);
					int cNum = indiNum+todo.size();
					_itoa(cNum,rc,10);
					CString sChildNum(rc);
					f.WriteString(L"<span class=\"nextIndiNumber\">");
					f.WriteString(sChildNum);
					f.WriteString(L"</span>\n");
				}
				f.WriteString(L"<span class=\"indiName\">");
					f.WriteString(filterUnknown(pchild->m_name.m_strGiven));
					f.WriteString(L"<span class=\"gen\">");
						_itoa(m_nLevel+1,rc,10);
						CString sChildGen(rc);
						f.WriteString(sChildGen);
					f.WriteString(L"</span>");
					f.WriteString(pchild->m_name.m_strSuffix);
				f.WriteString(L"</span>\n");

				bool firstch(true);

				if (pchild->m_iBirth>=0)
				{
					if (!firstch)
					{
						f.WriteString(L",\n");
					}
					firstch = false;
					f.WriteString(L"b. ");
					f.WriteString(pchild->m_revt[pchild->m_iBirth].m_dvDate.Display(DATE_SHORTDATE));
					CString s = pchild->m_revt[pchild->m_iBirth].m_strPlace;
					if (!s.IsEmpty())
					{
						f.WriteString(L", "+s);
					}
				}

				if (!bDescendants)
				{
					if (pchild->m_iDeath>=0)
					{
						if (!firstch)
						{
							f.WriteString(L",\n");
						}
						firstch = false;
						f.WriteString(L"d. ");
						f.WriteString(pchild->m_revt[pchild->m_iDeath].m_dvDate.Display(DATE_SHORTDATE));
						CString s = pchild->m_revt[pchild->m_iDeath].m_strPlace;
						if (!s.IsEmpty())
						{
							f.WriteString(L", "+s);
						}
					}
				}

				f.WriteString(L".\n</li>\n");
			}
			f.WriteString(L"</ol>\n</p>\n");
		}
	}
	f.WriteString(L"<p class=\"indiRefs\">\nReferences:\n");
	set<int>::iterator iteriSrc = riSrc.begin();
	bool firstsrc(true);
	while (iteriSrc != riSrc.end())
	{
		int iSrc = (*iteriSrc);
		if (iSrc >= 0)
		{
			if (firstsrc)
			{
				firstsrc = false;
			}
			else
			{
				f.WriteString(L"; ");
			}
			f.WriteString(m_pDoc->m_rSource[iSrc].m_strTitle);
		}
		++iteriSrc;
	}

	f.WriteString(L"</p>\n");

	f.WriteString(L"\n\n\n\n\n");
}
