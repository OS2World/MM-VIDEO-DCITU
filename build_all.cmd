@echo off

rem
rem $Id: //devel/DCITU/build_all.cmd#5 $
rem
rem DCITU for OS/2 -- Digital Camera Image Transfer Utility for OS/2
rem Copyright (C) 1997-2000 Stephane Charette, stephanecharette@telus.net
rem Copyright (C) 2000-2002 C Code Run Consulting & Investments, Inc.
rem
rem This program is free software; you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation; either version 2 of the License, or at
rem your option, any later version.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
rem General Public License for more details.
rem

if exist dcitu.exe del dcitu.exe > nul

rem Go build the JPEG library
cd jpeg
call build.cmd
cd ..

rem Compile all of the .CPP files
for %%a in (*.cpp) do (icc /G5 /Gd- /Gm /Gl /Gi /O+ /C /Tl60 %%a & if errorlevel 1 pause)

rem The DC50 and Casio support was never completed,
rem so exclude them from the link step
if exist camera_kodak_dc50.obj del camera_kodak_dc50.obj > nul
if exist camera_casio_qv10.obj del camera_casio_qv10.obj > nul

rem Link all of the .OBJ files together
icc /Fedcitu.exe /G5 /Gd- /Gm /Gl /Gi /O+ /Tl60 /B"/pm:pm /optfunc /packc /packd /exepack:2" *.obj

rem Compile and bind the resource file
if exist dcitu.exe rc -x2 main.rc dcitu.exe

rem Make the .HLP file if it doesn't exist
if not exist dcitu.hlp call makehelp.cmd

