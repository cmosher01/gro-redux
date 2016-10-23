#ifndef _name_
#define _name_

#include "cita.h"

class CName
{
public:
	HTREEITEM m_hTreeItem;
	CGedtreeDoc* m_pDoc;

	CString m_strDisplay;	// John Q. /Public/, II
	CString m_strName;		// John Q. Public, II
	CString m_strSurname;	// Public
	CString m_strRest;		// John Q. ~, II
	CString m_strGiven;		// John Q.
	CString m_strSuffix;	// , II
	CCitation m_cita;

public:
	CName(CGedtreeDoc* pDoc = NULL, HTREEITEM htiEvent = NULL);
	void Set(CGedtreeDoc* pDoc, HTREEITEM htiEvent);
	~CName();
	void Calc();
	CString Name();
	CString NameSlashes();
	CString Surname();
	CString GivenName();
	CString Filter(const CString& strName);
	void GetFromTree(const CString& strName);
	void PutToTree(HTREEITEM htiParent);
};

#endif
