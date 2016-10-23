# Microsoft Developer Studio Project File - Name="gedtree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gedtree - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gedtree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gedtree.mak" CFG="gedtree - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gedtree - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "gedtree - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "gedtree - Win32 Release NT" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GRO", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gedtree - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /D "NDEBUG" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 unicows.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib mfc42u.lib mfcs42u.lib msvcrt.lib msvcprt.lib comctl32.lib ole32.lib oleaut32.lib version.lib rpcrt4.lib oledlg.lib olepro32.lib urlmon.lib comsupp.lib ws2_32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /nodefaultlib /out:".\Release\GRO.EXE" /libpath:"F:\Program Files\Microsoft Visual Studio\VC98\Lib_LU" /libpath:"F:\Program Files\Microsoft Visual Studio\VC98\MFC_LU\Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gedtree - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /D "_DEBUG" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 unicows.lib mfcs42ud.lib mfcn42ud.lib mfcd42ud.lib mfco42ud.lib mfc42ud.lib msvcrtd.lib msvcprtd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib advapi32.lib shell32.lib comctl32.lib version.lib rpcrt4.lib uuid.lib oledlg.lib ole32.lib olepro32.lib oleaut32.lib urlmon.lib comsupp.lib ws2_32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /nodefaultlib /out:".\Debug\GRO.EXE"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gedtree - Win32 Release NT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gedtree___Win32_Release_NT"
# PROP BASE Intermediate_Dir "gedtree___Win32_Release_NT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_NT"
# PROP Intermediate_Dir "Release_NT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /D "NDEBUG" /win32
# SUBTRACT BASE MTL /nologo /mktyplib203
# ADD MTL /D "NDEBUG" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 unicows.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib mfc42u.lib mfcs42u.lib msvcrt.lib msvcprt.lib comctl32.lib ole32.lib oleaut32.lib version.lib rpcrt4.lib oledlg.lib olepro32.lib urlmon.lib comsupp.lib ws2_32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /nodefaultlib /out:".\Release\GRO.EXE"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib mfc42u.lib mfcs42u.lib msvcrt.lib msvcprt.lib comctl32.lib ole32.lib oleaut32.lib version.lib rpcrt4.lib oledlg.lib olepro32.lib urlmon.lib comsupp.lib ws2_32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386 /nodefaultlib /out:".\Release_NT\GRO.EXE"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "gedtree - Win32 Release"
# Name "gedtree - Win32 Debug"
# Name "gedtree - Win32 Release NT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\AppInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\attr.cpp
# End Source File
# Begin Source File

SOURCE=.\BigView.cpp
# End Source File
# Begin Source File

SOURCE=.\BirthCalc.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseFont.cpp
# End Source File
# Begin Source File

SOURCE=.\cita.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfirmDelete.cpp
# End Source File
# Begin Source File

SOURCE=.\Connect.cpp
# End Source File
# Begin Source File

SOURCE=.\date.cpp
# End Source File
# Begin Source File

SOURCE=.\DateValue.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgsavechanges.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebServer.cpp
# End Source File
# Begin Source File

SOURCE=.\DocSize.cpp
# End Source File
# Begin Source File

SOURCE=.\EditCitation.cpp
# End Source File
# Begin Source File

SOURCE=.\EditDate.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\EditName.cpp
# End Source File
# Begin Source File

SOURCE=.\EditRepo.cpp
# End Source File
# Begin Source File

SOURCE=.\EditSex.cpp
# End Source File
# Begin Source File

SOURCE=.\EditSource.cpp
# End Source File
# Begin Source File

SOURCE=.\EditSubm.cpp
# End Source File
# Begin Source File

SOURCE=.\event.cpp
# End Source File
# Begin Source File

SOURCE=.\fami.cpp
# End Source File
# Begin Source File

SOURCE=.\Find.cpp
# End Source File
# Begin Source File

SOURCE=.\GedCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\gedline.cpp
# End Source File
# Begin Source File

SOURCE=.\gedrecord.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtree.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtree.rc
# End Source File
# Begin Source File

SOURCE=.\gedtreeDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Gedtreeview.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewdl.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewil.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewin.cpp
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewpd.cpp
# End Source File
# Begin Source File

SOURCE=.\head.cpp
# End Source File
# Begin Source File

SOURCE=.\HeadNotes.cpp
# End Source File
# Begin Source File

SOURCE=.\Holiday.cpp
# End Source File
# Begin Source File

SOURCE=.\indi.cpp
# End Source File
# Begin Source File

SOURCE=.\IndiClip.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mydc.cpp
# End Source File
# Begin Source File

SOURCE=.\name.cpp
# End Source File
# Begin Source File

SOURCE=.\note.cpp
# End Source File
# Begin Source File

SOURCE=.\PickRels.cpp
# End Source File
# Begin Source File

SOURCE=.\PickRepo.cpp
# End Source File
# Begin Source File

SOURCE=.\PickSource.cpp
# End Source File
# Begin Source File

SOURCE=.\Places.cpp
# End Source File
# Begin Source File

SOURCE=.\Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\Register.cpp
# End Source File
# Begin Source File

SOURCE=.\repo.cpp
# End Source File
# Begin Source File

SOURCE=.\rfor.cpp
# End Source File
# Begin Source File

SOURCE=.\Setup.cpp
# End Source File
# Begin Source File

SOURCE=.\sour.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UnicodeSel.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\WebClient.cpp
# End Source File
# Begin Source File

SOURCE=.\WebServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Zoom.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\About.h
# End Source File
# Begin Source File

SOURCE=.\Ansel.h
# End Source File
# Begin Source File

SOURCE=.\AppInfo.h
# End Source File
# Begin Source File

SOURCE=.\attr.h
# End Source File
# Begin Source File

SOURCE=.\bibrtftpl.h
# End Source File
# Begin Source File

SOURCE=.\BigView.h
# End Source File
# Begin Source File

SOURCE=.\BirthCalc.h
# End Source File
# Begin Source File

SOURCE=.\CHILDFRM.H
# End Source File
# Begin Source File

SOURCE=.\ChooseFont.h
# End Source File
# Begin Source File

SOURCE=.\cita.h
# End Source File
# Begin Source File

SOURCE=.\ConfirmDelete.h
# End Source File
# Begin Source File

SOURCE=.\Connect.h
# End Source File
# Begin Source File

SOURCE=.\css.h
# End Source File
# Begin Source File

SOURCE=.\date.h
# End Source File
# Begin Source File

SOURCE=.\DateValue.h
# End Source File
# Begin Source File

SOURCE=.\Dlgsavechanges.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebServer.h
# End Source File
# Begin Source File

SOURCE=.\DocSize.h
# End Source File
# Begin Source File

SOURCE=.\EditCitation.h
# End Source File
# Begin Source File

SOURCE=.\EditDate.h
# End Source File
# Begin Source File

SOURCE=.\EditEvent.h
# End Source File
# Begin Source File

SOURCE=.\EditName.h
# End Source File
# Begin Source File

SOURCE=.\EditRepo.h
# End Source File
# Begin Source File

SOURCE=.\EditSex.h
# End Source File
# Begin Source File

SOURCE=.\EditSource.h
# End Source File
# Begin Source File

SOURCE=.\EditSubm.h
# End Source File
# Begin Source File

SOURCE=.\event.h
# End Source File
# Begin Source File

SOURCE=.\fami.h
# End Source File
# Begin Source File

SOURCE=.\Find.h
# End Source File
# Begin Source File

SOURCE=.\gedcmd.h
# End Source File
# Begin Source File

SOURCE=.\gedline.h
# End Source File
# Begin Source File

SOURCE=.\gedrecord.h
# End Source File
# Begin Source File

SOURCE=.\GEDTREE.H
# End Source File
# Begin Source File

SOURCE=.\Gedtreedoc.h
# End Source File
# Begin Source File

SOURCE=.\Gedtreeview.h
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewdl.h
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewil.h
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewin.h
# End Source File
# Begin Source File

SOURCE=.\gedtreeviewpd.h
# End Source File
# Begin Source File

SOURCE=.\head.h
# End Source File
# Begin Source File

SOURCE=.\HeadNotes.h
# End Source File
# Begin Source File

SOURCE=.\headrtftpl.h
# End Source File
# Begin Source File

SOURCE=.\Holiday.h
# End Source File
# Begin Source File

SOURCE=.\holidaydef.h
# End Source File
# Begin Source File

SOURCE=.\indi.h
# End Source File
# Begin Source File

SOURCE=.\IndiClip.h
# End Source File
# Begin Source File

SOURCE=.\indirtftpl.h
# End Source File
# Begin Source File

SOURCE=.\inditpl.h
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\mydc.h
# End Source File
# Begin Source File

SOURCE=.\MyRect.h
# End Source File
# Begin Source File

SOURCE=.\name.h
# End Source File
# Begin Source File

SOURCE=.\note.h
# End Source File
# Begin Source File

SOURCE=.\PickRels.h
# End Source File
# Begin Source File

SOURCE=.\PickRepo.h
# End Source File
# Begin Source File

SOURCE=.\PickSource.h
# End Source File
# Begin Source File

SOURCE=.\Places.h
# End Source File
# Begin Source File

SOURCE=.\Progress.h
# End Source File
# Begin Source File

SOURCE=.\reg.h
# End Source File
# Begin Source File

SOURCE=.\Register.h
# End Source File
# Begin Source File

SOURCE=.\repo.h
# End Source File
# Begin Source File

SOURCE=.\rfor.h
# End Source File
# Begin Source File

SOURCE=.\Setup.h
# End Source File
# Begin Source File

SOURCE=.\sour.h
# End Source File
# Begin Source File

SOURCE=.\sourrtftpl.h
# End Source File
# Begin Source File

SOURCE=.\sourtpl.h
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\thinsock.h
# End Source File
# Begin Source File

SOURCE=.\UnicodeSel.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser.h
# End Source File
# Begin Source File

SOURCE=.\WebClient.h
# End Source File
# Begin Source File

SOURCE=.\WebServer.h
# End Source File
# Begin Source File

SOURCE=.\Zoom.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\citation.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ged_il.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ged_in.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ged_pd.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ged_tre.ico
# End Source File
# Begin Source File

SOURCE=.\RES\gedtree.ico
# End Source File
# Begin Source File

SOURCE=.\RES\GEDTREE.RC2
# End Source File
# Begin Source File

SOURCE=.\res\idd_edit.ico
# End Source File
# Begin Source File

SOURCE=.\res\idd_repo.ico
# End Source File
# Begin Source File

SOURCE=.\RES\menudef.h
# End Source File
# Begin Source File

SOURCE=.\res\source.ico
# End Source File
# Begin Source File

SOURCE=.\RES\TOOLBAR.BMP
# End Source File
# End Group
# End Target
# End Project
# Section gedtree : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser.h
# 	2:16:DefaultSinkClass:CWebBrowser
# End Section
# Section gedtree : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser
# 	2:10:HeaderFile:webbrowser.h
# 	2:8:ImplFile:webbrowser.cpp
# End Section
