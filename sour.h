#ifndef header_sour
#define header_sour

#include "gedrecord.h"

class CSource : public CGedRecord
{
public:
	CString m_strAuthor;
	CString m_strTitle;
	CString m_strPublish;
	CString m_strText;
	int m_iRepository;

	CSource(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	CSource(const CSource& o);
	CSource& operator=(const CSource& o);
	void GetFromTree();
	void Calc() { }
	void PutToTree();
	CString GetDisplay();
	CString GetWebPage(CMemFile& tpl, const CString& sDocID = "");
	CString GetRTF(CMemFile& tpl);
	CString TextBlock(const CString& strText);
	CString WordBreak(const CString& strText);
	CString RTFTextBlock(const CString& strText);
	void ConvertToHTML();
	CString ConvertLine(const CString& sline, const CString& strRest, bool& in_table, int& ccol);
	CString ConvertTD(const CString& s, int ccol);
	int CountColumns(const CString& sline, const CString& srest);
	int CountColumnsRow(const CString& s);
};

#endif
