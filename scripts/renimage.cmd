/* REXX script to rename images according to the date they were downloaded.
 *
 * Written as an example of a simple script that can be called from
 * DCITU for OS/2.  See the "scripts" section of the options notebook
 * for additional information.  Use at your own risk.
 *
 * $Id: //devel/DCITU/scripts/renimage.cmd#1 $
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

say ''

/* get the old filename */
parse arg oldfilename
if oldfilename == '' then do
	say '[1;31mError - must specify 1 filename as a parameter![0m'
	return 1
	end

/* find the next available filename */
index = 0
do forever
	newfilename = date('S') || '-' || right(index, 3, '0') || '.jpg'

	/* break the loop if this filename is available */
	if stream(newfilename, 'c', 'query exists') == '' then leave

	/* otherwise, keep looking for a valid index */
	index = index + 1
	end

/* now rename the file */
say 'renaming [1m'oldfilename'[0m to [1m'newfilename'[0m...'
"@ren" oldfilename newfilename

return 0

