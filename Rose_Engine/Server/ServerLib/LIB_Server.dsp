# Microsoft Developer Studio Project File - Name="LIB_Server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIB_Server - Win32 Compaq Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIB_Server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIB_Server.mak" CFG="LIB_Server - Win32 Compaq Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIB_Server - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_Server - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_Server - Win32 Debug2" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_Server - Win32 Xeon Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_Server - Win32 Compaq Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LIB_Server", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIB_Server - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC_LIB"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LIB_Server - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VC_LIB"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"

!ELSEIF  "$(CFG)" == "LIB_Server - Win32 Debug2"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_Server___Win32_Debug20"
# PROP BASE Intermediate_Dir "LIB_Server___Win32_Debug20"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__SERVER" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"
# ADD LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"

!ELSEIF  "$(CFG)" == "LIB_Server - Win32 Xeon Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_Server___Win32_Xeon_Debug0"
# PROP BASE Intermediate_Dir "LIB_Server___Win32_Xeon_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xeon_Debug"
# PROP Intermediate_Dir "Xeon_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"
# ADD LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"

!ELSEIF  "$(CFG)" == "LIB_Server - Win32 Compaq Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_Server___Win32_Compaq_Debug"
# PROP BASE Intermediate_Dir "LIB_Server___Win32_Compaq_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Compaq_Debug"
# PROP Intermediate_Dir "Compaq_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "_DEBUG" /D "__SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_WINNT 0x0500" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"
# ADD LIB32 /nologo /out:"VC_LIB\d_LIB_Server.lib"

!ENDIF 

# Begin Target

# Name "LIB_Server - Win32 Release"
# Name "LIB_Server - Win32 Debug"
# Name "LIB_Server - Win32 Debug2"
# Name "LIB_Server - Win32 Xeon Debug"
# Name "LIB_Server - Win32 Compaq Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "IOCP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CAcceptTHREAD.cpp
# End Source File
# Begin Source File

SOURCE=.\CAcceptTHREAD.h
# End Source File
# Begin Source File

SOURCE=.\CIocpTHREAD.cpp
# End Source File
# Begin Source File

SOURCE=.\CIocpTHREAD.h
# End Source File
# Begin Source File

SOURCE=.\classIOCP.h
# End Source File
# Begin Source File

SOURCE=.\iocpSOCKET.cpp
# End Source File
# Begin Source File

SOURCE=.\iocpSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\ioDataPOOL.cpp
# End Source File
# Begin Source File

SOURCE=.\ioDataPOOL.h
# End Source File
# End Group
# Begin Group "SQL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classMYSQL.cpp
# End Source File
# Begin Source File

SOURCE=.\classMYSQL.h
# End Source File
# Begin Source File

SOURCE=.\classODBC.cpp
# End Source File
# Begin Source File

SOURCE=.\classODBC.h
# End Source File
# Begin Source File

SOURCE=.\classSQL.cpp
# End Source File
# Begin Source File

SOURCE=.\classSQL.h
# End Source File
# Begin Source File

SOURCE=.\CSqlTHREAD.cpp
# End Source File
# Begin Source File

SOURCE=.\CSqlTHREAD.h
# End Source File
# End Group
# Begin Group "URL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurlTHREAD.cpp
# End Source File
# Begin Source File

SOURCE=.\CurlTHREAD.h
# End Source File
# End Group
# Begin Group "IOCPSocketServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\iocpSocketSERVER.cpp
# End Source File
# Begin Source File

SOURCE=.\iocpSocketSERVER.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\blockLIST.cpp
# End Source File
# Begin Source File

SOURCE=.\blockLIST.h
# End Source File
# Begin Source File

SOURCE=.\classPACKET.cpp
# End Source File
# Begin Source File

SOURCE=.\classPACKET.h
# End Source File
# Begin Source File

SOURCE=.\ipLIST.cpp
# End Source File
# Begin Source File

SOURCE=.\ipLIST.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CDataPOOL.h
# End Source File
# Begin Source File

SOURCE=.\classINDEX.h
# End Source File
# End Group
# End Target
# End Project
