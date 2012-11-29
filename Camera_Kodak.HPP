/*
 * $Id: //devel/DCITU/Camera_Kodak.HPP#4 $
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

#ifndef Camera_Kodak_HPP
#define Camera_Kodak_HPP

#include "Camera.HPP"


class ImageObject;


class Camera_Kodak :
   public Camera
{
   public:

      /* constructor must pass wnd to Camera::Camera()
       */
      Camera_Kodak(                       GUIWindow &wnd );

      /* openConnection will call Camera::openConnection and then initiate
       * a BREAK command if successful
       */
      Boolean openConnection(             const ULONG defaultSpeed=BAUD_9600 );

      /* readBuffer will take read requests and handle all of the supported
       * functions such as checksum verification and timeout detection; this
       * method calls Camera::readBuffer()
       */
      Boolean readBuffer(                 CHAR *buffer, const ULONG size, const ULONG packetSize ) const;

      /* writeBuffer will take a write request and handle all of the supported
       * functions such as checksum verification and timeout detection; this
       * method calls Camera::writeBuffer()
       */
      Boolean writeBuffer(                const CHAR *buffer, const ULONG size ) const;

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

      /* convert from KDC format to JPG or BMP
       */
      Boolean convertFromKDC(             const CHAR *filename, const ULONG size ) const;

      /* delete the specified image from the camera
       */
      virtual Boolean deleteImage(        const ULONG number ) const;

      /* send a command to the camera; the format is specific to Kodak cameras;
       * note that a special case for some strange DC120 commands had to be
       * added since the P1 is sometimes set to other than "0"
       */
      Boolean sendCommand(                const CHAR p0, const CHAR p2=0, const CHAR p3=0, const CHAR p4=0, const CHAR p5=0, const CHAR p6=0, const CHAR specialP1=0 ) const;

      /* open the ATA card; this is called from the action that calls
       * Camera::openConnection()
       */
      virtual Boolean openCard(           VOID );

      /* close the ATA card; this is called from the action that calls
       * Camera::closeConnection()
       */
      virtual Boolean closeCard(          VOID );

      /* return a string with the camera type name
       */
      virtual CHAR *getCameraTypeName(    VOID ) const = 0;

      /* get the camera-specific information, such as the number of pictures
       * stored in the camera; this pure virtual method must be overloaded
       * in classes that inherit from the class Camera_Kodak; this is called
       * from the action that calls Camera::openConnection()
       */
      virtual Boolean getCameraInfo(      VOID ) = 0;

      /* get information on each of the pictures that are stored in the
       * camera; this pure virtual method must be overloaded in classes that
       * inherit from the class Camera_Kodak; this is called from the action
       * that calls Camera::openConnection()
       */
      virtual Boolean getPictureInfo(     VOID ) const = 0;


      // these static system codes are single byte values used to synchronize with the camera
      static const CHAR CMD_COMPLETE;  // camera to host -- after a command is completed
      static const CHAR CMD_ACK;       // camera to host -- after a request has been received
      static const CHAR CMD_CORRECT;   // BOTH directions, if the checksum is ok
      static const CHAR CMD_NAK;       // camera to host -- command was not recognized
      static const CHAR CMD_ERROR;     // camera to host -- when?
      static const CHAR CMD_ILLEGAL;   // BOTH directions, if the checksum is not ok (send again)
      static const CHAR CMD_CANCEL;    // HOST to camera -- sent to cancel outstanding command
      static const CHAR CMD_BUSY;      // camera to host -- try command again in 1 second (DC210 ONLY)

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _FileInfoTable
      {
         BYTE     name[11];            // byte 0-10
         BYTE     attribute;           // byte 11
         USHORT   time;                // byte 12-13 (see following byte breakdown:
                  // [f.e.d.c.b.a.9.8.7.6.5.4.3.2.1.0]
                  //  \_______/ \_________/ \_______/
                  //      |          |          |____ bi-seconds
                  //      |          |_______________ minutes
                  //      |__________________________ hours)
         USHORT   date;                // byte 14-15 (see following byte breakdown:
                  // [f.e.d.c.b.a.9.8.7.6.5.4.3.2.1.0]
                  //  \___________/ \_____/ \_______/
                  //        |          |        |____ day
                  //        |          |_____________ month
                  //        |________________________ years since 1980)
         ULONG    size;                // byte 16-19
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _FilePacket
      {
         BYTE     name[48];            // byte 0-47
         LONG     blockOffset;         // byte 48-51
         LONG     blockRead;           // byte 52-55
         BYTE     res[2];              // byte 56-57 (undocumented)
      };
      #pragma pack()

      typedef struct _FileInfoTable    FILEINFOTABLE;
      typedef struct _FilePacket       FILEPACKET;
};

#endif


