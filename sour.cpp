#include "stdafx.h"
#include "sour.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "gedline.h"

static const char cIDPrefix('S');

CSource::CSource(CGedtreeDoc* pDoc, HTREEITEM hTreeItem):
	CGedRecord(pDoc,hTreeItem),
	m_iRepository(-1)
{
}

CSource::CSource(const CSource& o):
	m_strAuthor(o.m_strAuthor),
	m_strTitle(o.m_strTitle),
	m_strPublish(o.m_strPublish),
	m_strText(o.m_strText),
	m_iRepository(o.m_iRepository)
{
}

CSource& CSource::operator=(const CSource& o)
{
	CGedRecord::operator=(o);
	m_strAuthor = o.m_strAuthor;
	m_strTitle = o.m_strTitle;
	m_strPublish = o.m_strPublish;
	m_strText = o.m_strText;
	m_iRepository = o.m_iRepository;
	return *this;
}

void CSource::GetFromTree()
{
	CTreeCtrl& tree = m_pDoc->m_tree;

	HTREEITEM htiSub = tree.GetChildItem(m_hTreeItem);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)tree.GetItemData(htiSub);

		if (pglChild->m_strTok=="AUTH")
		{
			m_strAuthor = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strAuthor);
		}
		else if (pglChild->m_strTok=="TITL")
		{
			m_strTitle = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strTitle);
		}
		else if (pglChild->m_strTok=="PUBL")
		{
			m_strPublish = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strPublish);
		}
		else if (pglChild->m_strTok=="TEXT")
		{
			m_strText = pglChild->m_strVal;
			m_pDoc->GetContLines(htiSub,m_strText);
		}
		else if (pglChild->m_strTok=="REPO")
		{
			m_iRepository = m_pDoc->LookupRepository(pglChild->m_strValAsID);
		}

		htiSub = tree.GetNextSiblingItem(htiSub);
	}

	Calc();
}

CString CSource::GetDisplay()
{
	CString s;
//	if (!m_strAuthor.IsEmpty())
//		s = m_strAuthor+". ";

	s += m_strTitle;

	if (m_strTitle.Right(1) != ".")
		s += ".";

	return s;
}

void CSource::PutToTree()
{
	m_pDoc->ResetSubValue(m_hTreeItem,"AUTH",m_strAuthor,FALSE,FALSE,TRUE);
	m_pDoc->ResetSubValue(m_hTreeItem,"TITL",m_strTitle,FALSE,FALSE,TRUE);
	m_pDoc->ResetSubValue(m_hTreeItem,"PUBL",m_strPublish,FALSE,FALSE,TRUE);
	m_pDoc->ResetSubValue(m_hTreeItem,"TEXT",m_strText,FALSE,FALSE,TRUE);
	if (m_iRepository>=0)
	{
		CRepository& repo = m_pDoc->m_rRepository[m_iRepository];
		m_pDoc->ResetSubValue(m_hTreeItem,"REPO",repo.GetID(),FALSE,FALSE,TRUE);
	}
	else
		m_pDoc->DeleteSubValue(m_hTreeItem,"REPO");
}

CString CSource::GetRTF(CMemFile& fTmpl)
{
	CString str;

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
				if (!sTok.CompareNoCase(_T("author")))
				{
					if (m_strAuthor.IsEmpty())
						str += "[unknown author]";
					else
						str += m_strAuthor;
				}
				else if (!sTok.CompareNoCase(_T("title")))
					str += m_strTitle;
				else if (!sTok.CompareNoCase(_T("publisher")))
					str += m_strPublish;
				else if (!sTok.CompareNoCase(_T("text")))
					str += RTFTextBlock(m_strText);
				else if (!sTok.CompareNoCase(_T("repository")))
				{
					if (m_iRepository>=0)
						str += m_pDoc->m_rRepository[m_iRepository].m_strName;
				}
				else if (!sTok.CompareNoCase(_T("address")))
				{
					if (m_iRepository>=0)
						str += m_pDoc->m_rRepository[m_iRepository].m_strAddr;
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

CString CSource::GetWebPage(CMemFile& fTmpl, const CString& sDocID)
{
	CString str;

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
				if (!sTok.CompareNoCase(_T("author")))
				{
					if (m_strAuthor.IsEmpty())
						str += "[unknown author]";
					else
						str += m_strAuthor;
				}
				else if (!sTok.CompareNoCase(_T("title")))
				{
					if (m_strTitle.IsEmpty())
						str += "[unknown title]";
					else
						str += m_strTitle;
				}
				else if (!sTok.CompareNoCase(_T("publisher")))
				{
					if (m_strPublish.IsEmpty())
						str += "[unknown publication information]";
					else
					{
						CString s = m_strPublish;
						if (s.Left(1) == "<" && s.Right(1)==">")
							s = s.Mid(1,s.GetLength()-2);
						bool bLink = s.Left(5)=="http:";
						if (bLink)
							s = "<a href=\""+s+"\">"+s+"</a>";
						str += s;
					}
				}
				else if (!sTok.CompareNoCase(_T("text")))
				{
					if (m_strText.IsEmpty())
						str += "[no transcription available]";
					else
						str += m_strText;
				}
				else if (!sTok.CompareNoCase(_T("repository")))
				{
					if (m_iRepository>=0)
						str += m_pDoc->m_rRepository[m_iRepository].m_strName;
				}
				else if (!sTok.CompareNoCase(_T("address")))
				{
					if (m_iRepository>=0)
						str += m_pDoc->m_rRepository[m_iRepository].m_strAddr;
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

CString CSource::TextBlock(const CString& strText)
{
	CString str;

	CString strRest(strText);
	int ieol;
	while ((ieol = strRest.Find(_T("\r\n"))) >= 0)
	{
		str += WordBreak(strRest.Left(ieol))+"<BR>\n";
		strRest = strRest.Mid(ieol+2);
	}
	str += strRest;

	return str;
}

CString CSource::WordBreak(const CString& strText)
{
	CString str, strRest(strText);

	while (strRest.GetLength()>60)
	{
		int ic = 60;
		while (ic<strRest.GetLength() && strRest[ic]!=' ') ic++;
		str += strRest.Left(ic)+"\n";
		if (ic>=strRest.GetLength())
			strRest = "";
		else
			strRest = strRest.Mid(ic+1);
	}
	str += strRest;

	return str;
}

CString CSource::RTFTextBlock(const CString& strText)
{
	CString str;
	int ieol;
	CString strRest;

	strRest = strText;
	str.Empty();
	while ((ieol = strRest.FindOneOf(_T("{}\\"))) >= 0)
	{
		CString s = strRest.Mid(ieol,1);
		str += strRest.Left(ieol)+"\\"+s;
		strRest = strRest.Mid(ieol+1);
	}
	str += strRest;

	strRest = str;
	str.Empty();
	while ((ieol = strRest.Find(_T("\r\n"))) >= 0)
	{
		str += WordBreak(strRest.Left(ieol))+"\\par\n";
		strRest = strRest.Mid(ieol+2);
	}
	str += strRest;

	return str;
}

void CSource::ConvertToHTML()
{
	CString s;

	CString strRest(m_strText);
	int i;
	bool in_table(false);
	int ccol;

	while ((i = strRest.Find(_T("\r\n"))) >= 0)
	{
		CString sline(strRest.Left(i));
		strRest = strRest.Mid(i+2);

		s += ConvertLine(sline,strRest,in_table,ccol);
	}
	s += ConvertLine(strRest,"",in_table,ccol);
	if (in_table)
	{
		s += "</table>\r\n";
		in_table = false;
	}

	if (s.Right(8)=="<br />\r\n")
		s = s.Left(s.GetLength()-8);

	m_strText = s;
}

CString CSource::ConvertLine(const CString& sline, const CString& strRest, bool& in_table, int& ccol)
{
	CString s;
	if (sline.Find(L"\t") >= 0)
	{
		if (!in_table)
		{
			s += "<table border=\"1\">\r\n";
			in_table = true;
			ccol = CountColumns(sline,strRest);
		}
		s += "<tr><td>"+ConvertTD(sline,ccol)+"</td></tr>\r\n";
	}
	else
	{
		if (in_table)
		{
			s += "</table>\r\n";
			in_table = false;
		}
		s += sline+"<br />\r\n";
	}
	return s;
}

CString CSource::ConvertTD(const CString& s, int ccol)
{
	// replace tabs with "</td><td>"
	CString str;
	CString strRest(s);
	int i;
	int c(1);
	while ((i = strRest.Find(_T("\t"))) >= 0)
	{
		c++;
		CString scol = strRest.Left(i);
		if (scol.IsEmpty())
			scol = "&nbsp;";
		str += scol+"</td><td>";
		strRest = strRest.Mid(i+1);
	}
	if (strRest.IsEmpty())
		strRest = "&nbsp;";
	str += strRest;

	while (c++ < ccol)
	{
		str += "</td><td>&nbsp;";
	}

	return str;
}

int CSource::CountColumns(const CString& sline, const CString& srest)
{
	int ccol = CountColumnsRow(sline);

	CString strRest(srest);
	int i;
	while ((i = strRest.Find(_T("\r\n"))) >= 0)
	{
		CString sline2(strRest.Left(i));
		strRest = strRest.Mid(i+2);

		if (sline2.Find(L"\t") < 0)
		{
			strRest = "";
			break;
		}
		int c = CountColumnsRow(sline2);
		if (c>ccol)
			ccol = c;
	}
	int c = CountColumnsRow(strRest);
	if (c>ccol)
		ccol = c;

	return ccol;
}

int CSource::CountColumnsRow(const CString& s)
{
	CString str;
	CString strRest(s);
	int i;
	int ccol = 1;
	while ((i = strRest.Find(_T("\t"))) >= 0)
	{
		ccol++;
		strRest = strRest.Mid(i+1);
	}
	return ccol;
}
