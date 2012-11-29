/*
 * $Id: //devel/DCITU/Camera_Kodak_DC25.HPP#4 $
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

#ifndef Camera_Kodak_DC25_HPP
#define Camera_Kodak_DC25_HPP

#include "Camera_Kodak.HPP"

class Camera_Kodak_DC25 :
   public Camera_Kodak
{
   public:

      /* constructor must pass wnd to Camera_Kodak::Camera_Kodak()
       */
      Camera_Kodak_DC25( GUIWindow &wnd );

      /* this call is overriden to reset the line control to 8/even/1
       */
      virtual Boolean openConnection(     const ULONG defaultSpeed=BAUD_9600 );

      /* return a string with the camera type name
       */
      virtual CHAR *getCameraTypeName(    VOID ) const;

      /* get the camera-specific information, such as the number of pictures
       * stored in the camera; this is called from the action that calls
       * Camera::openConnection()
       */
      virtual Boolean getCameraInfo(      VOID );

      /* get information on each of the pictures that are stored in the
       * camera; this is called from the action that calls
       * Camera::openConnection()
       */
      virtual Boolean getPictureInfo(     VOID ) const;

      /* the DC120 does not support the image delete functionality as
       * implemented by the DCITU, so this overload does nothing but
       * simply returns
       */
      virtual Boolean deleteImage(        const ULONG number ) const;

      /* open the ATA card; this is called from the action that calls
       * Camera::openConnection()
       */
      virtual Boolean openCard(           VOID );

      /* close the ATA card; this is called from the action that calls
       * Camera::closeConnection()
       */
      virtual Boolean closeCard(          VOID );

      /* read the large 24-bit thumbnail from the camera
       */
      virtual Boolean readLargeThumbnail( const ULONG number ) const;


      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _CameraInfoTable
      {
         BYTE     dataType;            // byte 0 (should == 1)
         BYTE     cameraType;          // byte 1 (==0x25 for dc25)
         BYTE     unkown1[7];          // byte 2-8
         BYTE     flash;               // byte 9 (0==auto, 1==fill, 2==off)
         BYTE     unknown2;            // byte 10
         BYTE     resolution;          // byte 11 (0==?, 1==? [high/low?])
         BYTE     unknown3[5];         // byte 12-16
         USHORT   takenNormPicsInMem;  // byte 17-18
         USHORT   takenHighPicsInMem;  // byte 19-20
         USHORT   remainHighPicsInMem; // byte 21-22
         USHORT   remainNormPicsInMem; // byte 23-24
         USHORT   takenNormPicsInCard; // byte 25-26
         USHORT   takenHighPicsInCard; // byte 27-28
         BYTE     unknown4[256-29];    // byte 29-255
      };
      #pragma pack()

      typedef struct _CameraInfoTable  CAMERAINFOTABLE;
};

#endif


