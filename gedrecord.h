#ifndef gedrecord_fami
#define gedrecord_fami

class CGedtreeDoc;

class CGedRecord
{
public:
	CString m_strID;
	int m_i;
	CGedtreeDoc* m_pDoc;
	HTREEITEM m_hTreeItem;

	CGedRecord(CGedtreeDoc* pDoc = NULL, HTREEITEM hTreeItem = NULL);
	CGedRecord(const CGedRecord& o);
	CGedRecord& operator=(const CGedRecord& o);
	~CGedRecord();
	static char GetPrefix(const CString& strTok);
	CString GetID();
	void SetIndex(int i);
	void CalcID();
	void Delete();
	BOOL Exists();
	void Set(CGedtreeDoc* pDoc, HTREEITEM hTreeItem);
	CString GetWebFilePath(const CString& sDocID = "");
	CString GetLink(const CString& strText = "", const CString& sDocID = "");

	virtual void Calc() = 0;
	virtual CString GetLinkText() { return m_strID; }
};

#endif
