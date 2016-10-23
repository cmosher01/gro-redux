#include "stdafx.h"
#include "gedtree.h"
#include "IndiClip.h"
#include "gedtreeDoc.h"

void CIndiClip::Copy(CGedtreeDoc* pDocFrom)
{
	m_pDoc = pDocFrom;
	m_riIndi.RemoveAll();
	for (int iIndi(0); iIndi<m_pDoc->m_rIndividual.GetSize(); iIndi++)
	{
		CIndividual& indi = m_pDoc->m_rIndividual[iIndi];
		if (indi.m_bSelected)
		{
			m_riIndi.Add(iIndi);
		}
	}
}

void CIndiClip::Paste(CGedtreeDoc* pDocTo)
{
}
