#include "stdafx.h"
#include "gedtree.h"
#include "cita.h"
#include "gedtreedoc.h"
#include "gedline.h"
#include "util.h"

CCitation::CCitation(CGedtreeDoc* pDoc, HTREEITEM hti):
	m_pDoc(pDoc),
	m_hTreeItem(hti),
	m_nQuality(-1),
	m_iSource(-1)
{
}

void CCitation::Set(CGedtreeDoc* pDoc, HTREEITEM hti)
{
	m_pDoc = pDoc;
	m_hTreeItem = hti;
}

CCitation::~CCitation()
{
}

void CCitation::GetFromTree(const CString& strSourceID)
{
	m_iSource = m_pDoc->LookupSource(strSourceID);

	CTreeCtrl& tree = m_pDoc->m_tree;

	HTREEITEM hSub = tree.GetChildItem(m_hTreeItem);
	while (hSub)
	{
		CGedLine* pglSub = (CGedLine*)tree.GetItemData(hSub);
		if (pglSub->m_strTok=="PAGE")
		{
			m_strPage = pglSub->GetCleanValue(CGedLine::COLLAPSE);
		}
		else if (pglSub->m_strTok=="DATA")
		{
			HTREEITEM hSubSub = tree.GetChildItem(hSub);
			while (hSubSub)
			{
				CGedLine* pglSub = (CGedLine*)tree.GetItemData(hSubSub);
				if (pglSub->m_strTok=="TEXT")
				{
					m_strText = pglSub->m_strVal;
					m_pDoc->GetContLines(hSubSub,m_strText);
				}
				hSubSub = tree.GetNextSiblingItem(hSubSub);
			}
		}
		else if (pglSub->m_strTok=="QUAY")
		{
			m_nQuality = pglSub->m_nVal;
		}
		hSub = tree.GetNextSiblingItem(hSub);
	}
}

void CCitation::Clear()
{
	m_iSource = -1;
	m_strPage.Empty();
	m_strText.Empty();
}

CString CCitation::Display()
{
	CString s;

	if (m_iSource>=0)
	{
		CSource& sour = m_pDoc->m_rSource[m_iSource];
//		CString& strAuthor = sour.m_strAuthor;
		CString& strTitle = sour.m_strTitle;

//		if (!strAuthor.IsEmpty())
//			s = strAuthor+". ";

		s += strTitle;
		if (strTitle.Right(1) != ".")
			s += ".";
	}
	else if (!m_strPage.IsEmpty() || !m_strText.IsEmpty())
	{
		s += "[Unknown source.]";
	}

	if (!m_strPage.IsEmpty())
	{
		s += " "+m_strPage;
		if (m_strPage.Right(1) != ".")
			s += ".";
	}

	return s;
}

void CCitation::PutToTree(HTREEITEM htiParent)
{
	ASSERT(m_pDoc);

	if (m_iSource>=0 || !m_strPage.IsEmpty() || !m_strText.IsEmpty())
	{
		if (m_iSource>=0)
		{
			CSource& sour = m_pDoc->m_rSource[m_iSource];
			m_hTreeItem = m_pDoc->ResetSubValue(htiParent,"SOUR",sour.GetID());
		}
		else
		{
			m_hTreeItem = m_pDoc->ResetSubValue(htiParent,"SOUR","",FALSE,TRUE);
		}
		m_pDoc->ResetSubValue(m_hTreeItem,"PAGE",m_strPage);
		if (m_nQuality>=0)
		{
			m_pDoc->ResetSubValue(m_hTreeItem,"QUAY",CUtil::str(m_nQuality));
		}
		else
		{
			m_pDoc->DeleteSubValue(m_hTreeItem,"QUAY");
		}
		if (m_strText.IsEmpty())
		{
			m_pDoc->DeleteSubValue(m_hTreeItem,"DATA");
		}
		else
		{
			HTREEITEM hData = m_pDoc->ResetSubValue(m_hTreeItem,"DATA","",FALSE,TRUE);
			m_pDoc->ResetSubValue(hData,"TEXT",m_strText,FALSE,FALSE,TRUE);
		}
	}
	else
	{
		if (m_hTreeItem)
			m_pDoc->DeleteItem(m_hTreeItem);
		m_hTreeItem = NULL;
	}
}
