// gedtreeDoc.cpp : implementation of the CGedtreeDoc class
//

#include "stdafx.h"
#include <stdlib.h>
#include <afxcview.h>
#include "gedtree.h"
#include "gedtreeDoc.h"
#include "childfrm.h"
#include "dlgsavechanges.h"
#include "gedtreeViewDL.h"
#include "gedtreeViewIN.h"
#include "gedtreeViewIL.h"
#include "gedline.h"
#include "progress.h"
#include "docsize.h"
#include "picksource.h"
#include "pickrepo.h"
#include "headnotes.h"
#include "ansel.h"
#include "reg.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define L(x) x
char *sTplIndi =
#include "inditpl.h"
;
char *sTplSour =
#include "sourtpl.h"
;
char *sTplHeadrtf =
#include "headrtftpl.h"
;
char *sTplIndirtf =
#include "indirtftpl.h"
;
char *sTplSourrtf =
#include "sourrtftpl.h"
;
char *sTplBibrtf =
#include "bibrtftpl.h"
;

const CSize MIN_BOUNDS(640,480);

/////////////////////////////////////////////////////////////////////////////
// CGedtreeDoc

IMPLEMENT_DYNCREATE(CGedtreeDoc, CDocument)

BEGIN_MESSAGE_MAP(CGedtreeDoc, CDocument)
	//{{AFX_MSG_MAP(CGedtreeDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_EDIT_RESIZEDOCUMENT, OnEditResizedocument)
	ON_COMMAND(ID_VIEW_SOURCES, OnViewSources)
	ON_COMMAND(ID_VIEW_REPOSITORIES, OnViewRepositories)
	ON_COMMAND(ID_VIEW_INDEXOFINDIVIDUALS, OnViewIndexofindividuals)
	ON_COMMAND(ID_VIEW_NOTES, OnViewNotes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static inline basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl endl2(basic_ostream<wchar_t, char_traits<wchar_t> >& _O)
{
	_O.put(L'\r');
	_O.put(L'\n');
	_O.flush();
	return (_O);
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeDoc construction/destruction

CGedtreeDoc::CGedtreeDoc():
	m_pointNext(VIEW_BORDER,VIEW_BORDER-50),
	m_pDL(0),
	m_cRootRecord(0),
	m_pPrg(0),
	m_pIndiList(NULL),
	m_bAnsel(FALSE),
	m_iSourceLast(-1),
	m_cIndi(0),
	m_cFami(0),
	m_cSour(0),
	m_cRepo(0),
	m_cNote(0),
	m_bHadXY(false),
	m_bsortIndiNeedsUpdate(true),
	m_cAppletChart(0),
	m_pAppletChart(0)
{
	// TODO: add one-time construction code here
	m_head.Set(this,NULL);

	BOOL bOK;
	bOK = m_wndTree.Create(NULL,_T(""),0,CRect(0,0,1,1),theApp.GetMainWnd(),0);
	bOK = m_tree.Create(TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,CRect(0,0,1,1),&m_wndTree,1024);

	Normalize();
}

CGedtreeDoc::~CGedtreeDoc()
{
	m_wndTree.DestroyWindow();
	delete [] m_pAppletChart;
}

BOOL CGedtreeDoc::SaveModified()
{
	// Same as CDocument::SaveModified, except that
	// we display the title of the window instead of "Untitled"
	// for unsaved documents.
	// And we use our own "save changes" dialog.

	BOOL bRet(TRUE);

	if (IsModified())
	{
		CDlgSaveChanges d(m_strTitle);

		int nResult = d.DoModal();

		if (nResult==IDYES)
			bRet = DoFileSave();
		else if (nResult==IDCANCEL)
			bRet = FALSE;
	}

	return bRet;
}

BOOL CGedtreeDoc::OnSaveDocument(LPCTSTR pszPathName)
{
	CGedtreeDoc* pOpenDocument = (CGedtreeDoc*)theApp.GetOpenDocument(pszPathName);
	if (pOpenDocument && pOpenDocument!=this)
		pOpenDocument->DisconnectFile();

	return CDocument::OnSaveDocument(pszPathName);
}

void CGedtreeDoc::DisconnectFile()
{
	CString strName = GetPathName();

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(strName,drive,dir,fname,ext);

	TCHAR path_buffer[_MAX_PATH];
	_tmakepath(path_buffer,drive,dir,"Old"+CString(fname),ext);

	SetPathName(path_buffer);

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
	p->SetDefaultTitle(this);

	CString strTitle = GetTitle();
	strTitle = "Old"+strTitle;
	SetTitle(strTitle);

	UpdateTitle();

	SetModifiedFlag(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeDoc serialization

const int levMax(100);

void CGedtreeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		reg_t reg = GetRegistration();
		if (reg!=registered_good)
		{
			AfxMessageBox(_T("You cannot save, because this program has not been properly REGISTERED yet. Please click OK, then wait 10 seconds...."));
			::Sleep(9999);
			AfxThrowUserException();
		}
	}

	AfxGetMainWnd()->SetForegroundWindow();
	{
		CProgress prg;
		m_pPrg = &prg;

		if (ar.IsStoring())
			WriteToArchive(ar);
		else
			ReadFromArchive(ar);
		m_pPrg = NULL;
	}
	AfxGetMainWnd()->SetForegroundWindow();
}

CPoint CGedtreeDoc::GetNextPos()
{
	m_pointNext += CSize(0,50);

	return m_pointNext;
}

void CGedtreeDoc::TestUnicode(CFile* pFile)
{
	m_bUnicode = FALSE;
	m_bUnicodeFlagInFile = FALSE;
	m_bReverse = FALSE;

	WORD wTest(0);

	pFile->Read(&wTest,sizeof(wTest));

	if (wTest==0x0030) // unicode '0'
	{
		m_bUnicode = TRUE;
	}
	else if (wTest==0x3000) // unicode '0' (reverse byte order)
	{
		m_bUnicode = TRUE;
		m_bReverse = TRUE;
	}
	else if (wTest==0xFEFF) // unicode file signature word
	{
		m_bUnicodeFlagInFile = TRUE;
		m_bUnicode = TRUE;
	}
	else if (wTest==0xFFFE) // unicode file signature word (reverse byte order)
	{
		m_bUnicodeFlagInFile = TRUE;
		m_bUnicode = TRUE;
		m_bReverse = TRUE;
	}
}

void CGedtreeDoc::PutLine(CArchive& ar, const CString& str)
{
	if (theApp.SaveAsUnicode())
		ar.WriteString(str);
	else
	{
		for (int i(0); i<str.GetLength(); i++)
		{
			_TUCHAR tc = str[i];
			BYTE c = ((LPBYTE)&tc)[0];
			BYTE cHighByte = ((LPBYTE)&tc)[1];
			if (cHighByte)
			{
				c = '?';
				m_bBad = TRUE;
			}
			ar << c;
		}
	}
}

static wchar_t swapbytes(wchar_t c)
{
	char* pcin = (char*)&c;

	wchar_t cOut;
	char* pcout = (char*)&cOut;

	pcout[0] = pcin[1];
	pcout[1] = pcin[0];

	return cOut;
}

BOOL CGedtreeDoc::GetLine(CArchive& ar, CString& str)
{
	str.Empty();
	BOOL bOK(TRUE);
	// eol sequences to check for: cr, lf, cr/lf, or lf/cr
	// this function reads (& returns) one line, and discards the eol
	try
	{
		if (!m_bUnicode)
		{
			// ASCII or ANSEL file.
			// Read one byte at a time until eol
			BYTE c;
			if (m_bUseLast)
				c = m_byteLast;
			else
				ar >> c;
			while (c != '\r' && c != '\n')
			{
				wchar_t tc;
				if (c>=0x80&&m_bAnsel)
					tc = FilterReadChar(c);
				else
					tc = ((wchar_t)c)&0x0000ffff;
				if (c>=0xE0&&m_bAnsel) // combining
				{
					BYTE c2;
					ar >> c2;
					wchar_t tc2 = ((wchar_t)c2)&0x0000ffff;
					str += tc2;
				}
				if (tc)
					str += tc;
				ar >> c;
			}
			ar >> m_byteLast; // '\n' (ignored);
			m_bUseLast =
				(c=='\r'&&m_byteLast!='\n')||
				(c=='\n'&&m_byteLast!='\r');
			// m_bUseLast=TRUE if the next char wasn't part of the eol sequence
		}
		else
		{
			// UNICODE file.
			// Read one (two-byte) word (one unicode char) at a time until eol
			// If byte-reversed unicode, then reverse bytes in each word

			_TUCHAR c;
			if (m_bUseLast)
				c = m_tucharLast;
			else
				ar >> c;
			if (m_bReverse) c = swapbytes(c);
			while (c != _T('\r') && c != _T('\n'))
			{
				str += c;
				ar >> c;
				if (m_bReverse) c = swapbytes(c);
			}
			ar >> m_tucharLast; // '\n' (ignored);
			m_bUseLast =
				(c==_T('\r')&&m_tucharLast!=_T('\n'))||
				(c==_T('\n')&&m_tucharLast!=_T('\r'));
		}
	}
	catch (CArchiveException* pEx)
	{
		if (pEx->m_cause==CArchiveException::endOfFile)
		{
			m_bUseLast = FALSE;
			bOK = !str.IsEmpty();
			pEx->Delete();
		}
		else
		{
			throw;
		}
	}

/*
	if (bOK)
		OutputDebugString(str+_T("\n"));
	else
		OutputDebugString(_T("<EOF>\n"));
*/

	return bOK;
}

wchar_t CGedtreeDoc::FilterReadChar(BYTE c)
{
	wchar_t wc(c);
	if (c>=0x80&&m_bAnsel)
	{
		bool bFound(false);
		int i(0);
		while (!bFound && rcAnsel[i])
		{
			if (c==rcAnsel[i])
				bFound = true;
			else
				i++;
		}
		if (bFound)
			wc = rwcUTF8[i];
		else
			wc = L'?';
	}
	return wc;
}

void CGedtreeDoc::ReadFromArchive(CArchive& ar)
{
	CString strLine;
	HTREEITEM htviParent[levMax];
//	CMapStringToString& pm = App()->m_mapTagMeaning;
	int levPrev = -1;

	DWORD filesize(ar.GetFile()->GetLength());
	m_pPrg->Reset("Reading records from file.",filesize);
	DWORD filepos(0);

	m_bUseLast = FALSE;

	if (m_bUnicodeFlagInFile)
	{
		WORD flag;
		ar.Read(&flag,sizeof(flag));
	}

	while (GetLine(ar,strLine))
	{
		filepos += strLine.GetLength()+2;
		strLine.TrimLeft();
		if (strLine.GetLength())
		{
			if (strLine.GetLength())
			{
				if (strLine.Right(1) == '\x1a')
					strLine = strLine.Left(strLine.GetLength()-1);

				CGedLine* pgl = new CGedLine(this,strLine);
				int levCur = pgl->Level();

				if (levCur==0)
				{
					m_cRootRecord++;
					if (pgl->m_strTok=="INDI")
						m_cIndi++;
					else if (pgl->m_strTok=="FAM")
						m_cFami++;
					else if (pgl->m_strTok=="SOUR")
						m_cSour++;
					else if (pgl->m_strTok=="REPO")
						m_cRepo++;
					else if (pgl->m_strTok=="NOTE")
						m_cNote++;
				}
				else if (levCur==1 && pgl->m_strTok=="CHAR")
				{
					SetInputCharSet(pgl->GetCleanValue(CGedLine::COLLAPSE|CGedLine::UPCASE));
				}
				else if (levCur<0||levCur>levPrev+1||levCur>=levMax)
				{
					if (levCur<0) // level is invalid
					{
						levCur = levPrev; // assume same as previous
						if (levCur<0) levCur = 0; // min level is 0
					}
					else // level is too big
					{
						levCur = levPrev+1;
					}
				}

				HTREEITEM htviCur = m_tree.InsertItem(0,levCur ? htviParent[levCur-1] : TVI_ROOT);
				m_tree.SetItemData(htviCur,(DWORD)pgl);
				htviParent[levCur] = htviCur;
				levPrev = levCur;
			}
		}
		m_pPrg->Set(filepos>filesize?filesize:filepos,levPrev?"":strLine);
	}
	GetFromTree();
}

const int cWriteOrder(10);
static const CString strWriteOrder[cWriteOrder] =
{
	"HEAD",
	"SUBM",
	"INDI",
	"FAM",
	"SOUR",
	"REPO",
	"NOTE",
	"OBJE",
	"" // any other types
};

void CGedtreeDoc::WriteToArchive(CArchive& ar)
{
	reg_t reg = GetRegistration();
	if (reg!=registered_good)
			AfxThrowUserException();

	m_head.PutToTree();

	m_pPrg->Reset("Writing records to file.",m_tree.GetCount());
	m_nPrg = 0;

	m_bBad = FALSE;
	if (theApp.SaveAsUnicode())
	{
		WORD nUnicodeFlag(0xfeff);
		ar.Write(&nUnicodeFlag,sizeof(nUnicodeFlag));
	}

	reg = GetRegistration();
	if (reg!=registered_good)
			AfxThrowUserException();

	for (int i(0); i<cWriteOrder; i++)
		WriteRecords(ar,strWriteOrder[i]);

	PutLine(ar,_T("0 TRLR\r\n"));

	if (m_bBad)
		AfxMessageBox(_T("Warning: Some of the (Unicode) characters in this Gedcom document ")
			_T("could not be translated into ASCII. Those characters will be converted to ")
			_T("question marks (\"?\") in the ")
			_T("ASCII file that was just saved.\n\nIf you want to preserve your Unicode characters, ")
			_T("choose the option to save your Gedcom documents as Unicode, and then save ")
			_T("this document again."));
}

void CGedtreeDoc::WriteRecords(CArchive& ar, const CString& strTok)
{
	HTREEITEM htvi = m_tree.GetRootItem();

	reg_t reg = GetRegistration();
	if (reg!=registered_good)
			AfxThrowUserException();

	while (htvi)
	{
		CGedLine* pgl = (CGedLine*)m_tree.GetItemData(htvi);
		if (strTok.IsEmpty())
		{
			BOOL bFound(FALSE);
			if (!pgl->m_strTok.IsEmpty()) // on the off chance tok is empty, don't look
				for (int i(0); i<cWriteOrder && !bFound; i++)
					bFound = pgl->m_strTok==strWriteOrder[i];
			if (!bFound)
				WriteItem(ar,htvi);
		}
		else
		{
			if
				(pgl->m_strTok==strTok &&
				(strTok=="HEAD"||!pgl->m_strID.IsEmpty()))
			{
				WriteItem(ar,htvi);
			}
		}
		htvi = m_tree.GetNextSiblingItem(htvi);
	}
}

void CGedtreeDoc::WriteItem(CArchive& ar, HTREEITEM hTreeItem)
{
	// First write this item.
	CGedLine* pgl = (CGedLine*)m_tree.GetItemData(hTreeItem);
	CString strLine = pgl->GetGedLine();
	PutLine(ar,strLine);
	m_pPrg->Set(++m_nPrg,strLine);

	// Then write this item's children, recursively.
	HTREEITEM hChild = m_tree.GetChildItem(hTreeItem);
	while (hChild)
	{
		WriteItem(ar,hChild);
		hChild = m_tree.GetNextSiblingItem(hChild);
	}
}

void CGedtreeDoc::GetFromTree()
{
	int iIndi(0);
	int iFami(0);
	int iSour(0);
	int iRepo(0);
	int iNote(0);

	m_bHadXY = false;

	m_pPrg->Reset("Creating primary GEDCOM records.",m_cRootRecord);
	int cRec(0);

	m_rIndividual.SetSize(m_cIndi);
	m_rFamily.SetSize(m_cFami);
	m_rSource.SetSize(m_cSour);
	m_rRepository.SetSize(m_cRepo);
	m_rNote.SetSize(m_cNote);
	HTREEITEM htiCur = m_tree.GetRootItem();
	while (htiCur)
	{
		CGedLine* pgl = (CGedLine*)m_tree.GetItemData(htiCur);
		HTREEITEM htiNext = m_tree.GetNextSiblingItem(htiCur);

		if (pgl->m_strTok=="INDI")
		{
			CIndividual indi(this,htiCur);
			indi.m_strID = pgl->m_strID;
			m_rIndividual[iIndi] = indi;
			m_mapIDToIndividual.SetAt(indi.m_strID,iIndi);
			m_rIndividual[iIndi].SetIndex(iIndi);
			iIndi++;
		}
		else if (pgl->m_strTok=="FAM")
		{
			CFamily fami(this,htiCur);
			fami.m_strID = pgl->m_strID;
			m_rFamily[iFami] = fami;
			m_mapIDToFamily.SetAt(fami.m_strID,iFami);
			m_rFamily[iFami].SetIndex(iFami);
			iFami++;
		}
		else if (pgl->m_strTok=="SOUR")
		{
			CSource sour(this,htiCur);
			sour.m_strID = pgl->m_strID;
			m_rSource[iSour] = sour;
			m_mapIDToSource.SetAt(sour.m_strID,iSour);
			m_rSource[iSour].SetIndex(iSour);
			iSour++;
		}
		else if (pgl->m_strTok=="REPO")
		{
			CRepository repo(this,htiCur);
			repo.m_strID = pgl->m_strID;
			m_rRepository[iRepo] = repo;
			m_mapIDToRepository.SetAt(repo.m_strID,iRepo);
			m_rRepository[iRepo].SetIndex(iRepo);
			iRepo++;
		}
		else if (pgl->m_strTok=="NOTE")
		{
			CNote note(this,htiCur);
			note.m_strID = pgl->m_strID;
			m_rNote[iNote] = note;
			m_mapIDToNote.SetAt(note.m_strID,iNote);
			m_rNote[iNote].SetIndex(iNote);
			iNote++;
		}
		else if (pgl->m_strTok=="HEAD")
		{
			m_head.Set(this,htiCur);
			m_head.m_strID.Empty();
			m_head.GetFromTree();
		}
		else if (pgl->m_strTok=="TRLR")
		{
			DeleteItem(htiCur);
			pgl = NULL;
		}
		else if (pgl->m_strTok=="SUBM")
		{
			DeleteItem(htiCur);
			pgl = NULL;
		}
		else if (pgl->m_strTok=="SUBN")
		{
			DeleteItem(htiCur);
			pgl = NULL;
		}
		if (pgl) m_pPrg->Set(++cRec,pgl->m_strID);

		htiCur = htiNext;
	}

	int i;

	m_pPrg->Reset("Parsing individual records.",m_rIndividual.GetSize());
	cRec = 0;
	for (i = 0; i<m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = m_rIndividual[i];
		if (indi.GetFromTree())
			m_bHadXY = true;
		m_pPrg->Set(++cRec,indi.Name());
	}

	m_pPrg->Reset("Parsing family records.",m_rFamily.GetSize());
	cRec = 0;
	for (i = 0; i<m_rFamily.GetSize(); i++)
	{
		CFamily& fami = m_rFamily[i];
		fami.GetFromTree();
		if (fami.m_iHusband >= 0)
			m_pPrg->Set(++cRec,m_rIndividual[fami.m_iHusband].Name());
		else
			m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset("Parsing source records.",m_rSource.GetSize());
	cRec = 0;
	for (i = 0; i<m_rSource.GetSize(); i++)
	{
		CSource& sour = m_rSource[i];
		sour.GetFromTree();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset("Parsing repository records.",m_rRepository.GetSize());
	cRec = 0;
	for (i = 0; i<m_rRepository.GetSize(); i++)
	{
		CRepository& repo = m_rRepository[i];
		repo.GetFromTree();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset("Parsing note records.",m_rNote.GetSize());
	cRec = 0;
	for (i = 0; i<m_rNote.GetSize(); i++)
	{
		CNote& note = m_rNote[i];
		note.GetFromTree();
		m_pPrg->Set(++cRec);
	}

	ResetIDs();

	m_mapIDToIndividual.RemoveAll();
	m_mapIDToFamily.RemoveAll();
	m_mapIDToSource.RemoveAll();
	m_mapIDToRepository.RemoveAll();
	m_mapIDToNote.RemoveAll();

	m_pPrg->Reset("Calculating individuals.",m_rIndividual.GetSize());
	cRec = 0;

	for (i = 0; i<m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = m_rIndividual[i];
		indi.Calc();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset("Calculating families.",m_rFamily.GetSize());
	cRec = 0;

	for (i = 0; i<m_rFamily.GetSize(); i++)
	{
		CFamily& fami = m_rFamily[i];
		fami.Calc();
		m_pPrg->Set(++cRec);
	}

	Normalize();
}

int CGedtreeDoc::LookupIndividual(const CString& strID)
{
	int i(-1);
	if (!m_mapIDToIndividual.Lookup(strID,i))
		i = -1;
	return i;
}

int CGedtreeDoc::LookupFamily(const CString& strID)
{
	int i(-1);
	if (!m_mapIDToFamily.Lookup(strID,i))
		i = -1;
	return i;
}

int CGedtreeDoc::LookupSource(const CString& strID)
{
	int i(-1);
	if (!m_mapIDToSource.Lookup(strID,i))
		i = -1;
	return i;
}

int CGedtreeDoc::LookupRepository(const CString& strID)
{
	int i(-1);
	if (!m_mapIDToRepository.Lookup(strID,i))
		i = -1;
	return i;
}

int CGedtreeDoc::LookupNote(const CString& strID)
{
	int i(-1);
	if (!m_mapIDToNote.Lookup(strID,i))
		i = -1;
	return i;
}

/////////////////////////////////////////////////////////////////////////////
// CGedtreeDoc diagnostics

#ifdef _DEBUG
void CGedtreeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGedtreeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGedtreeDoc commands

void CGedtreeDoc::OnCloseDocument() 
{
	HTREEITEM htvi(NULL);
	while (GetNextTraverse(htvi))
	{
		DeleteGedLine(htvi);
	}
	CDocument::OnCloseDocument();
}

BOOL CGedtreeDoc::GetNextTraverse(HTREEITEM& htvi, int* cLev)
{
	// first time, so get the root item (if there is one)
	if (!htvi)
	{
		htvi = m_tree.GetRootItem();
		if (cLev) *cLev = 0; // top level
		return !!htvi;
	}

	// if there is a child, get it
	HTREEITEM htviChild = m_tree.GetChildItem(htvi);
	if (htviChild)
	{
		htvi = htviChild;
		if (cLev) (*cLev)++; // one level down
		return TRUE;
	}

	// no children, so get sibling
	HTREEITEM htviSibling = m_tree.GetNextSiblingItem(htvi);
	if (htviSibling)
	{
		htvi = htviSibling;
		// same level
		return TRUE;
	}

	// no children or siblings, so go up chain to first uncle
	HTREEITEM htviParent(htvi);
	do
	{
		htviParent = m_tree.GetParentItem(htviParent);
		if (!htviParent)
		{
			// no parent, so we're back at the top (all done)
			return FALSE;
		}

		if (cLev) (*cLev)--; // one level up
		htvi = m_tree.GetNextSiblingItem(htviParent);
	}
	while (!htvi);
	return TRUE;
}

void CGedtreeDoc::CalcNormalBounds()
{
	m_rectUnscaledBoundsNoBorder.SetRectEmpty();
	for (int i(0); i<m_rIndividual.GetSize(); i++)
	{
		MyRect rect = m_rIndividual[i].GetUnscaledFrameRect();
		if (rect.right>MAX_BOUNDS.cx || rect.bottom>MAX_BOUNDS.cy)
		{
			m_rIndividual[i].MoveTo(CPoint(30,30));
		}
		m_rectUnscaledBoundsNoBorder |= rect;
	}

	CalcBounds();
}

void CGedtreeDoc::CalcBounds()
{
	if (m_rectUnscaledBoundsNoBorder.Width()<MIN_BOUNDS.cx)
		m_rectUnscaledBoundsNoBorder.right = m_rectUnscaledBoundsNoBorder.left+MIN_BOUNDS.cx;
	if (m_rectUnscaledBoundsNoBorder.Height()<MIN_BOUNDS.cy)
		m_rectUnscaledBoundsNoBorder.bottom = m_rectUnscaledBoundsNoBorder.top+MIN_BOUNDS.cy;
	if (m_rectUnscaledBoundsNoBorder.Width()>MAX_BOUNDS.cx)
		m_rectUnscaledBoundsNoBorder.right = m_rectUnscaledBoundsNoBorder.left+MAX_BOUNDS.cx;
	if (m_rectUnscaledBoundsNoBorder.Height()>MAX_BOUNDS.cy)
		m_rectUnscaledBoundsNoBorder.bottom = m_rectUnscaledBoundsNoBorder.top+MAX_BOUNDS.cy;

	m_rectUnscaledBounds = m_rectUnscaledBoundsNoBorder;
	m_rectUnscaledBounds.InflateRect(CSize(VIEW_BORDER,VIEW_BORDER));

	CalcScale();

	if (m_pDL) m_pDL->SetTotalSize();
}

void CGedtreeDoc::CalcScale()
{
	double nScale(1);
	if (m_pDL) nScale = m_pDL->GetScale();

	m_rectBounds.top    = m_rectUnscaledBounds.top   /nScale;
	m_rectBounds.left   = m_rectUnscaledBounds.left  /nScale;
	m_rectBounds.bottom = m_rectUnscaledBounds.bottom/nScale;
	m_rectBounds.right  = m_rectUnscaledBounds.right /nScale;
	m_rectBoundsNoBorder.top    = m_rectUnscaledBoundsNoBorder.top   /nScale;
	m_rectBoundsNoBorder.left   = m_rectUnscaledBoundsNoBorder.left  /nScale;
	m_rectBoundsNoBorder.bottom = m_rectUnscaledBoundsNoBorder.bottom/nScale;
	m_rectBoundsNoBorder.right  = m_rectUnscaledBoundsNoBorder.right /nScale;
}

CRect CGedtreeDoc::GetBounds()
{
	return m_rectBounds;
}

CRect CGedtreeDoc::GetUnscaledBounds()
{
	return m_rectUnscaledBounds;
}

CRect CGedtreeDoc::GetUnscaledBoundsNoBorder()
{
	return m_rectUnscaledBoundsNoBorder;
}

CSize CGedtreeDoc::Normalize()
{
	CalcNormalBounds();

	CSize szShift = -m_rectUnscaledBounds.TopLeft();

	if (szShift.cx||szShift.cy)
	{
		ShiftAllIndividuals(szShift);

		m_rectUnscaledBounds += szShift;
		m_rectUnscaledBoundsNoBorder += szShift;
		CalcScale();
		if (m_pDL) m_pDL->SetTotalSize();
	}
	return szShift;
}

void CGedtreeDoc::ShiftAllIndividuals(const CSize& sizShift)
{
	for (int i(0); i<m_rIndividual.GetSize(); i++)
	{
		m_rIndividual[i].ShiftUnscaled(sizShift);
	}
}

void CGedtreeDoc::ClearAllIndividuals()
{
	for (int i(0); i<m_rIndividual.GetSize(); i++)
	{
		m_rIndividual[i].m_bMark = FALSE;
	}
}

BOOL CGedtreeDoc::HitsIndi(CIndividual* pIndiTest)
{
	BOOL bHit(FALSE);
	for (int i(0); !bHit && i<m_rIndividual.GetSize(); i++)
	{
		if (i!=pIndiTest->m_i)
			bHit = m_rIndividual[i].HitTest(pIndiTest->GetFrameRect());
	}
	return bHit;
}

void CGedtreeDoc::AutoClean()
{
	if (m_rIndividual.GetSize())
	{
		ClearAllIndividuals();

//		CPoint pt(MAX_BOUNDS.cx/2,MAX_BOUNDS.cy/2);
		CPoint pt(5000,5000);
		for (int i(0); i<m_rIndividual.GetSize(); i++)
		{
			CIndividual& indi = m_rIndividual[i];
			if (!indi.m_bMark)
			{
				indi.MoveTo(pt);
				pt.Offset(20,20);
				indi.Clean(INT_MAX);
			}
		}

		ClearAllIndividuals();
	}
}

CIndividual* CGedtreeDoc::Individual(int i)
{
	return &m_rIndividual[i];
}

CFamily* CGedtreeDoc::Family(int i)
{
	return &m_rFamily[i];
}

HTREEITEM CGedtreeDoc::ResetSubValue(HTREEITEM hTreeItem, const CString& strTok, const CString& strVal,
	BOOL bAlwaysAdd, BOOL bEvenIfEmpty, BOOL bBreakIntoCont)
{
	HTREEITEM hChild = NULL;

	BOOL bFound(FALSE);
	int cLev(-1);

	// if strval is empty, but evenifempty is true, we
	// consider strval not empty:
	BOOL bEmpty = strVal.IsEmpty() && !bEvenIfEmpty;

	if (hTreeItem!=TVI_ROOT)
	{
		CGedLine* pgl = (CGedLine*)m_tree.GetItemData(hTreeItem);
		cLev = pgl->Level();
	}

	if (!bAlwaysAdd)
	{
		hChild = m_tree.GetChildItem(hTreeItem);
		if (hChild)
		{
			CGedLine* pglChild = (CGedLine*)m_tree.GetItemData(hChild);
			bFound = pglChild->m_strTok==strTok;
			while (hChild && !bFound)
			{
				hChild = m_tree.GetNextSiblingItem(hChild);
				if (hChild)
				{
					pglChild = (CGedLine*)m_tree.GetItemData(hChild);
					bFound = pglChild->m_strTok==strTok;
				}
			}
			if (bFound)
			{
				if (bEmpty || bBreakIntoCont)
				{
					DeleteItem(hChild);
					if (bBreakIntoCont) bFound = FALSE;
				}
				else
					pglChild->m_strVal = strVal;
				SetModifiedFlag();
			}
		}
	}

	if (!bFound && !bEmpty)
	{
		if (bBreakIntoCont)
		{
			CArray<CString,LPCTSTR> rstrTok, rstrVal;
			BreakContLines(strVal,rstrTok,rstrVal);

			hChild = InsertChild(strTok,hTreeItem,cLev+1,rstrVal[0]);
			for (int i(1); i<rstrTok.GetSize(); i++)
				InsertChild(rstrTok[i],hChild,cLev+2,rstrVal[i]);
		}
		else
			hChild = InsertChild(strTok,hTreeItem,cLev+1,strVal);
	}

	return hChild;
}

HTREEITEM CGedtreeDoc::InsertChild(const CString& strTok, HTREEITEM hTreeItem, int cLev, const CString& strVal)
{
	HTREEITEM htviCur = m_tree.InsertItem(_T(""),hTreeItem);

	CGedLine* pglChild = new CGedLine(this,"");
	pglChild->m_strTok = strTok;
	pglChild->m_strVal = strVal;
	pglChild->m_nLev = cLev;

	m_tree.SetItemData(htviCur,(DWORD)pglChild);

	SetModifiedFlag();

	return htviCur;
}

void CGedtreeDoc::DeleteSubValue(HTREEITEM hTreeItem, const CString& strTok)
{
	// check sub records of hTreeItem for strTok
	CArray<HTREEITEM,HTREEITEM> rhChild;
	HTREEITEM hChild = m_tree.GetChildItem(hTreeItem);
	if (hChild)
	{
		CGedLine* pglChild = (CGedLine*)m_tree.GetItemData(hChild);
		if (pglChild->m_strTok==strTok)
			rhChild.Add(hChild);
		while (hChild)
		{
			hChild = m_tree.GetNextSiblingItem(hChild);
			if (hChild)
			{
				pglChild = (CGedLine*)m_tree.GetItemData(hChild);
				if (pglChild->m_strTok==strTok)
					rhChild.Add(hChild);
			}
		}
		for (int i(0); i<rhChild.GetSize(); i++)
			DeleteItem(rhChild[i]);
	}
}

void CGedtreeDoc::DeleteItem(HTREEITEM hTreeItem)
{
	// First delete this item's children, recursively.
	HTREEITEM hChild = m_tree.GetChildItem(hTreeItem);
	while (hChild)
	{
		HTREEITEM hSib = m_tree.GetNextSiblingItem(hChild);
		DeleteItem(hChild);
		hChild = hSib;
	}

	// Then delete this item.
	DeleteGedLine(hTreeItem);
	m_tree.DeleteItem(hTreeItem);
	SetModifiedFlag();
}

void CGedtreeDoc::DeleteGedLine(HTREEITEM hTreeItem)
{
	CGedLine* pgl = (CGedLine*)m_tree.GetItemData(hTreeItem);
	delete pgl;
}

void CGedtreeDoc::ResetToken(HTREEITEM hTreeItem, const CString& strTok, const CString& strVal)
{
	if (strTok.IsEmpty()) return;

	CGedLine* pgl = (CGedLine*)m_tree.GetItemData(hTreeItem);
	pgl->m_strTok = strTok;
	pgl->m_strVal = strVal;
	pgl->Calc();
	SetModifiedFlag();
}

void CGedtreeDoc::Init()
{
	POSITION pos = GetFirstViewPosition();
	m_pDL = (CGedtreeViewDL*)GetNextView(pos); // first view is DropLine view
}

BOOL CGedtreeDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	// BEGIN from CDocument::OnOpenDocument()
	CFileException fe;
	CFile* pFile = GetFile(lpszPathName,
		CFile::modeRead|CFile::shareDenyWrite, &fe);
	if (pFile == NULL)
	{
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	// END from CDocument::OnOpenDocument()

	TestUnicode(pFile);

	// BEGIN from CDocument::OnOpenDocument()
	ReleaseFile(pFile, FALSE);
	// END from CDocument::OnOpenDocument()

//	if (!CDocument::OnOpenDocument(lpszPathName))
//		return FALSE;

	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	pFile = GetFile(lpszPathName,
		CFile::modeRead|CFile::shareDenyWrite, &fe);
	if (pFile == NULL)
	{
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

	CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete, 256*1024);
	loadArchive.m_pDocument = this;
	loadArchive.m_bForceFlat = FALSE;
	TRY
	{
		CWaitCursor wait;
		if (pFile->GetLength() != 0)
			Serialize(loadArchive);     // load me
		loadArchive.Close();
		ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e)
	{
		ReleaseFile(pFile, TRUE);
		DeleteContents();   // remove failed contents

		TRY
		{
			ReportSaveLoadException(lpszPathName, e,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}
		END_TRY
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

	SetModifiedFlag(FALSE);     // start off with unmodified

//	return TRUE;

	Init();

	if (!m_bHadXY)
		m_pDL->CleanAll();

	return TRUE;
}

BOOL CGedtreeDoc::OnNewDocument() 
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	Init();
	return TRUE;
}

void CGedtreeDoc::GetContLines(HTREEITEM htiParent, CString& strValue)
{
	// Search children of htiParent for CONT or CONC lines
	// and append values to strValue.
	HTREEITEM htiSub = m_tree.GetChildItem(htiParent);
	while (htiSub)
	{
		CGedLine* pglChild = (CGedLine*)m_tree.GetItemData(htiSub);

		if (pglChild->m_strTok=="CONT")
		{
			strValue += "\r\n"+pglChild->m_strVal;
		}
		else if (pglChild->m_strTok=="CONC")
		{
			strValue += pglChild->m_strVal;
		}
		else
		{
			// reached non-cont/conc line, so stop
			return;
		}

		htiSub = m_tree.GetNextSiblingItem(htiSub);
	}
}

const int ccMaxContLine(70);

void CGedtreeDoc::BreakContLines(const CString& strValue, CArray<CString,LPCTSTR>& rstrTok, CArray<CString,LPCTSTR>& rstrVal)
{
	// Toks are CONT or CONC for corresponding Vals
	rstrTok.RemoveAll();
	rstrVal.RemoveAll();

	CString strRest(strValue);

	int ieol;
	while ((ieol = strRest.Find(_T("\r\n"))) >= 0)
	{
		BreakConcLines(strRest.Left(ieol),rstrTok,rstrVal);
		strRest = strRest.Mid(ieol+2);
	}
	BreakConcLines(strRest,rstrTok,rstrVal);
}

void CGedtreeDoc::BreakConcLines(const CString& strValue, CArray<CString,LPCTSTR>& rstrTok, CArray<CString,LPCTSTR>& rstrVal)
{
	CString strLine(strValue);
	rstrTok.Add(CString("CONT"));
	while (strLine.GetLength()>ccMaxContLine)
	{
		int ic = ccMaxContLine;
		while (ic&&(strLine[ic]==' '||strLine[ic-1]==' ')) ic--;
		if (!ic)
		{
			// we can't break this line up correctly (like for example
			// a "- - - -" pattern that's longer than ccMaxContLine,
			// so just break it at ccMaxContLine, regardless.
			ic = ccMaxContLine;
		}
		rstrVal.Add(CString(strLine.Left(ic)));
		rstrTok.Add(CString("CONC"));
		strLine = strLine.Mid(ic);
	}
	rstrVal.Add(strLine);
}

void CGedtreeDoc::ResetAllViews(UINT flagsChanged)
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
		((CGedtreeView*)GetNextView(pos))->Reset(flagsChanged);
}

void CGedtreeDoc::UpdateTitle()
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
		((CGedtreeView*)GetNextView(pos))->UpdateTitle();
}

void CGedtreeDoc::ResetIDs()
{
	int i, cRec;

	m_pPrg->Reset(
		"Recalculating individual IDs.",
		m_rIndividual.GetSize());
	cRec = 0;
	for (i = 0; i<m_rIndividual.GetSize(); i++)
	{
		CIndividual& indi = m_rIndividual[i];
		indi.CalcID();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset(
		"Recalculating family IDs.",
		m_rFamily.GetSize());
	cRec = 0;
	for (i = 0; i<m_rFamily.GetSize(); i++)
	{
		CFamily& fami = m_rFamily[i];
		fami.CalcID();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset(
		"Recalculating source IDs.",
		m_rSource.GetSize());
	cRec = 0;
	for (i = 0; i<m_rSource.GetSize(); i++)
	{
		CSource& sour = m_rSource[i];
		sour.CalcID();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset(
		"Recalculating repository IDs.",
		m_rRepository.GetSize());
	cRec = 0;
	for (i = 0; i<m_rRepository.GetSize(); i++)
	{
		CRepository& repo = m_rRepository[i];
		repo.CalcID();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset(
		"Recalculating note IDs.",
		m_rNote.GetSize());
	cRec = 0;
	for (i = 0; i<m_rNote.GetSize(); i++)
	{
		CNote& note = m_rNote[i];
		note.CalcID();
		m_pPrg->Set(++cRec);
	}

	m_pPrg->Reset(
		"Resetting all IDs.",
		m_tree.GetCount());
	cRec = 0;
	HTREEITEM htvi(NULL);
	while (GetNextTraverse(htvi))
	{
		CGedLine* pgl = (CGedLine*)m_tree.GetItemData(htvi);
		pgl->CalcID();
		m_pPrg->Set(++cRec,pgl->m_strID);
	}
}

BOOL CGedtreeDoc::HasChildren(const CGedRecord& recParent)
{
	CGedLine* pglParent = (CGedLine*)m_tree.GetItemData(recParent.m_hTreeItem);

	BOOL bFound(FALSE);
	HTREEITEM htvi(NULL);
	while (!bFound && GetNextTraverse(htvi))
	{
		CGedLine* pgl = (CGedLine*)m_tree.GetItemData(htvi);
		bFound = pgl->IsChildOf(pglParent->m_strTok,pglParent->m_strID);
	}
	return bFound;
}

void CGedtreeDoc::OnFileSaveAs() 
{
	if (DoSave(NULL))
		UpdateTitle();
}

void CGedtreeDoc::OnFileSave() 
{
	if (DoFileSave())
		UpdateTitle();

	for (int i(0); i<m_rIndividual.GetSize(); i++)
	{
		m_rIndividual[i].DebugRelations();
	}
	for (i = 0; i<m_rFamily.GetSize(); i++)
	{
		m_rFamily[i].DebugRelations();
	}
}

void CGedtreeDoc::OnEditResizedocument() 
{
	MyRect r = m_rectUnscaledBoundsNoBorder;
	CDocSize d;

	d.m_nWidth = r.Width();
	d.m_nHeight = r.Height();
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		r.right = r.left+d.m_nWidth;
		r.bottom = r.top+d.m_nHeight;
		m_rectUnscaledBoundsNoBorder = r;
		CalcBounds();
	}
	else if (nButton==IDC_NORMALIZE)
	{
		Normalize();
	}
	ResetAllViews();
}

void CGedtreeDoc::OnViewSources() 
{
	// TODO: Add your command handler code here

	CPickSource d(this,NULL,TRUE);
	d.DoModal();
}

void CGedtreeDoc::OnViewRepositories() 
{
	// TODO: Add your command handler code here
	
	CPickRepo d(this,NULL,TRUE);
	d.DoModal();
}

void CGedtreeDoc::OnViewIndexofindividuals() 
{
	if (m_pIndiList)
	{
		m_pIndiList->ActivateFrame(SW_SHOW);
	}
	else
	{
		POSITION pos = theApp.GetFirstDocTemplatePosition();
		CDocTemplate* p = theApp.GetNextDocTemplate(pos); // DL tmpl
		p = theApp.GetNextDocTemplate(pos); // IN tmpl
		p = theApp.GetNextDocTemplate(pos); // PD tmpl
		p = theApp.GetNextDocTemplate(pos); // IL tmpl
		m_pIndiList = (CChildFrame*)p->CreateNewFrame(this,NULL);
		p->InitialUpdateFrame(m_pIndiList,this);
		((CGedtreeViewIL*)m_pIndiList->GetActiveView())->Init();
		p->InitialUpdateFrame(m_pIndiList,this);
	}
}

void CGedtreeDoc::OnViewNotes() 
{
	CHeadNotes d;
	d.m_strText = m_head.m_strNote;
	int nButton = d.DoModal();
	if (nButton==IDOK)
	{
		if (d.m_strText!=m_head.m_strNote)
		{
			m_head.m_strNote = d.m_strText;
			m_head.PutToTree();
		}
	}
}

void CGedtreeDoc::SetInputCharSet(const CString& str)
{
	if (m_bUnicode)
	{
		// If the file is a Unicode file, then we should have
		// already determined so. So here we just ignore the str.
	}
	else
	{
		//??? check for MAC or WIN, etc.
		if (str=="ANSEL")
		{
			m_bAnsel = TRUE;
		}
		else if (str=="ASCII")
		{
			// nothing
		}
		else if (str=="UNICODE")
		{
			// warning?
		}
		else
		{
		}
	}
}

static CGedtreeDoc* staticpDoc;

static int __cdecl compareIndi(const void *elem1, const void *elem2)
{
	int i = *(int*)(elem1);
	int j = *(int*)(elem2);
	CIndividual& indi1 = staticpDoc->m_rIndividual[i];
	CIndividual& indi2 = staticpDoc->m_rIndividual[j];
	CString sb1, sb2;
	if (indi1.m_iBirth>=0)
		sb1.Format(L"%04d",indi1.m_revt[indi1.m_iBirth].m_dvDate.GetSimpleYear());
	if (indi2.m_iBirth>=0)
		sb2.Format(L"%04d",indi2.m_revt[indi2.m_iBirth].m_dvDate.GetSimpleYear());
	CString s1 = indi1.m_name.Surname()+" "+indi1.m_name.GivenName()+" "+sb1;
	CString s2 = indi2.m_name.Surname()+" "+indi2.m_name.GivenName()+" "+sb2;
	return s1.CollateNoCase(s2);
}

static int __cdecl compareSour(const void *elem1, const void *elem2)
{
	int i = *(int*)(elem1);
	int j = *(int*)(elem2);
	CSource& sour1 = staticpDoc->m_rSource[i];
	CSource& sour2 = staticpDoc->m_rSource[j];
	CString s1, s2;
	s1 = sour1.m_strAuthor+" "+sour1.m_strTitle;
	s2 = sour2.m_strAuthor+" "+sour2.m_strTitle;
	return s1.CollateNoCase(s2);
}

CArray<int,int>* CGedtreeDoc::GetSortedIndis()
{
	if (m_bsortIndiNeedsUpdate)
	{
		int n = m_rIndividual.GetSize();

		CProgress prg;
		prg.Reset("Sorting individuals",n-1);

		m_rsortIndi.SetSize(n);
		for (int i(0); i<n; ++i)
		{
			m_rsortIndi[i] = i;
		}

		staticpDoc = this;
		for (i = 0; i<n-1; ++i)
		{
			prg.Set(i+1);
			for (int j(i+1); j<n; ++j)
			{
//				int cmp = compareIndi(&m_rsortIndi[i],&m_rsortIndi[j]);
				CIndividual& indi1 = m_rIndividual[m_rsortIndi[i]];
				CIndividual& indi2 = m_rIndividual[m_rsortIndi[j]];
				int cmp = indi1.m_name.Surname().CollateNoCase(indi2.m_name.Surname());
				if (!cmp)
				{
					cmp = indi1.m_name.GivenName().CollateNoCase(indi2.m_name.GivenName());
					if (!cmp)
					{
						int b1, b2;
						b1 = b2 = 99999;
						if (indi1.m_iBirth>=0)
							b1 = indi1.m_revt[indi1.m_iBirth].m_dvDate.GetSimpleYear();
						if (indi2.m_iBirth>=0)
							b2 = indi2.m_revt[indi2.m_iBirth].m_dvDate.GetSimpleYear();
						cmp = (b1<b2) ? -1 : (b1>b2) ? 1 : 0;
					}
				}

				if (cmp > 0)
				{
					int tmp = m_rsortIndi[i];
					m_rsortIndi[i] = m_rsortIndi[j];
					m_rsortIndi[j] = tmp;
				}
			}
		}

		static const int siz(10);
		int cLevel(1);
		--n;
		while (n /= siz)
			cLevel++;
		mcLevel = cLevel;

		m_bsortIndiNeedsUpdate = false;
	}
	return &m_rsortIndi;
}

void CGedtreeDoc::GetSortedSours(CArray<int,int>& rSour)
{
	staticpDoc = this;

	int n = m_rSource.GetSize();
	int* rLocalSour = new int[n];

	for (int i(0); i<n; i++)
		rLocalSour[i] = i;

	qsort(rLocalSour,n,sizeof(int), compareSour);

	rSour.SetSize(n);
	for (i = 0; i<n; i++)
		rSour[i] = rLocalSour[i];

	delete [] rLocalSour;
}

void CGedtreeDoc::SavePathToReg(int i)
{
	CString s;
	s.Format(L"%d",i);
	theApp.PutReg("OpenDocuments",s,GetPathName());
}

int CGedtreeDoc::PasteIndi(CIndividual& indiFrom)
{
	HTREEITEM hti = InsertChild(_T("INDI"));
	CIndividual indi(indiFrom);
	indi.Set(this,hti);
	indi.m_bSelected = TRUE;
	indi.m_bHidden = FALSE;
	indi.m_bMark = FALSE;
	int i = m_rIndividual.Add(indi);
	m_rIndividual[i].SetIndex(i);
	m_rIndividual[i].CalcID();

	CIndividual& indid = m_rIndividual[i];

	//indid.Calc();//???

	ResetSubValue(indid.m_hTreeItem,"_XY",indid.GetXY(indid.m_rectFrame.TopLeft()));


	return i;
}

void CGedtreeDoc::FixUpPastedIndi(int i)
{
	CIndividual& indi = m_rIndividual[i];

	indi.m_name.Set(this,NULL);
	indi.m_name.m_cita.Set(this,NULL);
	indi.m_name.PutToTree(indi.m_hTreeItem);
	m_bsortIndiNeedsUpdate = true;

	if (indi.m_nSex==1)
		ResetSubValue(indi.m_hTreeItem,"SEX","M");
	else if (indi.m_nSex==2)
		ResetSubValue(indi.m_hTreeItem,"SEX","F");

	int k;
	for (k = 0; k<indi.m_riSpouseToFamily.GetSize(); ++k)
	{
		CFamily& famiS = m_rFamily[indi.m_riSpouseToFamily[k]];
		ResetSubValue(indi.m_hTreeItem,"FAMS",famiS.GetID(),TRUE);
	}
	if (indi.m_iChildToFamily >= 0)
	{
		CFamily& famiC = m_rFamily[indi.m_iChildToFamily];
		ResetSubValue(indi.m_hTreeItem,"FAMC",famiC.GetID(),TRUE);
	}

	for (int j(0); j<indi.m_revt.GetSize(); ++j)
	{
		CEvt& evt = indi.m_revt[j];
		HTREEITEM htievt = InsertChild(_T("EVEN"),indi.m_hTreeItem,1);
		evt.Set(this,htievt);
		evt.m_cita.Set(this,NULL);
		evt.PutToTree();
	}

	for (j = 0; j<indi.m_rattr.GetSize(); ++j)
	{
		CAttr& attr = indi.m_rattr[j];
		HTREEITEM htiattr = InsertChild(_T("TITL"),indi.m_hTreeItem,1);
		attr.Set(this,htiattr);
		attr.m_evt.Set(this,NULL);
		attr.m_evt.m_cita.Set(this,NULL);
		attr.PutToTree();
	}

	//??? anything else to do?
}

int CGedtreeDoc::PasteFami(CFamily& famiFrom)
{
	HTREEITEM hti = InsertChild("FAM");
	CFamily fami(famiFrom);
	fami.Set(this,hti);
	int i = m_rFamily.Add(fami);
	m_rFamily[i].SetIndex(i);
	m_rFamily[i].CalcID();
	if (fami.m_iHusband >= 0)
	{
		CIndividual& indi = m_rIndividual[fami.m_iHusband];
		ResetSubValue(fami.m_hTreeItem,"HUSB",indi.GetID());
	}
	if (fami.m_iWife >= 0)
	{
		CIndividual& indi = m_rIndividual[fami.m_iWife];
		ResetSubValue(fami.m_hTreeItem,"WIFE",indi.GetID());
	}
	for (int j(0); j<fami.m_riChild.GetSize(); ++j)
	{
		CIndividual& indi = m_rIndividual[fami.m_riChild[j]];
		ResetSubValue(fami.m_hTreeItem,"CHIL",indi.GetID(),TRUE);
	}

	for (j = 0; j<fami.m_revt.GetSize(); ++j)
	{
		CEvt& evt = fami.m_revt[j];
		HTREEITEM htievt = InsertChild(_T("EVEN"),fami.m_hTreeItem,1);
		evt.Set(this,htievt);
		evt.m_cita.Set(this,NULL);
		evt.PutToTree();
	}

	return i;
}

int CGedtreeDoc::PasteSour(CSource& sourFrom)
{
	HTREEITEM hti = InsertChild("SOUR");
	CSource sour(sourFrom);
	sour.Set(this,hti);
	int i = m_rSource.Add(sour);
	m_rSource[i].SetIndex(i);
	m_rSource[i].CalcID();
	m_rSource[i].PutToTree();
	return i;
}

int CGedtreeDoc::PasteRepo(CRepository& repoFrom)
{
	HTREEITEM hti = InsertChild("REPO");
	CRepository repo(repoFrom);
	repo.Set(this,hti);
	int i = m_rRepository.Add(repo);
	m_rRepository[i].SetIndex(i);
	m_rRepository[i].CalcID();
	m_rRepository[i].PutToTree();
	return i;
}

int CGedtreeDoc::PasteNote(CNote& noteFrom)
{
	HTREEITEM hti = InsertChild("NOTE");
	CNote note(noteFrom);
	note.Set(this,hti);
	int i = m_rNote.Add(note);
	m_rNote[i].SetIndex(i);
	m_rNote[i].CalcID();
	m_rNote[i].PutToTree();
	return i;
}

void CGedtreeDoc::ClearContents()
{
	HTREEITEM htvi(NULL);
	while (GetNextTraverse(htvi))
		DeleteGedLine(htvi);
	m_tree.DeleteAllItems();
	ResetSubValue(TVI_ROOT,"HEAD","",TRUE,TRUE);
	m_iSourceLast = -1;
	m_rIndividual.RemoveAll();
	m_mapIDToIndividual.RemoveAll();
	m_rFamily.RemoveAll();
	m_mapIDToFamily.RemoveAll();
	m_rSource.RemoveAll();
	m_mapIDToSource.RemoveAll();
	m_rRepository.RemoveAll();
	m_mapIDToRepository.RemoveAll();
	m_rNote.RemoveAll();
	m_mapIDToNote.RemoveAll();
	mcLevel = -1;
	m_cIndi = 0;
	m_cFami = 0;
	m_cSour = 0;
	m_cRepo = 0;
	m_cNote = 0;
}
extern wstring toHTTPTime(SYSTEMTIME* st);
/*
void CGedtreeDoc::svgChart(const wstring& docid, thinsock& sock)
{
	wostringstream os;
	os << L"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << endl2;
	os << L"<svg width=\"100%\" height=\"100%\">" << endl2;
	os << L"<desc>" << docid << L"</desc>" << endl2;

	LOGFONT lf;
	theApp.m_font.GetLogFont(&lf);
	int ptsize = -lf.lfHeight;

	theApp.ResetFont();
	TEXTMETRIC tm;
	::GetTextMetrics(theApp.GetDC()->m_hDC,&tm);
	int pt = tm.tmHeight+tm.tmExternalLeading;

	double nVerticalResolution = theApp.GetDC()->GetDeviceCaps(LOGPIXELSY);
	int nPointSize = ((tm.tmHeight-tm.tmInternalLeading)*72)/nVerticalResolution+.50001;

	os << L"<style type=\"text/css\"><![CDATA[" << endl2;
		CString s;
		s.Format(L"text {font-family:\"%s\",sans-serif;font-size:%dpt}",lf.lfFaceName,nPointSize);
		os << (LPCTSTR)s << endl2;
		os << L"line {stroke:black;stroke-width:1}" << endl2;
	os << L"]]></style>" << endl2;
	os << L"<g>" << endl2;

	s.Format(L"<text x=\"20\" y=\"%d\">Right-click for commands and help.</text>",pt+3);
	os << (LPCTSTR)s << endl2;
	s.Format(L"<text x=\"20\" y=\"%d\">Left-click on a person to go to his or her page.</text>",2*pt+3);
	os << (LPCTSTR)s << endl2;

	int i;
	for (i = 0; i < m_rFamily.GetSize(); ++i)
		m_rFamily[i].writeSVG(os);
	for (i = 0; i < m_rIndividual.GetSize(); ++i)
		m_rIndividual[i].writeSVG(os,pt);

	os << L"</g>" << endl2;
	os << L"</svg>" << endl2;

	wstring strdoc = os.str();
	int cb = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),0,0,0,0);
	if (!cb)
	{
		::OutputDebugString(CString(L"svgChart WideCharToMultiByte (1)"));
		throw 404;
	}

	char* pb = new char[cb];
	int ok = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),pb,cb,0,0);
	if (!ok)
	{
		delete [] pb;
		::OutputDebugString(CString(L"svgChart WideCharToMultiByte (2)"));
		throw 404;
	}

	SYSTEMTIME st;
	::GetSystemTime(&st);
	wstring lmodtime = toHTTPTime(&st);

	wostringstream os2;
	os2 << L"HTTP/1.0 200 OK" << endl2;
	os2 << L"Content-Type: image/svg+xml" << endl2;
	os2 << L"Last-Modified: " << lmodtime << endl2;
	os2 << L"Content-Length: " << cb << endl2;
	os2 << L"Connection: Keep-Alive" << endl2;
	os2 << endl2;

	sock.send(os2.str().c_str(),os2.str().length(),0);
	sock.sendbinary(pb,cb,0);

	delete [] pb;
}
*/
void CGedtreeDoc::appletChart(const wstring& docid, thinsock& sock)
{
	if (!m_pAppletChart)
	{
		wostringstream os;
		writeApplet(os);
		wstring strdoc = os.str();
		m_cAppletChart = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),0,0,0,0);
		if (!m_cAppletChart)
		{
			::OutputDebugString(CString(L"appletChart WideCharToMultiByte (1)"));
			throw 404;
		}

		m_pAppletChart = new char[m_cAppletChart];
		int ok = ::WideCharToMultiByte(CP_UTF8,0,strdoc.c_str(),strdoc.length(),m_pAppletChart,m_cAppletChart,0,0);
		if (!ok)
		{
			delete [] m_pAppletChart;
			m_pAppletChart = 0;
			::OutputDebugString(CString(L"appletChart WideCharToMultiByte (2)"));
			throw 404;
		}
	}

	wostringstream os2;
	os2 << L"HTTP/1.0 200 OK" << endl2;
	os2 << L"Content-Type: text/plain; charset=UTF-8" << endl2;
	os2 << L"Content-Length: " << m_cAppletChart << endl2;
	os2 << L"Connection: close" << endl2;
	os2 << endl2;

	sock.send(os2.str().c_str(),os2.str().length(),0);
	sock.sendbinary(m_pAppletChart,m_cAppletChart,0);
}

void CGedtreeDoc::clearAppletChart()
{
	delete [] m_pAppletChart;
	m_pAppletChart = 0;
}

static const int siz(10);
void CGedtreeDoc::htmlIndex(int iLevel, int iBase, const wstring& docid, wostream& os)
{
	vector<int> rBase;
	vector<wstring> rName1;
	vector<wstring> rName2;
	vector<int> rBirth;
	vector<int> rDeath;

	CArray<int,int>* rSrtIndi = GetSortedIndis();

	int xlev = mcLevel-iLevel-1; // 0, 1, 2, 3 becomes 3, 2, 1, 0

	int i;

	long iUp(-1), iPrev(-1), iNext(-1);
	iUp = iBase;
	for (i = 0; i < xlev+2; i++)
		iUp /= siz;
	for (i = 0; i < xlev+2; i++)
		iUp *= siz;

	long iDiff(1);
	for (i = 0; i < xlev+1; i++)
		iDiff *= siz;
	iPrev = iBase-iDiff;
	iNext = iBase+iDiff;
	if (iPrev<0)
		iPrev = -1;
	if (iNext>=rSrtIndi->GetSize())
		iNext = -1;

	if (xlev)
	{
		// index page
		int iSkip(1);
		for (i = 0; i < xlev; i++)
			iSkip *= siz;

		int iEnd(iSkip+iBase-1);
		int iPrevEnd(-1);
		for (i = 0; i < siz; i++)
		{
			if (iBase>=rSrtIndi->GetSize())
				iBase = rSrtIndi->GetSize()-1;

			if (iBase==iPrevEnd) break;

			if (iEnd>=rSrtIndi->GetSize())
				iEnd = rSrtIndi->GetSize()-1;

			CIndividual* pIndi1 = Individual(rSrtIndi->GetAt(iBase));
			CString s1(pIndi1->m_name.Surname());
			s1 += ", "; s1 += pIndi1->m_name.GivenName();

			CIndividual* pIndi2 = Individual(rSrtIndi->GetAt(iEnd));
			CString s2(pIndi2->m_name.Surname());
			s2 += ", "; s2 += pIndi2->m_name.GivenName();

			rBase.push_back(iBase);
			rName1.push_back((LPCTSTR)s1);
			rName2.push_back((LPCTSTR)s2);

			iPrevEnd = iEnd;

			iBase += iSkip;
			iEnd += iSkip;
		}
	}
	else
	{
		// leaf index page
		int iPrevBase(-1);
		for (i = 0; i < siz; i++)
		{
			if (iBase>=rSrtIndi->GetSize())
				iBase = rSrtIndi->GetSize()-1;

			if (iBase==iPrevBase) break;

			CIndividual* pIndi1 = Individual(rSrtIndi->GetAt(iBase));
			CString s1(pIndi1->m_name.Surname());
			s1 += ", "; s1 += pIndi1->m_name.GivenName();

			int iIndi = pIndi1->m_i;

			long nBirth(0), nDeath(0);
			if (pIndi1->m_iBirth>=0)
				nBirth = pIndi1->m_revt[pIndi1->m_iBirth].m_dvDate.GetSimpleYear();
			if (pIndi1->m_iDeath>=0)
				nDeath = pIndi1->m_revt[pIndi1->m_iDeath].m_dvDate.GetSimpleYear();

			// insert indi into recordset
			rBase.push_back(iIndi);
			rName1.push_back((LPCTSTR)s1);
			rBirth.push_back(nBirth);
			rDeath.push_back(nDeath);
			iPrevBase = iBase;

			iBase++;
		}
	}

	os << L"<head>" << endl;
	os << L"<title>" << docid << L" Index</title>" << endl;
	os << L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
	os << L"<link rel=\"stylesheet\" href=\"/css\" type=\"text/css\" media=\"all\" />" << endl;
	os << L"</head>" << endl;
	os << L"<body>" << endl;

//	if (!iLevel)
//	{
		os << L"<p>" << endl;
//		os << L"This web site is generated by Genealogy Research Organizer (GRO).<br />" << endl;
//		os << L"This is the index of all people currently in the database.<br />" << endl;
		os << L"<a href=\"/\">home</a> <a href=\"?chart\">chart</a><br />" << endl;
		if (xlev)
			os << L"Click on a range of people to go down to the next level." << endl;
		else
			os << L"Click on a person to go to his or her page." << endl;
		os << L"</p><hr />" << endl;
//	}

	os << L"<table class=\"indexpage\">" << endl;

	os << L"<tr>" << endl;

	if (iLevel)
		os << L"<td rowspan=\"12\">(<a href=\"?level=0&amp;base=0\">top level</a>)</td>" << endl;

	if (iLevel > 1)
		os << L"<td rowspan=\"12\">(<a href=\"?level=" << iLevel-1 << L"&amp;base=" << iUp << L"\">one level up</a>)</td>" << endl;

	if (iPrev >= 0)
		os << L"<td colspan=\"3\">(<a href=\"?level=" << iLevel << L"&amp;base=" << iPrev << L"\">previous</a>)</td>" << endl;
	else
	{
		if (iLevel > 0)
			os << L"<td colspan=\"3\">(beginning of index)</td>" << endl;
		else
			os << L"<td colspan=\"3\">&nbsp;</td>" << endl;
	}

	os << L"</tr>" << endl;

	for (i = 0; i<rBase.size(); ++i)
	{
		os << L"<tr>" << endl;
		if (!xlev)
		{
			os << L"<td><a href=\"?indi=" << rBase[i] << L"\">" << rName1[i] << L"</a></td>" << endl;
			os << L"<td class=\"dates\">(";
			if (rBirth[i] > 0)
				os << rBirth[i];
			else
				os << L"?";
			os << L"-";
			if (rDeath[i] > 0)
				os << rDeath[i];
			else
				os << L"?";
			os << L")</td>" << endl;
		}
		else
		{
			os << L"<td><a href=\"";
			os << L"?level=" << iLevel+1 << L"&amp;base=" << rBase[i] << L"\">" << rName1[i] << L"</a></td>" << endl;
			os << L"<td><span class=\"thru\"><a href=\"";
			os << L"?level=" << iLevel+1 << L"&amp;base=" << rBase[i] << L"\">through</a></span></td>" << endl;
			os << L"<td><a href=\"";
			os << L"?level=" << iLevel+1 << L"&amp;base=" << rBase[i] << L"\">" << rName2[i] << L"</a></td>" << endl;
		}
		os << L"</tr>" << endl;
	}
	os << L"<tr>" << endl;
	os << L"<td colspan=\"3\">";
	if (iNext >= 0)
		os << L"(<a href=\"?level=" << iLevel << L"&amp;base=" << iNext << L"\">next</a>)" << endl;
	else if (iLevel > 0)
		os << L"(end of index)" << endl;
	else
		os << L"&nbsp;" << endl;
	os << L"</td>" << endl;
	os << L"</tr></table>" << endl;

	theApp.htmlFooter(os);
	os << L"</body>" << endl;
}

void CGedtreeDoc::htmlIndi(int iIndi, const wstring& docid, wostream& os)
{
	if (iIndi < 0 || m_rIndividual.GetSize() <= iIndi)
	{
		::OutputDebugString(CString(L"htmlIndi"));
		throw 404;
	}

	CIndividual* pIndi = Individual(iIndi);

	CString strBirth(L"?");
	if (pIndi->m_iBirth>=0)
	{
		long n = pIndi->m_revt[pIndi->m_iBirth].m_dvDate.GetSimpleYear();
		if (n)
			strBirth.Format(L"%d",n);
	}

	CString strDeath(L"?");
	if (pIndi->m_iDeath>=0)
	{
		long n = pIndi->m_revt[pIndi->m_iDeath].m_dvDate.GetSimpleYear();
		if (n)
			strDeath.Format(L"%d",n);
	}

	CString strTitle;
	strTitle.Format(L"%s, %s (%s-%s)",
		(LPCTSTR)pIndi->m_name.m_strSurname,
		(LPCTSTR)pIndi->m_name.m_strRest,
		(LPCTSTR)strBirth,
		(LPCTSTR)strDeath);

	os << L"<head>" << endl;
	os << L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
	os << L"<title>" << (LPCTSTR)strTitle << L"</title>" << endl;
	os << L"<link rel=\"stylesheet\" href=\"/css\" type=\"text/css\" media=\"all\" />" << endl;
	os << L"</head>" << endl;
	os << L"<body>" << endl;

	os << L"<p><a href=\"/\">home</a> <a href=\"?chart\">chart</a> <a href=\"?level=0&base=0\">index</a></p>" << endl;

	CMemFile tplIndi;
	tplIndi.Write(sTplIndi,strlen(sTplIndi));
	os << (LPCTSTR)pIndi->GetWebPage(tplIndi,docid.c_str());

	theApp.htmlFooter(os);
	os << L"</body>" << endl;
}

void CGedtreeDoc::htmlSour(int iSour, const wstring& docid, wostream& os)
{
	if (iSour < 0 || m_rSource.GetSize() <= iSour)
	{
		::OutputDebugString(CString(L"htmlSour"));
		throw 404;
	}

	CSource* pSour = &m_rSource[iSour];

	CString strTitle;
	if (pSour->m_strTitle.IsEmpty())
		strTitle += L"[unknown title]";
	else
		strTitle += pSour->m_strTitle;

	os << L"<head>" << endl;
	os << L"<title>" << (LPCTSTR)strTitle << L"</title>" << endl;
	os << L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
	os << L"<link rel=\"stylesheet\" href=\"/css\" type=\"text/css\" media=\"all\" />" << endl;
	os << L"</head>" << endl;
	os << L"<body>" << endl;

	os << L"<p><a href=\"/\">home</a> <a href=\"?chart\">chart</a> <a href=\"?level=0&base=0\">index</a></p>" << endl;

	CMemFile tplSour;
	tplSour.Write(sTplSour,strlen(sTplSour));
	os << (LPCTSTR)pSour->GetWebPage(tplSour,docid.c_str());

	theApp.htmlFooter(os);
	os << L"</body>" << endl;
}

static void SetDir(const CString& s)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(s,drive,dir,fname,ext);

	CString sPath = CString(drive)+CString(dir);

	::SetCurrentDirectory(sPath);
}

void CGedtreeDoc::webFile(const wstring& path, thinsock& sock, wstring& mime)
{
	CString origdir;
	::GetCurrentDirectory(MAX_PATH+1,origdir.GetBuffer(MAX_PATH+1));
	origdir.ReleaseBuffer();

	SetDir((LPCTSTR)GetPathName());

	// get file
	HANDLE f(INVALID_HANDLE_VALUE);
	try
	{
		wstring p2(path);
		std::replace(p2.begin(),p2.end(),'/','\\');
		HANDLE f = ::CreateFile(p2.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (f==INVALID_HANDLE_VALUE)
		{
			DWORD e = ::GetLastError();
			e = e;
			::OutputDebugString(CString(L"webFile can't find file"));
			throw 404;
		}

		DWORD dwSize = ::GetFileSize(f,0);
		if (dwSize==INVALID_FILE_SIZE)
		{
			::OutputDebugString(CString(L"webFile invalid file size"));
			throw 404;
		}

		{
			wostringstream os;
			os << L"HTTP/1.0 200 OK" << endl2;
			os << L"Content-Length: " << dwSize << endl2;
			os << L"Connection: close" << endl2;
			os << endl2;
			sock.send(os.str().c_str(),os.str().length(),0);
		}

		DWORD cr(0);
		char buf[100*1024];
		BOOL bok = ::ReadFile(f,buf,100*1024,&cr,0);
		if (!bok)
		{
			::OutputDebugString(CString(L"webFile error reading file"));
			throw 404;
		}

		while (cr)
		{
			try
			{
				sock.sendbinary(buf,cr,0);
			}
			catch (...)
			{
				::OutputDebugString(CString(L"webFile error calling sendbinary"));
				throw 404;
			}

			bok = ::ReadFile(f,buf,100*1024,&cr,0);
			if (!bok)
			{
				::OutputDebugString(CString(L"webFile error reading file (2)"));
				throw 404;
			}
		}

	}
	catch (...)
	{
		if (f!=INVALID_HANDLE_VALUE)
			::CloseHandle(f);

		SetDir(origdir);

		throw;
	}

	if (f!=INVALID_HANDLE_VALUE)
		::CloseHandle(f);

	SetDir(origdir);
}

void CGedtreeDoc::writeApplet(std::wostream& os)
{
	os << theApp.m_nMaxIndiWidth << endl;

	int i, n;

	n = m_rIndividual.GetSize();
	os << n << endl;
	for (i = 0; i < n; ++i)
		m_rIndividual[i].writeApplet(os);

	n = m_rFamily.GetSize();
	os << n << endl;
	for (i = 0; i < n; ++i)
		m_rFamily[i].writeApplet(os);
}
