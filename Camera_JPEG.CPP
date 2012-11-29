/*
 * $Id: //devel/DCITU/Camera_JPEG.CPP#4 $
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

#include "GUIWindow.HPP"
#include "ImageObject.HPP"
#include "Camera_JPEG.HPP"

/*
 * jpegLibraryError
 */
void jpegLibraryError(j_common_ptr cinfo)
{
   // get the library to print out the error
   char buffer[JMSG_LENGTH_MAX];
   (*cinfo->err->format_message) (cinfo, buffer);

   // we'll be sneaky -- use the static wnd object from ImageObject to gain
   // access to the logText debug routines; this prevents us from having to
   // declare our own static object to use
   ImageObject::wnd->logText( 0, IString("==> JPEG library error: ") + buffer);

   jpeg_destroy(cinfo);

   IInvalidRequest exception("JPEG LIBRARY ERROR", 1, IInvalidRequest::recoverable);
   ITHROW(exception);
}

