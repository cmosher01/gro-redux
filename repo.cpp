#include "stdafx.h"
#include "repo.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"

CRepository::CRepository(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem)
{
}

CRepository::CRepository(const CRepository& o):
	m_strName(o.m_strName),
	m_strAddr(o.m_strAddr)
{
}

CRepository& CRepository::operator=(const CRepository& o)
{
	CGedRecord::operator=(o);
	m_strName = o.m_strName;
	m_strAddr = o.m_strAddr;
	return *this;
}

void CRepository::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);

		if (pglChild->m_strTok=="NAME")
			m_strName = pglChild->m_strVal;
		else if (pglChild->m_strTok=="ADDR")
		{
			m_strAddr = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strAddr);
		}

		htiSub = tree.GetNextSiblingItem(htiSub);
	}
}

void CRepository::PutToTree()
{
	m_pDoc->ResetSubValue(m_hTreeItem,"NAME",m_strName);
	m_pDoc->ResetSubValue(m_hTreeItem,"ADDR",m_strAddr,FALSE,FALSE,TRUE);
}

void CRepository::Calc()
{
}
