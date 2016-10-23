#include "stdafx.h"
#include "gedtree.h"
#include "event.h"
#include "gedtreedoc.h"
#include "gedline.h"

CEvt::CEvt(CGedtreeDoc* pDoc, HTREEITEM htiEvent):
	m_pDoc(pDoc),
	m_hTreeItem(htiEvent),
	m_cita(pDoc,NULL),
	m_iNote(-1)
{
}

void CEvt::Set(CGedtreeDoc* pDoc, HTREEITEM hti)
{
	m_pDoc = pDoc;
	m_hTreeItem = hti;
}

void CEvt::Calc()
{
	m_cita.Calc();
	if (m_iNote>=0)
	{
		CNote& note = m_pDoc->m_rNote[m_iNote];
		m_strNote = note.m_strNote;
	}
	else
		m_strNote.Empty();
}

CEvt::~CEvt()
{
}

void CEvt::GetFromTree(const CString& strTypeTok)
{
	m_strTypeTok = strTypeTok;
	CTreeCtrl& tree = m_pDoc->m_tree;

	m_cita.Set(m_pDoc,NULL);
	HTREEITEM hSub = tree.GetChildItem(m_hTreeItem);
	while (hSub)
	{
		CGedLine* pglSub = (CGedLine*)tree.GetItemData(hSub);
		if (pglSub->m_strTok=="TYPE")
		{
			m_strType = pglSub->GetCleanValue(CGedLine::COLLAPSE);
		}
		else if (pglSub->m_strTok=="DATE")
		{
			m_dvDate.Set(pglSub->GetCleanValue(CGedLine::COLLAPSE|CGedLine::UPCASE));
		}
		else if (pglSub->m_strTok=="PLAC")
		{
			m_strPlace = pglSub->GetCleanValue(CGedLine::COLLAPSE);
		}
		else if (pglSub->m_strTok=="SOUR")
		{
			pglSub->CalcID();
			m_cita.Set(m_pDoc,hSub);
			m_cita.GetFromTree(pglSub->m_strValAsID);
		}
		else if (pglSub->m_strTok=="NOTE")
		{
			m_iNote = m_pDoc->LookupNote(pglSub->m_strValAsID);
		}
		hSub = tree.GetNextSiblingItem(hSub);
	}

	if (m_strType.IsEmpty())
		CalcType();

	if (m_strType.IsEmpty())
		m_strType = strTypeTok;
}

void CEvt::PutToTree()
{
	ASSERT(m_pDoc);

	m_pDoc->ResetToken(m_hTreeItem,m_strTypeTok);

	m_pDoc->ResetSubValue(m_hTreeItem,"DATE",m_dvDate.Get());
	m_pDoc->ResetSubValue(m_hTreeItem,"PLAC",m_strPlace);
	if (m_strTypeTok=="EVEN")
		m_pDoc->ResetSubValue(m_hTreeItem,"TYPE",m_strType);
	else
		m_pDoc->DeleteSubValue(m_hTreeItem,"TYPE");
	m_cita.PutToTree(m_hTreeItem);
	if (m_strNote.IsEmpty())
	{
		m_pDoc->DeleteSubValue(m_hTreeItem,"NOTE");
		if (m_iNote>=0) m_pDoc->m_rNote[m_iNote].Delete();
		m_iNote = -1;
	}
	else
	{
		bool bFoundLink(false);
		if (m_iNote<0)
		{
			if (m_strNote[0]==cID)
			{
				CString strValAsID(m_strNote);
				int i = strValAsID.Find(cID);
				if (i>=0)
				{
					strValAsID = strValAsID.Mid(i+1);
					i = strValAsID.Find(cID);
					if (i>=0)
					{
						strValAsID = strValAsID.Left(i);
						m_iNote = m_pDoc->LookupNote(strValAsID);
						if (m_iNote >= 0)
							bFoundLink = true;
					}
				}
			}
			if (!bFoundLink)
			{
				HTREEITEM hti = m_pDoc->InsertChild("NOTE");
				CNote note(m_pDoc,hti);
				m_iNote = m_pDoc->m_rNote.Add(note);
				m_pDoc->m_rNote[m_iNote].SetIndex(m_iNote);
				m_pDoc->m_rNote[m_iNote].CalcID();
			}
		}
		if (bFoundLink)
		{
			m_pDoc->m_rNote[m_iNote].CalcID();
		}
		else
		{
			m_pDoc->m_rNote[m_iNote].m_strNote = m_strNote;
			m_pDoc->m_rNote[m_iNote].PutToTree();
		}
		m_pDoc->ResetSubValue(m_hTreeItem,"NOTE",m_pDoc->m_rNote[m_iNote].GetID());
	}
}

void CEvt::CalcType()
{
	if (m_strTypeTok=="EVEN")
		m_strType = "[unknown]";
	else if (m_strTypeTok=="CENS")
		m_strType = "census";

	else if (m_strTypeTok=="BIRT")
		m_strType = "birth";
	else if (m_strTypeTok=="DEAT")
		m_strType = "death";
	else if (m_strTypeTok=="CHR")
		m_strType = "christening";
	else if (m_strTypeTok=="BURI")
		m_strType = "reposition";
	else if (m_strTypeTok=="CREM")
		m_strType = "cremation";
	else if (m_strTypeTok=="ADOP")
		m_strType = "adoption";
	else if (m_strTypeTok=="BAPM")
		m_strType = "baptism";
	else if (m_strTypeTok=="BARM")
		m_strType = "bar mitzvah";
	else if (m_strTypeTok=="BASM")
		m_strType = "bas mitzvah";
	else if (m_strTypeTok=="BLES")
		m_strType = "blessing";
	else if (m_strTypeTok=="CHRA")
		m_strType = "adult christening";
	else if (m_strTypeTok=="CONF")
		m_strType = "confirmation";
	else if (m_strTypeTok=="FCOM")
		m_strType = "first communion";
	else if (m_strTypeTok=="ORDN")
		m_strType = "ordination";
	else if (m_strTypeTok=="NATU")
		m_strType = "naturalization";
	else if (m_strTypeTok=="EMIG")
		m_strType = "emigration";
	else if (m_strTypeTok=="IMMI")
		m_strType = "immigration";
	else if (m_strTypeTok=="PROB")
		m_strType = "will probated";
	else if (m_strTypeTok=="WILL")
		m_strType = "signed will";
	else if (m_strTypeTok=="GRAD")
		m_strType = "graduated";
	else if (m_strTypeTok=="RETI")
		m_strType = "retirement";
	else if (m_strTypeTok=="RESI")
		m_strType = "residence";

	else if (m_strTypeTok=="MARR")
		m_strType = "marriage";
	else if (m_strTypeTok=="ANUL")
		m_strType = "annulment";
	else if (m_strTypeTok=="DIV")
		m_strType = "divorce";
	else if (m_strTypeTok=="DIVF")
		m_strType = "divorce filed";
	else if (m_strTypeTok=="ENGA")
		m_strType = "engagement";
	else if (m_strTypeTok=="MARB")
		m_strType = "marriage bann";
	else if (m_strTypeTok=="MARC")
		m_strType = "marriage contract";
	else if (m_strTypeTok=="MARL")
		m_strType = "marriage license";
	else if (m_strTypeTok=="MARS")
		m_strType = "marriage settlement";
}

void CEvt::Delete()
{
	ASSERT(m_pDoc);

	m_pDoc->DeleteItem(m_hTreeItem);

	if (m_iNote>=0) m_pDoc->m_rNote[m_iNote].Delete();
	m_iNote = -1;
}
