#ifndef header_head
#define header_head

#include "gedrecord.h"

class CHeader : public CGedRecord
{
public:
	HTREEITEM m_hTreeItemSubm;
	CString m_strNote;

	CHeader(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	void GetFromTree();
	void ProcessSchema(CMap<CString,LPCTSTR,CString,LPCTSTR>& map, HTREEITEM hti);
	void PutToTree();
	void Calc() { }
};

#endif
