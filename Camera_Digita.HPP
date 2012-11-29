/*
 * $Id: //devel/DCITU/Camera_Digita.HPP#4 $
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

#ifndef Camera_Digita_HPP
#define Camera_Digita_HPP

#include "Camera.HPP"


class Camera_Digita :
   public Camera
{
   public:

      /* constructor must pass wnd to Camera::Camera()
       */
      Camera_Digita(                      GUIWindow &wnd );

      /* open the connection to the camera; invoke the inherited method by
       * the same name, and if successful, call sendBreak()
       */
      virtual Boolean openConnection(     const ULONG defaultSpeed=BAUD_9600 );

      /* perform one last action just before the serial port is closed
       */
      virtual Boolean disconnect(         VOID ) const;

      /* tell the camera to change baud rate, and if successful, calls
       * Camera::setBaudRate()
       */
      virtual Boolean setBaudRate(        ULONG baud );

      /* send a CDP command to the camera
       */
      Boolean sendCommand(                const USHORT command, const CHAR *data=(CHAR*)0, const USHORT len=0 ) const;

      /* read the specified number of bytes from an incoming data packet
       */
      Boolean readDataPacket(             CHAR * &data, ULONG &datalen, const ULONG &offset = 0, const ULONG &totalSize = 0 ) const;

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

      /* at beacon initialization time, the frame packet size is set
       * to a specific size and agreed upon by both the host and the
       * camera (typically, 1023 bytes)
       */
      USHORT      framePacketSize;

      /* the mhbs (maximum host block size) is the largest number of bytes
       * that the camera can send back to the computer at one time; this
       * number is useful when transferring a file (note that this value
       * will normally be a multiple of framePacketSize)
       */
      ULONG       mhbs;


      #pragma pack(1)   // make certain the struct is byte-aligned
      struct BeaconAcknowledgmentStruct
      {
         BYTE     b1;            // byte 0 (must be '0x5A')
         BYTE     b2;            // byte 1 (must be '0xA5')
         BYTE     interface;     // byte 2 (must be '0x55')
         BYTE     comm;          // byte 3 (communication flag?)
         ULONG    baud;          // byte 4-7 (baud rate)
         USHORT   frameTo;       // byte 8-9 (frame size to camera)
         USHORT   frameFrom;     // byte 10-11 (frame size to computer)
         BYTE     checksum;      // byte 12
      };
      #pragma pack()

      /*
      #pragma pack(1)   // make certain the struct is byte-aligned
      struct PollWithLengthPacketStruct
      {
         USHORT   unused : 3;
         USHORT   reply : 1;
         USHORT   endOfBlock : 1;
         USHORT   beginOfBlock : 1;
         USHORT   length : 10;
      };
      // for some reason, I couldn't get this structure to pack
      // correctly; thus, I've decided to use a USHORT and pack
      // the bits myself!
      #pragma pack()
      #define PWL_BOB (1<<0)     // PollWithLength "beginning of block"
      #define PWL_EOB (1<<1)     // PollWithLength "end of block"
      #define PWL_REPLY (1<<2)   // PollWithLength "reply packet"
      */
      #define PWL_BOB (1<<10)    // PollWithLength "beginning of block"
      #define PWL_EOB (1<<11)    // PollWithLength "end of block"
      #define PWL_REPLY (1<<12)  // PollWithLength "reply packet"

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct BeaconCompletionStruct
      {
         BYTE     result;        // byte 0
         BYTE     comm;          // byte 1 (communication flag?)
         ULONG    baud;          // byte 2-5 (baud rate)
         USHORT   frameTo;       // byte 6-7 (frame size to camera)
         USHORT   frameFrom;     // byte 8-9 (frame size from camera)
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct CDPMessageHeaderStruct
      {
         ULONG    length;        // byte 0-3
         BYTE     version;       // byte 4
         BYTE     reserved[3];   // byte 5-7
         USHORT   command;       // byte 8-9
         USHORT   result;        // byte 10-11
         // ...DATA follows...   // byte 12...
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct FileNameStruct
      {
         ULONG    driveNo;       // byte 0-3 (drive number)
         CHAR     pathName[32];  // byte 4-35 (path)
         CHAR     dosName[16];   // byte 36-51 (filename)
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct GetFileListStruct
      {
         ULONG    listOrder;     // byte 0-3 (0==one file, 1==all ascending, 2==all descending)
         struct FileNameStruct fileName;  // byte 4-...
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct ResFileItemStruct
      {
         ULONG    driveNo;       // byte 0-3 (drive number)
         CHAR     pathName[32];  // byte 4-35 (path)
         CHAR     dosName[16];   // byte 36-51 (filename)
         ULONG    fileLen;       // byte 52-55 (length of file)
         ULONG    fileStatus;    // byte 56-59 (bit flags?)
      };
      #pragma pack()

      #pragma pack(1)   // make certain the struct is byte-aligned
      struct GetFileDataStruct
      {
         struct FileNameStruct filename; // name of the file
         ULONG       dataSelector;  // 0==entire file, 1==thumbnail
         ULONG       offset;        // offset into the file if dataSelector==0
         ULONG       length;        // byte count of data (huh?)
         ULONG       fileSize;      // total size of file
      };
      #pragma pack()

      typedef struct BeaconAcknowledgmentStruct BeaconAck;
      typedef struct PollWithLengthPacketStruct PollWithLen;
      typedef struct BeaconCompletionStruct     BeaconComplete;
      typedef struct CDPMessageHeaderStruct     CDPMessage;
      typedef struct FileNameStruct             FileName;
      typedef struct GetFileListStruct          GetFileList;
      typedef struct ResFileItemStruct          ResFileItem;
      typedef struct GetFileDataStruct          GetFileData;
};

#endif

