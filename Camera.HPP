/*
 * $Id: //devel/DCITU/Camera.HPP#5 $
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

#ifndef Camera_HPP
#define Camera_HPP


#include <icmdhdr.hpp>  // ICommandHandler
#include <iobjwin.hpp>  // IObjectWindow
#include <ithread.hpp>  // IThreadFn
#include <os2.h>

#include "GUIWindow.H"  // included to access debug information


// class prototype
class GUIWindow;
class ImageObject;
class CameraThread;
class Camera;


class CameraThread :
   public IThreadFn
{
   public:

      /* constructor must pass the 'wnd' to the camera connection
       * that inherits from Camera when the object is created
       */
      CameraThread( GUIWindow &wnd );

      /* destructor; does nothing but display a debug message
       */
      ~CameraThread( VOID );

      /* thread loop -- runs until the camera connection is
       * terminated
       */
      virtual void run( VOID );

      // instance variables
      Camera     *camera;
      GUIWindow  &wnd;
};


class Camera :
   public IObjectWindow,
   protected ICommandHandler
{
   public:

      enum CameraCommand
      {
         GET_SMALL_THUMBNAIL  = 1000,
         GET_LARGE_THUMBNAIL  = 1001,
         GET_IMAGE            = 1002,
         DELETE_IMAGE         = 1003,
         GET_CAMERA_INFO      = 1004,
         GET_IMAGE_INFO       = 1005,
         CONNECT              = 1006,
         DISCONNECT           = 1007,
         SET_BAUD_RATE        = 1008
      };

      /* constructor
       */
      Camera(                             GUIWindow &wnd );

      /* destructor
       */
      virtual ~Camera(                    VOID );

      /* inherited command processor
       */
      virtual Boolean command(            ICommandEvent &event );

      /* open the connection to the camera; this involves opening and
       * correctly configuring the COM port, setting the baud rate,
       * and if successful, will then query the camera for the pictures
       */
      virtual Boolean openConnection(     const ULONG defaultSpeed=BAUD_9600 );

      /* close the COM port
       */
      virtual Boolean closeConnection(    VOID );

      /* purge any remaining characters in the hardware input queue
       */
      virtual Boolean purgeInputQueue(    VOID ) const;

      /* classes that inherit from Camera should use this method to
       * open/initialize the ATA card
       */
      virtual Boolean openCard(           VOID );

      /* classes that inherit from Camera should use this method to
       * close/de-initialize the ATA card
       */
      virtual Boolean closeCard(          VOID );

      /* classes that inherit from Camera should use this method to
       * perform one last action just before the serial port is closed
       */
      virtual Boolean disconnect(         VOID ) const;

      /* classes that inherit from Camera should use this method to send the
       * required powerUp command before each request (i.e., Project O cameras
       * that must send the wakeup call and set the baud rate)
       */
      virtual Boolean powerUp(            VOID );

      /* classes that inherit from Camera should use this method to send the
       * required powerDown command after each request (i.e., Project O cameras
       * that must send the sleep call and set the baud rate)
       */
      virtual Boolean powerDown(          VOID );

      /* classes that inherit from Camera and require the powerUp() and
       * powerDown() method to be called should return true from this method
       */
      virtual Boolean requiresPowerUpDown( VOID ) const;

      /* used to post messages to the camera message queue and increment
       * the counter for the message counter
       */
      void sendMessage(                   const ULONG message, const ULONG id=0 ) const;

      /* read the specified number of bytes from the COM port; classes that
       * inherit from Camera call this method from their overloaded readBuffer
       * methods
       */
      Boolean readBuffer(                 CHAR *buffer, const ULONG size) const;

      /* write the specified number of bytes to the COM port; classes that
       * inherit from Camera call this method from their overloaded writeBuffer
       * methods
       */
      Boolean writeBuffer(                const CHAR *buffer, const ULONG size ) const;

      /* sets the COM port to the specified rate; inherited classes must first
       * take care of telling the camera that the rate will be changed
       */
      virtual Boolean setBaudRate(        ULONG baud );

      /* calls wnd.logText() to log debug information
       */
      virtual VOID logText(               const CHAR *msg1, const CHAR *msg2=0 ) const;

      /* read an expected one-byte response from the camera
       */
      virtual Boolean readResponse(       const CHAR response, CHAR *reply=0 ) const;

      /* write a one-byte response to the camera
       */
      virtual Boolean writeResponse(      const CHAR response ) const;

      /* takes the date & time records from the ImageObject, and asign them to
       * the filename specified
       */
      Boolean setFileDateTime(            const CHAR *filename, const ImageObject *obj ) const;

      /* send a command to the camera
       */
      Boolean sendCommand(                const CHAR command ) const;

      /* reverse the bytes in a USHORT
       */
      USHORT  shortFromCamera(            const USHORT number ) const;

      /* reverse the bytes in a ULONG
       */
      ULONG   longFromCamera(             const ULONG number ) const;

      /* send a BREAK over the serial line
       */
      Boolean sendBreak(                  const ULONG duration = 50 ) const;

      // pure virtual methods

      /* return a string with the camera type name
       */
      virtual CHAR *getCameraTypeName(    VOID ) const = 0;

      /* get camera-specific information
       */
      virtual Boolean getCameraInfo(      VOID ) = 0;

      /* get all of the picture information
       */
      virtual Boolean getPictureInfo(     VOID ) const = 0;

      /* read the specified colour thumbnail
       */
      virtual Boolean readLargeThumbnail( const ULONG number ) const = 0;

      /* read the specified grayscale thumbnail
       */
      virtual Boolean readSmallThumbnail( const ULONG number ) const = 0;

      /* read the specified image file
       */
      virtual Boolean readImage(          const ULONG number ) const = 0;

      /* delete the specified image file
       */
      virtual Boolean deleteImage(        const ULONG number ) const = 0;

      // instance variables
      GUIWindow  &wnd;
      HFILE       portHandle;
      ULONG       currentBaudRate;
      USHORT      numberOfPicsInMem;
      USHORT      numberOfPicsInCard;
      USHORT      cameraType;
      Boolean     poweredUp;
      Boolean     cancelRequested;
      Boolean     cardOpened;
      Boolean     connected;

      // enumerations
      enum baudRates
      {
         BAUD_0               = 0      ,
         BAUD_9600            = 9600   ,
         BAUD_19200           = 19200  ,
         BAUD_38400           = 38400  ,
         BAUD_57600           = 57600  ,
         BAUD_115200          = 115200 ,
         BAUD_230400          = 230400 ,
         NUMBER_OF_BAUD_RATES = 7
      };

      // the following list of camera types must be kept in
      // sync with the static definition of the camera names
      enum cameraTypes
      {
         AGFA_EPHOTO = 0,
       //CASIO_QV10A    ,
         DIGITA         ,
         EPSON_PHOTOPC  ,
       //KODAK_DC20     ,
         KODAK_DC25     ,
       //KODAK_DC50     ,
         KODAK_DC120    ,
         KODAK_DC200    ,
         KODAK_DC210    ,
         KODAK_DC215    ,
         KODAK_DC220    ,
         KODAK_DC240    ,
         KODAK_DC260    ,
         KODAK_DC265    ,
         KODAK_DC280    ,
         MINOLTA_DIMAGE ,
         NIKON_COOLPIX  ,
         OLYMPUS        ,
         SANYO_VPC_DSC  ,
         SIERRA_SD640   ,
         TOSHIBA_PDR_2  ,
         NUMBER_OF_CAMERAS
      };

      // static variables
      static char *cameraNames[NUMBER_OF_CAMERAS];
      static char *baudRateNames[NUMBER_OF_BAUD_RATES];

};

#endif

