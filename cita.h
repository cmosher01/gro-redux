#ifndef _citation_
#define _citation_

#include "datevalue.h"

class CGedtreeDoc;

class CCitation
{
public:
	HTREEITEM m_hTreeItem;
	CGedtreeDoc* m_pDoc;

	int m_iSource;
	CString m_strPage;
	CString m_strText;
	int m_nQuality;

public:
	CCitation(CGedtreeDoc* pDoc = NULL, HTREEITEM htiEvent = NULL);
	void Set(CGedtreeDoc* pDoc, HTREEITEM htiEvent);
	~CCitation();

	void Clear();
	void Calc() { }
	CString Display();

	void GetFromTree(const CString& strTypeTok);
	void PutToTree(HTREEITEM htiParent);
};

#endif
