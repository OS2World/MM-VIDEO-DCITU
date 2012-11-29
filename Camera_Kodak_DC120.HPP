/*
 * $Id: //devel/DCITU/Camera_Kodak_DC120.HPP#4 $
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

#ifndef Camera_Kodak_DC120_HPP
#define Camera_Kodak_DC120_HPP

#include "Camera_Kodak.HPP"

class Camera_Kodak_DC120 :
   public Camera_Kodak
{
   public:

      /* constructor must pass wnd to Camera_Kodak::Camera_Kodak()
       */
      Camera_Kodak_DC120( GUIWindow &wnd );

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


      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _CameraInfoTable
      {
         BYTE     dataType;            // byte 0 (should == 1)
         BYTE     cameraType;          // byte 1 (==2 for dc120)
         BYTE     firmware_INT;        // byte 2 (0 to 255)
         BYTE     firmware_DEC;        // byte 3 (0 to 99)
         BYTE     romVersion_INT;      // byte 4 (0 to 255)
         BYTE     romVersion_DEC;      // byte 5 (0 to 99)
         BYTE     romVersion8bit_INT;  // byte 6
         BYTE     romVersion8bit_DEC;  // byte 7
         BYTE     batteryStatus;       // byte 8 (0==ok, 1==weak, 3==empty)
         BYTE     ACAdapterFlag;       // byte 9 (0==none, 1==in use)
         BYTE     CaptureSleepTime;    // byte 10 (60 to 255 seconds)
         BYTE     ReviewSleepTime;     // byte 11 (60 to 255 seconds)
         ULONG    time;                // byte 12-15
         BYTE     zoomPosition;        // byte 16 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  \_/ \_/ \_____/
                  //   |   |     |_______ 0==37mm, 1==46mm, 2==61mm, 3==77mm, 4==92mm, 5==100mm, 6=111
                  //   |   |_____________ 0==multi-spot, 1==single spot, 2==close up
                  //   |_________________ reserved)
         BYTE     res1;                // byte 17
         BYTE     flashStatus;         // byte 18 (0==not charged, 1==charged)
         BYTE     compression;         // byte 19 (0==none, 1=best, 2=better, 3=good)
         BYTE     flashMode;           // byte 20 (0==auto, 1==fill-in, 2==off)
         BYTE     exposureComp;        // byte 21 (bit 7==sign bit, bit 0-6 is value between 0 and 15)
         BYTE     LVValue;             // byte 22 (see kodak document)
         BYTE     manualExposure;      // byte 23 (0==off, 1==manual)
         ULONG    manualExposureTime;  // byte 24-27 (32 bit value in 10 micro second increments)
         BYTE     res2;                // byte 28
         BYTE     shutterDelayFlag;    // byte 29 (0==off, 1==on)
         BYTE     memoryCardStatus;    // byte 30 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  | | | | | \___/
                  //  | | | | |   |______ reserved
                  //  | | | | |__________ 0==card closed, 1==card opened
                  //  | | | |____________ 0==card formated, 1==card not formatted
                  //  | | |______________ 0==card supported, 1==illegal card
                  //  | |________________ reserved
                  //  |__________________ 0==card not inserted, 1==card inserted)
         BYTE     frontCoverStatus;    // byte 31 (0==close, 1==open)
         BYTE     dateFormat;          // byte 32 (0==d/m/y, 1==m/d/y, 2==y/m/d)
         BYTE     timeFormat;          // byte 33 (0==12h, 1==24h)
         BYTE     distanceFormat;      // byte 34 (0==centi, 1==inch)
         BYTE     res3;                // byte 35
         USHORT   picturesInMemory;    // byte 36-37
         BYTE     res4[8];             // byte 38-45
         USHORT   remainNoCompMem;     // byte 46-47
         USHORT   remainBestMem;       // byte 48-49
         USHORT   remainBetterMem;     // byte 50-51
         USHORT   remainGoodMem;       // byte 52-53
         BYTE     res5[2];             // byte 54-55
         USHORT   picturesInCard;      // byte 56-57
         BYTE     res6[8];             // byte 58-65
         USHORT   remainNoCompCard;    // byte 66-67
         USHORT   remainBestCard;      // byte 68-69
         USHORT   remainBetterCard;    // byte 70-71
         USHORT   remainGoodCard;      // byte 72-73
         BYTE     res7[3];             // byte 74-76
         BYTE     volumeId[11];        // byte 77-87
         BYTE     res8[2];             // byte 88-89
         BYTE     cameraId[32];        // byte 90-121
         USHORT   albumsInMemory;      // byte 122-123
         USHORT   albumsInCard;        // byte 124-125
         BYTE     res9[256-126];       // byte 126-255
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _ImageInfoTable
      {
         BYTE     dataType;            // byte 0 (should == 1)
         BYTE     pictureType;         // byte 1 (==2 for CDI JPEG T.B.R.)
         BYTE     res1[2];             // byte 2-3
         BYTE     imageQuality;        // byte 4 (0==no comp, 1==best, 2==better, 3==good)
         BYTE     res2[3];             // byte 5-7
         ULONG    dataSize;            // byte 8-11
         ULONG    elapsedTime;         // byte 12-15 (in 0.5 increments since 0:00 1/1/1994)
         BYTE     flashFlag;           // byte 16
         BYTE     flashMode;           // byte 17
         BYTE     LVData;              // byte 18 (between 0 and 0x54, calculated as:
                  // LV == 6.5 + 0.125 * LVData
                  // i.e., if LVData==0x53, then light value is 16.875)
         BYTE     AFStep;              // byte 19 (distance between camera and object:
                  // 0x00-0x77 == 8.2m
                  // 0x78      == 6.7m
                  //      ...
                  // 0xD9      == 0.7m
                  // 0xDA-0xFF == 0.7m)
         BYTE     AFMode;              // byte 20
         BYTE     zoomStep;            // byte 21
         BYTE     imageIncompleteFlag; // byte 22 (0==ok, 1=ng)
         BYTE     res3;                // byte 23
         BYTE     EVValue;             // byte 24 [reserved, should be between 0 and 0x5d]
         BYTE     AVValue;             // byte 25 [reserved, should be between 0 and 0x41]
         BYTE     res4;                // byte 26
         BYTE     timerMode;           // byte 27 (0==off, 1==on)
         ULONG    exposureTime;        // byte 28-31 (32-bit value in 10 microsecond increments)
         BYTE     res5;                // byte 32
         BYTE     aperture;            // byte 33 (f-number)
         BYTE     batteryLevel;        // byte 34 (0==ok, 1==weak, 2==empty)
         BYTE     manualExposure;      // byte 35 (0==off, 1==on)
         BYTE     res6;                // byte 36
         BYTE     fileName[8];         // byte 37-44 (first 8 chars of the filename)
         BYTE     res7[3];             // byte 45-47
         ULONG    AWbGainRed;          // byte 48-51
         ULONG    AWbGainGreen;        // byte 52-55
         ULONG    AWbGainBlue;         // byte 56-59
         ULONG    FWbGainRed;          // byte 60-63
         ULONG    FWbGainGreen;        // byte 64-67
         ULONG    FWbGainBlue;         // byte 68-71
         ULONG    TWbGainRed;          // byte 72-75
         ULONG    TWbGainGreen;        // byte 76-79
         ULONG    TWbGainBlue;         // byte 80-83
         ULONG    DWbGainRed;          // byte 84-87
         ULONG    DWbGainGreen;        // byte 88-91
         ULONG    DWbGainBlue;         // byte 92-95
         ULONG    calibRed;            // byte 96-99
         ULONG    calibGreen;          // byte 100-103
         ULONG    calibBlue;           // byte 104-107
         BYTE     res8[256-108];       // byte 108-255
      };
      #pragma pack()

      typedef struct _CameraInfoTable  CAMERAINFOTABLE;
      typedef struct _ImageInfoTable   IMAGEINFOTABLE;
};

#endif


