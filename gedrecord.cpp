#include "stdafx.h"
#include "gedtree.h"
#include "gedrecord.h"
#include "gedtreedoc.h"
#include "gedline.h"

CGedRecord::CGedRecord(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	m_pDoc(pDoc),
	m_hTreeItem(hTreeItem),
	m_i(-1)
{
}

CGedRecord::CGedRecord(const CGedRecord& o):
	m_pDoc(o.m_pDoc),
	m_hTreeItem(o.m_hTreeItem),
	m_i(o.m_i),
	m_strID(o.m_strID)
{
}

CGedRecord& CGedRecord::operator=(const CGedRecord& o)
{
	m_pDoc = o.m_pDoc;
	m_hTreeItem = o.m_hTreeItem;
	m_i = o.m_i;
	m_strID = o.m_strID;
	return *this;
}

CGedRecord::~CGedRecord()
{
}

void CGedRecord::Set(CGedtreeDoc* pDoc, HTREEITEM hTreeItem)
{
	m_pDoc = pDoc;
	m_hTreeItem = hTreeItem;
}

CString CGedRecord::GetID()
{
	return CString(cID)+m_strID+CString(cID);
}

void CGedRecord::SetIndex(int i)
{
	m_i = i;
}

char CGedRecord::GetPrefix(const CString& strTok)
{
	if (strTok=="INDI")
		return 'I';
	else if (strTok=="FAM")
		return 'F';
	else if (strTok=="SOUR")
		return 'S';
	else if (strTok=="NOTE")
		return 'T';
	else if (strTok=="REPO")
		return 'R';
	else if (strTok=="SUBM")
		return 'M';
	else if (strTok=="SUBN")
		return 'N';
	else if (strTok=="OBJE")
		return 'X';

	ASSERT(FALSE);
	return ' ';
}

void CGedRecord::CalcID()
{
	CGedLine* pgl = (CGedLine*)m_pDoc->m_tree.GetItemData(m_hTreeItem);
	_TCHAR cIDPrefix = GetPrefix(pgl->m_strTok);
	m_strID.Format(_T("%c%d"),cIDPrefix,m_i);
	pgl->m_strID = m_strID;
}

void CGedRecord::Delete()
{
	CGedLine* pgl = (CGedLine*)m_pDoc->m_tree.GetItemData(m_hTreeItem);
	pgl->m_strID.Empty();
	m_i = -1;
}

BOOL CGedRecord::Exists()
{
	return m_i>=0;

/*
	CGedLine* pgl = (CGedLine*)m_pDoc->m_tree.GetItemData(m_hTreeItem);
	return !pgl->m_strID.IsEmpty();
*/
}

CString CGedRecord::GetWebFilePath(const CString& sDocID)
{
	CString str;
	if (sDocID.IsEmpty())
	{
		str = m_strID;
		str += ".html";
		str.MakeLower();
	}
	else
	{
		CString stype = m_strID.Left(1);
		if (stype=="I")
		{
			str += "?indi=";
			str += m_strID.Mid(1);
		}
		else if (stype=="S")
		{
			str += "?sour=";
			str += m_strID.Mid(1);
		}
	}

	return str;
}

CString CGedRecord::GetLink(const CString& strText, const CString& sDocID)
{
	CString str;
	str += "<a href=\"" +GetWebFilePath(sDocID)+ "\">";
	if (strText.IsEmpty())
		str += GetLinkText();
	else
		str += strText;
	str += "</a>";
	return str;
}
