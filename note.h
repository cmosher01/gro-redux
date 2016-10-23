#ifndef header_note
#define header_note

#include "gedrecord.h"

class CNote : public CGedRecord
{
public:
	CString m_strNote;

	CNote(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	CNote& operator=(const CNote& o);
	CNote(const CNote& o);

	void Calc();
	void GetFromTree();
	void PutToTree();
};

#endif
