/*
 * $Id: //devel/DCITU/Camera_Kodak_DC210.HPP#4 $
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

#ifndef Camera_Kodak_DC210_HPP
#define Camera_Kodak_DC210_HPP

#include "Camera_Kodak.HPP"

class Camera_Kodak_DC210 :
   public Camera_Kodak
{
   public:

      /* constructor must pass wnd to Camera_Kodak::Camera_Kodak()
       */
      Camera_Kodak_DC210(                 GUIWindow &wnd );

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


      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _CameraInfoTable
      {
         BYTE     dataType;            // byte 0 (should == 1)
         BYTE     cameraType;          // byte 1 (==4 for dc210)
         BYTE     firmware_INT;        // byte 2 (0 to 255)
         BYTE     firmware_HEX;        // byte 3 (0 to ff)
         BYTE     romVersion_INT;      // byte 4 (0 to 255)
         BYTE     romVersion_DEC;      // byte 5 (2 decimal places)
         BYTE     romVersion8bit_INT;  // byte 6
         BYTE     romVersion8bit_DEC;  // byte 7
         BYTE     batteryStatus;       // byte 8 (0==full, 1==low, 2==empty)
         BYTE     ACAdapterFlag;       // byte 9 (0==none, 1==in use)
         BYTE     res1[2];             // byte 10-11
         ULONG    time;                // byte 12-15
         BYTE     zoomPosition;        // byte 16 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  \_/ \_/ \_____/
                  //   |   |     |_______ 0==58mm, 1==51mm, 2==41mm, 3==34mm, 4==29mm, 5==macro
                  //   |   |_____________ 0==normal, 1==reserved, 2==close up
                  //   |_________________ reserved)
         BYTE     res2;                // byte 17
         BYTE     flashStatus;         // byte 18 (0==not charged, 1==charged)
         BYTE     compression;         // byte 19 (0==none, 1==low, 2==medium, 3==high)
         BYTE     flashMode;           // byte 20 (0==auto, 1==fill, 2==off, 3==auto red eye, 4==fill red eye)
         BYTE     exposureComp;        // byte 21
         BYTE     pictureSize;         // byte 22 (0==640x480, 1==1152x864)
         BYTE     fileType;            // byte 23 (2==raw, 3==jpeg, 4==flashpix)
         BYTE     IPChainEnable;       // byte 24 (0==disable, 1==enable) [undocumented as of v1.02]
         USHORT   totalPictures;       // byte 25-26
         USHORT   totalFlash;          // byte 27-28
         BYTE     timerMode;           // byte 29 (0==off, 1==on)
         BYTE     memoryCardStatus;    // byte 30 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  | | | | | \___/
                  //  | | | | |   |______ reserved
                  //  | | | | |__________ 0==card closed, 1==card opened
                  //  | | | |____________ 0==card formated, 1==card not formatted
                  //  | | |______________ 0==card supported, 1==illegal card
                  //  | |________________ 0==write protect off, 1==write protect on
                  //  |__________________ 0==card not inserted, 1==card inserted)
         BYTE     videoFormat;         // byte 31 (0==ntsc, 1==pal)
         BYTE     communicationMeans;  // byte 32 (0==serial, 1==irda)
         BYTE     cameraMode;          // byte 33 (2==connect)
         USHORT   res3;                // byte 34-35
         ULONG    irdaBaud;            // byte 36-39
         BYTE     res4[16];            // byte 40-55
         USHORT   picturesInCard;      // byte 56-57
         BYTE     res5[10];            // byte 58-67
         USHORT   remainingLowRes;     // byte 68-69
         USHORT   remainingMidRes;     // byte 70-71
         USHORT   remainingHiRes;      // byte 72-73
         BYTE     res6[3];             // byte 74-76
         BYTE     volumeId[11];        // byte 77-87
         BYTE     res7[2];             // byte 88-89
         BYTE     cameraId[32];        // byte 90-121
         BYTE     res8[256-122];       // byte 122-255
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _ImageInfoTable
      {
         BYTE     dataType;            // byte 0 (should be == 1)
         BYTE     cameraType;          // byte 1 (3==reserved, 4==dc210)
         BYTE     fileType;            // byte 2 (2==raw, 3=jpg, 4=flashpix)
         BYTE     pictureSize;         // byte 3 (0==640x480, 1==1152x864)
         BYTE     compression;         // byte 4 (0==none, 1==low, 2==medium, 3==high)
         BYTE     IPChainEnable;       // byte 5 [undocumented as of v1.02]
         USHORT   number;              // byte 6-7
         ULONG    dataSize;            // byte 8-11
         ULONG    elapsedTime;         // byte 12-15 (in 0.5 second increments since 0:00 1/1/1996)
         BYTE     flashFlag;           // byte 16 (0==no flash, 1==flash)
         BYTE     flashMode;           // byte 17 (0==auto, 1==fill, 2==off, 3==auto red-eye, 4==fill red-eye)
         BYTE     LVData;              // byte 18 (between 0 and 0x54, calculated as:
                  // LV == 6.5 + 0.125 * LVData
                  // i.e., if LVData==0x53, then light value is 16.875)
         BYTE     res1[2];             // byte 19-20
         BYTE     zoomStep;            // byte 21
         BYTE     res2;                // byte 22
         BYTE     EVValue;             // byte 23 [undocumented as of v1.02]
         BYTE     AVValue;             // byte 24 [undocumented as of v1.02]
         BYTE     res3;                // byte 25
         BYTE     apertureValue;       // byte 26 (f-value)
         BYTE     batteryLevel;        // byte 27 (0==ok, 1==weak, 2==empty)
         ULONG    exposureTime;        // byte 28-31
         BYTE     fileName[12];        // byte 32-43
         BYTE     res4[2];             // byte 44-45
         BYTE     exposureComp;        // byte 46
         BYTE     whiteBalance;        // byte 47 (0==auto)
         ULONG    gainRed;             // byte 48-51 [undocumented as of v1.02]
         ULONG    gainGreen;           // byte 52-55 [undocumented as of v1.02]
         ULONG    gainBlue;            // byte 56-59 [undocumented as of v1.02]
         ULONG    calibRed;            // byte 60-63 [undocumented as of v1.02]
         ULONG    calibGreen;          // byte 64-67 [undocumented as of v1.02]
         ULONG    calibBlue;           // byte 68-71 [undocumented as of v1.02]
         BYTE     res5;                // byte 72 (unknown byte -- undocumented)
         BYTE     res6[256-73];        // byte 73-255
      };
      #pragma pack()

      typedef struct _CameraInfoTable  CAMERAINFOTABLE;
      typedef struct _ImageInfoTable   IMAGEINFOTABLE;
};

#endif


