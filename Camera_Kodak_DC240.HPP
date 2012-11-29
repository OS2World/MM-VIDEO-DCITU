/*
 * $Id: //devel/DCITU/Camera_Kodak_DC240.HPP#4 $
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

#ifndef Camera_Kodak_DC240_HPP
#define Camera_Kodak_DC240_HPP

#include "Camera_Kodak.HPP"

class Camera_Kodak_DC240 :
   public Camera_Kodak
{
   public:

      /* constructor must pass wnd to Camera_Kodak::Camera_Kodak()
       */
      Camera_Kodak_DC240(                 GUIWindow &wnd );

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
      virtual Boolean getPictureInfo(     VOID              ) const;
      virtual Boolean readDirectory(      IString path="", const Boolean lookForFiles=false ) const;

      /* unlike the previous DC210/DC200/DC210 versions, the DC240 has JPEG
       * thumbnails of variable size
       */
      virtual Boolean readLargeThumbnail( const ULONG number ) const;


      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _CameraInfoTable
      {
         BYTE     dataType;            // byte 0 (should == 1)
         BYTE     cameraType;          // byte 1 (==5 for dc240)
         BYTE     firmware_INT;        // byte 2 (0 to 255)
         BYTE     firmware_HEX;        // byte 3 (0 to ff)
         BYTE     romVersion_INT;      // byte 4 (0 to 255)
         BYTE     romVersion_DEC;      // byte 5 (2 decimal places)
         BYTE     romVersion8bit_INT;  // byte 6
         BYTE     romVersion8bit_DEC;  // byte 7
         BYTE     batteryStatus;       // byte 8 (0==full, 1==low, 2==empty)
         BYTE     ACAdapterFlag;       // byte 9 (0==none, 1==in use)
         BYTE     strobeStatus;        // byte 10 (0==not charged, 1==charged)
         BYTE     memoryCardStatus;    // byte 11 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  | \_/ | | \___/
                  //  |  |  | |   |______ reserved
                  //  |  |  | |__________ 0==card not opened, 1==card opened
                  //  |  |  |____________ 0==card formatted, 1==card not formatted
                  //  |  |_______________ reserved
                  //  |__________________ 0==card not available, 1==card available
         BYTE     videoFormat;         // byte 12 (0==NTSC, 1==PAL)
         BYTE     res1;                // byte 13
         USHORT   picturesInCard;      // byte 14-15
         BYTE     volumeID[11];        // byte 16-26
         BYTE     res2;                // byte 27
         BYTE     cameraID[32];        // byte 28-59
         USHORT   remainingLowRes;     // byte 60-61
         USHORT   remainingMidRes;     // byte 62-63
         USHORT   remainingHiRes;      // byte 64-65
         USHORT   totalPictures;       // byte 66-67
         USHORT   totalFlash;          // byte 68-69
         BYTE     res3;                // byte 70
         BYTE     beep;                // byte 71 (0==off, 1==limited, 2==on)
         BYTE     res4[6];             // byte 72-77
         BYTE     fileType;            // byte 78 (3==EXIF)
         BYTE     pictureSize;         // byte 79 (0==640x480, 1==1280x960)
         BYTE     quality;             // byte 80 (1==high, 2==medium, 3==low)
         BYTE     IPChainDisable;      // byte 81 (0==enabled)
         BYTE     imageIncomplete;     // byte 82 (0==complete)
         BYTE     timerMode;           // byte 83 (0==off, 1==on)
         BYTE     res5[4];             // byte 84-87
         USHORT   year;                // byte 88-89
         BYTE     month;               // byte 90
         BYTE     day;                 // byte 91
         BYTE     hour;                // byte 92
         BYTE     minute;              // byte 93
         BYTE     second;              // byte 94
         BYTE     tenMSeconds;         // byte 95 (in 10 msecs)
         BYTE     res6;                // byte 96
         BYTE     flashMode;           // byte 97 (0==auto, 1==fill, 2==off, 3==red-eye)
         USHORT   exposureComp;        // byte 98-99 (x100, -200 to 200)
         BYTE     AEMode;              // byte 100 (0==auto, 1==center)
         BYTE     focusMode;           // byte 101 (0==auto, 2==close-up, 3==infinity)
         BYTE     AFMode;              // byte 102 (2==spot)
         BYTE     AWBMode;             // byte 103 (0==auto, 1==fluorescent, 2==tungsten, 3==daylight/strobe)
         ULONG    zoomMagnification;   // byte 104-107 (100-600)
         BYTE     res7[21];            // byte 108-128
         BYTE     exposureMode;        // byte 129 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  \_____/ \_____/
                  //     |       |_______ 0==normal
                  //     |_______________ 0==TTL-On
         BYTE     res8;                // byte 130
         BYTE     sharpness;           // byte 131 (-1==soft, 0==normal, 1==sharp)
         ULONG    exposureTime;        // byte 132-135 (in 10 uSec)
         USHORT   FValue;              // byte 136-137 (x100, 280-2300)
         BYTE     imageEffect;         // byte 138 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  \_____/ | | | |
                  //     |    | | | |____ grayscale (when == 1)
                  //     |    | | |______ sepia (when ==1)
                  //     |    | |________ document (when ==1)
                  //     |    |__________ border composition (when ==1)
                  //     |_______________ reserved
         BYTE     dateTimeStamp;       // byte 139 (0==off, 1==YYYYMMDD, 2==DDMMYYYY, 3==MMDDYYYY, 4==YYYYMMDDhhmm, 5==DDMMYYhhmm, 6==MMDDYYYYhhmm)
         BYTE     borderFilename[11];  // byte 140-151
         BYTE     exposureLock;        // byte 152 (0==off, 1==on)
         BYTE     res9[256-153];       // byte 153-255
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct _ImageInfoTable
      {
         BYTE     dataType;            // byte 0 (should be == 1)
         BYTE     cameraType;          // byte 1 (3==reserved, 5==dc240)
         BYTE     fileType;            // byte 2 (3==EXIF)
         BYTE     pictureSize;         // byte 3 (0==640x480, 1==1280x960)
         BYTE     imageQuality;        // byte 4 (1==high, 2==medium, 3==low)
         BYTE     IPChainDisable;      // byte 5 (0==enable)
         BYTE     imageIncomplete;     // byte 6 (0==complete)
         BYTE     timerMode;           // byte 7 (0==off, 1==on)
         BYTE     res1[4];             // byte 8-11
         USHORT   year;                // byte 12-13
         BYTE     month;               // byte 14
         BYTE     day;                 // byte 15
         BYTE     hour;                // byte 16
         BYTE     minute;              // byte 17
         BYTE     second;              // byte 18
         BYTE     tenMSeconds;         // byte 19 (in 10 msecs)
         BYTE     flash;               // byte 20 (0==not fired, 1==fired)
         BYTE     flashMode;           // byte 21 (0==auto, 1==fil, 2==off, 3==red-eye)
         USHORT   exposureComp;        // byte 22-23 (x100, -200 to 350)
         BYTE     AEMode;              // byte 24 (0==auto, 1==center)
         BYTE     focusMode;           // byte 25 (0==auto, 2==close-up, 3==infinity)
         BYTE     AFMode;              // byte 26 (2==spot)
         BYTE     AWBMode;             // byte 27 (0==auto, 1==fluorescent, 2==tungsten, 3==daylight/strobe)
         ULONG    zoomMagnification;   // byte 28-31 (100-600)
         BYTE     res2[12];            // byte 32-43
         ULONG    EV;                  // byte 44-47 (x1000, 5000-17000)
         BYTE     res3[4];             // byte 48-51
         BYTE     batteryStatus;       // byte 52 (0==full, 1==low, 2==empty)
         BYTE     exposureMode;        // byte 53 (???)
         BYTE     res4;                // byte 54
         BYTE     sharpness;           // byte 55 (-1==soft, 0==normal, 1==sharp)
         ULONG    exposureTime;        // byte 56-59 (in 10 uSec)
         USHORT   FValue;              // byte 60-61 (x100, 280-2300)
         BYTE     imageEffect;         // byte 62 (see following byte breakdown:
                  // [7.6.5.4.3.2.1.0]
                  //  \_____/ | | | |
                  //     |    | | | |____ grayscale (when == 1)
                  //     |    | | |______ sepia (when ==1)
                  //     |    | |________ document (when ==1)
                  //     |    |__________ border composition (when ==1)
                  //     |_______________ reserved
         BYTE     dateTimeStamp;       // byte 63 (0==off, 1==YYYYMMDD, 2==DDMMYYYY, 3==MMDDYYYY, 4==YYYYMMDDhhmm, 5==DDMMYYhhmm, 6==MMDDYYYYhhmm)
         BYTE     borderFilename[11];  // byte 64-75
         BYTE     exposureLock;        // byte 76 (0==off, 1==on)
         /* something here doesn't match -- the docs say that the member
          * thumbnailDataSize should start at byte 92, but looking at the
          * serial line, it seems to start at byte 93; without trying to
          * figure out exactly where the offset error took place, I'll
          * assume that there are 16 reserved bytes instead of 15 from
          * byte 77 to 92 (instead of byte 77 to 91)
          */
         BYTE     res5[16];            // byte 77-91 (+1)
         ULONG    thumbnailDataSize;   // byte 92-95 (+1)
         USHORT   thumbnailHeight;     // byte 96-97 (+1)
         USHORT   thumbnailWidth;      // byte 98-99 (+1)
         BYTE     protectStatus;       // byte 100 (0==not protected, 1==protected) (+1)
         BYTE     res6[3];             // byte 101-103 (+1)
         ULONG    fileSize;            // byte 104-107 (+1)
         BYTE     res7[256-108-1];     // byte 108-255 (+1)
      };
      #pragma pack()

      typedef struct _CameraInfoTable  CAMERAINFOTABLE;
      typedef struct _ImageInfoTable   IMAGEINFOTABLE;
};

#endif


