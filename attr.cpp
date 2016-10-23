#include "stdafx.h"
#include "gedtree.h"
#include "attr.h"
#include "gedtreedoc.h"
#include "gedline.h"

CAttr::CAttr(CGedtreeDoc* pDoc, HTREEITEM htiAttr):
	m_pDoc(pDoc),
	m_hTreeItem(htiAttr),
	m_evt(pDoc,NULL)
{
}

void CAttr::Set(CGedtreeDoc* pDoc, HTREEITEM hti)
{
	m_pDoc = pDoc;
	m_hTreeItem = hti;
}

void CAttr::Calc()
{
	m_evt.Calc();
}

CAttr::~CAttr()
{
}

void CAttr::GetFromTree(const CString& strTypeTok, const CString& strValue)
{
	m_strTypeTok = strTypeTok;
	m_strValue = strValue;

	m_evt.Set(m_pDoc,m_hTreeItem);
	m_evt.GetFromTree("EVEN");

	CalcType();
}

void CAttr::PutToTree()
{
	m_evt.Set(m_pDoc,m_hTreeItem);
	m_evt.PutToTree();
	m_pDoc->ResetToken(m_hTreeItem,m_strTypeTok,m_strValue);
	m_pDoc->DeleteSubValue(m_hTreeItem,"TYPE");// for now
}

void CAttr::CalcType()
{
	if (m_strTypeTok=="CAST")
		m_strType = "caste";
	else if (m_strTypeTok=="DSCR")
		m_strType = "description";
	else if (m_strTypeTok=="EDUC")
		m_strType = "education";
	else if (m_strTypeTok=="IDNO")
		m_strType = "national ID";
	else if (m_strTypeTok=="NATI")
		m_strType = "national origin";
	else if (m_strTypeTok=="NCHI")
		m_strType = "count of children";
	else if (m_strTypeTok=="NMR")
		m_strType = "count of marriages";
	else if (m_strTypeTok=="OCCU")
		m_strType = "occupation";
	else if (m_strTypeTok=="PROP")
		m_strType = "posession";
	else if (m_strTypeTok=="RELI")
		m_strType = "religion";
	else if (m_strTypeTok=="SSN")
		m_strType = "US Social Security number";
	else if (m_strTypeTok=="TITL")
		m_strType = "title";
}

void CAttr::Delete()
{
	ASSERT(m_pDoc);

	m_pDoc->DeleteItem(m_hTreeItem);
}
