@echo off

rem
rem $Id: //devel/DCITU/jpeg/build.cmd#3 $
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

del *.obj > nul
for %%a in (*.c) do icc /Tdp /O /Gm /G5 /C /Gd- %%a
copy *.obj ..
copy jconfig.h ..
copy jerror.h ..
copy jmorecfg.h ..
copy jpegint.h ..
copy jpeglib.h ..

