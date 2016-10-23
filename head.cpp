#include "stdafx.h"
#include "head.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"
#include "editsubm.h"

CHeader::CHeader(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem),
	m_hTreeItemSubm(NULL)
{
}

void CHeader::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	bool bHasSchema(false);
	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);

		if (pglChild->m_strTok=="NOTE")
		{
			m_strNote = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strNote);
		}
		else if (pglChild->m_strTok=="_SCHEMA")
		{
			bHasSchema = true;
			HTREEITEM htiIndiFami = tree.GetChildItem(htiSub);
			while (htiIndiFami)
			{
				CGedLine* pglIndiFami = (CGedLine*)tree.GetItemData(htiIndiFami);
				if (pglIndiFami->m_strTok=="INDI")
				{
					ProcessSchema(m_pDoc->m_mapschemaIndiTagToLabel,htiIndiFami);
				}
				else if (pglIndiFami->m_strTok=="FAM")
				{
					ProcessSchema(m_pDoc->m_mapschemaFamiTagToLabel,htiIndiFami);
				}
				htiIndiFami = tree.GetNextSiblingItem(htiIndiFami);
			}
		}
		htiSub = tree.GetNextSiblingItem(htiSub);
	}
	if (bHasSchema)
	{
		m_pDoc->m_mapschemaIndiTagToLabel.SetAt(L"_MDCL",L"Medical");
		m_pDoc->m_mapschemaIndiTagToLabel.SetAt(L"_DEG",L"Degree");
		m_pDoc->m_mapschemaIndiTagToLabel.SetAt(L"_MILT",L"Military");
	}
}

void CHeader::ProcessSchema(CMap<CString,LPCTSTR,CString,LPCTSTR>& map, HTREEITEM hti)
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	HTREEITEM htiSub = tree.GetChildItem(hti);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);

		HTREEITEM htiLab = tree.GetChildItem(htiSub);
		CGedLine* pglLab = (CGedLine*)tree.GetItemData(htiLab);

		map.SetAt(pglChild->m_strTok,pglLab->m_strVal);

		htiSub = tree.GetNextSiblingItem(htiSub);
	}
}

void CHeader::PutToTree()
{
	if (m_hTreeItem) m_pDoc->DeleteItem(m_hTreeItem);

	m_hTreeItem = m_pDoc->ResetSubValue(TVI_ROOT,"HEAD","",TRUE,TRUE);

	HTREEITEM h = m_pDoc->ResetSubValue(m_hTreeItem,"SOUR",theApp.m_info.m_strInternalName);
		m_pDoc->ResetSubValue(h,"NAME",theApp.m_pszAppName);
		m_pDoc->ResetSubValue(h,"VERS",theApp.m_info.m_strVersion);
		HTREEITEM h2 = m_pDoc->ResetSubValue(h,"CORP",theApp.m_info.m_strCompany);
			HTREEITEM h3 = m_pDoc->ResetSubValue(h2,"ADDR","41 Brook Pine Drive");
				m_pDoc->ResetSubValue(h3,"CONT","Shelton, CT 06484-5912");

	h = m_pDoc->ResetSubValue(m_hTreeItem,"GEDC","",FALSE,TRUE);
		m_pDoc->ResetSubValue(h,"VERS","5.5");
		m_pDoc->ResetSubValue(h,"FORM","LINEAGE-LINKED");

	m_pDoc->ResetSubValue(m_hTreeItem,"CHAR",theApp.SaveAsUnicode()?"UNICODE":"ASCII");

	char cPrefix = CGedRecord::GetPrefix("SUBM");
	CString strSubmID;
	strSubmID = CString(cPrefix)+"0";
	CString strSubmVal;
	strSubmVal = CString(cID)+strSubmID+CString(cID);
	m_pDoc->ResetSubValue(m_hTreeItem,"SUBM",strSubmVal);

	if (m_strNote.IsEmpty())
	{
		m_pDoc->DeleteSubValue(m_hTreeItem,"NOTE");
	}
	else
	{
		m_pDoc->ResetSubValue(m_hTreeItem,"NOTE",m_strNote,FALSE,FALSE,TRUE);
	}

	CTreeCtrl& tree = m_pDoc->m_tree;
	CEditSubm subm;
	if (m_hTreeItemSubm) m_pDoc->DeleteItem(m_hTreeItemSubm);
	m_hTreeItemSubm = m_pDoc->ResetSubValue(TVI_ROOT,"SUBM","",TRUE,TRUE);
		CGedLine* pglSubm = (CGedLine*)tree.GetItemData(m_hTreeItemSubm);
		pglSubm->m_strID = strSubmID;
		m_pDoc->ResetSubValue(m_hTreeItemSubm,"NAME",subm.m_strName);
		m_pDoc->ResetSubValue(m_hTreeItemSubm,"ADDR",subm.m_strAddr,FALSE,FALSE,TRUE);
}
