/*
 * $Id: //devel/DCITU/DebugWindow.HPP#4 $
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

#ifndef DebugWindow_HPP
#define DebugWindow_HPP

#define INCL_DOSFILEMGR
#include <icmdhdr.hpp>  // ICommandHandler
#include <icolor.hpp>   // IColor
#include <icritsec.hpp> // ICritSec
#include <iframe.hpp>   // IFrameWindow
#include <ikeyhdr.hpp>  // IKeyboardHandler
#include <ilistbox.hpp> // IListBox
#include <istring.hpp>  // IString
#include <isysmenu.hpp> // ISystemMenu
#include <os2.h>

#include "GUIWindow.hpp"


class DebugWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected IKeyboardHandler
{
   public:
      /* default constructor builds a listbox bound into a frame window
       * to be used for all debug text logs; this constructor also opens
       * the DCITU.LOG debug file
       */
      DebugWindow( GUIWindow &ownerWindow );

      /* destructor closes the DCITU.LOG file
       */
      ~DebugWindow( void );

      /* outputs the text to the listbox; if the DCITU.LOG file was opened
       * during the constructor, then the same text is also sent to file
       */
      void logText( const char *text );

      /* inherited methods
       */
      virtual Boolean systemCommand( ICommandEvent &event );
      virtual IFrameWindow &close(void);
      virtual Boolean virtualKeyPress( IKeyboardEvent &event );

      // instance variables
      GUIWindow  &wnd;
      IListBox    lbDebug;
      HFILE       fileHandle;
};

#endif


