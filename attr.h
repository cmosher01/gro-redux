#ifndef _attr_
#define _attr_

#include "event.h"

class CGedtreeDoc;

class CAttr
{
public:
	HTREEITEM m_hTreeItem;
	CGedtreeDoc* m_pDoc;

	CString m_strTypeTok;
	CString m_strType;
	CString m_strValue;
	CEvt m_evt;

	CAttr(CGedtreeDoc* pDoc = NULL, HTREEITEM htiAttr = NULL);
	~CAttr();
	void Set(CGedtreeDoc* pDoc, HTREEITEM hTreeItem);
	void CalcType();
	void Calc();

	void GetFromTree(const CString& strTypeTok, const CString& strValue);
	void PutToTree();
	void Delete();
};

#endif
