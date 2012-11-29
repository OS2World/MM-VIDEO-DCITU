/*
 * $Id: //devel/DCITU/Camera.CPP#10 $
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

// OS/2 definitions
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR

// local includes
#include "Camera.HPP"
#include "GUIWindow.H"
#include "GUIWindow.HPP"
#include "ImageObject.HPP"
#include "Camera_Digita.HPP"
#include "Camera_Kodak_DC25.HPP"
#include "Camera_Kodak_DC120.HPP"
#include "Camera_Kodak_DC210.HPP"
#include "Camera_Kodak_DC240.HPP"
#include "Camera_ProjectO.HPP"

// global includes
#include <iexcbase.hpp> // IException
#include <imsgbox.hpp>  // IMessageBox
#include <os2.h>



// static variables
char *Camera::cameraNames[Camera::NUMBER_OF_CAMERAS] =
      {
         // note that these names MUST be kept in sync
         // with the Camera::cameraTypes variable defined
         // in Camera.HPP
         "Agfa ePhoto"  ,
       //"Casio QV10A"  ,
         "Digita"       ,
         "Epson PhotoPC",
       //"Kodak DC20"   ,
         "Kodak DC25"   ,
       //"Kodak DC50"   ,
         "Kodak DC120"  ,
         "Kodak DC200"  ,
         "Kodak DC210"  ,
         "Kodak DC215"  ,
         "Kodak DC220"  ,
         "Kodak DC240"  ,
         "Kodak DC260"  ,
         "Kodak DC265"  ,
         "Kodak DC280"  ,
         "Minolta Dimage",
         "Nikon CoolPix",
         "Olympus"      ,
         "Sanyo VPC/DSC",
         "Sierra SD640" ,
         "Toshiba PDR-2"
      };

char *Camera::baudRateNames[Camera::NUMBER_OF_BAUD_RATES] =
      {
         "0"      ,
         "9600"   ,
         "19200"  ,
         "38400"  ,
         "57600"  ,
         "115200" ,
         "230400"
      };


// structure used to query the baud rate
#pragma pack(1)
struct _EXTGETBAUDRATE
{
   ULONG currentBaudRate;
   UCHAR currentFraction;
   ULONG minimumBaudRate;
   UCHAR minimumFraction;
   ULONG maximumBaudRate;
   UCHAR maximumFraction;
};
#pragma pack()


/*****************************************************************************
 *
 *    CameraThread::CameraThread
 *
 * constructor for camera thread class
 *
 *****************************************************************************/
CameraThread::CameraThread( GUIWindow &wnd ) :
   IThreadFn(),
   wnd( wnd )
{
   wnd.logText( 0, "-> camera connection thread created" );
   return;
}


/*****************************************************************************
 *
 *    CameraThread::~CameraThread
 *
 * destructor for camera thread class
 *
 *****************************************************************************/
CameraThread::~CameraThread()
{
   if( IWindow::isWindowValid(camera) )
   {
      if(IWindow::isWindowValid(&wnd))
      {
         wnd.logText( 0, "=> closing the camera connection object window" );
      }
      camera->close();
      //delete camera;
   }
   camera = (Camera*)0;

   if(IWindow::isWindowValid(&wnd))
   {
      wnd.logText( 0, "=> destructing camera connection thread" );
   }

   return;
}


/*****************************************************************************
 *
 *    CameraThread::run
 *
 * method that runs the camera thread; sets up the camera class and
 * starts event processing for the camera class
 *
 *****************************************************************************/
void CameraThread::run( void )
{
   try
   {
      // set ourselves up as an important thread -- raise our priority level
      if(wnd.connectionSettings.useHighestPriority)
      {
         wnd.logText( 0, "-> increase connection thread priority to highest level possible" );
         IThread::current().setPriority( IApplication::foregroundServer, 31 );
      }
      else
      {
         wnd.logText( 0, "-> increase priority for connection thread" );
         IThread::current().setPriority( IApplication::timeCritical, 31 );
      }

      // see if we must increase the queue size
      if(wnd.connectionSettings.queueSizeIncrease > 0)
      {
         wnd.logText( 0, "-> increase the message queue size by " + IString(wnd.connectionSettings.queueSizeIncrease) );
         IThread::current().setQueueSize(IThread::current().queueSize() + wnd.connectionSettings.queueSizeIncrease);
      }
      wnd.logText( 0, "-> thread message queue size is " + IString(IThread::current().queueSize()) );

      // find out the camera type and create the camera connection object
      wnd.logText( 0, "-> creating camera connection object window" );

      switch( wnd.connectionSettings.cameraType )
      {
         /********************************************************************
          * CameraThread::run : KODAK_DC25
          ********************************************************************/
         case Camera::KODAK_DC25:
         {
            wnd.logText( 0, "-> selected camera type is DC25" );
            camera = new Camera_Kodak_DC25( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : KODAK_DC50
          ********************************************************************
         case Camera::KODAK_DC50:
         {
            wnd.logText( 0, "-> selected camera type is DC50" );
            camera = new Camera_Kodak_DC50( wnd );
            break;
         }*/
         /********************************************************************
          * CameraThread::run : KODAK_DC120
          ********************************************************************/
         case Camera::KODAK_DC120:
         {
            wnd.logText( 0, "-> selected camera type is DC120" );
            camera = new Camera_Kodak_DC120( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : KODAK_DC200+
          ********************************************************************/
         case Camera::KODAK_DC200:
         case Camera::KODAK_DC210:
         case Camera::KODAK_DC215:
         {
            wnd.logText( 0, "-> selected camera type is DC200/DC210/DC215" );
            camera = new Camera_Kodak_DC210( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : KODAK_DC240
          ********************************************************************/
         case Camera::KODAK_DC240:
         case Camera::KODAK_DC280:
         {
            wnd.logText( 0, "-> selected camera type is DC240/DC280" );
            camera = new Camera_Kodak_DC240( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : project O
          ********************************************************************/
         case Camera::AGFA_EPHOTO:
         case Camera::EPSON_PHOTOPC:
         case Camera::OLYMPUS:
         case Camera::NIKON_COOLPIX:
         case Camera::SANYO_VPC_DSC:
         case Camera::SIERRA_SD640:
         case Camera::TOSHIBA_PDR_2:
         {
            wnd.logText( 0, "-> selected camera type is \"Project 'O'\"" );
            camera = new Camera_ProjectO( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : digita
          ********************************************************************/
         case Camera::DIGITA:
         case Camera::KODAK_DC220:
         case Camera::KODAK_DC260:
         case Camera::KODAK_DC265:
         case Camera::MINOLTA_DIMAGE:
         {
            wnd.logText( 0, "-> selected camera type is \"Digita\"" );
            camera = new Camera_Digita( wnd );
            break;
         }
         /********************************************************************
          * CameraThread::run : CASIO_QV10A
          ********************************************************************
         case Camera::CASIO_QV10A:
         {
            wnd.logText( 0, "-> selected camera type is \"Casio QV\"" );
            camera = new Camera_Casio_QV10( wnd );
            break;
         }*/
         /********************************************************************
          * CameraThread::run : default
          ********************************************************************/
         default:
         {
            wnd.logText( 0, "ERROR: camera type \"" + IString(wnd.connectionSettings.cameraType) + "\" not supported" );

            IMessageBox msgBox(  &wnd                          );
            msgBox.  setTitle(   "DCITU Camera Service Thread Error").
                     show(       "Camera type not supported."  ,
                                 IMessageBox::applicationModal |
                                 IMessageBox::defButton1       |
                                 IMessageBox::okButton         |
                                 IMessageBox::errorIcon        |
                                 IMessageBox::moveable         );
            wnd.close();
            break;
         }
      }

      if( camera )
      {
         // this will keep the class running until a WM_QUIT message is received
         wnd.logText( 0, "-> running connection thread message loop" );
         IThread::current().processMsgs();
         if(IWindow::isWindowValid(&wnd))
         {
            wnd.logText( 0, "=> NOTE: camera connection thread message loop is ending!" );
         }
      }
   }
   catch( IException &exception )
   {
      // exception occured -- show the associated text!
      wnd.logText( 0, "-> DCITU Camera Service Thread Exception Error:\n" + IString(exception.text()));
      IMessageBox msgBox(  &wnd                          );
      msgBox.  setTitle(   "DCITU Camera Service Thread Exception Error").
               show(       exception.text()              ,
                           IMessageBox::applicationModal |
                           IMessageBox::defButton1       |
                           IMessageBox::okButton         |
                           IMessageBox::errorIcon        |
                           IMessageBox::moveable         );
   }
   catch( ... )
   {
      wnd.logText( 0, "-> DCITU Camera Service Thread Exception Error:  unknown exception");
      IMessageBox msgBox(  &wnd                          );
      msgBox.  setTitle(   "DCITU Camera Service Thread Exception Error").
               show(       "Unknown exception."          ,
                           IMessageBox::applicationModal |
                           IMessageBox::defButton1       |
                           IMessageBox::okButton         |
                           IMessageBox::errorIcon        |
                           IMessageBox::moveable         );
   }
   return;
}


/*****************************************************************************
 *
 *    Camera::Camera
 *
 * constructor; initializes some variables and sets up the owner chain
 *
 *****************************************************************************/
Camera::Camera( GUIWindow &wnd ) :
   IObjectWindow(),
   ICommandHandler(),
   wnd( wnd )
{
   setOwner( &wnd );  // this ensures that when the parent window dies, we'll die too!

   // initialize these variables immediately
   cameraType        = wnd.connectionSettings.cameraType;
   cancelRequested   = false;
   connected         = false;
   cardOpened        = false;
   poweredUp         = false;

   wnd.logText( 0, "-> starting Camera::Camera with \"" + IString(Camera::cameraNames[wnd.connectionSettings.cameraType]) + "\"" );

   // set up the object window to automatically delete itself when it gets closed
   setAutoDeleteObject( true );
   setAutoDestroyWindow( true );

   // setup event handlers
   ICommandHandler::handleEventsFor( this );

   return;
}


/*****************************************************************************
 *
 *    Camera::~Camera
 *
 * destructor for camera class
 *
 *****************************************************************************/
Camera::~Camera()
{
   if(IWindow::isWindowValid(&wnd))
   {
      wnd.logText( 0, "=> destructing camera connection object window" );
   }

   // stop the event handlers
   ICommandHandler::stopHandlingEventsFor( this );

   return;
}


/*****************************************************************************
 *
 *    Camera::logText
 *
 * log text to the debug window & status bar
 *
 *****************************************************************************/
VOID Camera::logText( const CHAR *msg1, const CHAR *msg2 ) const
{
   // if msg1 was specified,
   // ...OR msg2 was specified AND the debug window exists
   if( msg1 || (msg2 && wnd.debugWindow) )
   {
      wnd.logText( msg1, msg2 );
   }
   return;
}


/*****************************************************************************
 *
 *    Camera::command
 *
 * process all command events (inherited from ICommandHandler)
 *
 *****************************************************************************/
Boolean Camera::command( ICommandEvent &event )
{
   if( ! cancelRequested )
   {
      // as long as "cancel" was not requested, then deal with this event!
      switch( event.commandId() )
      {
         /********************************************************************
          *    CameraCommand::CONNECT
          ********************************************************************/
         case CONNECT:
         {
            Boolean result;
            // open the connection
            result = openConnection();
            if( result )
            {
               if( requiresPowerUpDown() )
               {
                  // these cameras require a "power up" command call
                  result = powerUp();
               }
               else if( currentBaudRate != wnd.connectionSettings.baudRate )
               {
                  // set the transfer rate
                  result = setBaudRate(wnd.connectionSettings.baudRate);
               }
            }

            // display the current baud rate
            if( result )
            {
               struct _EXTGETBAUDRATE baudRate;
               ULONG baudRateLen = sizeof(struct _EXTGETBAUDRATE);

               DosDevIOCtl(portHandle           ,  // filehandle obtained from DosOpen()
                           IOCTL_ASYNC          ,  // device category
                           ASYNC_EXTGETBAUDRATE ,  // get extended baud rate
                           (PVOID)0             ,  // pointer to parameter buffer
                           (ULONG)0             ,  // length of parameter buffer
                           (PULONG)0            ,  // pointer to length of parameter buffer
                           (PVOID)&baudRate     ,  // pointer to data buffer
                           (ULONG)baudRateLen   ,  // max length of data buffer
                           (PULONG)&baudRateLen ); // pointer to length of data buffer

               wnd.efBaud.setText(IString(baudRate.currentBaudRate));
            }

            if( result )
            {
               result = openCard();
            }

            if( result )
            {
               result = getCameraInfo();
            }

            if( result )
            {
               result = getPictureInfo();
            }

            if( result )
            {
               result = powerDown();
            }

            // did an error occur in any of the above steps?
            if( ! result )
            {
               if( cardOpened )
               {
                  closeCard();
               }
               if( requiresPowerUpDown() )
               {
                  // these cameras require a "power down" command call
                  powerDown();
               }
               else if( currentBaudRate > BAUD_9600 )
               {
                  setBaudRate(BAUD_9600);
               }
               closeConnection();
               wnd.efBaud.setText("");
               logText( "Could not establish connection." );
            }
            else
            {
               int numberOfImages = wnd.cnr.objectCount();
               logText( "Connection established.  " + IString(numberOfImages) + " image" + (numberOfImages==1?"":"s") + " found." );

               // remember the new image number to prevent duplicate numbers
               if(wnd.connectionSettings.rememberNumbering)
               {
                  wnd.connectionSettings.profile.setDefaultApplicationName("Connection Settings");
                  wnd.connectionSettings.profile.addOrReplaceElementWithKey("Numbering Resumes At", wnd.connectionSettings.resumeNumberingAt);
               }

               wnd.getSelectionStatus();

               // do we run the "connection" script?
               if( wnd.connectionSettings.scriptAfterConnecting )
               {
                  IString script( wnd.connectionSettings.scriptAfterConnectingName );
                  if( script != "" )
                  {
                     script.change("%i",IString(numberOfImages));
                     wnd.runScript( script, wnd.connectionSettings.scriptAfterConnectingFg );
                  }
               }

               // do we download the thumbnails?
               if( wnd.connectionSettings.autoDownloadThumbnails && wnd.connectionSettings.cameraType != Camera::KODAK_DC120 )
               {
                  if(wnd.connectionSettings.autoDownload4bitThumbnails &&
                     wnd.connectionSettings.cameraType == Camera::KODAK_DC210)
                  {
                     // dowload the 4-bit thumbnails

                     // see if we have to pause
                     if( (wnd.connectionSettings.pauseBetweenCommands) && (!cancelRequested) )
                     {
                        logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
                        IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
                     }

                     IContainerControl::ObjectCursor cursor( wnd.cnr, IContainerObject::none );
                     for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
                     {
                        sendMessage(GET_SMALL_THUMBNAIL,
                                    ((ImageObject*)cursor.current())->originalNumber );
                     }
                  }
                  if(wnd.connectionSettings.autoDownload24bitThumbnails)
                  {
                     // download the 24-bit thumbnails

                     // see if we have to pause
                     if( (wnd.connectionSettings.pauseBetweenCommands) && (!cancelRequested) )
                     {
                        logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
                        IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
                     }

                     IContainerControl::ObjectCursor cursor( wnd.cnr, IContainerObject::none );
                     for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
                     {
                        sendMessage(GET_LARGE_THUMBNAIL,
                                    ((ImageObject*)cursor.current())->originalNumber );
                     }
                  }
               }
               if(wnd.connectionSettings.autoTransferImages)
               {
                  // download the full images

                  // see if we have to pause
                  if( (wnd.connectionSettings.pauseBetweenCommands) && (!cancelRequested) )
                  {
                     logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
                     IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
                  }

                  IContainerControl::ObjectCursor cursor( wnd.cnr, IContainerObject::none );
                  for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
                  {
                     sendMessage(GET_IMAGE,
                                 ((ImageObject*)cursor.current())->originalNumber );
                  }
               }
               powerDown();
            }
            wnd.enableMenuBarItems();
            break;
         }
         /********************************************************************
          *    CameraCommand::DISCONNECT
          ********************************************************************/
         case DISCONNECT:
         {
            powerUp();
            if( cardOpened )
            {
               closeCard();
            }
            if( requiresPowerUpDown() )
            {
               powerDown();
            }
            else if( currentBaudRate > BAUD_9600 )
            {
               setBaudRate(BAUD_9600);
            }

            disconnect();

            closeConnection();
            logText( "" );
            wnd.efSelected.setText("");
            wnd.efBaud.setText("");
            wnd.enableMenuBarItems();

            // do we run the "disconnect" script?
            if( wnd.connectionSettings.scriptDisconnect )
            {
               if( wnd.connectionSettings.scriptDisconnectName != "" )
               {
                  wnd.runScript( wnd.connectionSettings.scriptDisconnectName, wnd.connectionSettings.scriptDisconnectFg );
               }
            }
            break;
         }
         /********************************************************************
          *    CameraCommand::GET_LARGE_THUMBNAIL
          ********************************************************************/
         case GET_LARGE_THUMBNAIL:
         {
            if( powerUp() )
            {
               if( readLargeThumbnail( event.parameter2().asUnsignedLong() ) )
               {
                  logText( "" );
               }
               else
               {
                  logText( "Unable to transfer colour thumbnail." );
               }
               powerDown();
            }
            break;
         }
         /********************************************************************
          *    CameraCommand::GET_SMALL_THUMBNAIL
          ********************************************************************/
         case GET_SMALL_THUMBNAIL:
         {
            if( powerUp() )
            {
               if( readSmallThumbnail( event.parameter2().asUnsignedLong() ) )
               {
                  logText( "" );
               }
               else
               {
                  logText( "Unable to transfer grayscale thumbnail." );
               }
               powerDown();
            }
            break;
         }
         /********************************************************************
          *    CameraCommand::GET_IMAGE
          ********************************************************************/
         case GET_IMAGE:
         {
            Boolean powerUpSuccessful = true;
            IString image_name;
            ImageObject *obj = ImageObject::findObject(event.parameter2().asUnsignedLong());
            if( obj )
            {
               image_name = wnd.connectionSettings.dir+obj->name;
               obj->addBlueLine();
               obj->disableDataUpdate();
            }

            if( (!wnd.isRegistered) && (wnd.connectionSettings.baudRate>BAUD_9600) )
            {
               // go back to 9600 baud since this copy is not registered
               wnd.connectionSettings.baudRate = BAUD_9600;

               if( requiresPowerUpDown() )
               {
                  powerUpSuccessful = powerUp();
               }
               else
               {
                  powerUpSuccessful = setBaudRate(BAUD_9600);
               }
            }
            else
            {
               if( requiresPowerUpDown() )
               {
                  powerUpSuccessful = powerUp();
               }
            }

            if( powerUpSuccessful )
            {
               Boolean result = readImage( event.parameter2().asUnsignedLong() );
               powerDown();
               if( (!result) && (wnd.connectionSettings.deleteFailedTransfers) )
               {
                  // delete the file since it wasn't downloaded correctly
                  if( obj )
                  {
                     DosDelete(image_name);
                  }
               }
               // if an image was transfered, do we need to run a script?
               if(result && wnd.connectionSettings.scriptAfterTransfer)
               {
                  IString script( wnd.connectionSettings.scriptAfterTransferName );
                  if( script != "" )
                  {
                     script.change( "%s", image_name );
                     wnd.runScript( script, wnd.connectionSettings.scriptAfterTransferFg );
                  }
               }
            }
            if( obj )
            {
               obj->enableDataUpdate();
            }
            break;
         }
         /********************************************************************
          *    CameraCommand::DELETE_IMAGE
          ********************************************************************/
         case DELETE_IMAGE:
         {
            if( powerUp() )
            {
               IString image_name;
               ImageObject *obj = ImageObject::findObject(event.parameter2().asUnsignedLong());
               if( obj )
               {
                  image_name = wnd.connectionSettings.dir+obj->name;
               }

               if( deleteImage( event.parameter2().asUnsignedLong() ) )
               {
                  wnd.getSelectionStatus();

                  // do we run a script after an image was deleted?
                  if( wnd.connectionSettings.scriptAfterDelete )
                  {
                     IString script( wnd.connectionSettings.scriptAfterDeleteName );
                     if( script != "" )
                     {
                        script.change( "%s", image_name );
                        wnd.runScript( script, wnd.connectionSettings.scriptAfterDeleteFg );
                     }
                  }

                  // do we run a script when no more images remain?
                  if( wnd.cnr.objectCount() == 0 )
                  {
                     wnd.efSelected.setText("");

                     if( wnd.connectionSettings.scriptNoMoreImages )
                     {
                        IString script( wnd.connectionSettings.scriptNoMoreImagesName );
                        if( script != "" )
                        {
                           wnd.runScript( script, wnd.connectionSettings.scriptNoMoreImagesFg );
                        }
                     }
                  }

                  logText( "" );
               }
               else
               {
                  logText( "Unable to delete file." );
               }
               powerDown();
            }
            break;
         }
      }
   }

   // decrement the purge counter
   int counter = wnd.pbPurge.text().asInt() - 1;
   wnd.pbPurge.setText( IString(counter) );
   if( counter < 1 )
   {
      wnd.pbPurge.unlatch().disableLatching().setText("0");
      wnd.flyOverHelpHandler.removeHelpText( wnd.pbPurge.handle() );
      wnd.menuBar.checkItem( MB_PURGE_RCID, false ).
                  disableItem( MB_PURGE_RCID ).
                  setText( MB_PURGE_RCID, "~Purge items\tAlt+P" );
      cancelRequested = false;
   }
   else
   {
      wnd.menuBar.enableItem( MB_PURGE_RCID ).
         checkItem( MB_PURGE_RCID, cancelRequested ).
         setText( MB_PURGE_RCID, "~Purge items (" + IString(counter) + ")\tAlt+P" );
   }

   // see if we have to pause
   if( (wnd.connectionSettings.pauseBetweenCommands) && (!cancelRequested) && (counter > 0) )
   {
      logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
      IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
   }

   // set this event as having been handled
   event.setResult( true );
   return true;
}


/*****************************************************************************
 *
 *    Camera::sendMessage
 *
 * asynchronously post a message to the camera processing queue
 *
 *****************************************************************************/
void Camera::sendMessage( const ULONG message, const ULONG id ) const
{
   // increment the counter
   int counter = wnd.pbPurge.text().asInt() + 1;

   // is this the first item added to the queue?
   if( counter == 1 )
   {
      // change the flyover help text
      wnd.flyOverHelpHandler.setHelpText( wnd.pbPurge.handle(), IString("click to purge all items in the camera queue") );
   }
   wnd.pbPurge.enableLatching().setText( IString(counter) );
   wnd.menuBar.enableItem( MB_PURGE_RCID ).
               checkItem( MB_PURGE_RCID, cancelRequested ).
               setText( MB_PURGE_RCID, IString("~Purge items (")+IString(counter)+IString(")") );

   // post the message to the queue
   postEvent(  IWindow::command           ,
               IEventParameter1(message)  ,
               IEventParameter2(id)       );
   return;
}


/*****************************************************************************
 *
 *    Camera::openConnection
 *
 * open the serial port and setup the connection
 *
 *****************************************************************************/
Boolean Camera::openConnection( const ULONG defaultSpeed )
{
   APIRET rc;
   ULONG action;
   Boolean result = false;
   IString comPort;

   // are we using a normal port name, or an advanced port name?
   if( wnd.connectionSettings.advancedComPort )
   {
      comPort = wnd.connectionSettings.advancedComPortName;
   }
   else
   {
      comPort = IString("COM") + IString(wnd.connectionSettings.comPort);
   }

   logText( "Opening communication port \"" + comPort + "\"." );

   if( connected )
   {
      // we're already connected!
      logText( 0, "...ERROR: already connected!" );
      rc = 1;
   }
   else
   {
      // set some basic connection values
      cancelRequested = false;
      currentBaudRate = 0;

      logText( 0, "...camera connection object type: " + IString(getCameraTypeName()) );

      // open the communication port
      rc = DosOpen(  comPort                 ,  // filename
                     &portHandle             ,  // file handle
                     &action                 ,  // action that DosOpen carried out
                     0                       ,  // new size of file
                     FILE_NORMAL             ,  // new file attributes
                     FILE_OPEN               ,  // what to do with the file
                     OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                     OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                     OPEN_SHARE_DENYREADWRITE|  // deny others from read/write
                     OPEN_ACCESS_READWRITE   ,  // open for read & write
                     (PEAOP2)0               ); // extended attributes
   }

   // declare variables needed in this scope for the next 2 calls
   DCBINFO dcbInfo;
   ULONG dcbInfoLen = sizeof( DCBINFO );

   if( ! rc )
   {
      // get the DCBINFO from the opened port
      logText( 0, "...getting DCBINFO..." );
      rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_GETDCBINFO     ,  // device-specific function
                        (PVOID)0             ,  // pointer to parameter buffer
                        (ULONG)0             ,  // length of parameter buffer
                        (PULONG)0            ,  // pointer to length of parameter buffer
                        (PVOID)&dcbInfo      ,  // pointer to data buffer
                        (ULONG)dcbInfoLen    ,  // max length of data buffer
                        (PULONG)&dcbInfoLen  ); // pointer to length of data buffer
   }

   if( ! rc )
   {
      logText( 0, "...-> fbCtlHndShake == 0b"+ IString(dcbInfo.fbCtlHndShake).c2b() );
      logText( 0, "...-> fbFlowReplace == 0b"+ IString(dcbInfo.fbFlowReplace).c2b() );
      logText( 0, "...-> fbTimeout == 0b"    + IString(dcbInfo.fbTimeout).    c2b() );

      // set the port the way we want

      /* Bits & meaning for the fbCtlHndShake byte
       *
       * [7.6.5.4.3.2.1.0]
       *  | | | | | | \_/
       *  | | | | | |  |_ [1&0] DTR control mode (0==disable, 1==enable, 2==input handshaking)
       *  | | | | | |____ [2] reserved, must be 0
       *  | | | | |______ [3] output handshaking using CTS (0==disable, 1==enable)
       *  | | | |________ [4] output handshaking using DSR (0==disable, 1==enable)
       *  | | |__________ [5] output handshaking using DCD (0==disable, 1==enable)
       *  | |____________ [6] input sensitivity using DSR (0==disable, 1==enable)
       *  |______________ [7] reserved, must be 0
       */
    //dcbInfo.fbCtlHndShake = (1<<0);  // enable DTR control mode
      if( wnd.connectionSettings.controlModeDTR == ConnectionSettings::DTR_DISABLE )
      {
         dcbInfo.fbCtlHndShake = 0;
      }
      else if( wnd.connectionSettings.controlModeDTR == ConnectionSettings::DTR_ENABLE )
      {
         dcbInfo.fbCtlHndShake = 1;
      }
      else if( wnd.connectionSettings.controlModeDTR == ConnectionSettings::DTR_HANDSHAKE )
      {
         dcbInfo.fbCtlHndShake = 2;
      }
      else
      {
         logText( 0, "WARNING: illegal value in ConnectionSettings.controlModeDTR" );
         dcbInfo.fbCtlHndShake = 1;
      }
      dcbInfo.fbCtlHndShake |=((wnd.connectionSettings.enableCTS?1:0)<<3) |
                              ((wnd.connectionSettings.enableDSR?1:0)<<4) |
                              ((wnd.connectionSettings.enableDCD?1:0)<<5) |
                              ((wnd.connectionSettings.enableInputDSR?1:0)<<6);

      /* Bits & meaning for the fbFlowReplace byte
       *
       * [7.6.5.4.3.2.1.0]
       *  \_/ | | | | | |
       *   |  | | | | | |_ [0] automatic transmit flow control (0==disable, 1==enable)
       *   |  | | | | |___ [1] automatic receive flow control (0==disable, 1==enable)
       *   |  | | | |_____ [2] error replacement character (0==disable, 1==enable)
       *   |  | | |_______ [3] null character stripping (0==disable, 1==enable)
       *   |  | |_________ [4] break replacement character (0==disable, 1==enable)
       *   |  |___________ [5] automatic receive flow control (0==normal, 1==full-duplex)
       *   |______________ [7&6] RTS control mode (0==disable, 1==enable, 2==input handshaking, 3==toggle on transmit)
       */
    //dcbInfo.fbFlowReplace = (1<<5) | (1<<6);  // enable full-duplex and RTS line
      if( wnd.connectionSettings.controlModeRTS == ConnectionSettings::RTS_DISABLE )
      {
         dcbInfo.fbFlowReplace = 0;
      }
      else if( wnd.connectionSettings.controlModeRTS == ConnectionSettings::RTS_ENABLE )
      {
         dcbInfo.fbFlowReplace = (1<<6);
      }
      else if( wnd.connectionSettings.controlModeRTS == ConnectionSettings::RTS_HANDSHAKE )
      {
         dcbInfo.fbFlowReplace = (2<<6);
      }
      else if( wnd.connectionSettings.controlModeRTS == ConnectionSettings::RTS_TOGGLE )
      {
         dcbInfo.fbFlowReplace = (3<<6);
      }
      else
      {
         logText( 0, "WARNING: illegal value in ConnectionSettings.controlModeRTS" );
         dcbInfo.fbFlowReplace = (1<<6);
      }
      dcbInfo.fbFlowReplace |=((wnd.connectionSettings.enableTransmitFlowControl?1:0)<<0) |
                              ((wnd.connectionSettings.enableReceiveFlowControl?1:0)<<1) |
                              (1<<5);

      /* Bits & meaning for the fbTimeout byte
       *
       * [7.6.5.4.3.2.1.0]
       *  | \_/ \_/ \_/ |
       *  |  |   |   |  |_ [0] write infinite timeout
       *  |  |   |   |____ [2&1] read timeout (1==normal, 2==wait, 3==no-wait)
       *  |  |   |________ [4&3] extended buffer (0==none, 1==disabled, 2==enabled, 3==auto)
       *  |  |____________ [6&5] receive trigger (0==1 CHAR, 1==4 CHARs, 2==8 CHARs, 3==14 CHARs)
       *  |_______________ [7] transmit load count (0==1 CHAR, 1=16 CHARs)
       */
      BYTE bitsToMaskOut = (1<<0) | (1<<2);  // remove bits 0 and 2
      BYTE bitsToMaskIn =  (1<<1);           // set bit 1

      // should we enable extended buffering?
      Boolean extendedBuffering;

      if( (dcbInfo.fbTimeout>>3)&3 )
      {
         logText( 0, "...extended buffering detected..." );
         extendedBuffering = true;
      }
      else
      {
         logText( 0, "...no extended buffering detected..." );
         extendedBuffering = false;
      }

      if( wnd.connectionSettings.extendedBuffer == ConnectionSettings::EXT_BUFFER_AUTO )
      {
         logText( 0, "...extended buffering is on auto-detect..." );
      }
      else if( wnd.connectionSettings.extendedBuffer == ConnectionSettings::EXT_BUFFER_ON )
      {
         logText( 0, "...extended buffering is forced enabled..." );
         extendedBuffering = true;
      }
      else if( wnd.connectionSettings.extendedBuffer == ConnectionSettings::EXT_BUFFER_OFF )
      {
         logText( 0, "...extended buffering is forced disabled..." );
         extendedBuffering = false;
      }
      else
      {
         logText( 0, "WARNING: illegal value specified for extended buffering" );
         extendedBuffering = false;
      }

      // if there is an extended buffer then configure it!
      if( extendedBuffering )
      {
         logText( 0, "...enabling extended buffering..." );
         bitsToMaskOut |= (1<<3) | (1<<5);                     // clear bit 3, 5
         bitsToMaskIn  |= (1<<4) | (1<<6) | (1<<7);            // set bits 4, 6, 7
      }
      else
      {
         logText( 0, "...disabling extended buffering..." );
         bitsToMaskOut |= (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7);   // clear bits 3 to 7
      }

      logText( 0, IString("...enable read/write timeout at ") + IString(wnd.connectionSettings.timeout) + " seconds..." );
      dcbInfo.usWriteTimeout  = wnd.connectionSettings.timeout * 100;
      dcbInfo.usReadTimeout   = wnd.connectionSettings.timeout * 100;
      dcbInfo.fbTimeout      &= (~bitsToMaskOut);
      dcbInfo.fbTimeout      |=   bitsToMaskIn;

      // send this new "configuration" to the port
      logText( 0, "...setting new DCBINFO values..." );
      logText( 0, "...-> fbCtlHndShake == 0b"+ IString(dcbInfo.fbCtlHndShake).c2b() );
      logText( 0, "...-> fbFlowReplace == 0b"+ IString(dcbInfo.fbFlowReplace).c2b() );
      logText( 0, "...-> fbTimeout == 0b"    + IString(dcbInfo.fbTimeout).    c2b() );
      rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_SETDCBINFO     ,  // device-specific function
                        (PVOID)&dcbInfo      ,  // pointer to parameter buffer
                        (ULONG)dcbInfoLen    ,  // length of parameter buffer
                        (PULONG)&dcbInfoLen  ,  // pointer to length of parameter buffer
                        (PVOID)0             ,  // pointer to data buffer
                        (ULONG)0             ,  // max length of data buffer
                        (PULONG)0            ); // pointer to length of data buffer
   }

   if( ! rc )
   {
      logText( 0, "...getting DCBINFO (again) for verification..." );
      rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_GETDCBINFO     ,  // device-specific function
                        (PVOID)0             ,  // pointer to parameter buffer
                        (ULONG)0             ,  // length of parameter buffer
                        (PULONG)0            ,  // pointer to length of parameter buffer
                        (PVOID)&dcbInfo      ,  // pointer to data buffer
                        (ULONG)dcbInfoLen    ,  // max length of data buffer
                        (PULONG)&dcbInfoLen  ); // pointer to length of data buffer
      if( ! rc )
      {
         logText( 0, "...-> fbCtlHndShake == 0b"+ IString(dcbInfo.fbCtlHndShake).c2b() );
         logText( 0, "...-> fbFlowReplace == 0b"+ IString(dcbInfo.fbFlowReplace).c2b() );
         logText( 0, "...-> fbTimeout == 0b"    + IString(dcbInfo.fbTimeout).    c2b() );
      }
   }

   if( ! rc )
   {
      // set the port to 8/none/1
      logText( 0, "...setting serial line control to 8/none/1..." );
      UCHAR ucParm[3];
      ucParm[0] = 8; // 8==8 data bits
      ucParm[1] = 0; // 0==no parity
      ucParm[2] = 0; // 0==1 stop bit
      ULONG ulParmLen = 3;
      rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_SETLINECTRL    ,  // device-specific function
                        (PVOID)&ucParm       ,  // pointer to parameter buffer
                        (ULONG)ulParmLen     ,  // length of parameter buffer
                        (PULONG)&ulParmLen   ,  // pointer to length of parameter buffer
                        (PVOID)0             ,  // pointer to data buffer
                        (ULONG)0             ,  // max length of data buffer
                        (PULONG)0            ); // pointer to length of data buffer
   }

   if( ! rc )
   {
      // set the port speed to the default value
      if( ! Camera::setBaudRate( defaultSpeed ) )
      {
         rc = 1;
      }
   }

   if( ! rc )
   {
      logText( 0, "...time between commands is "            + IString(wnd.connectionSettings.pauseBetweenCommands) + " msecs" );

      logText( 0, "...directory used to save images is \""  + wnd.connectionSettings.dir + "\"" );

      logText( 0, "...partial transfer retries: "           + IString(wnd.connectionSettings.retryPartialTransfers ? "enabled" : "disabled"));

      // get some information on this port
      logText( 0, "...getting port information:" );

      struct _EXTGETBAUDRATE baudRate;
      ULONG baudRateLen = sizeof(struct _EXTGETBAUDRATE);

      rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_EXTGETBAUDRATE ,  // get extended baud rate
                        (PVOID)0             ,  // pointer to parameter buffer
                        (ULONG)0             ,  // length of parameter buffer
                        (PULONG)0            ,  // pointer to length of parameter buffer
                        (PVOID)&baudRate     ,  // pointer to data buffer
                        (ULONG)baudRateLen   ,  // max length of data buffer
                        (PULONG)&baudRateLen ); // pointer to length of data buffer
      if( ! rc )
      {
         logText( 0, "...current rate setting for this COM port == " + IString(baudRate.currentBaudRate) );
         logText( 0, "...maximum rate supported by this COM port == " + IString(baudRate.maximumBaudRate) );
      }
   }

   // did an error occur anywhere in the above code?
   if( rc )
   {
      DosClose( portHandle ); // in case the port was opened and the error was afterwards
      logText( "Error setting up the serial port " + comPort + ".", "...ERROR: rc==" + IString(rc) );
   }
   else
   {
      // indicate that all is well!
      logText( 0, "serial port opened for connection" );
      poweredUp = false;
      connected = true;
      result = true;
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera::closeConnection
 *
 * close the serial port connection and clean up
 *
 *****************************************************************************/
Boolean Camera::closeConnection()
{
   Boolean result = false;
   APIRET rc;

   logText( "Closing communication port." );
   wnd.cnr.disable();

   // find out if any characters are still in the input queue
   purgeInputQueue();

   if( ! connected )
   {
      // we're not connected!
      logText( 0, "...ERROR: not connected!" );
      // close the port anyway
      DosClose( portHandle );
      rc = 1;
   }
   else
   {
      rc = DosClose( portHandle );
   }

   // did an error occur anywhere in the above code?
   if( rc )
   {
      logText( "Error closing serial port", "...ERROR: rc==" + IString(rc) );
   }
   else
   {
      result = true;
   }

   // indicate that we're done
   cancelRequested = false;
   connected = false;
   wnd.cnr.setRefreshOff().deleteAllObjects().setRefreshOn().refresh().enable();

   return result;
}


/*****************************************************************************
 *
 *    Camera::purgeInputQueue
 *
 * purge all remaining characters from the input queue
 *
 *****************************************************************************/
Boolean Camera::purgeInputQueue() const
{
   Boolean result = true;

   // find out if any characters are still in the input queue
   RXQUEUE queue;
   ULONG queueSize = sizeof( RXQUEUE );
   APIRET rc = DosDevIOCtl(portHandle           ,  // filehandle obtained from DosOpen()
                           IOCTL_ASYNC          ,  // device category
                           ASYNC_GETINQUECOUNT  ,  // device-specific function
                           (PVOID)0             ,  // pointer to parameter buffer
                           (ULONG)0             ,  // length of parameter buffer
                           (PULONG)0            ,  // pointer to length of parameter buffer
                           (PVOID)&queue        ,  // pointer to data buffer
                           (ULONG)queueSize     ,  // max length of data buffer
                           (PULONG)&queueSize   ); // pointer to length of data buffer
   if( rc )
   {
      logText( 0, "...ERROR: could not get the number of characters in the input queue" );
   }
   else if( queue.cch )
   {
      logText( 0, "purging " + IString(queue.cch) + " characters in the input queue:" );
      Boolean doneLoop = true;
      int chars_to_purge_at_once = 16;
      for( int i=0; doneLoop && i<queue.cch; i+=chars_to_purge_at_once )
      {
         CHAR buffer[16];
         doneLoop = readBuffer( buffer, (chars_to_purge_at_once>queue.cch-i?queue.cch-i:chars_to_purge_at_once) );
      }
   }
   else
   {
      logText( 0, "no characters to purge from the input queue" );
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera::openCard
 *
 * mark the memory card as having been opened
 *
 *****************************************************************************/
Boolean Camera::openCard()
{
   cardOpened = true;

   return true;
}


/*****************************************************************************
 *
 *    Camera::closeCard
 *
 * mark the memory card as having been closed
 *
 *****************************************************************************/
Boolean Camera::closeCard()
{
   cardOpened = false;

   return true;
}


/*****************************************************************************
 *
 *    Camera::disconnect
 *
 * perform one last action before the serial port is closed
 *
 *****************************************************************************/
Boolean Camera::disconnect() const
{
   return true;
}


/*****************************************************************************
 *
 *    Camera::powerUp
 *
 * send the "power up" command to the camera
 *
 *****************************************************************************/
Boolean Camera::powerUp()
{
   poweredUp = true;
   return true;
}


/*****************************************************************************
 *
 *    Camera::powerDown
 *
 * send the "power down" command to the camera
 *
 *****************************************************************************/
Boolean Camera::powerDown()
{
   poweredUp = false;
   return true;
}


/*****************************************************************************
 *
 *    Camera::requiresPowerUpDown
 *
 * determines if the camera requires the power up and power down command
 *
 *****************************************************************************/
Boolean Camera::requiresPowerUpDown() const
{
   return false;
}


/*****************************************************************************
 *
 *    Camera::readBuffer
 *
 * reads the specified number of bytes from the camera
 *
 *****************************************************************************/
Boolean Camera::readBuffer( CHAR *buffer, const ULONG size ) const
{
   Boolean result = false;

   if( wnd.debugWindow )               // in-depth debug information
   {                                   // ...
      logText( 0, "...reading " + IString(size) + " bytes" );
   }                                   // end of in-depth debug information

   APIRET rc;
   ULONG bytes_read_so_far = 0;
   ULONG bytes_read_this_time = 0;

   do
   {
      // reset "bytes read this time" since I wouldn't be surprised to
      // find out that some serial port drivers don't set the variable
      // to 0 when an error has taken place
      bytes_read_this_time = 0;

      rc = DosRead( portHandle, (PVOID)&(buffer[bytes_read_so_far]), size - bytes_read_so_far, &bytes_read_this_time );
      if( rc )
      {
         logText( 0, "...WARNING: DosRead() returned rc==" + IString(rc) );
      }

      if( wnd.debugWindow )               // in-depth debug information
      {                                   // ...
         IString debug( ">>" );           // ...
         for( int i=0; i<bytes_read_this_time; i++ )
         {                                // ...
            debug += " 0x" + IString((int)(buffer[i+bytes_read_so_far])).d2x();
         }                                // ...
         logText( 0, debug );             // ...
      }                                   // end of in-depth debug information

      // remember how many bytes we've read in all
      bytes_read_so_far += bytes_read_this_time;

   } while( (bytes_read_so_far < size) &&                      // while there are more bytes to read...
            (bytes_read_this_time > 0) &&                      // and at least 1 byte was read...
            (wnd.connectionSettings.retryPartialTransfers) &&  // and the retry is enabled...
            (rc == 0) );                                       // and there are no errors

   // even with the retries (if enabled), did we read what we had to read?
   if( bytes_read_so_far != size )
   {
      logText( 0, "...WARNING: only " + IString(bytes_read_so_far) + " bytes of a " + IString(size) + "-byte packet were read" );
   }
   else
   {
      // buffer was read correctly...or at least the right number of bytes was read!
      result = true;
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera::writeBuffer
 *
 * write the specified number of bytes to the camera
 *
 *****************************************************************************/
Boolean Camera::writeBuffer( const CHAR *buffer, const ULONG size ) const
{
   Boolean result = false;
   ULONG bytesWritten;

   if( wnd.debugWindow )               // in-depth debug information
   {                                   // ...
      logText( 0, "...writing " + IString(size) + " bytes" );
      IString debug( "<<" );           // ...
      for( int i=0; i<size; i++ )      // ...
      {                                // ...
         debug += " 0x" + IString((int)(buffer[i])).d2x();
      }                                // ...
      logText( 0, debug );             // ...
   }                                   // end of in-depth debug information

   APIRET rc = DosWrite( portHandle, (PVOID)buffer, size, &bytesWritten );
   if( rc )
   {
      logText( 0, "...WARNING: DosWrite() returned rc==" + IString(rc) );
   }

   if( bytesWritten != size )
   {
      logText( 0, "...ERROR: only " + IString(bytesWritten) + " bytes of a " + IString(size) + "-byte packet were written" );
   }
   else
   {
      // buffer was written correctly
      result = true;

      // flush the port to send immediately
      DosResetBuffer( portHandle );
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera::setFileDateTime
 *
 * set the date and time for the file
 *
 *****************************************************************************/
Boolean Camera::setFileDateTime( const CHAR *filename, const ImageObject *obj ) const
{
   // set the date/time to the image file
   FILESTATUS3 fileStatus3;
   if( DosQueryPathInfo((PSZ)filename,
                        (ULONG)FIL_STANDARD,
                        (PVOID)&fileStatus3,
                        (ULONG)sizeof(FILESTATUS3)) == 0 )
   {
      // reset the file creation date/time fields
      fileStatus3.fdateCreation.day       = obj->day;
      fileStatus3.fdateCreation.month     = obj->month;
      fileStatus3.fdateCreation.year      = obj->year-1980;
      fileStatus3.ftimeCreation.twosecs   = obj->secs/2;
      fileStatus3.ftimeCreation.minutes   = obj->mins;
      fileStatus3.ftimeCreation.hours     = obj->hour;
      // reset the file last access date/time fields
      fileStatus3.fdateLastAccess.day     = obj->day;
      fileStatus3.fdateLastAccess.month   = obj->month;
      fileStatus3.fdateLastAccess.year    = obj->year-1980;
      fileStatus3.ftimeLastAccess.twosecs = obj->secs/2;
      fileStatus3.ftimeLastAccess.minutes = obj->mins;
      fileStatus3.ftimeLastAccess.hours   = obj->hour;
      // reset the file last write date/time fields
      fileStatus3.fdateLastWrite.day      = obj->day;
      fileStatus3.fdateLastWrite.month    = obj->month;
      fileStatus3.fdateLastWrite.year     = obj->year-1980;
      fileStatus3.ftimeLastWrite.twosecs  = obj->secs/2;
      fileStatus3.ftimeLastWrite.minutes  = obj->mins;
      fileStatus3.ftimeLastWrite.hours    = obj->hour;

      // write this information
      DosSetPathInfo((PSZ)filename,
                     (ULONG)FIL_STANDARD,
                     (PVOID)&fileStatus3,
                     (ULONG)sizeof(FILESTATUS3),
                     (ULONG)0 );
   }
   return true;
}


/*****************************************************************************
 *
 *    Camera::setBaudRate
 *
 * set the port baud rate
 *
 *****************************************************************************/
Boolean Camera::setBaudRate( ULONG baud )
{
   Boolean result = false;
   APIRET rc;

   // this method assumes that derived setBaudRate() method has
   // has already sent the new baud rate request to the camera

   switch( baud )
   {
      case BAUD_230400:
      case BAUD_115200:
      case BAUD_57600:
      case BAUD_38400:
      case BAUD_19200:
      case BAUD_9600:
      {
         // the specified rate is valid...everything is ok
         break;
      }
      default:
      {
         // invalid rate specified, so default to 9600
         logText( 0, "...ignoring invalid baud rate of " + IString(baud) + "; using 9600" );
         baud = BAUD_9600;
         break;
      }
   }

   // ok! change the baud rate!
   logText( 0, "...setting serial port speed to " + IString(baud) );

   #pragma pack(1)
   struct _EXTSETBAUDRATE
   {
      ULONG bitRate;
      UCHAR fraction;
   };
   #pragma pack()

   struct _EXTSETBAUDRATE baudRate;

   ULONG baudRateLen = sizeof(struct _EXTSETBAUDRATE);
   baudRate.bitRate = baud;
   baudRate.fraction = '\0';

   rc = DosDevIOCtl( portHandle           ,  // filehandle obtained from DosOpen()
                     IOCTL_ASYNC          ,  // device category
                     ASYNC_EXTSETBAUDRATE ,  // set extended baud rate
                     (PVOID)&baudRate     ,  // pointer to parameter buffer
                     (ULONG)baudRateLen   ,  // length of parameter buffer
                     (PULONG)&baudRateLen ,  // pointer to length of parameter buffer
                     (PVOID)0             ,  // pointer to data buffer
                     (ULONG)0             ,  // max length of data buffer
                     (PULONG)0            ); // pointer to length of data buffer
   if( rc )
   {
      logText( 0, "...ERROR: DosDevIOCtl(ASYNC_EXTSETBAUDRATE) returned rc==" + IString(rc) );
   }
   else
   {
      currentBaudRate = baud;

      // after a change of baud rate, the camera may require a delay
      if( wnd.debugWindow )
      {
         logText( 0, "...port baud rate has been set" );
         logText( 0, "...pausing for " + IString(wnd.connectionSettings.pauseAfterBaudChange) + " msecs" );
      }
      IThread::current().sleep( wnd.connectionSettings.pauseAfterBaudChange );

      result = true;
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera::readResponse
 *
 * read a 1-byte response from the camera
 *
 *****************************************************************************/
Boolean Camera::readResponse( const CHAR response, CHAR *reply ) const
{
   Boolean result = false;

   if( reply )
   {
      reply[0] = (CHAR)0;
   }

   logText( 0, "wait for a 0x" + IString((int)response).d2x() + " response from the camera..." );

   CHAR buffer;
   if( Camera::readBuffer( &buffer, 1 ) )
   {
      // something was read...

      // save what was sent back if it is needed
      if( reply )
      {
         reply[0] = buffer;
      }

      // is it what we expected?
      if( buffer!=response )
      {
         logText( 0, "...ERROR: camera replied with 0x" + IString((int)buffer).d2x() );
         result = false;
      }
      else
      {
         result = true;
      }
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera::writeResponse
 *
 * send a 1-byte response to the camera
 *
 *****************************************************************************/
Boolean Camera::writeResponse( const CHAR response ) const
{
   logText( 0, "send a 0x" + IString( (int)response ).d2x() + " response to the camera..." );

   return Camera::writeBuffer( &response, 1 );
}


/*****************************************************************************
 *
 *    Camera::sendCommand
 *
 * send a 1-byte command to the camera
 *
 *****************************************************************************/
Boolean Camera::sendCommand( const CHAR command ) const
{
   logText( 0, "send a 1-byte command (0x" + IString( (int)command ).d2x() + ") to the camera..." );

   return Camera::writeBuffer( &command, 1 );
}


/*****************************************************************************
 *
 *    Camera::shortFromCamera
 *
 * convert little/big endian (2-byte integer)
 *
 *****************************************************************************/
USHORT Camera::shortFromCamera( const USHORT number ) const
{
   // reverse the high and low bytes
   return (USHORT)(number>>8) | (number<<8);
}


/*****************************************************************************
 *
 *    Camera::longFromCamera
 *
 * convert little/big endian (4-byte integer)
 *
 *****************************************************************************/
ULONG Camera::longFromCamera( const ULONG number ) const
{
   // reverse all bytes (from 4-3-2-1 to 1-2-3-4)
   CHAR buffer[4];
   CHAR *ptr = (CHAR*)&number;
   buffer[3] = *ptr++;
   buffer[2] = *ptr++;
   buffer[1] = *ptr++;
   buffer[0] = *ptr;
   *ptr-- = buffer[3];
   *ptr-- = buffer[2];
   *ptr-- = buffer[1];
   *ptr   = buffer[0];
   return number;
}


/*****************************************************************************
 *
 *    Camera::sendBreak
 *
 * send a BREAK signal over the serial line for a specific amount of time
 *
 *****************************************************************************/
Boolean Camera::sendBreak( const ULONG duration ) const
{
   if(wnd.connectionSettings.ignoreBreak)
   {
      logText( 0, "-- ignoring request to send BREAK signal --" );
   }
   else
   {
      logText( 0, "<< sending BREAK for " + IString(duration) + " ms..." );

      USHORT data = 0;
      ULONG  dataLength = 0;

      DosDevIOCtl(   portHandle           ,  // filehandle obtained from DosOpen()
                     IOCTL_ASYNC          ,  // device category
                     ASYNC_SETBREAKON     ,  // set extended baud rate
                     (PVOID)0             ,  // pointer to parameter buffer
                     (ULONG)0             ,  // length of parameter buffer
                     (PULONG)0            ,  // pointer to length of parameter buffer
                     (PVOID)&data         ,  // pointer to data buffer
                     (ULONG)sizeof(data)  ,  // max length of data buffer
                     (PULONG)dataLength   ); // pointer to length of data buffer

      logText( 0, "(pause for " + IString(duration) + ")" );
      IThread::current().sleep(duration);

      DosDevIOCtl(   portHandle           ,  // filehandle obtained from DosOpen()
                     IOCTL_ASYNC          ,  // device category
                     ASYNC_SETBREAKOFF    ,  // set extended baud rate
                     (PVOID)0             ,  // pointer to parameter buffer
                     (ULONG)0             ,  // length of parameter buffer
                     (PULONG)0            ,  // pointer to length of parameter buffer
                     (PVOID)&data         ,  // pointer to data buffer
                     (ULONG)sizeof(data)  ,  // max length of data buffer
                     (PULONG)dataLength   ); // pointer to length of data buffer

      // sleep for the same duration, giving the receiving end time to recuperate
      logText( 0, "(pause again for " + IString(duration) + ")" );
      IThread::current().sleep(duration);
   }

   return true;
}

