/*
 * $Id: //devel/DCITU/GUIWindow.HPP#7 $
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

#ifndef GUIWindow_HPP
#define GUIWindow_HPP

#include <icmdhdr.hpp>  // ICommandHandler
#include <icnrcol.hpp>  // IContainerColumn
#include <icnrctl.hpp>  // IContainerControl
#include <icnrdihd.hpp> // ICnrDrawHandler
#include <icnrehdr.hpp> // ICnrEditHandler
#include <icnrhdr.hpp>  // ICnrHandler
#include <icnrmhdr.hpp> // ICnrMenuHandler
#include <icolor.hpp>   // IColor
#include <icritsec.hpp> // ICritSec
#include <icustbut.hpp> // ICustomButton
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <ifont.hpp>    // IFont
#include <iframe.hpp>   // IFrameWindow
#include <imcelcv.hpp>  // IMultiCellCanvas
#include <imenubar.hpp> // IMenuBar
#include <imsgbox.hpp>  // IMessageBox
#include <ipopmenu.hpp> // IPopUpMenu
#include <iprofile.hpp> // IProfile
#include <islider.hpp>  // IProgressIndicator
#include <isizehdr.hpp> // IResizeHandler
#include <ithread.hpp>  // IThread, IThreadMemberFn

#include "ConnectionSettings.HPP"
#include "GUIWindow.H"


// class prototypes
class GUIWindow;
class CnrMleEditHandler;
class DebugWindow;
class OptionsWindow;
class CameraThread;


// this class is used as the MLE handler for the container;
// i.e., when the user decides to edit the name of an image
class CnrMleEditHandler : public IHandler
{
   public:
      virtual Boolean dispatchHandlerEvent(IEvent &event);

      GUIWindow *wnd;
};


class GUIWindow :
   public IFrameWindow,
   public ICommandHandler,
   protected IResizeHandler,
   protected ICnrHandler,
   protected ICnrDrawHandler,
   protected ICnrMenuHandler,
   protected ICnrEditHandler
{
   public:

      /* default constructor builds the IFrameWindow, starts the necessary
       * threads and initializes the DCITU environment
       */
      GUIWindow( const IString &iniFileName );

      /* destructor stops the handlers and destroys the camera object window
       */
      ~GUIWindow( VOID );

      /* method used to set the current drive & directory
       */
      GUIWindow &setPath(void);

      /* method inherited from IResizeHandler; necessary to reposition and
       * redraw the slider control at the bottom-left corner of the window
       */
      virtual Boolean windowResize( IResizeEvent  &event );

      /* method inherited from ICommandHandler; detects when the window is
       * being closed, and pops up a confirmation window if the camera is
       * still connected
       */
      virtual Boolean systemCommand(ICommandEvent &event );

      /* method inherited from ICommandHandler; used to handle all of the
       * menubar actions from the main GUI window
       */
      virtual Boolean command(      ICommandEvent &event );

      /* method inherited from ICnrDrawHandler; necessary to handle the drawing
       * of the thumbnails in the details view container
       */
      virtual Boolean drawDetailsItem( ICnrDrawItemEvent &event );

      /* method inherited from ICnrHandler; necessary to handle the drawing
       * of the target emphasis in the container
       */
      virtual Boolean windowScrolled( ICnrScrollEvent &event );

      /* method inherited from ICnrMenuHandler; used to display popup menus in
       * the container
       */
      virtual Boolean makePopUpMenu( IMenuEvent &event );

      /* method inherited from ICnrHandler; necessary to determine when the
       * selection emphasis should be re-drawn
       */
      virtual Boolean selectedChanged( ICnrEmphasisEvent &event );

      /* methods inherited from ICnrEditHandler; used to determine the start
       * and end of a container edit event
       */
      virtual Boolean beginEdit( ICnrBeginEditEvent &event );
      virtual Boolean endEdit( ICnrEndEditEvent &event );

      /* show the "About" messagebox window
       */
      void showAboutWindow( void );

      /* logs text to the status line of the GUI window, and to the debug
       * window if it is active
       */
      void logText( const char *text, const char *debugText=0 );

      /* started on a secondary thread to determine which COM ports are
       * available for use
       */
      void findAvailablePorts( void );

      /* creates and initializes the debug window
       */
      void createDebugWindow( void );

      /* enables certain menu items in the menu bar depending on the state of
       * the application
       */
      void enableMenuBarItems( void );

      /* disables all menu items in the menu bar; mostly called when the
       * connection thread is attempting to connect or disconnect from the
       * camera
       */
      void disableMenuBarItems( void );

      /* forces the slider control to a position based on the percentage of a
       * task that has been completed
       */
      void slideTo( int offset, int total=0 );

      /* attempts to validate the registration information, and sets
       * appropriate flags
       */
      void verifyRegistration( void );

      /* sets up the appropriate structures to call an external script
       */
      void runScript( const char *scriptName, const Boolean foreground );

      /* update the selection entryfield in the status bar
       */
      void getSelectionStatus( void );

      // instance variables
      IFlyText             flyText;
      IFlyOverHelpHandler  flyOverHelpHandler;

      IMultiCellCanvas     cnvMain;
      IMultiCellCanvas     cnvStatus;
      IMenuBar             menuBar;
      // controls used on the main window:
      IContainerControl    cnr;
      IContainerColumn     col1; // image
      IContainerColumn     col2; // name
      IContainerColumn     col3; // size
      IContainerColumn     col4; // date
      IContainerColumn     col5; // time
      // controls used on the status bar:
      IProgressIndicator   slider;
      IEntryField          efStatus;
      IEntryField          efSelected;
      IEntryField          efBaud;
      ICustomButton        pbPurge;
      // other variables
      IProfile             profile;
      ConnectionSettings   connectionSettings;
      IMessageBox         *aboutMsgBox;
      DebugWindow         *debugWindow;
      OptionsWindow       *optionsWindow;
      CameraThread        *cameraThread;
      int                  currentSliderTick;
      Boolean              isRegistered;
      IString              registeredName;
      IString              registeredKey;
      ULONG                oldDrive;
      IString              oldPath;

      CnrMleEditHandler    cnrMleEditHandler;
};

#endif

