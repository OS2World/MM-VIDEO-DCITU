/*
 * $Id: //devel/DCITU/Camera_ProjectO.HPP#4 $
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

#ifndef Camera_ProjectO_HPP
#define Camera_ProjectO_HPP

#include "Camera.HPP"


/* The "Project O" documentation was taken from a series of web pages:
 *    http://www.best.com/~rudnicki/olylex.html and
 *    http://www.average.org/digicam/protocol.html.
 * This prototol is apparently used by a series of digitial cameras,
 * including those from the following list:
 *    Agfa ePhoto,
 *    Epson PhotoPC,
 *    Olympus,
 *    Sanyo VPC and DSC,
 *    Sierra SD640,
 *    Toshiba PDR-2
 */
class Camera_ProjectO :
   public Camera
{
   public:

      /* constructor must pass wnd to Camera::Camera()
       */
      Camera_ProjectO(                    GUIWindow &wnd );

      /* this call is overriden since these cameras default to 19200 instead
       * of the 9600 that is assumed by Camera::openConnection()
       */
      virtual Boolean openConnection(     const ULONG defaultSpeed=BAUD_19200 );

      /* the Project 'O' cameras are "powered up" from their initial "sleep"
       * mode at 19200 baud to whatever speed the user has selected; this
       * has to be done at the start of every command
       */
      virtual Boolean powerUp(            VOID );

      /* the Project 'O' cameras are "powered down" after every command
       * transaction has been completed
       */
      virtual Boolean powerDown(          VOID );

      /* this returns TRUE since the powerUp() and powerDown() methods are
       * required for the Project 'O' type cameras
       */
      virtual Boolean requiresPowerUpDown( VOID ) const;

      /* tell the camera to change baud rate, and if successful, calls
       * Camera::setBaudRate()
       */
      virtual Boolean setBaudRate(        ULONG baud );

      /* read the specified colour thumbnail from the camera and use it as the
       * object's icon in the main window's container
       */
      virtual Boolean readLargeThumbnail( const ULONG number ) const;

      /* read the specified grayscale thumbnail from the camera and use it as
       * the object's icon in the main window's container
       */
      virtual Boolean readSmallThumbnail( const ULONG number ) const;

      /* read the specified image from the camera
       */
      virtual Boolean readImage(          const ULONG number ) const;

      /* delete the specified image from the camera
       */
      virtual Boolean deleteImage(        const ULONG number ) const;

      /* send a command to write a word (4 bytes) to a register; an overloaded
       * USHORT version is provided which converts from USHORT to ULONG;
       * (1b,43,06,00,00,xx,mm,mm,mm,mm,nn,nn -> 06)
       */
      Boolean writeWord(                  const UCHAR wordReg, const USHORT value, const Boolean startup=false ) const;
      Boolean writeWord(                  const UCHAR wordReg, const ULONG value, const Boolean startup=false ) const;

      /* send a command to read a word (4 bytes) from a register; an overloaded
       * USHORT version is provided which converts the USHORT to ULONG;
       * (1b,43,02,00,01,xx -> 03,00,04,00,mm,mm,mm,mm,nn,nn -> 06)
       */
      Boolean readWord(                   const UCHAR wordReg, USHORT &answer ) const;
      Boolean readWord(                   const UCHAR wordReg, ULONG &answer  ) const;

      /* send an action command to the camera (1b,43,03,00,02,xx,00 -> 06 05)
       */
      Boolean sendAction(                 const UCHAR action ) const;

      /* return a string with the camera type name
       */
      virtual CHAR *getCameraTypeName(    VOID ) const;

      /* get the camera-specific information, such as the number of pictures
       * stored in the camera; this is called from the action that calls
       * Camera::openConnection()
       */
      virtual Boolean getCameraInfo(      VOID );

      /* get information on each of the pictures that are stored in the
       * camera; since the "Project O" type cameras don't seem to have
       * any image-specific information, this method will fake the name,
       * date and time for each image
       */
      virtual Boolean getPictureInfo(     VOID ) const;


      // these static system codes are single byte values used to synchronize with the camera
      static const CHAR CMD_INIT;      // initialization byte
      static const CHAR CMD_DONE;      // action complete
      static const CHAR CMD_ACK;       // positive acknowledgement
      static const CHAR CMD_ERROR;     // unable to execute command
      static const CHAR CMD_NAK;       // negative acknowledgement
      static const CHAR CMD_TERM;      // termination byte
};

#endif


