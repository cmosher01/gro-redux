#ifndef _indiclip_
#define _indiclip_

class CGedtreeDoc;

class CIndiClip
{
public:
	CIndiClip() {}
	virtual ~CIndiClip() {}
	void Copy(CGedtreeDoc* pDocFrom);
	void Paste(CGedtreeDoc* pDocTo);

private:
	CArray<int,int> m_riIndi;
	CGedtreeDoc* m_pDoc;
};

#endif
