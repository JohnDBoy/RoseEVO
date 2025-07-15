# Microsoft Developer Studio Project File - Name="Client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Client - Win32 VR_Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Client.mak" CFG="Client - Win32 VR_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Client - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Client - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Client - Win32 VR_Debug" (based on "Win32 (x86) Application")
!MESSAGE "Client - Win32 VR_Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Client", TQCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\SHO"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I ".\." /I "..\znzin11\engine\include" /I "..\GameCtrl" /I ".\\" /I ".\interface" /I "..\engine\include" /I "..\TGameCtrl" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Tools\TriggerVFS\TriggerVFS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USE_BG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TriggerVFS.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\SHO\r_Client.exe" /libpath:"..\GameCtrl\lib" /libpath:"..\engine\lib" /libpath:"..\TGameCtrl\libs" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib" /libpath:"..\Tools\TriggerVFS\LIBS"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy GlobalLUA
PostBuild_Cmds=copy .\GlobalSCR.lua   ..\SHO
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "f:\SHO"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\interface" /I "..\engine\include" /I "..\TGameCtrl" /I "..\Tools\TriggerVFS\TriggerVFS" /I "D:\SDK\intel\plsuite\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TriggerVFS.lib /nologo /subsystem:windows /debug /machine:I386 /out:"f:\SHO\d_Client.exe" /pdbtype:sept /libpath:"..\engine\lib" /libpath:"..\TGameCtrl\Libs" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib" /libpath:"..\Tools\TriggerVFS\LIBS"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy GlobalLUA
PostBuild_Cmds=copy .\GlobalSCR.lua   ..\SHO
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Client___Win32_VR_Debug"
# PROP BASE Intermediate_Dir "Client___Win32_VR_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\data"
# PROP Intermediate_Dir "Client___Win32_VR_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\znzin11\engine\include" /I "..\GameCtrl" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I ".\\" /I ".\interface" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\interface" /I "..\znzin11\engine\include" /I "..\GameCtrl" /I "..\znzin11\3rdparty\lua-4.0.1\include" /D "_DEBUG" /D "__VIRTUAL_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USE_BG" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\engine\lib" /libpath:"..\GameCtrl\lib" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\data\vd_Client.exe" /pdbtype:sept /libpath:"..\Znzin11\engine\lib" /libpath:"..\engine\lib" /libpath:"..\GameCtrl\lib" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy GlobalLUA
PostBuild_Cmds=copy .\GlobalSCR.lua   ..\SHO
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Client___Win32_VR_Release"
# PROP BASE Intermediate_Dir "Client___Win32_VR_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\SHO"
# PROP Intermediate_Dir "Client___Win32_VR_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /I "..\znzin11\engine\include" /I "..\GameCtrl" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I ".\." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /ZI /I ".\." /I "..\znzin11\engine\include" /I "..\GameCtrl" /I ".\\" /I ".\interface" /I "..\engine\include" /I "..\TGameCtrl" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Tools\TriggerVFS\TriggerVFS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__VIRTUAL_SERVER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\data/r_Client.exe" /libpath:"..\engine\lib" /libpath:"..\GameCtrl\lib" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TriggerVFS.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\SHO\vr_Client.exe" /libpath:"..\GameCtrl\lib" /libpath:"..\engine\lib" /libpath:"..\TGameCtrl\libs" /libpath:"..\znzin11\3rdparty\lua-4.0.1\lib" /libpath:"..\Tools\TriggerVFS\LIBS"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy GlobalLUA
PostBuild_Cmds=copy .\GlobalSCR.lua   ..\SHO
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Client - Win32 Release"
# Name "Client - Win32 Debug"
# Name "Client - Win32 VR_Debug"
# Name "Client - Win32 VR_Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dll_bugguard\bugguard.h
# End Source File
# Begin Source File

SOURCE=.\Util\BugGUARD.h
# End Source File
# Begin Source File

SOURCE=.\Util\CBITArray.h
# End Source File
# Begin Source File

SOURCE=.\Util\CCheat.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CCheat.h
# End Source File
# Begin Source File

SOURCE=.\Util\CClientSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\Util\CD3DUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CD3DUtil.h
# End Source File
# Begin Source File

SOURCE=.\Util\CDSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CDSound.h
# End Source File
# Begin Source File

SOURCE=.\Util\CFileLIST.h
# End Source File
# Begin Source File

SOURCE=.\Util\CFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\Util\CFileSystemNormal.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CFileSystemNormal.h
# End Source File
# Begin Source File

SOURCE=.\Util\CFileSystemTriggerVFS.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CFileSystemTriggerVFS.h
# End Source File
# Begin Source File

SOURCE=.\Util\CGameSTB.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CGameSTB.h
# End Source File
# Begin Source File

SOURCE=.\Util\CGameStr.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CGameStr.h
# End Source File
# Begin Source File

SOURCE=.\Util\classHASH.h
# End Source File
# Begin Source File

SOURCE=.\Util\classIME.h
# End Source File
# Begin Source File

SOURCE=.\Util\classLOG.h
# End Source File
# Begin Source File

SOURCE=.\Util\classLUA.h
# End Source File
# Begin Source File

SOURCE=.\Util\classMD5.h
# End Source File
# Begin Source File

SOURCE=.\Util\classQUEUE.h
# End Source File
# Begin Source File

SOURCE=.\Util\classSTB.h
# End Source File
# Begin Source File

SOURCE=.\Util\classSTR.h
# End Source File
# Begin Source File

SOURCE=.\Util\classUTIL.h
# End Source File
# Begin Source File

SOURCE=.\Util\CMessageQ.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CMessageQ.h
# End Source File
# Begin Source File

SOURCE=.\Util\CMMPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Util\CRangeTBL.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CRangeTBL.h
# End Source File
# Begin Source File

SOURCE=.\Util\CRawSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\Util\CshoSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\Util\CSocket.h
# End Source File
# Begin Source File

SOURCE=.\Util\CVector.h
# End Source File
# Begin Source File

SOURCE=.\Util\CWaveFILE.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\CWaveFILE.h
# End Source File
# Begin Source File

SOURCE=.\Util\DLLIST.h
# End Source File
# Begin Source File

SOURCE=.\Util\ijl.h
# End Source File
# Begin Source File

SOURCE=.\Util\JSingleton.h
# End Source File
# Begin Source File

SOURCE=.\Util\PacketHEADER.h
# End Source File
# Begin Source File

SOURCE=.\Util\RTTI.h
# End Source File
# Begin Source File

SOURCE=.\Util\SLLIST.h
# End Source File
# Begin Source File

SOURCE=.\Util\VFSManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\VFSManager.h
# End Source File
# Begin Source File

SOURCE=.\Util\WndMsgQ.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\WndMsgQ.h
# End Source File
# Begin Source File

SOURCE=.\Util\Util_r.lib
# End Source File
# Begin Source File

SOURCE=.\Util\BugGUARD.lib
# End Source File
# Begin Source File

SOURCE=.\Util\ijl15.lib
# End Source File
# End Group
# Begin Group "IO_Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IO_Basic.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Basic.h
# End Source File
# Begin Source File

SOURCE=.\IO_Effect.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Effect.h
# End Source File
# Begin Source File

SOURCE=.\IO_Event.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Event.h
# End Source File
# Begin Source File

SOURCE=.\IO_MATERIAL.CPP
# End Source File
# Begin Source File

SOURCE=.\IO_MATERIAL.H
# End Source File
# Begin Source File

SOURCE=.\IO_MESH.CPP
# End Source File
# Begin Source File

SOURCE=.\IO_MESH.H
# End Source File
# Begin Source File

SOURCE=.\IO_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Model.h
# End Source File
# Begin Source File

SOURCE=.\IO_Morpher.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Morpher.h
# End Source File
# Begin Source File

SOURCE=.\IO_Skeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Skeleton.h
# End Source File
# Begin Source File

SOURCE=.\IO_Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\IO_Terrain.h
# End Source File
# End Group
# Begin Group "winMAIN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\CApplication.h
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\winMAIN.cpp
# End Source File
# End Group
# Begin Group "Mouse & Keyboard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\CKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\CMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\CMouse.h
# End Source File
# End Group
# Begin Group "OBJECT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CGameOBJ.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameOBJ.h
# End Source File
# Begin Source File

SOURCE=.\CModelCHAR.cpp
# End Source File
# Begin Source File

SOURCE=.\CModelCHAR.h
# End Source File
# Begin Source File

SOURCE=.\CObjCHAR.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjCHAR.h
# End Source File
# Begin Source File

SOURCE=.\CObjFIXED.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjFIXED.h
# End Source File
# Begin Source File

SOURCE=.\CObjGROUND.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjGROUND.h
# End Source File
# Begin Source File

SOURCE=.\CObjMORPH.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjMORPH.h
# End Source File
# Begin Source File

SOURCE=.\CObjUSER.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjUSER.h
# End Source File
# Begin Source File

SOURCE=.\CSkyDOME.cpp
# End Source File
# Begin Source File

SOURCE=.\CSkyDOME.h
# End Source File
# Begin Source File

SOURCE=.\OBJECT.cpp
# End Source File
# Begin Source File

SOURCE=.\OBJECT.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound\CMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\CMusic.h
# End Source File
# Begin Source File

SOURCE=.\Sound\IO_Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound\IO_Sound.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Group "DLGs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\DLGs\AvatarInvenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\AvatarInvenDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\BasePAN.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\BasePAN.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CharacterDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CharacterDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\ChattingDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\ChattingDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CMinimapDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CMinimapDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CNumberInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CNumberInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\COptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\COptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CPartyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CPartyDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CPartyMember.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CPartyMember.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CQuestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CQuestDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CRestartDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CRestartDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSkillDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSkillDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CStoreDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CStoreDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSystemDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSystemDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSystemMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CSystemMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CTargetMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\CTargetMenu.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\DealDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\DealDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\DropDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\DropDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\ExchangeDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\ExchangeDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\InventoryDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\InventoryDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\MakeDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\MakeDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\MessageDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\MessageDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\QueryDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\QueryDLG.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\QuickToolBAR.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\QuickToolBAR.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\StoreInvenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\StoreInvenDlg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\TargetObjectDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DLGs\TargetObjectDLG.h
# End Source File
# End Group
# Begin Group "ExternalUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\ExternalUI\CCreateAvata.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CCreateAvata.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CExternalUI.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CExternalUIManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CExternalUIManager.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CLogin.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CNewAccount.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectAvata.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectAvata.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectCharacter.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSelectServer.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\CSetting.h
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\ExternalUILobby.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\ExternalUI\ExternalUILobby.h
# End Source File
# End Group
# Begin Group "Command"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\Command\CTCmdHotExec.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\Command\CTCmdHotExec.h
# End Source File
# Begin Source File

SOURCE=.\Interface\Command\CTCmdNumberInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\Command\CTCmdNumberInput.h
# End Source File
# Begin Source File

SOURCE=.\Interface\Command\CTCmdOpenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\Command\CTCmdOpenDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interface\CChatBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CChatBox.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CCommandPT.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CCommandPT.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CDigitEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CDigitEffect.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CEnduranceProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CEnduranceProperty.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CEventProc.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CEventProc.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CHelpMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CHelpMgr.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CKeyMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CKeyMove.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CLoading.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CLoading.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CNameBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CNameBox.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CObj.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CSplitHangul.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CSplitHangul.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CStringTable.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CTDrawImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CTDrawImpl.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CTFontImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CTFontImpl.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CTSoundImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CTSoundImpl.h
# End Source File
# Begin Source File

SOURCE=.\Interface\CUIMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\CUIMediator.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DragNDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DragNDrop.h
# End Source File
# Begin Source File

SOURCE=.\Interface\DrawItemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\DrawItemInfo.h
# End Source File
# Begin Source File

SOURCE=.\Interface\InterfaceType.h
# End Source File
# Begin Source File

SOURCE=.\Interface\IO_ImageRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\IO_ImageRes.h
# End Source File
# Begin Source File

SOURCE=.\Interface\it_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\it_mgr.h
# End Source File
# Begin Source File

SOURCE=.\Interface\LookUpImageIndex.h
# End Source File
# Begin Source File

SOURCE=.\Interface\TypeResource.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\TypeResource.h
# End Source File
# End Group
# Begin Group "Event"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Event\CEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Event\CEvent.h
# End Source File
# Begin Source File

SOURCE=.\Event\GF_EFFECT.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\GF_ITEM.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\GF_SYSTEM.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\QF_CONVERSATION.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\QF_QUEST.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\QF_SKILL.CPP
# End Source File
# Begin Source File

SOURCE=.\Event\Quest_FUNC.h
# End Source File
# Begin Source File

SOURCE=.\Event\Quest_FUNC_DEF.inc
# End Source File
# Begin Source File

SOURCE=.\Event\Quest_FUNC_REG.inc
# End Source File
# End Group
# Begin Group "Bullet & Skill"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BULLET.CPP
# End Source File
# Begin Source File

SOURCE=.\BULLET.H
# End Source File
# Begin Source File

SOURCE=.\Skill.cpp
# End Source File
# Begin Source File

SOURCE=.\Skill.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "AI_LIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI_LIB\AI_Action.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_LIB\AI_Condition.cpp
# End Source File
# Begin Source File

SOURCE=.\AI_LIB\CAI_FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\AI_LIB\CAI_FILE.H
# End Source File
# Begin Source File

SOURCE=.\AI_LIB\CAI_LIB.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\Common\Calculation.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Calculation.h
# End Source File
# Begin Source File

SOURCE=.\Common\CEconomy.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CEconomy.h
# End Source File
# Begin Source File

SOURCE=.\Common\CHotICON.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CHotICON.h
# End Source File
# Begin Source File

SOURCE=.\Common\CInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CInventory.h
# End Source File
# Begin Source File

SOURCE=.\Common\CItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CItem.h
# End Source File
# Begin Source File

SOURCE=.\Common\CObjAI.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CObjAI.h
# End Source File
# Begin Source File

SOURCE=.\Common\CQuest.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CQuest.h
# End Source File
# Begin Source File

SOURCE=.\Common\CRegenAREA.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CRegenAREA.h
# End Source File
# Begin Source File

SOURCE=.\Common\CSkillSlot.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CSkillSlot.h
# End Source File
# Begin Source File

SOURCE=.\Common\CUserDATA.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CUserDATA.h
# End Source File
# Begin Source File

SOURCE=.\Common\DataTYPE.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_AI.CPP
# End Source File
# Begin Source File

SOURCE=.\Common\IO_AI.H
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Motion.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Motion.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Skill.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Skill.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_STB.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_STB.h
# End Source File
# End Group
# Begin Group "LUA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\export_GM.lua

!IF  "$(CFG)" == "Client - Win32 Release"

!ELSEIF  "$(CFG)" == "Client - Win32 Debug"

USERDEP__EXPOR=".\Game_FUNC.h"	
# Begin Custom Build - Export Game Function Interface $(InputPath)
InputDir=.
InputPath=.\export_GM.lua

"$(InputDir)\Game_FUNC.h2" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\lua $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Debug"

USERDEP__EXPOR=".\Game_FUNC.h"	
# Begin Custom Build - Export Game Function Interface $(InputPath)
InputDir=.
InputPath=.\export_GM.lua

"$(InputDir)\Game_FUNC.h2" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\lua $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\export_QF.lua

!IF  "$(CFG)" == "Client - Win32 Release"

!ELSEIF  "$(CFG)" == "Client - Win32 Debug"

# Begin Custom Build - Export Quest Function Interface $(InputPath)
InputDir=.
InputPath=.\export_QF.lua

"$(InputDir)\Event\Quest_FUNC.h2" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\lua $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Debug"

# Begin Custom Build - Export Quest Function Interface $(InputPath)
InputDir=.
InputPath=.\export_QF.lua

"$(InputDir)\Event\Quest_FUNC.h2" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\lua $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Client - Win32 VR_Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Game_FUNC.cpp
# End Source File
# Begin Source File

SOURCE=.\Game_FUNC.h
# End Source File
# Begin Source File

SOURCE=.\Game_FUNC_DEF.inc
# End Source File
# Begin Source File

SOURCE=.\Game_FUNC_REG.inc
# End Source File
# Begin Source File

SOURCE=.\GlobalScr.lua
# End Source File
# Begin Source File

SOURCE=.\LUA_Func.cpp
# End Source File
# Begin Source File

SOURCE=.\LUA_Func.h
# End Source File
# Begin Source File

SOURCE=.\LUA_Init.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\CNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetwork.h
# End Source File
# Begin Source File

SOURCE=.\Network\NET_Prototype.h
# End Source File
# Begin Source File

SOURCE=.\Network\RecvPACKET.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\RecvPACKET.h
# End Source File
# Begin Source File

SOURCE=.\Network\SendPACKET.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\SendPACKET.h
# End Source File
# End Group
# Begin Group "FORBetaTest"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UIforTest.cpp
# End Source File
# Begin Source File

SOURCE=.\UIforTest.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\CCamera.h
# End Source File
# Begin Source File

SOURCE=.\CClientStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\CClientStorage.h
# End Source File
# Begin Source File

SOURCE=.\CViewMSG.cpp
# End Source File
# Begin Source File

SOURCE=.\CViewMSG.h
# End Source File
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\JCommandState.cpp
# End Source File
# Begin Source File

SOURCE=.\JCommandState.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Client.ico
# End Source File
# Begin Source File

SOURCE=.\Client.rc
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Lua BIN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lua.exe
# End Source File
# Begin Source File

SOURCE=.\luac.exe
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
