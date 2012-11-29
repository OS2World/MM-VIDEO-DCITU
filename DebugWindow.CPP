/*
 * $Id: //devel/DCITU/DebugWindow.CPP#5 $
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

#include "DebugWindow.HPP"
#include "GUIWindow.HPP"
#include "GUIWindow.H"
#include <itime.hpp>


/*****************************************************************************
 *
 *    DebugWindow::DebugWindow
 *
 * constructor for the debug window
 *
 *****************************************************************************/
DebugWindow::DebugWindow( GUIWindow &ownerWindow ) :

   IFrameWindow(  IResourceId( DEBUG_WND_RCID ),
                  IWindow::desktopWindow(),  // parent
                  &ownerWindow,              // owner
                  IRectangle(),
                 (IFrameWindow::classDefaultStyle  |
                  IFrameWindow::noMoveWithOwner    |
                  IFrameWindow::shellPosition      |
                  IFrameWindow::dialogBackground)  &
                 ~IFrameWindow::windowList,
                  IString(STR_TITLE) + " - Debug Log" ),

   wnd(           ownerWindow ),

   lbDebug(       DEBUG_LB_RCID, this, this )
{
   // set the same icon as the main window
   setIcon( WND_RCID );

   // make certain the window is deleted when closed
   setAutoDeleteObject( true );
   setAutoDestroyWindow( true );

   // start event processing
   IKeyboardHandler::handleEventsFor( this );
   ICommandHandler::handleEventsFor( this );

   // open the debug file
   IString filename( ((GUIWindow*)owner())->connectionSettings.dir + "DCITU.LOG" );

   ULONG action;
   if( DosOpen(   filename                ,  // filename
                  &fileHandle             ,  // file handle
                  &action                 ,  // action that DosOpen carried out
                  0                       ,  // new size of file
                  FILE_NORMAL             ,  // new file attributes
                  OPEN_ACTION_CREATE_IF_NEW| // what to do with the file if new...
                  OPEN_ACTION_REPLACE_IF_EXISTS,// ...and if it exists
                  OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                //OPEN_FLAGS_WRITE_THROUGH|  // do not cache write to the file
                //OPEN_FLAGS_NO_CACHE     |  // completely do away with cacheing
                  OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                  OPEN_SHARE_DENYWRITE    |  // deny others from write
                  OPEN_ACCESS_WRITEONLY   ,  // open for write
                  (PEAOP2)0               ) )// no extended attributes
   {
      fileHandle = (HFILE)0;
   }

   // setup the listbox
   lbDebug. setBackgroundColor( backgroundColor() );
   logText( IString()         +
            STR_TITLE         +
            ", v"             +
            STR_VERSION_MAJOR +
            "."               +
            STR_VERSION_MINOR +
            STR_VERSION_REV   +
            " ("              +
            STR_DATE          +
            ")"               );

   logText( IString()         +
            STR_NAME          +
            ", "              +
            STR_EMAIL         );

   if( ((GUIWindow*)owner())->isRegistered )
   {
      logText( "This copy is registered to " + ((GUIWindow*)owner())->registeredName );
   }
   else
   {
      logText( "This copy is not registered." );
   }

   logText( "" );

   if( fileHandle )
   {
      logText( "Debug information is logged to \"" + filename + "\"" );
   }
   else
   {
      logText( "WARNING: COULD NOT OPEN THE DEBUG FILE \"" + filename + "\"!" );
      logText( "" );
   }

   logText( "Debug log:" );
   logText( "" );

   // set the listbox as the frame's client
   setClient( &lbDebug );

   // re-size to a more reasonable default size
   if( wnd.profile.containsApplication("Debug Window") )
   {
      // move & size the window to what was saved in the .INI file
      wnd.profile.setDefaultApplicationName( "Debug Window" );
      moveSizeTo( IRectangle( wnd.profile.integerWithKey( "x1" ),
                              wnd.profile.integerWithKey( "y1" ),
                              wnd.profile.integerWithKey( "x2" ),
                              wnd.profile.integerWithKey( "y2" ) ) );
   }
   else
   {
      // re-size to a more reasonable default size
      sizeTo( ISize(200,200) );
   }

   return;
}


/*****************************************************************************
 *
 *    DebugWindow::~DebugWindow
 *
 * destructor for the debug window; closes the debug file handle
 *
 *****************************************************************************/
DebugWindow::~DebugWindow()
{
   // stop event processing
   IKeyboardHandler::stopHandlingEventsFor( this );
   ICommandHandler::stopHandlingEventsFor( this );

   // close the debug file
   if( fileHandle )
   {
      DosClose( fileHandle );
      fileHandle = (HFILE)0;
   }
   return;
}


/*****************************************************************************
 *
 *    DebugWindow::logText
 *
 * adds the text to the the debug window and to the debug out file
 *
 *****************************************************************************/
void DebugWindow::logText( const char *text )
{
   // add the text to the debug window
   lbDebug.addAsLast( text );

   // if the debug file is open, write it to file
   if( fileHandle )
   {
      ULONG bytesWritten;
      IString message = IString(text) + "\r\n";

      // if extensive debug logging is enabled, then put a timestamp on each message
      message = ITime::now().asString("[%H:%M:%S] ") + message;

      DosWrite( fileHandle, (char*)message, message.length(), &bytesWritten );
   }
   return;
}


/*****************************************************************************
 *
 *    DebugWindow::systemCommand
 *
 * process the system commands; inherited from ICommandHandler
 *
 *****************************************************************************/
Boolean DebugWindow::systemCommand( ICommandEvent &event )
{
   Boolean result = false;

   if( event.commandId() == ISystemMenu::idClose )
   {
      // post a message to the main GUI window to toggle the debug window switch
      wnd.postEvent( IWindow::command,
                     IEventParameter1(MB_OUTPUT_RCID),
                     IEventParameter2(0) );

      // indicate that this event has been processed
      result = true;
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    DebugWindow::close
 *
 * trap close() inherited from IFrameWindow and save the window position
 *
 *****************************************************************************/
IFrameWindow &DebugWindow::close()
{
   // get the window's frame rectangle
   IRectangle wndRect( isMinimized()||isMaximized() ? restoreRect() : rect() );

   // save the window position
   wnd.profile.setDefaultApplicationName( "Debug Window" ).
               addOrReplaceElementWithKey( "x1", wndRect.minX() ).
               addOrReplaceElementWithKey( "y1", wndRect.minY() ).
               addOrReplaceElementWithKey( "x2", wndRect.maxX() ).
               addOrReplaceElementWithKey( "y2", wndRect.maxY() );

   return IFrameWindow::close();
}


/*****************************************************************************
 *
 *    DebugWindow::virtualKeyPress
 *
 * catch virtual keys; inherited from IKeyboardHandler
 *
 *****************************************************************************/
Boolean DebugWindow::virtualKeyPress( IKeyboardEvent &event )
{
   Boolean result = false;

   // if the TAB or SHIFT-TAB key was pressed...
   if(   event.virtualKey() == IKeyboardEvent::tab ||
         event.virtualKey() == IKeyboardEvent::backTab )
   {
      // ...then set the focus back to the main GUI window
      wnd.setFocus();
      result = true;
   }

   // if the ESC key was pressed...
   if( event.virtualKey() == IKeyboardEvent::esc )
   {
      // ...then clear the debug listbox
      logText( "(clearing debug log window)" );
      lbDebug.removeAll();
      result = true;
   }

   event.setResult( result );
   return result;
}

