#include "stdafx.h"
#include "note.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"

CNote::CNote(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem)
{
}

CNote& CNote::operator=(const CNote& o)
{
	CGedRecord::operator=(o);
	m_strNote = o.m_strNote;
	return *this;
}

CNote::CNote(const CNote& o):
	m_strNote(o.m_strNote)
{
}

void CNote::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	CGedLine* pgl = (CGedLine*)tree.GetItemData(m_hTreeItem);
	m_strNote = pgl->m_strVal;
	m_pDoc->GetContLines(m_hTreeItem,m_strNote);
}

void CNote::PutToTree()
{
	m_pDoc->DeleteItem(m_hTreeItem);
	m_hTreeItem = m_pDoc->ResetSubValue(TVI_ROOT,"NOTE",m_strNote,TRUE,FALSE,TRUE);
	CTreeCtrl& tree = m_pDoc->m_tree;
	CGedLine* pgl = (CGedLine*)tree.GetItemData(m_hTreeItem);
	pgl->m_strID = m_strID;
}

void CNote::Calc()
{
}
