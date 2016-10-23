#ifndef header_repo
#define header_repo

#include "gedrecord.h"

class CRepository : public CGedRecord
{
public:
	CString m_strName;
	CString m_strAddr;

	CRepository(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	CRepository(const CRepository& o);
	CRepository& operator=(const CRepository& o);
	void Calc();
	void GetFromTree();
	void PutToTree();
};

#endif
