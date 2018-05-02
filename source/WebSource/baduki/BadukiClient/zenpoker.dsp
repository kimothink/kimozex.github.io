# Microsoft Developer Studio Project File - Name="zenpoker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zenpoker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zenpoker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zenpoker.mak" CFG="zenpoker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zenpoker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "zenpoker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zenpoker - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\\..\\..\\cdxlib\\Release\\cdxlib.lib /nologo /subsystem:windows /machine:I386 /out:"Release/baduki.exe"

!ELSEIF  "$(CFG)" == "zenpoker - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\\..\\..\\cdxlib\\Release\\cdxlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/baduki.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "zenpoker - Win32 Release"
# Name "zenpoker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\맞고\NewGostopClient\AdminLogin.cpp"
# End Source File
# Begin Source File

SOURCE=.\AlarmMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Animation.cpp
# End Source File
# Begin Source File

SOURCE=.\ChipAni.cpp
# End Source File
# Begin Source File

SOURCE=.\CScrollText.cpp
# End Source File
# Begin Source File

SOURCE=.\CTextList.cpp
# End Source File
# Begin Source File

SOURCE=.\dtrace.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpView.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageEx.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkSock.cpp
# End Source File
# Begin Source File

SOURCE=.\NGClientControl.cpp
# End Source File
# Begin Source File

SOURCE=.\NGNCSProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TxtQ.cpp
# End Source File
# Begin Source File

SOURCE=.\UserList.cpp
# End Source File
# Begin Source File

SOURCE=.\zenpoker.cpp
# End Source File
# Begin Source File

SOURCE=.\zenpoker.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\맞고\NewGostopClient\AdminLogin.h"
# End Source File
# Begin Source File

SOURCE=.\AlarmMsg.h
# End Source File
# Begin Source File

SOURCE=.\Animation.h
# End Source File
# Begin Source File

SOURCE=..\BadukiCommonDefine\BadukiCommonDefine.h
# End Source File
# Begin Source File

SOURCE=.\ChipAni.h
# End Source File
# Begin Source File

SOURCE=.\ClientDefine.h
# End Source File
# Begin Source File

SOURCE=.\CScrollText.h
# End Source File
# Begin Source File

SOURCE=.\CTextList.h
# End Source File
# Begin Source File

SOURCE=.\dtrace.h
# End Source File
# Begin Source File

SOURCE=.\Externvar.h
# End Source File
# Begin Source File

SOURCE=.\FrameSkip.h
# End Source File
# Begin Source File

SOURCE=.\GDINew.h
# End Source File
# Begin Source File

SOURCE=.\Globalvar.h
# End Source File
# Begin Source File

SOURCE=.\HttpView.h
# End Source File
# Begin Source File

SOURCE=.\ImageEx.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NetworkSock.h
# End Source File
# Begin Source File

SOURCE=.\NGClientControl.h
# End Source File
# Begin Source File

SOURCE=.\NGNCSProtocol.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\BadukiCommonDefine\serverclientcommon.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TxtQ.h
# End Source File
# Begin Source File

SOURCE=.\UserList.h
# End Source File
# Begin Source File

SOURCE=.\zenpoker.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\cursor.cur
# End Source File
# Begin Source File

SOURCE=".\res\icon(1).ico"
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Picon.ico
# End Source File
# Begin Source File

SOURCE=.\res\zenpoker.ico
# End Source File
# Begin Source File

SOURCE=.\res\zenpoker.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
