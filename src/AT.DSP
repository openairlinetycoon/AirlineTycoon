# Microsoft Developer Studio Project File - Name="AT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AT.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AT.MAK" CFG="AT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AT - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AT - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AT - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 dplayx.lib ddraw.lib ole32.lib winmm.lib dxguid.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dplayx.lib ddraw.lib ole32.lib winmm.lib dxguid.lib dsound.lib /nologo /version:1.1 /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AT - Win32 Release"
# Name "AT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Doku"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\0_ref.txt
# End Source File
# Begin Source File

SOURCE=.\2do.txt
# End Source File
# Begin Source File

SOURCE=.\_doku.txt
# End Source File
# Begin Source File

SOURCE=..\AddOn.doc
# End Source File
# Begin Source File

SOURCE=..\AddOn.xls
# End Source File
# Begin Source File

SOURCE=.\cd_prot.h

!IF  "$(CFG)" == "AT - Win32 Release"

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cipher.txt
# End Source File
# End Group
# Begin Group "Diverse"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AskBrick.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Auftrag.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Block.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Brick.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\City.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DataTabl.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HallDiskMenu.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Klacker.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NewGamePopup.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# ADD CPP /Od /Ob0

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlaneTyp.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Route.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Script2.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# ADD CPP /Yc
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Talker.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tips.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TitlePopup.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Wichtige"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AtNet.cpp
# End Source File
# Begin Source File

SOURCE=.\GameFrame.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Person.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Player.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# ADD CPP /Ob1
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNet.cpp
# End Source File
# Begin Source File

SOURCE=.\Protect.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# ADD CPP /Od /Ob0

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sim.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# ADD CPP /Ob1
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Synthese.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TakeOff.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Library"

# PROP Default_Filter "coo+"
# Begin Source File

SOURCE=.\Checkup.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Colorfx.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\global.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HLine.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# ADD CPP /Ob0

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Init.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SBBM.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SecurityManager.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Smackprs.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\DeltaVid.cpp
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter "lib"
# Begin Group "SB-Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SBLib\source\Debug.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SBLib\source\Network.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SBLib\source\String.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SBLib\source\Tool2.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\sbl.h
# End Source File
# Begin Source File

SOURCE=.\TeakLibW.h
# End Source File
# Begin Source File

SOURCE=..\Sbl\Release\SBL.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=D:\Projekte\Sys\SoundEngine\Dll\Debug\SSE.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=D:\Projekte\Sys\SoundEngine\Dll\Release\SSE.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\SoundEngine\Dll\Release\SSE.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\TeakLibW\Release\TeakLibW.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Sbl\Debug\SBL.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\TeakLibW\Debug\TeakLibW.lib
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\libs\SBL.lib
# End Source File
# Begin Source File

SOURCE=..\libs\SSE.lib
# End Source File
# Begin Source File

SOURCE=..\libs\TeakLibW.lib
# End Source File
# Begin Source File

SOURCE=..\smksdk\SMACKW32.LIB
# End Source File
# End Group
# Begin Group "Räume"

# PROP Default_Filter "cpp"
# Begin Group "Normale"

# PROP Default_Filter "Cpp"
# Begin Source File

SOURCE=.\ArabAir.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Aufsicht.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Bank.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Buero.Cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Designer.cpp
# End Source File
# Begin Source File

SOURCE=.\DutyFree.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Editor.cpp
# End Source File
# Begin Source File

SOURCE=.\Fracht.cpp
# End Source File
# Begin Source File

SOURCE=.\Kiosk.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LastMin.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Makler.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Museum.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Personal.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\reise.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ricks.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RouteBox.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Rushmore.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sabotage.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Security.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistk.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tafel.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WeltAll.cpp
# End Source File
# Begin Source File

SOURCE=.\Werbung.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Werkstat.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\World.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Spezielle"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Abend.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AirportView.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Credits.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Globe.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Insel.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Intro.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Laptop.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Nasa.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Outro.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Planer.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PlanProp.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Schedule.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdRaum.cpp

!IF  "$(CFG)" == "AT - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Ressourcen"

# PROP Default_Filter "res"
# Begin Source File

SOURCE=.\Misc\Dlg_ger.res
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Misc\Ein_ger.res
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Misc\Register.res
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Misc\Std_ger.res
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=".\Res\16-16.ico"
# End Source File
# Begin Source File

SOURCE=.\Res\AtIcone.ico
# End Source File
# Begin Source File

SOURCE=.\Debug.log

!IF  "$(CFG)" == "AT - Win32 Release"

!ELSEIF  "$(CFG)" == "AT - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# End Target
# End Project
