/* REXX script to set the date/time for an image (Kodak DC265)
 *
 * Written as an example of a simple script that can be called from
 * DCITU for OS/2.  See the "scripts" section of the options notebook
 * for additional information.  Use at your own risk.
 *
 * $Id: //devel/DCITU/scripts/kodak_dc265_set_datetime.cmd#1 $
 *
 * DCITU for OS/2 -- Digital Camera Image Transfer Utility for OS/2
 * Copyright (C) 1997-2000 Stephane Charette, stephanecharette@telus.net
 * Copyright (C) 2000-2002 C Code Run Consulting & Investments, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or at
 * your option, any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

/* make certain that all of the REXX functions are loaded */
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

parse arg fileargs

if fileargs == '' then do
   say ''
   say 'This script will set the date/time of images downloaded'
   say 'from Kodak DC265 cameras.  This script may also work with'
   say 'other cameras with little modifications.'
   say ''
   say 'Usage:  kodak_dc265_set_datetime filename1 [filename2 filename3 ...]'
   say ''
   say 'Example:  kodak_dc265_set_datetime house_pics*.jpg vaction*.jpg kids_3.jpg'
   say ''
   say 'Note:  you also need to have FileSetDateTime available somewhere in'
   say '       your path; it can be downloaded from "http://hobbes.nmsu.edu/".'
   return 1
   end

/* ensure that FileSetDateTime.exe is availble */
touchTool = 'FileSetDateTime.exe'
"@"touchTool ">nul"
ret = rc
if ret <> 16 then do
   say 'ERROR: failed to run "'touchTool'"!'d2c(7)
   return 1
   end

/* work through all of the filespecs one at a time */
do idx = 1 to words(fileargs)

   filespec = word(fileargs, idx)
   say 'Processing' filespec'...'

   call SysFileTree filespec, 'file', 'fo'
   if file.0 == 0 then do
      say 'Cannot find a file to match "'filespec'"!'
      end
   else do fileIdx = 1 to file.0
      call setDateTimeFor file.fileIdx
      end
   end

return


setDateTimeFor : procedure expose touchTool

   /* set the date/time of the image */

   parse arg filename

   /* For the Kodak DC265, we'll begin looking around offset 0x01B0 for a
    * specific 20-byte string.  In most images, the string is at offset
    * 0x01C4 but I've also seen it at offset 0x01D0.  (This is with the
    * same camera, just 2 consecutive pictures!)
    */

   /* open the file */
   call charout , '   ->' filename': '
   call stream filename, 'c', 'open read'

   /* read a 500-byte block into memory */
   buffer = charin(filename, x2d('1C0'), 500)
   call stream filename, 'c', 'close'

   /* look through this block for the following string:
    *
    *       '2???:??:?? ??:??:??'
    *
    * for example:
    *
    *       '2001:06:28 19:12:03'
    *
    */
   parse var buffer '2'yyyy':'mm1':'dd' 'hh':'mm2':'ss .

   /* add back the '2' to the start of the year */
   yyyy = '2'yyyy

   /* cut off the 'ss' after the 2nd digit */
   ss = left(ss, 2)

   /* do some sanity check to ensure we've found the right thing */
   if datatype(yyyy) <> 'NUM' | ,
      datatype(mm1)  <> 'NUM' | ,
      datatype(dd)   <> 'NUM' | ,
      datatype(hh)   <> 'NUM' | ,
      datatype(mm2)  <> 'NUM' | ,
      datatype(ss)   <> 'NUM' | ,
      yyyy  <  2000  | ,
      yyyy  >  2999  | ,
      mm1   <  1     | ,
      mm1   >  12    | ,
      dd    <  1     | ,
      dd    >  31    | ,
      hh    <  0     | ,
      hh    >  23    | ,
      mm2   <  0     | ,
      mm2   >  59    | ,
      ss    <  0     | ,
      ss    >  59    then do
      say 'ERROR: cannot determine date/time!'d2c(7)
      return
      end

   /* now we know the exact date/time this picture was taken */
   dateTimeStamp = yyyy'-'mm1'-'dd hh':'mm2':'ss
   call charout , dateTimeStamp || ' '

   /* call the command that will set the date/time */
   "@"touchTool dateTimeStamp filename
   ret = rc

   /* ensure that the date/time was set correctly */
   if ret == 0 then do
      say '[OK]'
      end
   else do
      say '[ERROR!]'d2c(7)
      end

   return

