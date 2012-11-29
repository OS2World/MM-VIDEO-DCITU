/*
 * $Id: //devel/DCITU/GUIWindow.CPP#9 $
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

#define INCL_DOSMISC
#define INCL_WIN
#define INCL_PM

#include "GUIWindow.H"
#include "GUIWindow.HPP"
#include "DebugWindow.HPP"
#include "OptionsWindow.HPP"
#include "ImageObject.HPP"
#include "Camera.HPP"
#include <os2.h>


GUIWindow::GUIWindow(const IString &iniFileName) :

   IFrameWindow(  STR_TITLE,
                  IResourceId( WND_RCID ),
                  IFrameWindow::classDefaultStyle  |
                  IFrameWindow::accelerator        |
                  IFrameWindow::animated           |
                  IFrameWindow::dialogBackground   |
                  IFrameWindow::menuBar            |
                  IFrameWindow::minimizedIcon      ),

   IResizeHandler(),

   flyText(       0, this ),

   flyOverHelpHandler( &flyText, 1, 1 ),

   cnvMain(       CNV_MAIN_RCID, this, this ),

   cnvStatus(     CNV_STATUS_RCID, this, this ),

   menuBar(       this,
                  IMenuBar::wrapper ),

   cnr(           CNR_RCID,
                  &cnvMain,
                  &cnvMain,
                  IRectangle(),
                 (IContainerControl::classDefaultStyle   |
                  IContainerControl::autoPosition        |
                  IContainerControl::group               |
                  IContainerControl::tabStop             |
                  IContainerControl::multipleSelection)  &
                 ~IContainerControl::singleSelection     ,
                 (IContainerControl::classDefaultAttribute|
                  IContainerControl::handleDrawItem      |
                  IContainerControl::detailsView)        &
                 ~IContainerControl::iconView            ),

   slider(        SLIDER_RCID,
                  &cnvStatus,
                  &cnvStatus,
                  IRectangle(),
                  10,
                  0,
                  10 ),

   efStatus(      EF_STATUS_RCID,
                  &cnvStatus,
                  &cnvStatus,
                  IRectangle(),
                  IEntryField::classDefaultStyle   |
                  IEntryField::readOnly            ),

   efSelected(    EF_SELECTED_RCID,
                  &cnvStatus,
                  &cnvStatus,
                  IRectangle(),
                  IEntryField::classDefaultStyle   |
                  IEntryField::centerAlign         |
                  IEntryField::readOnly            &
                 ~IEntryField::leftAlign           ),

   efBaud(        EF_BAUD_RCID,
                  &cnvStatus,
                  &cnvStatus,
                  IRectangle(),
                  IEntryField::classDefaultStyle   |
                  IEntryField::centerAlign         |
                  IEntryField::readOnly            &
                 ~IEntryField::leftAlign           ),

   pbPurge(       PB_PURGE_RCID,
                  &cnvStatus,
                  &cnvStatus,
                  IRectangle(),
                  ICustomButton::classDefaultStyle |
                  ICustomButton::noPointerFocus    ),

   // image
   col1(          IContainerColumn::isIcon,
                 (IContainerColumn::classDefaultHeadingStyle|
                  IContainerColumn::alignCentered)          &
                 ~IContainerColumn::alignLeft               ,
                  IContainerColumn::classDefaultDataStyle   |
                  IContainerColumn::horizontalSeparator     |
                  IContainerColumn::verticalSeparator       ),

   // name
   col2(          offsetof(ImageObject,name)                ,
                 (IContainerColumn::classDefaultHeadingStyle|
                  IContainerColumn::alignCentered)          &
                 ~IContainerColumn::alignLeft               ,
                  IContainerColumn::classDefaultDataStyle   |
                  IContainerColumn::horizontalSeparator     |
                  IContainerColumn::verticalSeparator       &
                 ~IContainerColumn::readOnly                ), // added ~RO for modifying the image names

   // size
   col3(          offsetof(ImageObject,size)                ,
                 (IContainerColumn::classDefaultHeadingStyle|
                  IContainerColumn::alignCentered)          &
                 ~IContainerColumn::alignLeft               ,
                  IContainerColumn::classDefaultDataStyle   |
                  IContainerColumn::horizontalSeparator     |
                  IContainerColumn::verticalSeparator       ),

   // date
   col4(          offsetof(ImageObject,date)                ,
                 (IContainerColumn::classDefaultHeadingStyle|
                  IContainerColumn::alignCentered)          &
                 ~IContainerColumn::alignLeft               ,
                  IContainerColumn::classDefaultDataStyle   |
                  IContainerColumn::horizontalSeparator     |
                  IContainerColumn::verticalSeparator       ),

   // time
   col5(          offsetof(ImageObject,time)                ,
                 (IContainerColumn::classDefaultHeadingStyle|
                  IContainerColumn::alignCentered)          &
                 ~IContainerColumn::alignLeft               ,
                  IContainerColumn::classDefaultDataStyle   |
                  IContainerColumn::horizontalSeparator     ),

   profile(       iniFileName ),

   connectionSettings( profile),

   oldDrive(      0           ),
   oldPath(       ""          )
{
   // initialize class variables
   aboutMsgBox = (IMessageBox*)0;
   debugWindow = (DebugWindow*)0;
   optionsWindow = (OptionsWindow*)0;
   cameraThread = (CameraThread*)0;
   isRegistered = false;
   registeredName = "";

   // automatically delete the window when the object is closed
   // (these were disabled to stop the SYS3175 problem with
   // IFlyOverHelpHandler when the window is closed)
   //setDestroyOnClose( true );
   //setAutoDestroyWindow( true );
   //setAutoDeleteObject( true );

   // initialize static variables
   ImageObject::wnd = this;
   cnrMleEditHandler.wnd = this;

   // check if this application is registered
   verifyRegistration();

   // set the main canvas
   cnvMain.    setRowHeight(     1, 5,    false ). // .
               setRowHeight(     2, 0,    true  ). // cnr
               setRowHeight(     3, 5,    false ). // .
               setColumnWidth(   1, 5,    false ). // .
               setColumnWidth(   2, 0,    true  ). // cnr
               setColumnWidth(   3, 5,    false ). // .
               addToCell(  &cnr, 2, 2, 1, 1     ).
               setBackgroundColor( backgroundColor() );

   // set the image object refresh offset, based on the font used
   IFont cnrFont( &cnr );
   ImageObject::setRefreshOffset( cnrFont.maxCharHeight()-2 );

   // setup the container
   col1.setHeadingText( "Image" ).setDisplayWidth( 96 );
   col2.setHeadingText( "Name" ).enableDataUpdate();;
   col3.setHeadingText( "Size" );
   col4.setHeadingText( "Date" );
   col5.setHeadingText( "Time" );
   cnr.        setRefreshOff().
               addColumn( &col1 ).
               addColumn( &col2 ).
               addColumn( &col3 ).
               addColumn( &col4 ).
               addColumn( &col5 ).
               setLineSpacing(74-(cnrFont.maxCharHeight()*2)).
               setRefreshOn().
               refresh().
               setBackgroundColor( IColor::white );

   // set the main canvas as the frame's client
   setClient( &cnvMain );

   // set the status canvas
   cnvStatus.  setRowHeight(     1, 0,    true  ).
               setRowHeight(     2, 0,    false ).
               setColumnWidth(   1, 75,   false ).
               setColumnWidth(   2, 0,    true  ).
               setColumnWidth(   3, 0,    false ).
               setColumnWidth(   4, 0,    false ).
               setColumnWidth(   5, 0,    false ).
               addToCell(  &slider,       1, 1, 1, 2  ).
               addToCell(  &efStatus,     2, 1, 1, 2  ).
               addToCell(  &efSelected,   3, 1, 1, 2  ).
               addToCell(  &efBaud,       4, 1, 1, 2  ).
               addToCell(  &pbPurge,      5, 2, 1, 1  ).
               setBackgroundColor( backgroundColor() );

   // install the status canvas on the frame window
   addExtension( &cnvStatus, IFrameWindow::belowClient, IFrameWindow::none );//thinLine );

   // configure the menubar
   menuBar. setConditionalCascade( MB_THUMBNAIL_RCID, MB_COLOUR_TN_RCID ).
            setConditionalCascade( MB_SELECT_RCID, MB_SELECT_ALL_RCID );

   // configure the slider control in the status bar
   slider.  setPrimaryScale( IProgressIndicator::scale2 ).
            enableSnapToTick().
            setShaftBreadth(10).
            setTickLength(2).
            setTickLength(0,3).
            setTickLength(4,3).
            setTickLength(9,3).
            setBackgroundColor(backgroundColor());
   slideTo( 0 );

   // configure the text status entry fields
   efStatus.setLimit( 50 ).
            setText("Loading...").
            setBackgroundColor(backgroundColor()).
            setHiliteForegroundColor(efStatus.foregroundColor()).
            setHiliteBackgroundColor(efStatus.backgroundColor());

   efBaud.  setLimit( 6 ).
            setText( "" ).
            setBackgroundColor(backgroundColor()).
            setHiliteForegroundColor(efStatus.foregroundColor()).
            setHiliteBackgroundColor(efStatus.backgroundColor());

   efSelected. setLimit( 7 ).
               setText( "" ).
               setBackgroundColor(backgroundColor()).
               setHiliteForegroundColor(efStatus.foregroundColor()).
               setHiliteBackgroundColor(efStatus.backgroundColor());

   // configure the purge latch button
   pbPurge.latch(false,true).setLatchedBackgroundColor(IColor::red,false).setText("0");

   // setup the handlers
   IResizeHandler::  handleEventsFor( this );
   ICommandHandler:: handleEventsFor( this );
   ICnrHandler::     handleEventsFor( &cnr );
   ICnrDrawHandler:: handleEventsFor( &cnr );
   ICnrMenuHandler:: handleEventsFor( &cnr );
   ICnrEditHandler:: handleEventsFor( &cnr );
   flyOverHelpHandler.handleEventsFor( &cnvStatus );

   ICnrEditHandler:: setMLEHandler( &cnrMleEditHandler );

   // is this window in the .INI file?
   if( profile.containsApplication("DCITU Main Window") )
   {
      // move & size the window to what was saved in the .INI file
      profile.setDefaultApplicationName( "DCITU Main Window" );
      moveSizeTo( IRectangle( profile.integerWithKey( "x1" ),
                              profile.integerWithKey( "y1" ),
                              profile.integerWithKey( "x2" ),
                              profile.integerWithKey( "y2" ) ) );

      // is the debug flag enabled?
      if( profile.integerWithKey("debug") )
      {
         // create the debug window
         createDebugWindow();
         logText( 0, "using " + iniFileName + " for settings" );
      }
   }
   else
   {
      // re-size to a more reasonable default size
      sizeTo( cnvMain.minimumSize()+cnvStatus.minimumSize()+ISize(20,75) );
      createDebugWindow(); // if the .INI file does not exist, enable debug window!
      logText( 0, "creating the new file " + iniFileName + " for settings" );
   }

   // the window is now ready to be shown
   logText( 0, "showing main window" );
   show();

   // create a new thread to handle the camera connection
   logText( 0, "starting new thread for the camera connection" );
   cameraThread = new CameraThread( *this );
   IThread thread2( cameraThread, true );

   // create a new thread to check the com ports
   logText( 0, "starting new thread to find available COM ports" );
   IThread thread3( new IThreadMemberFn<GUIWindow>(*this, findAvailablePorts) );


   // get the current disk and path so we can restore them when we're done
   ULONG tmp = 0;
   DosQueryCurrentDisk(&oldDrive, &tmp);
   oldPath = " ";
   tmp = 1;
   DosQueryCurrentDir(oldDrive, &(oldPath[1]), &tmp);
   oldPath.leftJustify(tmp + 1);
   DosQueryCurrentDir(oldDrive, &(oldPath[1]), &tmp);

   setPath();

   // see if we need to increase our thread priority
   if(connectionSettings.useHighestPriority)
   {
      logText( 0, "increase GUI thread priority to highest level possible" );
      IThread::current().setPriority( IApplication::foregroundServer, 31 );
   }

   return;
}


GUIWindow::~GUIWindow()
{
   // stop the window handlers
   flyOverHelpHandler.stopHandlingEventsFor( &cnvStatus );
   IResizeHandler::  stopHandlingEventsFor( this );
   ICommandHandler:: stopHandlingEventsFor( this );
   ICnrHandler::     stopHandlingEventsFor( &cnr );
   ICnrDrawHandler:: stopHandlingEventsFor( &cnr );
   ICnrMenuHandler:: stopHandlingEventsFor( &cnr );
   ICnrEditHandler:: stopHandlingEventsFor( &cnr );

   // is there a camera thread?
   if( cameraThread )
   {
      // close the camera thread
      delete( cameraThread );
      cameraThread = (CameraThread*)0;
   }

   // is there a debug window?
   if( debugWindow )
   {
      // this toggles the window -- thus, turning it off
      // since we've checked to see that it is active
      createDebugWindow();
   }

   // restore the working drive/path
   if(oldDrive != 0)
   {
      DosSetDefaultDisk(oldDrive);
   }
   if(oldPath != "")
   {
      DosSetCurrentDir(oldPath);
   }

   return;
}


GUIWindow &GUIWindow::setPath(void)
{
   APIRET rc1 = 0;
   APIRET rc2 = 0;

   // this method was implemented to get around a strange bug with WinStartApp()
   // that caused DCITU to hang the system every time the working drive & path
   // was specified for a script -- thus, the path is now changed when DCITU
   // starts, and remains that way until DCITU ends

   // get the transfer directory
   IString path(connectionSettings.dir);

   // get rid of the trailing slash on the path
   path.remove(path.lastIndexOfAnyOf("\\/"));

   if((path.length() > 2) && (path[2] == ':'))
   {
      // get the disk number
      ULONG disknum;
      disknum = IString::upperCase(path)[1] - 'A' + 1;

      // set the current disk
      logText( 0, "==> set \"" + IString((char)(disknum + 'A' - 1)) + ":\" as current drive" );
      rc1 = DosSetDefaultDisk(disknum);
      if(rc1)
      {
         logText( 0, "==> ignoring error: DosSetDefaultDisk() returned rc=" + IString(rc1) + " (0x" + IString(rc1).d2x() + ")" );
      }
   }

   if(path != ".")
   {
      // set the current path
      logText( 0, "==> set \"" + path + "\" as current directory" );
      rc2 = DosSetCurrentDir(path);
      if(rc2)
      {
         logText( 0, "==> ignoring error: DosSetCurrentDir() returned rc=" + IString(rc2) + " (0x" + IString(rc2).d2x() + ")" );
      }
   }

   // some type of error has taken place
   if(rc1 || rc2)
   {
      ULONG disk = 0;
      ULONG tmp = 0;
      IString currentPath = " ";

      // get the current disk
      DosQueryCurrentDisk(&disk, &tmp);

      // get the length of the path
      tmp = 1;
      DosQueryCurrentDir(disk, &(currentPath[1]), &tmp);

      // make some room to get the path
      currentPath.leftJustify(tmp + 3);
      DosQueryCurrentDir(disk, &(currentPath[4]), &tmp);
      currentPath.stripTrailing('\0');

      // prepend the drive
      currentPath[1] = disk + 'A' - 1;
      currentPath[2] = ':';
      currentPath[3] = '\\';

      // show a warning to the user
      IString message("Failed to change the ");

      if(rc1 && !rc2)
      {
         message += "drive";
      }
      else if(!rc1 && rc2)
      {
         message += "directory";
      }
      else
      {
         message += "drive and directory";
      }

      message += " to \"" + path + "\".  Current working directory is \"" + currentPath + "\".";

      IMessageBox mbox(this);
      mbox. setTitle(STR_TITLE).
            show( message,
                  IMessageBox::systemModal   |
                  IMessageBox::defButton1    |
                  IMessageBox::moveable      |
                  IMessageBox::okButton      |
                  IMessageBox::errorIcon     );
   }

   return *this;
}


Boolean GUIWindow::windowResize( IResizeEvent &event )
{
   slider.moveArmToTick( currentSliderTick );
   event.setResult( false );
   return false;
}


Boolean GUIWindow::systemCommand( ICommandEvent &event )
{
   Boolean result = false;

   // is this window being closed?
   if( event.commandId() == ISystemMenu::idClose )
   {
      // are we connected?
      if( cameraThread->camera->connected )
      {
         IMessageBox msgBox( this );
         if( msgBox. show( "Quit without disconnecting from the camera?",
                     IMessageBox::defButton2       |
                     IMessageBox::moveable         |
                     IMessageBox::queryIcon        |
                     IMessageBox::applicationModal |
                     IMessageBox::yesNoButton      ) == IMessageBox::no )
         {
            // user does NOT want to quit!
            result = true; // set the event as having been handled
         }
      }

      if( ! result )
      {
         // user is quitting

         // get the window's frame rectangle
         IRectangle wndRect( isMinimized()||isMaximized() ? restoreRect() : rect() );

         // save the window position
         profile. setDefaultApplicationName( "DCITU Main Window" ).
                  addOrReplaceElementWithKey( "x1", wndRect.minX() ).
                  addOrReplaceElementWithKey( "y1", wndRect.minY() ).
                  addOrReplaceElementWithKey( "x2", wndRect.maxX() ).
                  addOrReplaceElementWithKey( "y2", wndRect.maxY() ).
                  addOrReplaceElementWithKey( "debug", debugWindow?true:false );

         hide();

         // is the debug window showing?
         if( debugWindow )
         {
            // close it (to save the window position)
            createDebugWindow();
         }

         // get the OS/2 version number
         ULONG versionBuffer[2];
         DosQuerySysInfo( QSV_VERSION_MAJOR, QSV_VERSION_MINOR, versionBuffer, sizeof(versionBuffer) );

         // are we using Warp 3 or earlier?
         if( (versionBuffer[0] <= 20) && (versionBuffer[1] < 40) )
         {
            logText( 0, "-> detected OS/2 version 3 or earlier; using alternate shutdown" );
            logText( 0, "-> calling ICurrentApplication::exit" );
            IApplication::current().exit();
            logText( 0, "-> exit failed, calling ICurrentThread::stopProcessingMsgs" );
            IThread::current().stopProcessingMsgs();
         }
         else
         {
            logText( 0, "-> detected OS/2 version 4 or greater; using normal shutdown" );
         }
      }
   }

   // pass the event to the next handler
   event.setResult( result );
   return result;
}


Boolean GUIWindow::command( ICommandEvent &event )
{
   Boolean result = false;

   switch( event.commandId() )
   {
      case MB_MOVE_FOCUS_RCID:
      {
         // move the focus to one of the secondary windows...if they exist
         if( debugWindow )
         {
            debugWindow->setFocus();
         }
         else if( optionsWindow )
         {
            optionsWindow->setFocus();
         }
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_PURGE_RCID:
      {
         // toggle the purge latch button
         pbPurge.click();
         // no break statement -- fall through to PB_PURGE_RCID
      }
      case PB_PURGE_RCID:
      {
         if( pbPurge.text() != "0" )
         {
            // set the "cancel" flag to the latched state
            cameraThread->camera->cancelRequested = pbPurge.isLatched();
            menuBar.checkItem( MB_PURGE_RCID, pbPurge.isLatched() ).enableItem( MB_PURGE_RCID );
         }
         else
         {
            // prevent the button from being latched when there are 0 items
            pbPurge.unlatch().disableLatching();
            menuBar.checkItem( MB_PURGE_RCID, false ).disableItem( MB_PURGE_RCID );
         }

         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_ABOUT_RCID:
      {
         if( aboutMsgBox==0 )
         {
            // create a new "About" message box
            aboutMsgBox = new IMessageBox( 0 );

            // show the "About" message box on a secondary thread to prevent queue lockup
            IThread aboutWindowThread( new IThreadMemberFn<GUIWindow>(*this, showAboutWindow) );
         }
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_OUTPUT_RCID:
      {
         createDebugWindow();
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_OPTIONS_RCID:
      {
         if( optionsWindow==0 )
         {
            // create a new options window
            optionsWindow = new OptionsWindow( *this );
         }
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_SAVE_OPTIONS_RCID:
      {
         // save the settings
         connectionSettings.save();
         logText( "Options saved." );
         result = true;
         break;
      }
      case MB_CONNECT_RCID:
      {
         // the user wants to connect to the camera
         disableMenuBarItems();
         cameraThread->camera->sendMessage(Camera::CONNECT);
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_DISCONNECT_RCID:
      {
         // the user wants to connect to the camera
         disableMenuBarItems();
         cameraThread->camera->sendMessage(Camera::DISCONNECT);
         // mark the event as having been handled
         result = true;
         break;
      }
      case MB_COLOUR_TN_RCID:
      {
         // for every image selected, send a message to get the thumbnail
         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cameraThread->camera->sendMessage(
               Camera::GET_LARGE_THUMBNAIL,
               ((ImageObject*)cursor.current())->originalNumber );
         }
         result = true;
         break;
      }
      case MB_GRAYSCALE_TN_RCID:
      {
         // for every image selected, send a message to get the thumbnail
         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cameraThread->camera->sendMessage(
               Camera::GET_SMALL_THUMBNAIL,
               ((ImageObject*)cursor.current())->originalNumber );
         }
         result = true;
         break;
      }
      case MB_TRANSFER_RCID:
      {
         // for every image selected, send a message to get the image
         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cameraThread->camera->sendMessage(
               Camera::GET_IMAGE,
               ((ImageObject*)cursor.current())->originalNumber );
         }
         result = true;
         break;
      }
      case MB_SELECT_ALL_RCID:
      {
         // set the selected state for each object

         // to speed things up, we'll disable the container handler...
         ICnrHandler::stopHandlingEventsFor(&cnr);

         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::none );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cnr.setSelected( cursor.current(), true );
         }

         // ...and then re-enable the handler when we're done
         ICnrHandler::handleEventsFor( &cnr );

         // now start a new thread to calculate the selection status text
         IThread(new IThreadMemberFn<GUIWindow>(*this, &GUIWindow::getSelectionStatus));

         // since the container handler was disabled, we have to call this ourselves
         enableMenuBarItems();

         result = true;
         break;
      }
      case MB_SELECT_NONE_RCID:
      {
         // clear the selected state for each object

         // to speed things up, we'll disable the container handler...
         ICnrHandler::stopHandlingEventsFor(&cnr);

         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cnr.setSelected( cursor.current(), false );
         }

         // ...and then re-enable the handler when we're done
         ICnrHandler::handleEventsFor( &cnr );

         // now start a new thread to calculate the selection status text
         IThread(new IThreadMemberFn<GUIWindow>(*this, &GUIWindow::getSelectionStatus));

         // since the container handler was disabled, we have to call this ourselves
         enableMenuBarItems();

         result = true;
         break;
      }
      case MB_SELECT_TOGGLE_RCID:
      {
         // toggle the selected state for each object

         // to speed things up, we'll disable the container handler...
         ICnrHandler::stopHandlingEventsFor(&cnr);

         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::none );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cnr.setSelected( cursor.current(), !cnr.isSelected(cursor.current()) );
         }

         // ...and then re-enable the handler when we're done
         ICnrHandler::handleEventsFor( &cnr );

         // now start a new thread to calculate the selection status text
         IThread(new IThreadMemberFn<GUIWindow>(*this, &GUIWindow::getSelectionStatus));

         // since the container handler was disabled, we have to call this ourselves
         enableMenuBarItems();

         result = true;
         break;
      }
      case MB_DELETE_RCID:
      {
         // for every image selected, send a message to delete the image
         IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
         for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
         {
            cameraThread->camera->sendMessage(
               Camera::DELETE_IMAGE,
               ((ImageObject*)cursor.current())->originalNumber );
         }
         result = true;
         break;
      }
   }

   event.setResult( result );
   return result;
}


void GUIWindow::showAboutWindow()
{
   // show the "About" messagebox
   aboutMsgBox->  setTitle( IString(STR_TITLE)+IString(" - About") ).
                  show( IString( STR_TITLE         ) +
                        IString( ", v"             ) +
                        IString( STR_VERSION_MAJOR ) +
                        IString( "."               ) +
                        IString( STR_VERSION_MINOR ) +
                        IString( STR_VERSION_REV   ) +
                        IString( "\n"              ) +
                        IString( STR_DATE          ) +
                        IString( "\n\nWritten by " ) +
                        IString( STR_NAME          ) +
                        IString( " ("              ) +
                        IString( STR_EMAIL         ) +
                        IString( ")\n\n"           ) +
                        IString(isRegistered?"Registered copy.":"Unregistered copy." ),
                        IMessageBox::defButton1    |
                        IMessageBox::moveable      |
                        IMessageBox::informationIcon|
                        IMessageBox::okButton      );

   // user has discarded the "About" message box -- delete all references to it

   // prevent a race condition if the mbox is being deleted and the user
   // selects to display the message box again
   IMessageBox *mbox = aboutMsgBox;
   aboutMsgBox = (IMessageBox*)0;
   delete( mbox );

   // put the focus back on the main window
   setFocus();

   return;
}


void GUIWindow::logText( const char *text, const char *debugText )
{
   // is the debug window enabled?
   if( debugWindow )
   {
      // log the text in the debug window
      if( text )
      {
         debugWindow->logText( text );
      }
      if( debugText )
      {
         debugWindow->logText( debugText );
      }
   }

   // if there is text to display
   if( text )
   {
      // show the text in the status window
      efStatus.setText( text );
   }
   return;
}


void GUIWindow::createDebugWindow()
{
   if( debugWindow==0 )
   {
      // create a new debug window
      ICritSec criticalSection;
      menuBar.checkItem( MB_OUTPUT_RCID );
      debugWindow = new DebugWindow( *this );
      debugWindow->show();
   }
   else
   {
      // null out the pointer
      DebugWindow *wnd = debugWindow;
      if( wnd )   // this should ALWAYS be true; necessary for ICritSec
      {
         ICritSec criticalSection;
         debugWindow = (DebugWindow*)0;
      }

      IThread::current().sleep(10); // give threads time to exit

      // change the menu bar and tell the window to close itself
      menuBar.checkItem( MB_OUTPUT_RCID, false );
      wnd->close();

      // delete the window
      delete wnd;
   }
   return;
}


void GUIWindow::findAvailablePorts( void )
{
/*
   logText( "Looking for available COM ports." );
*/
   logText( 0, "serial port auto-detect disabled; tagging all ports as available" );

   // check all COM ports from COM1 to COM9
   for( int i=1; i<10; i++ )
   {
      /*
       * There were reports that "scanning" for available COM ports was causing
       * a problem with some video cards (ATI?) that uses a range of IO ports
       * which are often reserved for COM4.  Thus, to prevent this problem, the
       * following "detection" code was commented out.
       */

/*
      slideTo( i, 10 );
      IString port = IString("COM")+IString(i);
      logText( IString("Looking for ")+port );

      APIRET rc;
      HFILE fileHandle;
      ULONG action;
      rc = DosOpen(  port                    ,  // filename
                     &fileHandle             ,  // file handle
                     &action                 ,  // action that DosOpen carried out
                     0                       ,  // new size of file
                     FILE_NORMAL             ,  // new file attributes
                     FILE_OPEN               ,  // what to do with the file
                     OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                     OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                     OPEN_SHARE_DENYREADWRITE|  // deny others from accessing the port
                     OPEN_ACCESS_READWRITE   ,  // open for read & write
                     (PEAOP2)0               ); // extended attributes
      if( rc )
      {
         logText( 0, "...unavailable" );
         connectionSettings.comPortAvailability[i-1] = false;
      }
      else
      {
         logText( 0, "...port is available" );
*/
         connectionSettings.comPortAvailability[i-1] = true;
         // close the file handle
/*
         DosClose( fileHandle );
      }
*/
   }

   // to prevent some problems with this thread finishing
   // before the camera connection thread has had a chance
   // to finish, sleep for a short amount of time
   IThread::current().sleep(500);

/*
   slideTo( 0 );
*/
   logText( "", "application ready to run; enabling menu bar" );
   enableMenuBarItems();

   // if autoconnect is enabled, then start the command
   if( connectionSettings.autoConnect )
   {
      logText( 0, "sending auto-connect message" );
      menuBar.selectItem( MB_CONNECT_RCID );
   }

   return;
}


void GUIWindow::enableMenuBarItems( void )
{
   Boolean selectState = IContainerControl::ObjectCursor(cnr,IContainerObject::selected).setToFirst();
   Boolean objectState = IContainerControl::ObjectCursor(cnr,IContainerObject::none).setToFirst();

   menuBar. enableItem( MB_CONNECT_RCID,    !cameraThread->camera->connected ).
            enableItem( MB_DISCONNECT_RCID,  cameraThread->camera->connected ).
            enableItem( MB_TRANSFER_RCID,    selectState ).
            enableItem( MB_THUMBNAIL_RCID,   selectState && connectionSettings.cameraType!=Camera::KODAK_DC120 ).
            enableItem( MB_COLOUR_TN_RCID,   selectState && connectionSettings.cameraType!=Camera::KODAK_DC120 ).
            enableItem( MB_GRAYSCALE_TN_RCID,selectState && (connectionSettings.cameraType==Camera::KODAK_DC200 || connectionSettings.cameraType==Camera::KODAK_DC210 || connectionSettings.cameraType==Camera::KODAK_DC215) ).
            enableItem( MB_SELECT_RCID,      objectState ).
            enableItem( MB_SELECT_ALL_RCID,  objectState ).
            enableItem( MB_SELECT_NONE_RCID, objectState ).
            enableItem( MB_SELECT_TOGGLE_RCID,objectState ).
            enableItem( MB_DELETE_RCID,      selectState && connectionSettings.cameraType!=Camera::KODAK_DC120 && connectionSettings.cameraType!=Camera::KODAK_DC25 ).
            enableItem( MB_OPTIONS_RCID      ).
            enableItem( MB_SAVE_OPTIONS_RCID ).
            enableItem( MB_OUTPUT_RCID       ).
            enableItem( MB_ABOUT_RCID        );
   return;
}


void GUIWindow::disableMenuBarItems( void )
{
   menuBar. disableItem( MB_CONNECT_RCID     ).
            disableItem( MB_DISCONNECT_RCID  ).
            disableItem( MB_TRANSFER_RCID    ).
            disableItem( MB_OPTIONS_RCID     ).
            disableItem( MB_SAVE_OPTIONS_RCID).
            disableItem( MB_OUTPUT_RCID      ).
            disableItem( MB_ABOUT_RCID       ).
            disableItem( MB_THUMBNAIL_RCID   ).
            disableItem( MB_COLOUR_TN_RCID   ).
            disableItem( MB_GRAYSCALE_TN_RCID).
            disableItem( MB_SELECT_RCID      ).
            disableItem( MB_SELECT_ALL_RCID  ).
            disableItem( MB_SELECT_NONE_RCID ).
            disableItem( MB_SELECT_TOGGLE_RCID).
            disableItem( MB_DELETE_RCID      ).
            disableItem( MB_PURGE_RCID       );
   return;
}


Boolean GUIWindow::drawDetailsItem( ICnrDrawItemEvent &event )
{
   Boolean result = false;

   // are we drawing a column title or an object?
   if( event.object() )
   {
      // only handle items in the first column
      if( event.column() == &col1 )
      {
         // remember this rectangle -- used to refresh when cursored changes occur
         ((ImageObject*)event.object())->imageRect = event.itemRect();
         RECTL refreshRect = event.itemRect().asRECTL();

         LONG color = CLR_WHITE; // default to white for the background
         if( event.container()->isSelected(event.object()) )
         {
            // object is selected -- use dark gray border
            color = CLR_DARKGRAY;
         }
         if( event.container()->isCursored(event.object()) )
         {
            // object has cursor focus -- use black border
            color = CLR_BLACK;
         }
         // draw the background
         WinFillRect( event.itemPresSpaceHandle(), &refreshRect, color );

         BITMAPINFO bitmapTable;

         bitmapTable.cbFix = sizeof(BITMAPINFOHEADER);
         bitmapTable.cx = 96;
         bitmapTable.cy = 72;
         bitmapTable.cPlanes = 1;
         bitmapTable.cBitCount = 24;

         // get the image bits stored in the object
         char *bits = ((ImageObject*)event.object())->bits;

         ULONG xoffset = ((event.itemRect().width()-bitmapTable.cx)/2) - 1;
         ULONG yoffset = ((event.itemRect().height()-bitmapTable.cy)/2) - 1;
         POINTL points[4]={event.itemRect().minX() + xoffset,
                           event.itemRect().minY() + yoffset + 1,
                           event.itemRect().minX() + bitmapTable.cx + xoffset - 1,
                           event.itemRect().minY() + bitmapTable.cy + yoffset,
                           0,0,bitmapTable.cx,bitmapTable.cy};

         // draw the bitmap
         GpiDrawBits(   event.itemPresSpaceHandle(),  // presentation space
                        (PVOID)bits,                  // image bits
                        (PBITMAPINFO2)&bitmapTable,   // bitmap information table
                        (LONG)4,                      // point count
                        (PPOINTL)points,              // point array
                        (LONG)ROP_SRCCOPY,            // copy from source
                        (ULONG)BBO_IGNORE );          // options

         // set the event as having been handled
         result = true;
      }
   }

   event.setResult(result);
   return result;
}


Boolean GUIWindow::windowScrolled( ICnrScrollEvent &event )
{
   // force all objects to reset their rectangle since the window scrolled
   IContainerControl::ObjectCursor cursor( cnr, IContainerObject::none );
   for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
   {
      ((ImageObject*)cursor.current())->imageRect = IRectangle( 0, 0, 0, 0 );
   }
   event.setResult( false );
   return false;
}


void GUIWindow::slideTo( int offset, int total )
{
   if( total == 0 )
   {
      // force the slider back to the home position
      currentSliderTick = 0;
   }
   else
   {
      // sanity check
      if( offset >= total )
      {
         // the Digita cameras can sometimes send messages
         // such as "byte 400 of 300" since they pad extra zeros
         offset = total - 1;
      }

      // calculate a percentage (or rather, on a scale of 10)
      currentSliderTick = (offset*10)/total;
   }

   slider.moveArmToTick( currentSliderTick );

   return;
}


void GUIWindow::verifyRegistration()
{
   IProfile userProfile( IProfile::userProfile() );

   if( userProfile.containsApplication( "DC210_SC" ) )
   {
      logText( 0, "found registration information" );
      userProfile.setDefaultApplicationName(    "DC210_SC"  );
      IString reg  = userProfile.elementWithKey("RegNumber" );
      IString name = userProfile.elementWithKey("RegName"   );
      IString key  = IString(STR_VERSION_MAJOR) +
                     IString(STR_REG_KEY) +
                     IString(STR_VERSION_MAJOR);

      // build up the decoded name and sequence number
      IString decodedName( "" );
      for( int i=0; i<16; i++ )
      {
         decodedName += (reg.subString(i*2+1,2).x2c() ^ key[i+1]);
      }
      IString decodedNumber( "" );
      for( i=16; i<20; i++ )
      {
         decodedNumber += (reg.subString(i*2+1,2).x2c() ^ key[i+1]);
      }

      // check if both names are equal (why can I not use '=='?)
      if( decodedName.subString(1,16).strip().indexOf(IString(name).space(0).subString(1,16).strip()) == 1 )
      {
         // the registration is valid!
         logText( 0, "registration validation successful" );
         isRegistered = true;
         registeredName = name;
         registeredKey = reg;
      }
      else
      {
         logText( 0, "registration validation failed" );
         isRegistered = false;
         registeredName = "";
         registeredKey = "";
      }
   }

   if( ! isRegistered )
   {
/*
      connectionSettings.baudRate = Camera::BAUD_9600;
*/
      /* any "unregistered" copies are now considered "open source"... */
      isRegistered   = true;
      registeredName = "Stephane Charette (open source version)";
      registeredKey  = "62760F62727033F221CC72BC18F733A906BA2B03";
   }

   return;
}


Boolean GUIWindow::makePopUpMenu( IMenuEvent &event )
{
   Boolean result = false;

   // create an empty popup menu
   IPopUpMenu *menu = new IPopUpMenu( &cnr, POPUP_MENU_RCID );
   // set the menu to automatically delete itself when no longer needed
   menu->setAutoDeleteObject( true );

   // are we connected?
   if( cameraThread->camera->connected )
   {
      // is this popup menu for an object or for the container?
      IContainerObject *obj = popupMenuObject();
      Boolean selectState = ( obj ? cnr.isSelected(obj) : false );

      if( obj && selectState )
      {
         // object-specific popup menu
         menu->addText(MB_TRANSFER_RCID,"~Transfer Image\tF4").
               addText(MB_COLOUR_TN_RCID,"~Colour Thumbnail\tF5").
               addText(MB_GRAYSCALE_TN_RCID,"~Grayscale Thumbnail\tF6").
               enableItem(MB_COLOUR_TN_RCID, connectionSettings.cameraType!=Camera::KODAK_DC120 ).
               enableItem(MB_GRAYSCALE_TN_RCID, connectionSettings.cameraType==Camera::KODAK_DC200 || connectionSettings.cameraType==Camera::KODAK_DC210 || connectionSettings.cameraType==Camera::KODAK_DC215 );
      }
      else
      {
         menu->addText(MB_SELECT_ALL_RCID,"Select ~All\tAlt+A").
               addText(MB_SELECT_NONE_RCID,"Select ~None\tAlt+N").
               addText(MB_SELECT_TOGGLE_RCID,"Select ~Toggle\tAlt+T");
      }

      // specify that we've handled this event
      result = true;
   }
   else
   {
      // we're not yet connected
      menu->addText(MB_CONNECT_RCID,"~Connect\tF2").
            addSeparator().
            addText(MB_OPTIONS_RCID,"~Options\tAlt+O").
            addText(MB_SAVE_OPTIONS_RCID,"~Save Options\tAlt+S");
      // specify that we've handled this event
      result = true;
   }

   // show the menu
   menu->show( event.mousePosition() );

   event.setResult( result );
   return result;
}


Boolean GUIWindow::selectedChanged( ICnrEmphasisEvent &event )
{
   Boolean result = false;

   enableMenuBarItems();

   getSelectionStatus();

   event.setResult( result );
   return result;
}


void GUIWindow::runScript( const char *scriptName, const Boolean foreground )
{
   IString script( IString(scriptName).word(1) );
   IString parms( IString(scriptName).removeWords(1,1) );
   //IString path( connectionSettings.dir );

   // get rid of the trailing slash on the path
   //path.remove(path.lastIndexOfAnyOf("\\/"));

   logText( 0, "==> starting " + IString(foreground?"foreground":"background") + " script \"" + script + "\" with \"" + parms + "\"" );

   PROGDETAILS details = {0};
   details.Length = sizeof(PROGDETAILS);
   details.progt.progc = PROG_DEFAULT;
   details.progt.fbVisible = SHE_VISIBLE;
   details.pszExecutable = script;
   details.pszParameters = parms;
   // for some reason, trying to set the path was causing serious hang problems
   // on my system -- for this reason, the current working drive & directory is
   // changed on startup in the constructor, and changed back in the destructor
   //details.pszStartupDir = path;

   if(!foreground)
   {
      // start the application minimized
      details.swpInitial.fl = SWP_DEACTIVATE |
                              SWP_HIDE       |
                              SWP_MINIMIZE   |
                              SWP_MOVE       |
                              SWP_SIZE       |
                              SWP_ZORDER     ;
      details.swpInitial.hwndInsertBehind = this->handle();
   }

   if( WinStartApp(NULLHANDLE, &details, NULL, NULL, SAF_INSTALLEDCMDLINE) == NULL )
   {
      ERRORID errorId = WinGetLastError(IThread::current().anchorBlock());
      IString errorTxt(errorId);
      logText( 0, "==> cannot start script; error id: " + errorTxt + " (0x" + errorTxt.d2x() + ")" );
   }

   return;
}


void GUIWindow::getSelectionStatus(void)
{
   // this normally is called on a 2nd thread, to prevent us from locking
   // up the GUI thread when many images are selected in the container
   int selectedImages = 0;
   IContainerControl::ObjectCursor cursor( cnr, IContainerObject::selected );
   for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
   {
      selectedImages ++;
   }

   efSelected.setText(IString(selectedImages) + "/" + IString(cnr.objectCount()));

   return;
}


Boolean GUIWindow::beginEdit( ICnrBeginEditEvent &event )
{
   disableMenuBarItems();
   return ICnrEditHandler::beginEdit(event);
}


Boolean GUIWindow::endEdit( ICnrEndEditEvent &event )
{
   enableMenuBarItems();
   return ICnrEditHandler::endEdit(event);
}


Boolean CnrMleEditHandler::dispatchHandlerEvent(IEvent &event)
{
   Boolean result = false; // default is:  this event has not been handled

   // if this is a keypress...
   if(event.eventId() == WM_CHAR)
   {
      // extract the WM_CHAR fields from the event

      USHORT fsflags = event.parameter1().lowNumber();
      //UCHAR ucrepeat = event.parameter1().char3();
      //UCHAR ucscancode = event.parameter1().char4();
      USHORT usch = event.parameter2().lowNumber();
      //USHORT usvk = event.parameter2().highNumber();

      // is this the ENTER key?
      if( (fsflags & KC_CHAR) && (usch == 13) )
      {
         // terminate this edit session -- the name is done!
         wnd->cnr.closeEdit();
         result = true; // prevent the key from reaching the window
      }
   }

   return result;
}

