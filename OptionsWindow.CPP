/*
 * $Id: //devel/DCITU/OptionsWindow.CPP#8 $
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

#define INCL_WINWINDOWMGR  // WinCreateWindow(), WC_NOTEBOOK, WS_VISIBLE, HWND_TOP
#define INCL_DOSFILEMGR    // DosQueryPath
#define INCL_DOSMISC       // DosError, DosQuerySysInfo

#include "ConnectionSettings.HPP"
#include "OptionsWindow.HPP"
#include "OptionsCanvas.H"
#include "GUIWindow.HPP"
#include "GUIWindow.H"
#include "Camera.HPP"

#include <os2.h>


/*****************************************************************************
 *
 *    OptionsWindow::OptionsWindow
 *
 * constructor to create and initialize the options window
 *
 *****************************************************************************/
OptionsWindow::OptionsWindow( GUIWindow &ownerWindow ) :
   IFrameWindow(  IResourceId( WND_OPTIONS )             ,
                  IWindow::desktopWindow()               ,
                  &ownerWindow                           ,
                  IRectangle()                           ,
                 (IFrameWindow::classDefaultStyle        |
                  IFrameWindow::noMoveWithOwner          |
                  IFrameWindow::shellPosition            |
                  IFrameWindow::dialogBackground)        &
                ~(IFrameWindow::windowList               |
                  IFrameWindow::maximizeButton)          ,
                  IString(STR_TITLE)+IString(" - Options") ),

   flyText( 0, this ),
   flyOverHelpHandler( &flyText, 100, 500 ),

   wnd(           ownerWindow ),

   cnvMain(       CNV_MAIN, this, this ),

   // create the notebook control
   notebook( WinCreateWindow( cnvMain.handle().asUnsigned(),// parent
                              WC_NOTEBOOK,                  // registered class name
                              "",                           // window text
                              0x0808 | WS_VISIBLE,          // window style
                              0,                            // x coordinate
                              0,                            // y coordinate
                              0,                            // width
                              0,                            // height
                              cnvMain.handle().asUnsigned(),// owner
                              HWND_TOP,                     // sibling window
                              0,                            // window ID
                              NULL,                         // control data
                              NULL ) ),                     // presentation parameter

   // create the notebook page window
   wndConnection( CNV_OPTIONS_CONNECT,    &notebook, &notebook, IFrameWindow::dialogResource ),
   wndCamera(     CNV_OPTIONS_CAMERAS,    &notebook, &notebook, IFrameWindow::dialogResource ),
   wndDirectory(  CNV_OPTIONS_TRANSFER,   &notebook, &notebook, IFrameWindow::dialogResource ),
   wndPreference( CNV_OPTIONS_PREFERENCES,&notebook, &notebook, IFrameWindow::dialogResource ),
   wndScripts(    CNV_OPTIONS_SCRIPT,     &notebook, &notebook, IFrameWindow::dialogResource ),
   wndRegister(   CNV_OPTIONS_REGISTER,   &notebook, &notebook, IFrameWindow::dialogResource ),
   wndSerial(     CNV_OPTIONS_SERIAL,     &notebook, &notebook, IFrameWindow::dialogResource ),

   // controls used on the "Connection" notebook page
   cbCameraType(              CB_CAMERA_TYPE,            &wndConnection ),
   ckAdvancedCOMPort(         CK_ADVANCED_COM_PORT,      &wndConnection ),
   sbCOMPort(                 SB_COM_PORT,               &wndConnection ),
   efCOMPort(                 EF_COM_PORT,               &wndConnection ),
   sbBaudRate(                SB_BAUD_RATE,              &wndConnection ),
   cbExtBuffers(              CB_EXT_BUFFERS,            &wndConnection ),
   sbTimeout(                 SB_TIMEOUT,                &wndConnection ),
   sbPause(                   SB_PAUSE_BETWEEN_COMMANDS, &wndConnection ),
   sbPauseAfterBaudChange(    SB_BAUD_RATE_PAUSE,        &wndConnection ),
   sbRetryPartialTransfers(   SB_RETRY_PARTIALS,         &wndConnection ),

   // controls used on the "Camera" notebook page
   ckIgnoreMemoryCard(        CK_IGNORE_MEMORY_CARD,     &wndCamera  ),
   ckConvertKDCToJPG(         CK_CONVERT_KDC_TO_JPG,     &wndCamera  ),
   ckRawPath(                 CK_USE_RAW_PATH,           &wndCamera  ),
   ckRememberNumbering(       CK_REMEMBER_NUMBERING,     &wndCamera  ),
   txtNumbering(              TXT_LAST_IMAGE_NUMBER,     &wndCamera  ),
   sbNumbering(               SB_LAST_IMAGE_NUMBER,      &wndCamera  ),
   ckAlternateStartup(        CK_ALTERNATE_STARTUP,      &wndCamera  ),
   ckKeepThumbnails(          CK_KEEP_THUMBNAILS,        &wndCamera  ),
   ckIgnoreBreak(             CK_IGNORE_BREAK,           &wndCamera  ),

   // controls used on the "Serial Line" notebook page
   sbDTR(                     SB_DTR,                    &wndSerial  ),
   sbRTS(                     SB_RTS,                    &wndSerial  ),
   ckCTS(                     CK_CTS,                    &wndSerial  ),
   ckDSR(                     CK_DSR,                    &wndSerial  ),
   ckDCD(                     CK_DCD,                    &wndSerial  ),
   ckInputDSR(                CK_INPUT_DSR,              &wndSerial  ),
   ckTransmitFlowControl(     CK_TRANSMIT_FLOW_CONTROL,  &wndSerial  ),
   ckReceiveFlowControl(      CK_RECEIVE_FLOW_CONTROL,   &wndSerial  ),
   pbResetSerial(             PB_RESET_SERIAL,           &wndSerial  ),

   // controls used on the "Transfer" notebook page
   efDir(                     EF_TRANSFER_DIR,           &wndDirectory  ),
   txtDir(                    TXT_TRANSFER_DIR,          &wndDirectory  ),
   txtRelativePath(           TXT_TRANSFER_DIR_RELATIVE, &wndDirectory  ),
   ckDeleteFailedTransfer(    CK_DELETE_FAILED_TRANSFERS,&wndDirectory  ),

   // controls used on the "Preferences" notebook page
   ckHighestPriority(         CK_USE_HIGHEST_PRIORITY,   &wndPreference ),
   ckConnectOnStartup(        CK_CONNECT_ON_STARTUP,     &wndPreference ),
   ckAutoDownloadThumbnails(  CK_AUTO_DOWNLOAD_THUMBNAILS,&wndPreference),
   ck4bitThumbnails(          CK_4_BIT_THUMBNAIL,        &wndPreference ),
   ck24bitThumbnails(         CK_24_BIT_THUMBNAIL,       &wndPreference ),
   ckAutoDownloadImages(      CK_AUTO_DOWNLOAD_IMAGES,   &wndPreference ),
   ckOverwriteImages(         CK_OVERWRITE_IMAGES,       &wndPreference ),
   ckDeleteImages(            CK_DELETE_IMAGES,          &wndPreference ),

   // controls used on the "Scripts" notebook page
   ckScriptAfterConnecting(   CK_SCRIPT_AFTER_CONNECTION,      &wndScripts ),
   ckScriptAfterConnectingFg( CK_SCRIPT_AFTER_CONNECTION_FG,   &wndScripts ),
   efScriptAfterConnecting(   EF_SCRIPT_AFTER_CONNECTION,      &wndScripts ),
   ckScriptAfterTransfer(     CK_SCRIPT_AFTER_TRANSFER,        &wndScripts ),
   ckScriptAfterTransferFg(   CK_SCRIPT_AFTER_TRANSFER_FG,     &wndScripts ),
   efScriptAfterTransfer(     EF_SCRIPT_AFTER_TRANSFER,        &wndScripts ),
   ckScriptAfterDelete(       CK_SCRIPT_AFTER_DELETE,          &wndScripts ),
   ckScriptAfterDeleteFg(     CK_SCRIPT_AFTER_DELETE_FG,       &wndScripts ),
   efScriptAfterDelete(       EF_SCRIPT_AFTER_DELETE,          &wndScripts ),
   ckScriptNoMoreImages(      CK_SCRIPT_NO_MORE_IMAGES,        &wndScripts ),
   ckScriptNoMoreImagesFg(    CK_SCRIPT_NO_MORE_IMAGES_FG,     &wndScripts ),
   efScriptNoMoreImages(      EF_SCRIPT_NO_MORE_IMAGES,        &wndScripts ),
   ckScriptDisconnect(        CK_SCRIPT_AFTER_DISCONNECTING,   &wndScripts ),
   ckScriptDisconnectFg(      CK_SCRIPT_AFTER_DISCONNECTING_FG,&wndScripts ),
   efScriptDisconnect(        EF_SCRIPT_AFTER_DISCONNECTING,   &wndScripts ),

   // controls used on the "Register" notebook page
   efRegisterName(            EF_REGISTER_NAME,          &wndRegister   ),
   efRegisterKey(             EF_REGISTER_KEY,           &wndRegister   ),
   pbRegister(                PB_REGISTER,               &wndRegister   ),

   // create the pushbutton canvas at the bottom of the notebook
   cnvPB(         CNV_OPTIONS_BUTTONS, &cnvMain, &cnvMain, IRectangle(),
                 (ISetCanvas::classDefaultStyle          |
                  ISetCanvas::packExpanded)              &
                ~(ISetCanvas::packTight)                 ),

   pbOk(          PB_OK, &cnvPB, &cnvPB, IRectangle()    ,
                  IPushButton::classDefaultStyle         |
                  IPushButton::defaultButton             |
                  IPushButton::tabStop                   |
                  IPushButton::group                     ),

   pbCancel(      PB_CANCEL, &cnvPB, &cnvPB, IRectangle(),
                  IPushButton::classDefaultStyle         |
                  IPushButton::tabStop                   ),

   pbHelp(        PB_HELP, &cnvPB, &cnvPB, IRectangle()  ,
                  IPushButton::classDefaultStyle         |
                  IPushButton::help                      |
                  IPushButton::noPointerFocus            ),

   helpWindow(    IResourceId(HELPTABLE_RCID), this ),

   connectionSettings( wnd.connectionSettings )
{
   initializationDone = false;

   // setup handlers; this *must* be done on the main thread to work correctly
   IEditHandler::handleEventsFor(      &efDir                     );
   IFocusHandler::handleEventsFor(     &efDir                     );
   ISelectHandler::handleEventsFor(    &cbCameraType              );
   ISelectHandler::handleEventsFor(    &ckAdvancedCOMPort         );
   ISelectHandler::handleEventsFor(    &ckRememberNumbering       );
   ISelectHandler::handleEventsFor(    &ckAutoDownloadThumbnails  );
   ISelectHandler::handleEventsFor(    &ck4bitThumbnails          );
   ISelectHandler::handleEventsFor(    &ck24bitThumbnails         );
   ISelectHandler::handleEventsFor(    &ckScriptAfterConnecting   );
   ISelectHandler::handleEventsFor(    &ckScriptAfterTransfer     );
   ISelectHandler::handleEventsFor(    &ckScriptAfterDelete       );
   ISelectHandler::handleEventsFor(    &ckScriptNoMoreImages      );
   ISelectHandler::handleEventsFor(    &ckScriptDisconnect        );
   IFocusHandler::handleEventsFor(     &efScriptAfterConnecting   );
   IFocusHandler::handleEventsFor(     &efScriptAfterTransfer     );
   IFocusHandler::handleEventsFor(     &efScriptAfterDelete       );
   IFocusHandler::handleEventsFor(     &efScriptNoMoreImages      );
   IFocusHandler::handleEventsFor(     &efScriptDisconnect        );
   ICommandHandler::handleEventsFor(   this                       );
   ICommandHandler::handleEventsFor(   &wndRegister               );
   ICommandHandler::handleEventsFor(   &wndSerial                 );
   flyOverHelpHandler.handleEventsFor( &cnvMain                   );

   // had some problems calling overloaded inherited methods since the
   // constructions wasn't complete, so all initialization is now done
   // on a second thread; note that a pause had to be inserted to help
   // prevent race conditions between the end of the constructor and the
   // start of the initialization!
   IThread(new IThreadMemberFn<OptionsWindow>(*this, &OptionsWindow::initialize));

   return;
}


void OptionsWindow::initialize(void)
{
   // note that a pause had to be inserted to help prevent race conditions
   // between the end of the constructor and the start of the initialization!

   IThread::current().sleep(10); // I *hate* stuff like this...  :(

   // make certain the window is deleted when closed
   setAutoDeleteObject( true );
   setAutoDestroyWindow( true );

   // set the same icon as the main window
   setIcon( WND_RCID );

   // set the file to use when displaying context-sensitive help
   Boolean helpWasLoaded = true;
   try
   {
      helpWindow.addLibraries( "dcitu.hlp" ).setTitle(STR_TITLE);
   }
   catch( IInvalidRequest &exception )
   {
      helpWasLoaded = false;
   }

   if(!helpWasLoaded)
   {
      // maybe the /I= option is being used -- look in the same path as the .EXE
      IString helpFile( IApplication::current().argv(0).subString(1, IApplication::current().argv(0).lastIndexOfAnyOf("/\\")) + "dcitu.hlp" );
      helpWasLoaded = true;
      try
      {
         helpWindow.addLibraries(helpFile).setTitle(STR_TITLE);
      }
      catch( IInvalidRequest &exception )
      {
         helpWasLoaded = false;
      }
   }

   if(!helpWasLoaded)
   {
      // ok -- we really don't know where the help file is hiding
      IMessageBox msgBox( this );
      msgBox.setTitle( "" ).
         show( "The help file DCITU.HLP could not be opened!  Help will be disabled.",
         IMessageBox::defButton1       |
         IMessageBox::moveable         |
         IMessageBox::warningIcon      |
         IMessageBox::systemModal      |
         IMessageBox::okButton         );
      pbHelp.disable();
   }

   // setup the multicell canvas
   cnvMain. setRowHeight( 1, 5, false ).
            setRowHeight( 2, 0, true ).
            setRowHeight( 3, 5, false ).
            setRowHeight( 4, 0, false ).
            setRowHeight( 5, 5, false ).
            setColumnWidth( 1, 5, false ).
            setColumnWidth( 2, 0, true ).
            setColumnWidth( 3, 5, false ).
            addToCell( &notebook, 2, 2 ).
            addToCell( &cnvPB,      2, 4 ).
            setBackgroundColor( IColor(204,204,204) );

   /* setup all of the controls used on the "Connection" page:
    *
    * - camera type
    * - COM port
    * - baud rate
    * - extended buffer detection
    * - timeout detection
    * - pause between commands
    * - retry partial transfers
    */

   // setup the camera type
   cbCameraType.add( 1, Camera::cameraNames, Camera::NUMBER_OF_CAMERAS );
   // do not select the camera yet -- wait until everyting else has been configured
   // correctly (this ensures that when the ISelectHandler is called, we'll have
   // access to all that we need!)

   // setup the COM port
   ckAdvancedCOMPort.select( connectionSettings.advancedComPort );

   for(int i=0; i<9; i++ )
   {
      if( connectionSettings.comPortAvailability[i] )
      {
         // this COM port exists...add it to the list
         sbCOMPort.addAsLast( IString("COM")+IString(i+1) );
      }
   }
   sbCOMPort.spinTo( connectionSettings.comPort - 1 );
   efCOMPort.setText( connectionSettings.advancedComPortName );
   // show/hide the COM port EF/SB depending on the checkbox selection
   sbCOMPort.show(!ckAdvancedCOMPort.isSelected() );
   efCOMPort.show( ckAdvancedCOMPort.isSelected() );

   // setup the baud rate
   for( i=1; i<Camera::NUMBER_OF_BAUD_RATES; i++ )
   {
      sbBaudRate.addAsLast( Camera::baudRateNames[i] );
   }
   sbBaudRate.spinTo( IString(connectionSettings.baudRate) );

   // setup the extended buffer detection
   cbExtBuffers.addAsLast( "auto-detect"     );
   cbExtBuffers.addAsLast( "enable buffers"  );
   cbExtBuffers.addAsLast( "disable buffers" );
   cbExtBuffers.select( connectionSettings.extendedBuffer );

   // setup the timeout detection
   for( i=1; i<=20; i++ )
   {
      sbTimeout.addAsLast( IString(i) + " second" + (i==1?"":"s") );
   }
   sbTimeout.spinTo( connectionSettings.timeout-1 );

   // setup the pause between commands
   for( i=0; i<=2000; i+=50 )
   {
      sbPause.addAsLast( IString(i) + " msecs" );
   }
   sbPause.spinTo( IString(connectionSettings.pauseBetweenCommands) + " msecs" );

   // setup the "retry partial transfer" spinbutton
   sbRetryPartialTransfers.addAsLast( "disabled");
   sbRetryPartialTransfers.addAsLast( "enabled" );
   sbRetryPartialTransfers.spinTo( connectionSettings.retryPartialTransfers );

   // setup the pause after a change of baud rate
   for( i=0; i<=1000; i+=100 )
   {
      sbPauseAfterBaudChange.addAsLast( IString(i) + " msecs" );
   }
   sbPauseAfterBaudChange.spinTo( IString(connectionSettings.pauseAfterBaudChange) + " msecs" );

   /* setup all of the controls used on the "Camera" page:
    * - Kodak: ignore memory card
    * - Kodak: convert KDC to JPG
    * - Kodak: raw path
    * - Project O: remember numbering
    * - Project O: resume numbering at
    * - Project O: use alternate startup
    * - Project O: keep thumbnails
    * - All: ignore BREAK
    */
   ckIgnoreMemoryCard.  select( connectionSettings.ignoreMemoryCard );
   ckConvertKDCToJPG.   select( connectionSettings.convertKDCtoJPG );
   ckRawPath.           select( connectionSettings.useRawPath );
   ckRememberNumbering. select( connectionSettings.rememberNumbering );
   sbNumbering.         setRange(IRange(0, 999999)).
                        spinTo( connectionSettings.resumeNumberingAt ).
                        setLimit(6);
   ckAlternateStartup.  select( connectionSettings.useAlternateStartup );
   ckKeepThumbnails.    select( connectionSettings.keepThumbnails );
   ckIgnoreBreak.       select( connectionSettings.ignoreBreak );

   /* setup all of the controls used on the "Serial Line" page:
    * - DTR control
    * - RTS control
    * - CTS
    * - DSR
    * - DCD
    * - DSR sensitivity
    * - transmit flow control
    * - receive flow control
    */
   sbDTR.addAsLast( "disable"    );
   sbDTR.addAsLast( "enable"     );
   sbDTR.addAsLast( "handshake"  );
   sbDTR.spinTo( connectionSettings.controlModeDTR );

   sbRTS.addAsLast( "disable"    );
   sbRTS.addAsLast( "enable"     );
   sbRTS.addAsLast( "handshake"  );
   sbRTS.addAsLast( "toggle"     );
   sbRTS.spinTo( connectionSettings.controlModeRTS );

   ckCTS.               select( connectionSettings.enableCTS );
   ckDSR.               select( connectionSettings.enableDSR );
   ckDCD.               select( connectionSettings.enableDCD );
   ckInputDSR.          select( connectionSettings.enableInputDSR );
   ckTransmitFlowControl.select(connectionSettings.enableTransmitFlowControl );
   ckReceiveFlowControl.select( connectionSettings.enableReceiveFlowControl );

   /* setup all of the controls used on the "Transfer" page:
    * - directory
    * - delete failed transfers
    * - invalid path warning string
    */
   efDir.                  setLimit(80).setText( connectionSettings.dir );
   ckDeleteFailedTransfer. select( connectionSettings.deleteFailedTransfers );
   validatePath();

   /* setup all of the controls used on the "Preferences" page:
    * - connect on startup
    * - auto download thumbnails
    * - 4-bit thumbnails
    * - 24-bit thumbnails
    * - auto download images
    * - overwrite images
    * - delete images
    */
   ckHighestPriority.         select( connectionSettings.useHighestPriority );
   ckConnectOnStartup.        select( connectionSettings.autoConnect );
   ckAutoDownloadThumbnails.  select( connectionSettings.autoDownloadThumbnails );
   ck4bitThumbnails.          select( connectionSettings.autoDownload4bitThumbnails );
   ck24bitThumbnails.         select( connectionSettings.autoDownload24bitThumbnails );
   ckAutoDownloadImages.      select( connectionSettings.autoTransferImages );
   ckOverwriteImages.         select( connectionSettings.overwriteDuplicates );
   ckDeleteImages.            select( connectionSettings.deleteAfterTransfer );

   /* setup all of the controls used on the "Scripts" page:
    * - after connecting
    * - after transfer
    * - after delete
    * - no more images
    * - disconnect
    */
   if( !connectionSettings.scriptAfterConnecting ) ckScriptAfterConnecting.select();
   ckScriptAfterConnecting.   click();
   ckScriptAfterConnectingFg. select(connectionSettings.scriptAfterConnectingFg);
   efScriptAfterConnecting.   setLimit(80).
                              setText(connectionSettings.scriptAfterConnectingName);

   if( !connectionSettings.scriptAfterTransfer ) ckScriptAfterTransfer.select();
   ckScriptAfterTransfer.     click();
   ckScriptAfterTransferFg.   select(connectionSettings.scriptAfterTransferFg);
   efScriptAfterTransfer.     setLimit(80).
                              setText(connectionSettings.scriptAfterTransferName);

   if( !connectionSettings.scriptAfterDelete ) ckScriptAfterDelete.select();
   ckScriptAfterDelete.       click();
   ckScriptAfterDeleteFg.     select(connectionSettings.scriptAfterDeleteFg);
   efScriptAfterDelete.       setLimit(80).
                              setText(connectionSettings.scriptAfterDeleteName);

   if( !connectionSettings.scriptNoMoreImages ) ckScriptNoMoreImages.select();
   ckScriptNoMoreImages.      click();
   ckScriptNoMoreImagesFg.    select(connectionSettings.scriptNoMoreImagesFg);
   efScriptNoMoreImages.      setLimit(80).
                              setText(connectionSettings.scriptNoMoreImagesName);

   if( !connectionSettings.scriptDisconnect ) ckScriptDisconnect.select();
   ckScriptDisconnect.        click();
   ckScriptDisconnectFg.      select(connectionSettings.scriptDisconnectFg);
   efScriptDisconnect.        setLimit(80).
                              setText(connectionSettings.scriptDisconnectName);

   /* setup all of the controls used on the "Register" page:
    * - name
    * - key
    * - pushbutton
    */
   analyzeRegistration();

   /* setup the notebook
    */
   notebook.setMajorTabBackgroundColor(   backgroundColor() ).
            setPageBackgroundColor(       backgroundColor() ).
            setOrientation(               INotebook::backpagesRightTabsTop ).
            enableTabStop();

   // get the OS/2 version number
   ULONG versionBuffer[2];
   DosQuerySysInfo( QSV_VERSION_MAJOR, QSV_VERSION_MINOR, versionBuffer, sizeof(versionBuffer) );
   // are we using Warp 3 or earlier?
   if( (versionBuffer[0] <= 20) && (versionBuffer[1] < 40) )
   {
      // set the Warp v3 settings -- these are auto-handled in Warp v4 and higher
      notebook.setBinding( INotebook::spiral ).
               setMajorTabSize( ISize(128,32) ).
               setTabShape( INotebook::rounded );
   }


   // create the notebook settings object
   INotebook::PageSettings settings(   INotebook::PageSettings::autoPageSize |
                                       INotebook::PageSettings::majorTab );

   settings.setTabText( "Connection" );
   notebook.addLastPage( settings, &wndConnection );
   settings.setTabText( "Camera" );
   notebook.addLastPage( settings, &wndCamera );
   settings.setTabText( "Serial Line" );
   notebook.addLastPage( settings, &wndSerial );
   settings.setTabText( "Transfer" );
   notebook.addLastPage( settings, &wndDirectory );
   settings.setTabText( "Preferences" );
   notebook.addLastPage( settings, &wndPreference );
   settings.setTabText( "Scripts" );
   notebook.addLastPage( settings, &wndScripts );
   settings.setTabText( "Register" );
   notebook.addLastPage( settings, &wndRegister );

   /* setup the pushbutton controls
    */
   pbOk.       setText( "~OK" );
   pbCancel.   setText( "~Cancel" );
   pbHelp.     setText( "~Help" );
   cnvPB.      setDeckCount(1).
               enableTabStop().
               enableGroup();

   // set the main canvas as the frame's client
   setClient( &cnvMain );

   // is this window in the .INI file?
   if( wnd.profile.containsApplication("Options Window") )
   {
      // move & size the window to what was saved in the .INI file
      wnd.profile.setDefaultApplicationName( "Options Window" );
      moveSizeTo( IRectangle( wnd.profile.integerWithKey( "x1" ),
                              wnd.profile.integerWithKey( "y1" ),
                              wnd.profile.integerWithKey( "x2" ),
                              wnd.profile.integerWithKey( "y2" ) ) );
   }
   else
   {
      // re-size to a more reasonable default size
      sizeTo( cnvMain.minimumSize() );
   }

   // now select the correct camera type!
   cbCameraType.select( connectionSettings.cameraType );

   // put the focus on OK pushbutton
   show();
   notebook.setFocus();

   initializationDone = true;

   return;
}


/*****************************************************************************
 *
 *    OptionsWindow::~OptionsWindow
 *
 * destructor for options window
 *
 *****************************************************************************/
OptionsWindow::~OptionsWindow()
{
   // remove event handlers
   flyOverHelpHandler.stopHandlingEventsFor( &cnvMain );
   ICommandHandler::stopHandlingEventsFor( &wndSerial );
   ICommandHandler::stopHandlingEventsFor( &wndRegister );
   ICommandHandler::stopHandlingEventsFor( this );
   ISelectHandler::stopHandlingEventsFor( &ckScriptAfterConnecting );
   ISelectHandler::stopHandlingEventsFor( &ckScriptAfterTransfer );
   ISelectHandler::stopHandlingEventsFor( &ckScriptAfterDelete );
   ISelectHandler::stopHandlingEventsFor( &ckScriptNoMoreImages );
   ISelectHandler::stopHandlingEventsFor( &ckScriptDisconnect );
   IFocusHandler::stopHandlingEventsFor( &efScriptAfterConnecting );
   IFocusHandler::stopHandlingEventsFor( &efScriptAfterTransfer );
   IFocusHandler::stopHandlingEventsFor( &efScriptAfterDelete );
   IFocusHandler::stopHandlingEventsFor( &efScriptNoMoreImages );
   IFocusHandler::stopHandlingEventsFor( &efScriptDisconnect );
   ISelectHandler::stopHandlingEventsFor( &cbCameraType );
   ISelectHandler::stopHandlingEventsFor( &ckAdvancedCOMPort );
   ISelectHandler::stopHandlingEventsFor( &ckRememberNumbering );
   ISelectHandler::stopHandlingEventsFor( &ckAutoDownloadThumbnails );
   ISelectHandler::stopHandlingEventsFor( &ck4bitThumbnails );
   ISelectHandler::stopHandlingEventsFor( &ck24bitThumbnails );
   IFocusHandler::stopHandlingEventsFor( &efDir );
   IEditHandler::stopHandlingEventsFor( &efDir );
   return;
}



/*****************************************************************************
 *
 *    OptionsWindow::enter
 *
 * handle the selection of a new camera type
 *
 *****************************************************************************/
Boolean OptionsWindow::enter( IControlEvent &event )
{
   // the user has chosen a new camera type, so handle
   // all of the enable/disable of camera-specific controls
   IString camera( cbCameraType.itemText(cbCameraType.selection()) );

   if( initializationDone )
   {
      // since a new camera was selected, change the line controls
      resetDefaultSerialLine();
   }

   // start by assuming that all of the optional controls are available
   ckAutoDownloadThumbnails.  enable();
   ck4bitThumbnails.          enable();
   ck24bitThumbnails.         enable();
   ckDeleteImages.            enable();

   /**************************************************************************
    * Kodak DC120 or DC50 or DC25
    **************************************************************************/
   if( camera == Camera::cameraNames[Camera::KODAK_DC120] ||
     //camera == Camera::cameraNames[Camera::KODAK_DC50] ||
       camera == Camera::cameraNames[Camera::KODAK_DC25] )
   {
      ckIgnoreMemoryCard.enable();
      ckConvertKDCToJPG.enable(camera!=Camera::cameraNames[Camera::KODAK_DC25]);
      ckDeleteImages.disable();
      ck4bitThumbnails.disable();
      if( camera == Camera::cameraNames[Camera::KODAK_DC120] )
      {
         ckAutoDownloadThumbnails.disable();
         ck24bitThumbnails.disable();
      }
      else
      {
         ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
      }
   }
   else
   {
      ckIgnoreMemoryCard.disable();
      ckConvertKDCToJPG.disable();
   }

   /**************************************************************************
    * Kodak DC200 or DC210 or DC215
    **************************************************************************/
   if( camera == Camera::cameraNames[Camera::KODAK_DC200] ||
       camera == Camera::cameraNames[Camera::KODAK_DC210] ||
       camera == Camera::cameraNames[Camera::KODAK_DC215] )
   {
      // enable the thumbnails only if auto-download is selected
      ck4bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
      ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
   }

   /**************************************************************************
    * Kodak DC240 or DC280
    **************************************************************************/
   if( camera == Camera::cameraNames[Camera::KODAK_DC240] ||
       camera == Camera::cameraNames[Camera::KODAK_DC280] )
   {
      ck4bitThumbnails.disable();
      ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
      ckRawPath.enable();
   }
   else
   {
      ckRawPath.disable();
   }

   /**************************************************************************
    * "Project O"
    **************************************************************************/
   if( camera == Camera::cameraNames[Camera::AGFA_EPHOTO]   ||
       camera == Camera::cameraNames[Camera::EPSON_PHOTOPC] ||
       camera == Camera::cameraNames[Camera::NIKON_COOLPIX] ||
       camera == Camera::cameraNames[Camera::OLYMPUS]       ||
       camera == Camera::cameraNames[Camera::SANYO_VPC_DSC] ||
       camera == Camera::cameraNames[Camera::SIERRA_SD640]  ||
       camera == Camera::cameraNames[Camera::TOSHIBA_PDR_2] )
   {
      ckRememberNumbering.enable();
      txtNumbering.enable( ckRememberNumbering.isSelected() );
      sbNumbering.enable(  ckRememberNumbering.isSelected() );
      ckAlternateStartup.enable();
      ckKeepThumbnails.enable();
      // enable the 24-bit thumbnails only if auto-download is selected
      ck4bitThumbnails.disable();
      ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
   }
   else
   {
      ckRememberNumbering.disable();
      txtNumbering.disable();
      sbNumbering.disable();
      ckAlternateStartup.disable();
      ckKeepThumbnails.disable();
   }

   /**************************************************************************
    * Casio QV10
    **************************************************************************
   if( camera == Camera::cameraNames[Camera::CASIO_QV10A] )
   {
      ckEnableLCD.enable();
      ck4bitThumbnails.disable();
      ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
   }
   else
   {
      ckEnableLCD.disable();
   }*/

   /**************************************************************************
    * Digita
    **************************************************************************/
   if( camera == Camera::cameraNames[Camera::DIGITA] ||
       camera == Camera::cameraNames[Camera::KODAK_DC220] ||
       camera == Camera::cameraNames[Camera::KODAK_DC260] ||
       camera == Camera::cameraNames[Camera::KODAK_DC265] ||
       camera == Camera::cameraNames[Camera::MINOLTA_DIMAGE] )
   {
      ck4bitThumbnails.disable();
      ck24bitThumbnails.enable( ckAutoDownloadThumbnails.isSelected() );
   }

   // indicate that this event has been handled
   event.setResult( true );
   return true;
}


/*****************************************************************************
 *
 *    OptionsWindow::selected
 *
 * handle the selection of a control we want to capture
 *
 *****************************************************************************/
Boolean OptionsWindow::selected( IControlEvent &event )
{
   if( event.dispatchingWindow() == &cbCameraType )
   {
      // if this is a change in the "camera type", then run OptionsWindow::enter()
      return enter( event );
   }

   // one of the checkboxes we need to control has been selected or deselected

   // if the "advanced com port selection" checkbox was clicked...
   if( event.dispatchingWindow() == &ckAdvancedCOMPort )
   {
      // toggle the state of the checkbox
      ckAdvancedCOMPort.select( sbCOMPort.isVisible() );
      // show/hide the COM port EF/SB depending on the checkbox selection
      sbCOMPort.show(!ckAdvancedCOMPort.isSelected() );
      efCOMPort.show( ckAdvancedCOMPort.isSelected() );
      // indicate that this event has been handled
      event.setResult( false );
      return false;
   }

   // was the Project O's "remember numbering" checkbox selected?
   if( event.dispatchingWindow() == &ckRememberNumbering )
   {
      txtNumbering.enable( ckRememberNumbering.isSelected() );
      sbNumbering.enable(  ckRememberNumbering.isSelected() );
   }


   // if the "auto download thumbnails" checkbox was clicked...
   if( event.dispatchingWindow() == &ckAutoDownloadThumbnails )
   {
      // when the checkbox that was clicked is the "auto download thumbnails", we need
      // to either enable or disable the appropriate thumbnail buttons
      ck24bitThumbnails.enable(ckAutoDownloadThumbnails.isSelected() );
      if(cbCameraType.text() == Camera::cameraNames[Camera::KODAK_DC200] ||
         cbCameraType.text() == Camera::cameraNames[Camera::KODAK_DC210] ||
         cbCameraType.text() == Camera::cameraNames[Camera::KODAK_DC215])
      {
         // these cameras supports 4-bit thumbnails, so enable/disable this one as well
         ck4bitThumbnails.enable(ckAutoDownloadThumbnails.isSelected());
      }

      // since the "auto download" button was the one that was changed, there
      // is a chance that it was just selected and the other thumbnail buttons
      // are both unselected -- if this is the case, we need to select them
      if( ckAutoDownloadThumbnails.isSelected() )
      {
         if((ck4bitThumbnails.isEnabled() && !ck4bitThumbnails.isSelected()) ||
            (ck24bitThumbnails.isEnabled() && !ck24bitThumbnails.isSelected()) )
         {
            if( ck4bitThumbnails.isEnabled() )
            {
               ck4bitThumbnails.select();
            }
            if( ck24bitThumbnails.isEnabled() )
            {
               ck24bitThumbnails.select();
            }
         }
      }
   }

   // check if the "auto-download thumbnails" should be de-selected
   if( (!ck4bitThumbnails.isEnabled() && !ck24bitThumbnails.isSelected() ) ||
      (!ck24bitThumbnails.isEnabled() && !ck4bitThumbnails.isSelected()) ||
      (ck4bitThumbnails.isEnabled() && !ck4bitThumbnails.isSelected() && !ck24bitThumbnails.isSelected()) )
   {
      ckAutoDownloadThumbnails.select(false);
      ck4bitThumbnails.disable();
      ck24bitThumbnails.disable();
   }

   // were one of the "script" checkboxes selected?
   IColor onColour(IColor::white);
   IColor offColour(backgroundColor());
   Boolean status;

   if( event.dispatchingWindow() == &ckScriptAfterConnecting )
   {
      status = !ckScriptAfterConnecting.isSelected();

      ckScriptAfterConnecting.   select(status);
      ckScriptAfterConnectingFg. enable(status);
      efScriptAfterConnecting.   setBackgroundColor(status?onColour:offColour).
                                 enable(status);
   }
   if( event.dispatchingWindow() == &ckScriptAfterTransfer )
   {
      status = !ckScriptAfterTransfer.isSelected();

      ckScriptAfterTransfer.     select(status);
      ckScriptAfterTransferFg.   enable(status);
      efScriptAfterTransfer.     setBackgroundColor(status?onColour:offColour).
                                 enable(status);
   }
   if( event.dispatchingWindow() == &ckScriptAfterDelete )
   {
      status = !ckScriptAfterDelete.isSelected();

      ckScriptAfterDelete.       select(status);
      ckScriptAfterDeleteFg.     enable(status);
      efScriptAfterDelete.       setBackgroundColor(status?onColour:offColour).
                                 enable(status);
   }
   if( event.dispatchingWindow() == &ckScriptNoMoreImages )
   {
      status = !ckScriptNoMoreImages.isSelected();

      ckScriptNoMoreImages.      select(status);
      ckScriptNoMoreImagesFg.    enable(status);
      efScriptNoMoreImages.      setBackgroundColor(status?onColour:offColour).
                                 enable(status);
   }
   if( event.dispatchingWindow() == &ckScriptDisconnect )
   {
      status = !ckScriptDisconnect.isSelected();

      ckScriptDisconnect.        select(status);
      ckScriptDisconnectFg.      enable(status);
      efScriptDisconnect.        setBackgroundColor(status?onColour:offColour).
                                 enable(status);
   }

   // indicate that this event has been handled
   event.setResult( true );
   return true;
}


/*****************************************************************************
 *
 *    OptionsWindow::lostFocus
 *
 * add a "\" at the end of the directory path
 *
 *****************************************************************************/
Boolean OptionsWindow::lostFocus( IControlEvent &event )
{
   Boolean result = false;

   // is the directory name empty?
   if( efDir.text() == "" )
   {
      // ...then use the current directory
      efDir.setText( "." );
   }

   // does the directory name end with a "\" character?
   if( (efDir.text())[efDir.text().length()] != '\\' )
   {
      // ...if not, then add it!
      efDir.setText( efDir.text() + "\\" );
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    OptionsWindow::systemCommand
 *
 * command handler to handle pushbutton events
 *
 *****************************************************************************/
Boolean OptionsWindow::systemCommand( ICommandEvent &event )
{
   Boolean result = false; // event has not been handled

   if(event.commandId() == ISystemMenu::idClose)
   {
      // pretend the user has clicked on the "OK" pushbutton
      pbOk.click();
      result = true;
   }

   event.setResult(result);
   return result;
}

/*****************************************************************************
 *
 *    OptionsWindow::command
 *
 * command handler to handle pushbutton events
 *
 *****************************************************************************/
Boolean OptionsWindow::command( ICommandEvent &event )
{
   Boolean result = false;

   // which button was pressed?
   switch( event.commandId() )
   {
      case PB_OK:
      {
         // this event also generated by ALT-F4 or closing the window -- see systemCommand()

         wnd.logText( "Options accepted." );

         // get the window's frame rectangle
         IRectangle wndRect( isMinimized()||isMaximized() ? restoreRect() : rect() );

         // save the window position
         wnd.profile.setDefaultApplicationName( "Options Window" ).
                     addOrReplaceElementWithKey( "x1", wndRect.minX() ).
                     addOrReplaceElementWithKey( "y1", wndRect.minY() ).
                     addOrReplaceElementWithKey( "x2", wndRect.maxX() ).
                     addOrReplaceElementWithKey( "y2", wndRect.maxY() );

         // get the values from the window to the settings

         // find out what camera is selected
         int newCameraType = Camera::KODAK_DC210;
         for( int i=0; i<Camera::NUMBER_OF_CAMERAS; i++ )
         {
            if( cbCameraType.text() == Camera::cameraNames[i] )
            {
               newCameraType = i;
               break;
            }
         }

         // has the camera type changed?
         if( newCameraType != connectionSettings.cameraType )
         {
            if( wnd.cameraThread->camera->connected )
            {
               // we're already connected -- don't actually change the camera type
               IMessageBox alreadyConnectedMsgBox( this );
               alreadyConnectedMsgBox.setTitle( "" ).
                     show( "The camera type will not be changed since a connection has already been established!",
                     IMessageBox::defButton1       |
                     IMessageBox::moveable         |
                     IMessageBox::informationIcon  |
                     IMessageBox::applicationModal |
                     IMessageBox::okButton         );
            }
            else
            {
               // change the camera type!!!

               // kill the old camera connection thread
               delete( wnd.cameraThread );

               // remember the new camera type
               connectionSettings.cameraType = newCameraType;

               // create a new camera connection thread
               wnd.cameraThread = new CameraThread( wnd );

               // start the new thread
               IThread thread( wnd.cameraThread, true );
            }
         }

         // get the COM port
         connectionSettings.advancedComPort = ckAdvancedCOMPort.isSelected();
         connectionSettings.comPort = sbCOMPort.text().subString(4,1).asInt();
         connectionSettings.advancedComPortName = efCOMPort.text();

         // get the baud rate
         connectionSettings.baudRate = sbBaudRate.text().asUnsigned();

         // is the baud rate valid?
         if( (!wnd.isRegistered) && (connectionSettings.baudRate>Camera::BAUD_9600) )
         {
            // this copy is unregistered, so go back down to 9600 baud
            connectionSettings.baudRate = Camera::BAUD_9600;
            IMessageBox wrongBaudRateMsgBox( this );
            wrongBaudRateMsgBox.setTitle( "" ).
                  show( IString( "Since this copy of DCITU for OS/2 is not "
                                 "registered, images can only be downloaded "
                                 "at speeds of 9600. For information on how "
                                 "to register, please see the REGISTER.TXT "
                                 "file that came with this application, or "
                                 "send an e-mail message to ") +
                        IString( STR_NAME )     +
                        IString( " at "   )     +
                        IString( STR_EMAIL)     +
                        IString( "."      )     ,
                  IMessageBox::defButton1       |
                  IMessageBox::moveable         |
                  IMessageBox::informationIcon  |
                  IMessageBox::applicationModal |
                  IMessageBox::okButton         );
         }

         // get the extended buffers
         if( cbExtBuffers.text() == "auto-detect" )
         {
            connectionSettings.extendedBuffer = ConnectionSettings::EXT_BUFFER_AUTO;
         }
         else if( cbExtBuffers.text() == "enable buffers" )
         {
            connectionSettings.extendedBuffer = ConnectionSettings::EXT_BUFFER_ON;
         }
         else
         {
            connectionSettings.extendedBuffer = ConnectionSettings::EXT_BUFFER_OFF;
         }

         // get the timeout detection
         connectionSettings.timeout = sbTimeout.text().word(1).asInt();

         // get the pause between commands
         connectionSettings.pauseBetweenCommands= sbPause.text().word(1).asInt();

         // get the pause after a change of baud rate
         connectionSettings.pauseAfterBaudChange = sbPauseAfterBaudChange.text().word(1).asInt();

         // get the "retry partial transfers"
         connectionSettings.retryPartialTransfers = (sbRetryPartialTransfers.text() == "enabled");

         // get the memory card ignore flag
         connectionSettings.ignoreMemoryCard = ckIgnoreMemoryCard.isSelected();

         // get the KDC conversion
         connectionSettings.convertKDCtoJPG = ckConvertKDCToJPG.isSelected();

         // get the raw directory path
         connectionSettings.useRawPath = ckRawPath.isSelected();

         // get the persistent image numbering
         connectionSettings.rememberNumbering = ckRememberNumbering.isSelected();
         connectionSettings.resumeNumberingAt = sbNumbering.value();

         // get the alternate camera startup
         connectionSettings.useAlternateStartup = ckAlternateStartup.isSelected();

         // get the keep thumbnails
         connectionSettings.keepThumbnails = ckKeepThumbnails.isSelected();

         // get the ignore BREAK flag
         connectionSettings.ignoreBreak = ckIgnoreBreak.isSelected();

         // get the serial line DTR control mode
         if( sbDTR.text() == "disable" )
         {
            connectionSettings.controlModeDTR = ConnectionSettings::DTR_DISABLE;
         }
         else if( sbDTR.text() == "enable" )
         {
            connectionSettings.controlModeDTR = ConnectionSettings::DTR_ENABLE;
         }
         else
         {
            connectionSettings.controlModeDTR = ConnectionSettings::DTR_HANDSHAKE;
         }

         // get the serial line RTS control mode
         if( sbRTS.text() == "disable" )
         {
            connectionSettings.controlModeRTS = ConnectionSettings::RTS_DISABLE;
         }
         else if( sbRTS.text() == "enable" )
         {
            connectionSettings.controlModeRTS = ConnectionSettings::RTS_ENABLE;
         }
         else if( sbRTS.text() == "handshake" )
         {
            connectionSettings.controlModeRTS = ConnectionSettings::RTS_HANDSHAKE;
         }
         else
         {
            connectionSettings.controlModeRTS = ConnectionSettings::RTS_TOGGLE;
         }

         // get the serial line CTS
         connectionSettings.enableCTS = ckCTS.isSelected();

         // get the serial line DSR
         connectionSettings.enableDSR = ckDSR.isSelected();

         // get the serial line DCD
         connectionSettings.enableDCD = ckDCD.isSelected();

         // get the serial line DSR input sensitivity
         connectionSettings.enableInputDSR = ckInputDSR.isSelected();

         // get the serial line transmit flow control
         connectionSettings.enableTransmitFlowControl = ckTransmitFlowControl.isSelected();

         // get the serial line receive flow control
         connectionSettings.enableReceiveFlowControl = ckReceiveFlowControl.isSelected();

         // get the transfer directory
         if(efDir.text() != connectionSettings.dir)
         {
            connectionSettings.dir = efDir.text();
            wnd.setPath(); // set the new directory as the current
         }

         // get the delete failed transfers
         connectionSettings.deleteFailedTransfers = ckDeleteFailedTransfer.isSelected();

         // get the thread priority
         connectionSettings.useHighestPriority = ckHighestPriority.isSelected();

         // get the connect on startup
         connectionSettings.autoConnect = ckConnectOnStartup.isSelected();

         // get the auto download thumbnails
         connectionSettings.autoDownloadThumbnails = ckAutoDownloadThumbnails.isSelected();

         // get the 4-bit thumbnails
         connectionSettings.autoDownload4bitThumbnails = ck4bitThumbnails.isSelected();

         // get the 24-bit thumbnails
         connectionSettings.autoDownload24bitThumbnails = ck24bitThumbnails.isSelected();

         // get the auto download images
         connectionSettings.autoTransferImages = ckAutoDownloadImages.isSelected();

         // get the overwrite duplicate images
         connectionSettings.overwriteDuplicates = ckOverwriteImages.isSelected();

         // get the delete images after transfer
         connectionSettings.deleteAfterTransfer = ckDeleteImages.isSelected();

         // get the script options
         connectionSettings.scriptAfterConnecting     = ckScriptAfterConnecting. isSelected();
         connectionSettings.scriptAfterConnectingFg   = ckScriptAfterConnectingFg.isSelected();
         connectionSettings.scriptAfterConnectingName = efScriptAfterConnecting. text();
         connectionSettings.scriptAfterTransfer       = ckScriptAfterTransfer.   isSelected();
         connectionSettings.scriptAfterTransferFg     = ckScriptAfterTransferFg. isSelected();
         connectionSettings.scriptAfterTransferName   = efScriptAfterTransfer.   text();
         connectionSettings.scriptAfterDelete         = ckScriptAfterDelete.     isSelected();
         connectionSettings.scriptAfterDeleteFg       = ckScriptAfterDeleteFg.   isSelected();
         connectionSettings.scriptAfterDeleteName     = efScriptAfterDelete.     text();
         connectionSettings.scriptNoMoreImages        = ckScriptNoMoreImages.    isSelected();
         connectionSettings.scriptNoMoreImagesFg      = ckScriptNoMoreImagesFg.  isSelected();
         connectionSettings.scriptNoMoreImagesName    = efScriptNoMoreImages.    text();
         connectionSettings.scriptDisconnect          = ckScriptDisconnect.      isSelected();
         connectionSettings.scriptDisconnectFg        = ckScriptDisconnectFg.    isSelected();
         connectionSettings.scriptDisconnectName      = efScriptDisconnect.      text();

         // no break -- fall into the code for the CANCEL pushbutton!
      }
      case PB_CANCEL:
      {
         // close the window
         wnd.optionsWindow = (OptionsWindow*)0;
         close();
         break;
      }
      case PB_RESET_SERIAL:
      {
         // reset the serial line controls to default values
         resetDefaultSerialLine();
         result = true;
         break;
      }
      case PB_REGISTER:
      {
         // if the registration name and key fields are not blank...
         if( (efRegisterName.text()!="") && (efRegisterKey.text()!="") )
         {
            // ...then write them out to the .INI file
            IProfile userProfile( IProfile::userProfile() );
            userProfile.setDefaultApplicationName( "DC210_SC" );
            userProfile.addOrReplaceElementWithKey( "RegName", efRegisterName.text() );
            userProfile.addOrReplaceElementWithKey( "RegNumber", efRegisterKey.text() );
         }

         // verify the registration
         wnd.verifyRegistration();

         // see if the registration was a success
         analyzeRegistration();

         // if it wasn't, then display a message
         if( ! wnd.isRegistered )
         {
            IMessageBox invalidMsgBox( this );
            invalidMsgBox. setTitle( "" ).
                           show( "Invalid registration.  Please try again.",
                           IMessageBox::defButton1       |
                           IMessageBox::moveable         |
                           IMessageBox::errorIcon        |
                           IMessageBox::applicationModal |
                           IMessageBox::okButton         );
         }
         result = true;
         break;
      }
   }

   event.setResult( result );
   return result;
}


/*****************************************************************************
 *
 *    OptionsWindow::edit
 *
 * handle the "edit" event for the directory entryfield
 *
 *****************************************************************************/
Boolean OptionsWindow::edit( IControlEvent &event )
{
   if( event.dispatchingWindow() == &efDir )
   {
      validatePath();
      event.setResult( false );
   }

   return false;
}


/*****************************************************************************
 *
 *    OptionsWindow::analyzeRegistration
 *
 * see if the user registration is valid
 *
 *****************************************************************************/
VOID OptionsWindow::analyzeRegistration()
{
   efRegisterName.setLimit(40);
   efRegisterKey.setLimit(40);
   // setup certain controls depending on registration
   if( wnd.isRegistered )
   {
      // DCITU is registered!
      efRegisterName.disableDataUpdate().
                     setText( wnd.registeredName ).
                     setBackgroundColor( backgroundColor() );
      efRegisterName.setLeftIndex(0);
      efRegisterKey. disableDataUpdate().
                     setText( wnd.registeredKey ).
                     setBackgroundColor( backgroundColor() );
      efRegisterKey. setLeftIndex(0);
      pbRegister.disable();
   }
   else
   {
      // DCITU is not registered!
      efRegisterName.enableDataUpdate().
                     setText( "" ).
                     setBackgroundColor( IColor::white );
      efRegisterKey. enableDataUpdate().
                     setText( "" ).
                     setBackgroundColor( IColor::white );
      pbRegister.enable();
      // spin the baud rate button back to 9600
      sbBaudRate.spinTo( Camera::baudRateNames[1] );
   }

   return;
}


/*****************************************************************************
 *
 *    OptionsWindow::resetDefaultSerialLine
 *
 * reset all of the serial line controls to default values
 *
 *****************************************************************************/
VOID OptionsWindow::resetDefaultSerialLine()
{
   // the user has chosen a new camera type, so handle
   // all of the enable/disable of camera-specific controls
   IString camera( cbCameraType.itemText(cbCameraType.selection()) );

   if(camera == Camera::cameraNames[Camera::AGFA_EPHOTO  ]  ||
    //camera == Camera::cameraNames[Camera::CASIO_QV10A  ]  ||
      camera == Camera::cameraNames[Camera::DIGITA       ]  ||
      camera == Camera::cameraNames[Camera::EPSON_PHOTOPC]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC25   ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC120  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC200  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC210  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC215  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC220  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC240  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC260  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC265  ]  ||
      camera == Camera::cameraNames[Camera::KODAK_DC280  ]  ||
      camera == Camera::cameraNames[Camera::MINOLTA_DIMAGE] ||
      camera == Camera::cameraNames[Camera::NIKON_COOLPIX]  ||
      camera == Camera::cameraNames[Camera::OLYMPUS      ]  ||
      camera == Camera::cameraNames[Camera::SANYO_VPC_DSC]  ||
      camera == Camera::cameraNames[Camera::SIERRA_SD640 ]  ||
      camera == Camera::cameraNames[Camera::TOSHIBA_PDR_2])
   {
      // these cameras use a simple 3-wire cable
      sbDTR.spinTo( ConnectionSettings::DTR_ENABLE );
      sbRTS.spinTo( ConnectionSettings::RTS_ENABLE );
      ckCTS.               select( false);
      ckDSR.               select( false);
      ckDCD.               select( false);
      ckInputDSR.          select( false);
      ckTransmitFlowControl.select(false);
      ckReceiveFlowControl.select( false);
   }

   /*if(camera == Camera::cameraNames[Camera::KODAK_DC50])
   {
      // these cameras use a standard 9-pin serial cable
      sbDTR.spinTo( (unsigned long)ConnectionSettings::DTR_DISABLE );
      sbRTS.spinTo( (unsigned long)ConnectionSettings::RTS_DISABLE );
      ckCTS.               select( false);
      ckDSR.               select( false);
      ckDCD.               select( false);
      ckInputDSR.          select( false);
      ckTransmitFlowControl.select(true );
      ckReceiveFlowControl.select( true );
   }*/

   return;
}


/*****************************************************************************
 *
 *    OptionsWindow::validatePath
 *
 * validate the path specified in the image transfer directory and show a
 * warning if it doesn't exist
 *
 *****************************************************************************/
VOID OptionsWindow::validatePath()
{
   // get the name of the path we need to check
   IString name( efDir.text() );

   // is the name anything other than 3 chars long? (exclude "C:\" but not "..\")
   if( name.length() != 3 || name[2] != ':' )
   {
      // if the last character is "\" or "/"...
      char lastChar = name[name.length()];
      if( lastChar == '\\' || lastChar == '/' )
      {
        // ...then we need to remove it
        name.remove(name.length());
      }
   }

   // now that we have the path, disable the OS/2 error popup window
   DosError(FERR_DISABLEHARDERR);

   // query the path
   FILESTATUS3 fileStatus3;
   APIRET rc = DosQueryPathInfo(name, FIL_STANDARD, &fileStatus3, sizeof(FILESTATUS3));

   // re-enable the error popup window
   DosError(FERR_ENABLEHARDERR);

   // where we successful with DosQueryPathInfo?  is this a directory?
   if( rc == 0 && fileStatus3.attrFile & FILE_DIRECTORY )
   {
      // the name is a valid directory!
      txtDir.hide();
   }
   else
   {
      // the name is NOT a valid directory!
      txtDir.show();
   }

   // now see if the path seems to be relative
   if(name.length() > 0)
   {
      if(name[1] == '.' || name[1] == '\\')
      {
         txtRelativePath.show();
      }
      else
      {
         txtRelativePath.hide();
      }
   }
   else
   {
      txtRelativePath.hide();
   }

   return;
}


