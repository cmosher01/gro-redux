// EditEvent.cpp : implementation file
//

#include "stdafx.h"
#include "gedtree.h"
#include "gedtreedoc.h"
#include "EditEvent.h"
#include "editdate.h"
#include "editcitation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEvent dialog


CEditEvent::CEditEvent(BOOL bFamily, BOOL bAttr, CWnd* pParent /*=NULL*/)
	: CDialog(CEditEvent::IDD, pParent),
	m_bFamily(bFamily),
	m_bAttr(bAttr)
{
	//{{AFX_DATA_INIT(CEditEvent)
	m_strPlace = _T("");
	m_strType = _T("");
	m_nType = -1;
	m_strNote = _T("");
	//}}AFX_DATA_INIT
}


void CEditEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TYPE, m_comboType);
	if (!m_comboType.GetCount())
		FillEvents();

	if (!pDX->m_bSaveAndValidate)
	{
		ValueToData();
	}

	//{{AFX_DATA_MAP(CEditEvent)
	DDX_Control(pDX, IDC_PLACE, m_editPlace);
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELETE, m_buttonDelete);
	DDX_Control(pDX, IDC_NOTE, m_editNote);
	DDX_Control(pDX, IDC_SOURCE, m_staticSource);
	DDX_Control(pDX, IDC_DATE, m_staticDate);
	DDX_Control(pDX, IDC_STRING, m_editType);
	DDX_Text(pDX, IDC_PLACE, m_strPlace);
	DDX_Text(pDX, IDC_STRING, m_strType);
	DDX_CBIndex(pDX, IDC_TYPE, m_nType);
	DDX_Text(pDX, IDC_NOTE, m_strNote);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DataToValue();
	}

	Enable();
	SetStaticTitles();
}

static const CString strAttrName[] =
{
	"caste",
	"description",
	"education",
	"national ID",
	"national origin",
	"count of children",
	"count of marriages",
	"occupation",
	"posession",
	"religion",
	"US Social Security number",
	"title",
	""
};

static const CString strEventNameI[] =
{
	"(other)",
	"birth",
	"death",
	"adoption",
	"adult christening",
	"baptism",
	"bar mitzvah",
	"bas mitzvah",
	"blessing",
	"census",
	"christening",
	"confirmation",
	"cremation",
	"emigration",
	"first communion",
	"graduated",
	"immigration",
	"naturalization",
	"ordination",
	"reposition",
	"residence",
	"retirement",
	"will probated",
	"will signed",
	""
};

static const CString strEventNameF[] =
{
	"(other)",
	"marriage",
	"annulment",
	"census",
	"divorce",
	"divorce filed",
	"engagement",
	"marriage bann",
	"marriage contract",
	"marriage license",
	"marriage settlement",
	""
};

static const CString strAttrToken[] =
{
	"CAST",
	"DSCR",
	"EDUC",
	"IDNO",
	"NATI",
	"NCHI",
	"NMR",
	"OCCU",
	"PROP",
	"RELI",
	"SSN",
	"TITL",
	""
};

static const CString strEventTokenI[] =
{
	"EVEN",
	"BIRT",
	"DEAT",
	"ADOP",
	"CHRA",
	"BAPM",
	"BARM",
	"BASM",
	"BLES",
	"CENS",
	"CHR",
	"CONF",
	"CREM",
	"EMIG",
	"FCOM",
	"GRAD",
	"IMMI",
	"NATU",
	"ORDN",
	"BURI",
	"RESI",
	"RETI",
	"PROB",
	"WILL",
	""
};

static const CString strEventTokenF[] =
{
	"EVEN",
	"MARR",
	"ANUL",
	"CENS",
	"DIV",
	"DIVF",
	"ENGA",
	"MARB",
	"MARC",
	"MARL",
	"MARS",
	""
};

void CEditEvent::FillEvents()
{
	int i(0);
	if (m_bAttr)
		while (!strAttrName[i].IsEmpty())
		{
			m_comboType.AddString(strAttrName[i++]);
		}
	else if (m_bFamily)
		while (!strEventNameF[i].IsEmpty())
		{
			m_comboType.AddString(strEventNameF[i++]);
		}
	else
		while (!strEventNameI[i].IsEmpty())
		{
			m_comboType.AddString(strEventNameI[i++]);
		}
}

void CEditEvent::DataToValue()
{
	if (m_bAttr)
	{
		m_attr.m_strTypeTok = strAttrToken[m_nType];
		m_attr.CalcType();
		m_attr.m_strValue = m_strType;
		m_attr.m_evt.m_strPlace = m_strPlace;
		m_attr.m_evt.m_strNote = m_strNote;
	}
	else
	{
		m_evt.m_strPlace = m_strPlace;
		if (m_bFamily)
			m_evt.m_strTypeTok = strEventTokenF[m_nType];
		else
			m_evt.m_strTypeTok = strEventTokenI[m_nType];

		if (m_nType)
			m_evt.CalcType();
		else
			m_evt.m_strType = m_strType;
		if (m_evt.m_strType.IsEmpty())
			m_evt.CalcType();

		m_evt.m_strNote = m_strNote;
	}
}

void CEditEvent::ValueToData()
{
	if (m_bAttr)
	{
		m_nType = 0;
		int i = 0;
		while (!strAttrToken[i].IsEmpty())
		{
			if (m_attr.m_strTypeTok==strAttrToken[i])
				m_nType = i;
			i++;
		}
		m_strType = m_attr.m_strValue;
		m_strPlace = m_attr.m_evt.m_strPlace;
		m_strNote = m_attr.m_evt.m_strNote;
	}
	else
	{
		m_strPlace = m_evt.m_strPlace;
		m_nType = 0;
		int i = 0;
		if (m_bFamily)
			while (!strEventTokenF[i].IsEmpty())
			{
				if (m_evt.m_strTypeTok==strEventTokenF[i])
					m_nType = i;
				i++;
			}
		else
			while (!strEventTokenI[i].IsEmpty())
			{
				if (m_evt.m_strTypeTok==strEventTokenI[i])
					m_nType = i;
				i++;
			}
		if (!m_nType)
			m_strType = m_evt.m_strType;

		m_strNote = m_evt.m_strNote;
	}
}

void CEditEvent::SetStaticTitles()
{
	if (m_bAttr)
	{
		m_staticDate.SetWindowText(m_attr.m_evt.m_dvDate.Display(DATE_LONGDATE));
		m_staticSource.SetWindowText(m_attr.m_evt.m_cita.Display());
		GetDlgItem(IDC_EVENT)->SetWindowText(_T("attribute"));
		SetWindowText(_T("Attribute"));
		GetDlgItem(IDC_DELETE)->SetWindowText(_T("Delete Attribute"));
	}
	else
	{
		m_staticDate.SetWindowText(m_evt.m_dvDate.Display(DATE_LONGDATE));
		m_staticSource.SetWindowText(m_evt.m_cita.Display());
		GetDlgItem(IDC_EVENT)->SetWindowText(_T("event"));
		SetWindowText(_T("Event"));
		GetDlgItem(IDC_DELETE)->SetWindowText(_T("Delete Event"));
	}
}

void CEditEvent::Enable()
{
	if (m_nType && !m_bAttr)
	{
		m_editType.ShowWindow(SW_HIDE);
		m_strType.Empty();
	}
	else
		m_editType.ShowWindow(SW_SHOW);
}

BEGIN_MESSAGE_MAP(CEditEvent, CDialog)
	//{{AFX_MSG_MAP(CEditEvent)
	ON_BN_CLICKED(IDC_CHANGEDATE, OnChangedate)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_BN_CLICKED(IDC_CHANGESOURCE, OnChangesource)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEvent message handlers

void CEditEvent::OnChangedate() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CEditDate d;
	if (m_bAttr)
		d.m_dv = m_attr.m_evt.m_dvDate;
	else
		d.m_dv = m_evt.m_dvDate;
	if (d.DoModal()==IDOK)
	{
		if (m_bAttr)
			m_attr.m_evt.m_dvDate = d.m_dv;
		else
			m_evt.m_dvDate = d.m_dv;
		UpdateData(FALSE);
	}
}

void CEditEvent::OnSelchangeType() 
{
	UpdateData();
}

void CEditEvent::OnChangesource() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CEditCitation d;
	if (m_bAttr)
		d.m_cita = m_attr.m_evt.m_cita;
	else
		d.m_cita = m_evt.m_cita;
	if (d.DoModal()==IDOK)
	{
		if (m_bAttr)
			m_attr.m_evt.m_cita = d.m_cita;
		else
			m_evt.m_cita = d.m_cita;
	}
	UpdateData(FALSE);
}

void CEditEvent::OnDelete() 
{
	if (m_bAttr)
	{
		if (theApp.ConfirmDelete("attribute"))
			EndDialog(IDC_DELETE);
	}
	else
	{
		if (theApp.ConfirmDelete("event"))
			EndDialog(IDC_DELETE);
	}
}

void CEditEvent::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)
	{
		if (m_editNote.m_hWnd)
		{
			PositionControls(cx,cy);
		}
	}
}

static const int nDlgMargin(10);
static const CSize sizButton(75,23);

void CEditEvent::PositionControls(int cx, int cy)
{
	SetRedraw(FALSE);

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectEdit;
	m_editPlace.GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.right = rectClient.right-nDlgMargin;
	m_editPlace.MoveWindow(rectEdit);

	CRect rectOK;
	rectOK.bottom = rectClient.bottom-nDlgMargin;
	rectOK.top = rectOK.bottom-sizButton.cy;
	rectOK.left = nDlgMargin;
	rectOK.right = rectOK.left+sizButton.cx;
	m_buttonOK.MoveWindow(rectOK);

	CRect rectCancel;
	rectCancel.bottom = rectOK.bottom;
	rectCancel.top = rectOK.top;
	rectCancel.left = rectOK.right+nDlgMargin;
	rectCancel.right = rectCancel.left+sizButton.cx;
	m_buttonCancel.MoveWindow(rectCancel);

	CRect rectDelete;
	rectDelete.bottom = rectOK.bottom;
	rectDelete.top = rectOK.top;
	rectDelete.right = rectClient.right-nDlgMargin;
	rectDelete.left = rectClient.right-2*sizButton.cx;
	m_buttonDelete.MoveWindow(rectDelete);

	CRect rectText;
	rectText.top = 150;
	rectText.left = nDlgMargin;
	rectText.right = rectClient.right-nDlgMargin;
	rectText.bottom = rectOK.top-nDlgMargin;
	m_editNote.MoveWindow(rectText);

	SetRedraw();
	Invalidate();
}

BOOL CEditEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RECT r;
	GetClientRect(&r);
	PositionControls(r.right,r.bottom);
	SetIcon(theApp.LoadIcon(IDD),TRUE);
	return TRUE;
}
