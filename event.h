#ifndef _event_
#define _event_

#include "datevalue.h"
#include "cita.h"

class CGedtreeDoc;

class CEvt
{
public:
	HTREEITEM m_hTreeItem;
	CGedtreeDoc* m_pDoc;

	CString m_strTypeTok;
	CString m_strType;
	CDateValue m_dvDate;
	CString m_strPlace;
	CCitation m_cita;
	CString m_strNote;
	int m_iNote;

	CEvt(CGedtreeDoc* pDoc = NULL, HTREEITEM htiEvent = NULL);
	~CEvt();
	void Set(CGedtreeDoc* pDoc, HTREEITEM hTreeItem);
	void CalcType();
	void Calc();

	void GetFromTree(const CString& strTypeTok);
	void PutToTree();
	void Delete();
};

#endif
