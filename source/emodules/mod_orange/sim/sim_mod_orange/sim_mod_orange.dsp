# Microsoft Developer Studio Project File - Name="sim_mod_orange" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sim_mod_orange - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sim_mod_orange.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sim_mod_orange.mak" CFG="sim_mod_orange - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sim_mod_orange - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sim_mod_orange - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sim_mod_orange - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_MOD_ORANGE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\resreader\lang\\" /I "..\..\resreader\them\\" /I "..\..\resreader\\" /I "..\..\gui\kernel\\" /I "..\..\gui\common\\" /I "..\..\gui\message\\" /I "..\..\gui\widget\button\\" /I "..\..\gui\widget\static\\" /I "..\..\gui\widget\listmenu\\" /I "..\..\gui\widget\progsheet\\" /I "..\..\gui\widget\slider\\" /I "..\..\gui\widget\txtbox\\" /I "..\..\gui\widget\gmsgbox\\" /I "..\..\gui\widget\spinbox\\" /I "..\..\gui\widget\\" /I "..\..\gui\win\ctrlwin\\" /I "..\..\gui\win\frmwin\\" /I "..\..\gui\win\lyrwin\\" /I "..\..\gui\win\manwin\\" /I "..\..\gui\win\\" /I "..\..\gui\core\lcddev\\" /I "..\..\gui\core\memdev\\" /I "..\..\gui\core\cc\convertcolor\\" /I "..\..\gui\core\cc\convertmono\\" /I "..\..\gui\core\cc\\" /I "..\..\gui\core\font\sft\sfte\lib\\" /I "..\..\gui\core\font\sft\sfte\\" /I "..\..\gui\core\font\sft\\" /I "..\..\gui\core\font\buildin\\" /I "..\..\gui\core\font\truetype\src\base\\" /I "..\..\gui\core\font\truetype\src\cache\\" /I "..\..\gui\core\font\truetype\src\raster\\" /I "..\..\gui\core\font\truetype\src\sfnt\\" /I "..\..\gui\core\font\truetype\src\smooth\\" /I "..\..\gui\core\font\truetype\src\src_plus\\" /I "..\..\gui\core\font\truetype\src\truetype\\" /I "..\..\gui\core\font\truetype\src\bdf\\" /I "..\..\gui\core\font\truetype\src\cff\\" /I "..\..\gui\core\font\truetype\src\cid\\" /I "..\..\gui\core\font\truetype\src\gxvalid\\" /I "..\..\gui\core\font\truetype\src\gzip\\" /I "..\..\gui\core\font\truetype\src\lzw\\" /I "..\..\gui\core\font\truetype\src\otvalid\\" /I "..\..\gui\core\font\truetype\src\pcf\\" /I "..\..\gui\core\font\truetype\src\pfr\\" /I "..\..\gui\core\font\truetype\src\psaux\\" /I "..\..\gui\core\font\truetype\src\pshinter\\" /I "..\..\gui\core\font\truetype\src\psnames\\" /I "..\..\gui\core\font\truetype\src\tools\ftrandom\\" /I "..\..\gui\core\font\truetype\src\tools\\" /I "..\..\gui\core\font\truetype\src\type1\\" /I "..\..\gui\core\font\truetype\src\type42\\" /I "..\..\gui\core\font\truetype\src\winfonts\\" /I "..\..\gui\core\font\truetype\src\autofit\\" /I "..\..\gui\core\font\truetype\src\\" /I "..\..\gui\core\font\truetype\\" /I "..\..\gui\core\font\\" /I "..\..\gui\core\kernel\\" /I "..\..\gui\core\\" /I "..\..\gui\\" /I ".\\" /I "..\..\..\..\includes\\" /I "..\..\..\..\INCLUDES\EMOD\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\CORE\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\GDI\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\LAYER\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\MESSAGE\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\WIDGET\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\WIN\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_MOD_ORANGE_EXPORTS" /D "SIM_PC_WIN" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "sim_mod_orange - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_MOD_ORANGE_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\resreader\lang\\" /I "..\..\resreader\them\\" /I "..\..\resreader\\" /I "..\..\gui\kernel\\" /I "..\..\gui\common\\" /I "..\..\gui\message\\" /I "..\..\gui\widget\button\\" /I "..\..\gui\widget\static\\" /I "..\..\gui\widget\listmenu\\" /I "..\..\gui\widget\progsheet\\" /I "..\..\gui\widget\slider\\" /I "..\..\gui\widget\txtbox\\" /I "..\..\gui\widget\gmsgbox\\" /I "..\..\gui\widget\spinbox\\" /I "..\..\gui\widget\\" /I "..\..\gui\win\ctrlwin\\" /I "..\..\gui\win\frmwin\\" /I "..\..\gui\win\lyrwin\\" /I "..\..\gui\win\manwin\\" /I "..\..\gui\win\\" /I "..\..\gui\core\lcddev\\" /I "..\..\gui\core\memdev\\" /I "..\..\gui\core\cc\convertcolor\\" /I "..\..\gui\core\cc\convertmono\\" /I "..\..\gui\core\cc\\" /I "..\..\gui\core\font\sft\sfte\lib\\" /I "..\..\gui\core\font\sft\sfte\\" /I "..\..\gui\core\font\sft\\" /I "..\..\gui\core\font\buildin\\" /I "..\..\gui\core\font\truetype\src\base\\" /I "..\..\gui\core\font\truetype\src\cache\\" /I "..\..\gui\core\font\truetype\src\raster\\" /I "..\..\gui\core\font\truetype\src\sfnt\\" /I "..\..\gui\core\font\truetype\src\smooth\\" /I "..\..\gui\core\font\truetype\src\src_plus\\" /I "..\..\gui\core\font\truetype\src\truetype\\" /I "..\..\gui\core\font\truetype\src\\" /I "..\..\gui\core\font\truetype\\" /I "..\..\gui\core\font\\" /I "..\..\gui\core\kernel\\" /I "..\..\gui\core\\" /I "..\..\gui\\" /I "..\..\\" /I "..\..\..\..\includes\\" /I "..\..\..\..\INCLUDES\ELIBS\\" /I "..\..\..\..\LIVEDESK\LIVETOUCH\INCLUDE\\" /I "..\..\..\..\INCLUDES\EMOD\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\GUI\\" /I "..\..\..\..\INCLUDES\EMOD\MOD_ORANGE\RES\\" /I "..\..\CONFIG\\" /I "..\..\GUI\CORE\FONT\TRUETYPE\INCLUDE\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIM_MOD_ORANGE_EXPORTS" /D "SIM_PC_WIN" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\..\workspace\sim\bin\mod\orange.mod" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\..\workspace\sim\bin\mod\orange.mod" /implib:"..\..\..\..\workspace\sim\bin\mod\orange.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "sim_mod_orange - Win32 Release"
# Name "sim_mod_orange - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sim_mod_orange.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "mod_orange"

# PROP Default_Filter ""
# Begin Group "resreader"

# PROP Default_Filter ""
# Begin Group "lang"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\resreader\lang\langdec.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "them"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\resreader\them\os_related.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\resreader\them\themdec.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\kernel\gui_alloc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\kernel\gui_kernel.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\kernel\gui_lock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\kernel\gui_time.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\kernel\gui_timer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\common\memblock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\common\rect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\common\region.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "message"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\message\message.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "widget"

# PROP Default_Filter ""
# Begin Group "button"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\button\button.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "static"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\static\static.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "listmenu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\glidecurve.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listbox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listbox_process.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listbuffer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listlayer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listlayer_item.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listlayer_process.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\listmenu.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\menuitem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\menulist.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\listmenu\speed_step.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "progsheet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\progsheet\progsheet.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "slider"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\slider\bmp_op.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\slider\rect_op.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\slider\slider.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "txtbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtbox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtbuffer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtitem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtlayer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtlayer_process.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\widget\txtbox\txtlist.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "gmsgbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\gmsgbox\gmsgbox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "spinbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\widget\spinbox\spinbox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\widget\winclass.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "win"

# PROP Default_Filter ""
# Begin Group "ctrlwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\win\ctrlwin\ctrlwin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "frmwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\win\frmwin\dialog.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\frmwin\frmwin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "lyrwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\cursor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\fb_ops.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_alpha.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_cache.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_ck.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_lock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_move.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_setbottom.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\lyrwin\lyrwin_settop.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "manwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\win\manwin\manwin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\win\win.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\win_common.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\win\win_lock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "lcddev"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\lcddev\lcdwin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\lcddev\lcdwin1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\lcddev\lcdwin2.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\lcddev\lcdwin3.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "memdev"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_16.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_8.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_aa.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_auto.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_banding.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_clear.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_cmpwithlcd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_copyfromlcd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_createfixed.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_getdataptr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_getxsize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_getysize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_measure.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_reduceysize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_setorg.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_usage.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_usagebm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_write.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_writealpha.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_writeex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\memdev\memdev_xy2ptr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cc"

# PROP Default_Filter ""
# Begin Group "convertcolor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp111.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp222.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp233.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp323.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp332.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp444_12.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp444_12_1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp444_16.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp555.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp556.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp565.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp655.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp8666.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp8666_1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdp8888.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm233.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm323.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm332.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm444_12.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm444_16.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm555.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm556.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm565.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm655.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertcolor\lcdpm8888.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "convertmono"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\cc\convertmono\lcdp0.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertmono\lcdp2.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\cc\convertmono\lcdp4.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Group "sft"

# PROP Default_Filter ""
# Begin Group "sfte"

# PROP Default_Filter ""
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\sft\sfte\lib\sfte_os.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\core\font\sft\sfte\sfte.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\core\font\sft\gui_sft.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "buildin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\buildin\f6x8.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "truetype"

# PROP Default_Filter ""
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftapi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftbbox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftbdf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftbitmap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftcalc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftdbgmem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftdebug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftgasp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftgloadr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftglyph.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftgxval.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftinit.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftlcdfil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftmm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftnames.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftobjs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftotval.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftoutln.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftpatent.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftpfr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftrfork.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftstream.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftstroke.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftsynth.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftsystem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\fttrigon.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\fttype1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftwinfnt.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\base\ftxf86.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cache"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcbasic.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftccache.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftccmap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcglyph.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcimage.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcmanag.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcmru.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cache\ftcsbits.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "raster"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\raster\ftraster.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\raster\ftrend1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "sfnt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\sfnt\sfnt.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "smooth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\smooth\ftgrays.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\smooth\ftsmooth.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "src_plus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\src_plus\ftstdlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "truetype No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttdriver.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttgload.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttgxvar.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttinterp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttobjs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\truetype\ttpload.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "bdf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\bdf\bdf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cff\cff.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\cid\type1cid.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "gxvalid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\gxvalid\gxvalid.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "gzip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\gzip\ftgzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "lzw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\lzw\ftlzw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "otvalid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\otvalid\otvalid.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "pcf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\pcf\pcf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "pfr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\pfr\pfr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "psaux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\psaux\psaux.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "pshinter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\pshinter\pshinter.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "psnames"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\psnames\psnames.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "type1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\type1\type1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "type42"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\type42\type42.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "winfonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\winfonts\winfnt.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "autofit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\src\autofit\autofit.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\gui_ttf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\truetype\gui_ttf_cache.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\core\font\dilation.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\font\shadow.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "kernel No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\gui\core\kernel\big5_to_unicode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui2dlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__addspacehex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__calctextrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__divideround.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__divideround32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__getfontsizey.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__getnumchars.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__handleeoline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__intersectrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__intersectrects.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__memset.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__memset16.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__memset32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__read.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__reducerect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__settext.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__strcmp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__strlen.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui__wrap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_addbin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_adddec.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_adddecmin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_adddecshift.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_addhex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_addkeymsghook.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_alloc_allocinit.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_alloc_alloczero.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_alphablend.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_argb_draw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_bitstring_draw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_bmp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_bmp_serialize.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_calccolordist.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_clearrectex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_color2viscolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_copyrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowl.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowli.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowlpx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowmi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowmpx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrows.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowsi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorarrowspx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossl.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossli.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrosslpx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossmi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossmpx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrosss.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrosssi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorcrossspx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorheaderm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorheadermi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorheadermpx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorpal.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_cursorpali.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispbin.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispceol.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispchar.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispchars.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_disphex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstring.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringatceol.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringhcenter.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringinrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringinrectex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringinrectmax.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringinrectwrap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_dispstringlen.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawbitmap.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawbitmapex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawbitmapexp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawbitmapmag.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawfocusrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawgraph.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawhline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawpie.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawpixel.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawpolyline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawrectex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_drawvline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_errorout.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_exec.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_fillpolygon.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_fillrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_fillrectex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getbitmappixelcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getbitmappixelindex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getclientrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getdisppos.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getfont.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getfontinfo.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getfontsizey.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getlinestyle.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getstringdistx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_gettextalign.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_gettextextend.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_gettextmode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getversionstring.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_getysizeoffont.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_goto.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_invertrect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_isinfont.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_log.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_mergerect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_mouse.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_mouse_driverps2.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_moverect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_onkey.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_pen.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_pid.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_rectsintersect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_savecontext.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_selectlayer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_selectlcd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setcliprect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setcolorindex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setdecchar.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setdefault.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setdrawmode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setfont.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setfontmode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setlborder.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setlinestyle.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setlutcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setlutcolorex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setlutentry.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setorg.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setpixelindex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_setscreen.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_settextalign.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_settextmode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_settextstyle.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_sif.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_sif_prop.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_touch.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_touch_driveranalog.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_touch_storestate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_touch_storeunstable.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_dispstring.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodebig5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1250.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1251.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1253.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1255.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1256.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1257.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp1258.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodecp874.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeeucjp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeeuckr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodegb2312.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodegbk.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88591.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885910.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885911.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885912.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885913.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885914.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885915.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso885916.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88592.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88593.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88594.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88595.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88596.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88597.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88598.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeiso88599.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodekio8r.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodenone.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodesjis.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_uc_encodeutf8.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_waitevent.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_waitkey.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\gui_warn.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guialloc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guiarc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guichar.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicharline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicharm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicharp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicirc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicolor2index.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicore.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guicurs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guiencjs.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guiindex2color.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guipolye.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guipolym.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guipolyr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guirealloc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guistream.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guitask.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guitime.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guitimer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guiuc0.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guival.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\guivalf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_api.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_drawbitmap_565.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_drawbitmap_m565.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_drawvline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_getcolorindex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_getex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_getnumdisplays.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_getpixelcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_index2colorex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_l0_generic.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_mirror.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_mixcolors256.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_rotate180.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_rotateccw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_rotatecw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_selectlcd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_setapi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_setcliprectex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcd_updatecolorindices.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdaa.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdcolor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdgetp.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdinfo.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdinfo0.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdinfo1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdl0delta.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdl0mag.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdp1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdp565_index2color.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdpm565_index2color.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdrle4.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\kernel\lcdrle8.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\core\core.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\core\core_longname.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\gui\gui.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\gui\gui_x_epos.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\magic.c

!IF  "$(CFG)" == "sim_mod_orange - Win32 Release"

!ELSEIF  "$(CFG)" == "sim_mod_orange - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\mod_orange.c

!IF  "$(CFG)" == "sim_mod_orange - Win32 Release"

!ELSEIF  "$(CFG)" == "sim_mod_orange - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "awslibs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\workspace\sim\bin\Sim_DE_Drv.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\DISPLAY.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIMAUDIO.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_SYSTEMFUN.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_PC_WINAPI.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_PC_KERNEL.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_EPOS.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_ELIBS.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WORKSPACE\SIM\BIN\SIM_EBIOS.LIB
# End Source File
# End Group
# Begin Source File

SOURCE=.\funExport.def
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
