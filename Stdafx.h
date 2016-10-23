#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "afxtempl.h"
#include "afxmt.h"
#pragma warning(disable:4100) // C4100: unreferenced formal parameter
#pragma warning(disable:4706) // C4706: assignment within conditional expression
#pragma warning(disable:4701) // C4701: local variable 'var' may be used without having been initialized
#pragma warning(disable:4786) // C4786: identifier was truncated to '255' characters in the browser information

#include <stdio.h>
#include <winsock2.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
