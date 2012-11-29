/*
 * $Id: //devel/DCITU/Camera_Digita.CPP#4 $
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

#include "Camera_Digita.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"
#include <stdio.h>

/*****************************************************************************
 *
 *    Camera_Digita::Camera_Digita
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_Digita::Camera_Digita( GUIWindow &wnd ) :
   Camera( wnd )
{
   framePacketSize = 1023;
   return;
}


/*****************************************************************************
 *
 *    Camera_Digita::getCameraTypeName
 *
 * return the name of the camera
 *
 *****************************************************************************/
CHAR *Camera_Digita::getCameraTypeName() const
{
   return "Digita";
}


/*****************************************************************************
 *
 *    Camera_Digita::openConnection
 *
 * setup the camera connection
 *
 *****************************************************************************/
Boolean Camera_Digita::openConnection( const ULONG defaultSpeed )
{
   logText( 0, "-> portions of code for DIGITA support (and a loaner Kodak DC260) was provided by David LaRue, david.larue@acm.org");

   Boolean result = Camera::openConnection(defaultSpeed);
   if( result )
   {
      result = sendBreak();
      if( result )
      {
         // get the camera information that always follows a BREAK

         // ignore the beacon, and instead just purge those extra characters;
         // this was decided since some extra characters sometimes show up
         // between the break and the beacon (mostly 0x00) which could have
         // been difficult to predict and remove!

         logText( 0, "...purging Digita beacon message..." );
         result = purgeInputQueue();
         if( result )
         {
            BeaconAck beacon;

            beacon.b1         = 0x5A;
            beacon.b2         = 0xA5;
            beacon.interface  = 0x55;
            beacon.comm       = 0x00; // (polled transport mode)
            beacon.baud       = longFromCamera(wnd.connectionSettings.baudRate);
            beacon.frameTo    = shortFromCamera(framePacketSize);
            beacon.frameFrom  = shortFromCamera(framePacketSize);
            beacon.checksum   = '\0';

            // calculate the checksum (add bytes 0 to 11, ignore overflow)
            BYTE *ptr = (BYTE*)&beacon;
            for( int i = 0; i < sizeof(BeaconAck) - 1; i++ )
            {
               beacon.checksum += ptr[i];
            }

            logText( 0, "...sending generic beacon acknowledgement..." );
            result = writeBuffer( (CHAR*)&beacon, sizeof(BeaconAck) );

            if( result )
            {
               // receive the Digita "beacon complete" message
               BeaconComplete beaconComplete;
               result = readBuffer( (CHAR*)&beaconComplete, sizeof(BeaconComplete) );
               if( result )
               {
                  if( beaconComplete.result != 0 )
                  {
                     logText( 0, "ERROR: beacon reply has a result of " + IString((int)beaconComplete.result) );
                  }
                  else if( beaconComplete.frameTo != beaconComplete.frameFrom )
                  {
                     logText( 0, "ERROR: packet sizes (TO and FROM) are not of equal size" );
                  }
                  else
                  {
                     ULONG agreedBaudRate = longFromCamera(beaconComplete.baud);
                     framePacketSize = shortFromCamera(beaconComplete.frameTo);

                     logText( 0, "...beacon negotiation complete; using " + IString(framePacketSize) + "-byte packets at " + IString(agreedBaudRate) );
                     result = Camera::setBaudRate( agreedBaudRate );
                  }
               }
            }

         }
      }
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera::disconnect
 *
 * perform one last action before the serial port is closed
 *
 *****************************************************************************/
Boolean Camera_Digita::disconnect() const
{
   // tell the camera to power off
   return sendCommand(0x1A);
}


/*****************************************************************************
 *
 *    Camera_Digita::sendCommand
 *
 * send a command with optional data to the camera
 *
 *****************************************************************************/
Boolean Camera_Digita::sendCommand( const USHORT command, const CHAR *data, const USHORT len ) const
{
   logText( 0, "sending command 0x" + IString(command).d2x() );

   //PollWithLen pwl;
   // Note: I ran into a bit-packing problem with the PollWithLen struct,
   // so I've decided to use a plain old USHORT and pack the bits myself!
   USHORT pwl = (1<<13) | PWL_BOB | PWL_EOB | ((sizeof(CDPMessage) + len) & 0x03FF);
   pwl = shortFromCamera(pwl);

   Boolean result;

   logText( 0, "- start poll-with-length to send a CDP command" );
   result = writeBuffer( (CHAR*)&pwl, sizeof(USHORT) );

   if( !result )
   {
      logText( 0, "ERROR: failed to initiate a poll-with-length transaction" );
   }
   else
   {
      // camera should tell us "ok"...
      BYTE ack[2];
      result = readBuffer( ack, 2 );
      if( (!result) || (ack[0]!='\0') || (ack[1]!=1) )
      {
         logText( 0, "ERROR: invalid poll-with-length ACK received" );
         result = false;
      }
      else
      {
         // send our command
         CDPMessage cdp;
         cdp.length        = longFromCamera(sizeof(CDPMessage) + len - sizeof(ULONG));
         cdp.version       = '\0';
         cdp.reserved[0]   = '\0';
         cdp.reserved[1]   = '\0';
         cdp.reserved[2]   = '\0';
         cdp.command       = shortFromCamera(command);
         cdp.result        = 0;

         logText( 0, "- camera ready; sending CDP command" );
         result = writeBuffer( (CHAR*)&cdp, sizeof(CDPMessage) );

         // see if we need to send any data parameters
         if( len && result )
         {
            logText( 0, "- sending CDP command data" );
            result = writeBuffer(data, len);
         }

         // send 2 undocumented bytes
         if( result )
         {
            BYTE whatInTheWorldAreTheseTwoBytes[2] = {0};
            result = writeBuffer( whatInTheWorldAreTheseTwoBytes, 2 );
         }

         if( !result )
         {
            logText( 0, "ERROR: failed to send CDP command" );
         }
         else
         {
            // if we got here, then all is OK!
            logText( 0, "- camera received the CDP command" );
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::readDataPacket
 *
 * read an incoming data packet
 *
 *****************************************************************************/
Boolean Camera_Digita::readDataPacket( CHAR * &data, ULONG &datalen, const ULONG &offset, const ULONG &totalSize ) const
{
   Boolean result = false;
   datalen = 0;

   // receive the camera's reply
   logText( 0, "- reading poll-with-length reply" );
   USHORT pwl;
   result = readBuffer( (CHAR*)&pwl, sizeof(USHORT) );
   pwl = shortFromCamera(pwl);

   if( (!result) || ((pwl & PWL_REPLY) == 0) )
   {
      logText( 0, "ERROR: invalid poll-with-length reply received" );
   }
   else
   {
      // if we get here, then everything is OK!
      USHORT packetlen = pwl & 0x03FF;

      // send an ACK so the camera can start sending information
      logText( 0, "- sending ACK");
      BYTE ack[2];
      ack[0] = 0;
      ack[1] = 1;
      result = writeBuffer( (CHAR*)ack, 2 );

      if( result )
      {
         CDPMessage echo;
         logText( 0, "- reading CDP echo" );
         result = readBuffer( (CHAR*)&echo, sizeof(CDPMessage) );

         if( !result || (echo.result != 0) )
         {
            logText( 0, "ERROR: invalid CDP echo or result code" );
         }
         else
         {
            // since we've just read 12 bytes as the echo, reduce by 12
            // the number of bytes left to read for this packet!
            packetlen -= 12;

            datalen = longFromCamera(echo.length) - 8;
            logText( 0, "- get ready to read " + IString(datalen) + " bytes of data" );

            ULONG totalBytesRead = 0;
            delete [] data;
            data = new CHAR[datalen];
            logText( 0, "- start reading data packet:");

            do
            {
               logText( 0, "- read data packet with len == " + IString(packetlen));
               result = readBuffer( &(data[totalBytesRead]), packetlen );

               if( result )
               {
                  // we've just read a data packet...

                  // ...tally up the number of bytes...
                  totalBytesRead += packetlen;

                  // ...update the slider control...
                  if(totalBytesRead < datalen)
                  {
                     ULONG totalSizeToUse;
                     ULONG soFar = offset + totalBytesRead;

                     if( totalSize > 0 )
                     {
                        totalSizeToUse = totalSize;
                     }
                     else
                     {
                        totalSizeToUse = datalen;
                     }
                     wnd.slideTo( soFar, totalSizeToUse );
                     logText( "Transfered " + IString(soFar) + " of " + IString(totalSizeToUse) + " bytes." );
                  }

                  // ...and if it isn't the EOB...
                  if( (pwl & PWL_EOB) == 0)
                  {
                     // ...then read the PWL (2 bytes) at the end of the packet...
                     result = readBuffer( (CHAR*)&pwl, sizeof(USHORT) );
                     pwl = shortFromCamera(pwl);
                     packetlen = pwl & 0x03FF;

                     // ...and tell the camera we're ready for the next one!
                     if( result )
                     {
                        result = writeBuffer( (CHAR*)ack, 2 );
                     }
                  }
               }
            }
            while( result && (totalBytesRead < datalen) );

            // do we need to reset the slider control?
            if( (totalSize == 0) || ((totalSize > 0) && (offset + totalBytesRead >= totalSize)) )
            {
               // reset the slider control
               wnd.slideTo( 0 );
            }
         }
      }
   }

   // read mystery bytes at the end of the data packet
   if( result )
   {
      CHAR trailer[2];
      result = readBuffer( &(trailer[0]), 2 );
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::readLargeThumbnail
 *
 * read the large 24-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_Digita::readLargeThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText("Retrieving the thumbnail " + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   IString thumbnailFilename;
   if( obj )
   {
         // command 0x42: GetFileData
         GetFileData getFileData = {0};
         getFileData.filename.driveNo = longFromCamera(obj->driveNumber);
         strcpy(getFileData.filename.pathName, obj->path);
         strcpy(getFileData.filename.dosName, obj->nameInCamera);

         getFileData.dataSelector   = longFromCamera(1); // 0==entire file, 1==thumbnail only
         getFileData.length         = longFromCamera(mhbs);
         getFileData.fileSize       = longFromCamera(obj->size.asInt());

         result = sendCommand(0x42, (CHAR*)&getFileData, sizeof(GetFileData));

         if( result )
         {
            CHAR *data = (CHAR*)0;
            ULONG len = 0;
            result = readDataPacket(data, len);

            if( result )
            {
               #pragma pack(1)
               struct ThumbnailDataStruct
               {
                  ULONG    offset;     // (PartialTag)
                  ULONG    length;     // (PartialTag)
                  ULONG    fileSize;   // (PartialTag)
                  ULONG    dataSize;   // (ThumbnailData)
                  ULONG    height;     // (ThumbnailData)
                  ULONG    width;      // (ThumbnailData)
                  ULONG    type;       // (ThumbnailData)
                  CHAR     data;       // (ThumbnailData)
               };

               struct ThumbnailDataStruct *thumbnailData = (struct ThumbnailDataStruct *)&(data[0]);

               logText(0, "thumbnail data size = " + IString(longFromCamera(thumbnailData->dataSize)));
               logText(0, "thumbnail height = " + IString(longFromCamera(thumbnailData->height)));
               logText(0, "thumbnail width = " + IString(longFromCamera(thumbnailData->width)));
               logText(0, "thumbnail type = " + IString(longFromCamera(thumbnailData->type)));

/*               if(   (longFromCamera(thumbnailData->height) != 80) ||
                     (longFromCamera(thumbnailData->width) != 54))
               {
                  // this is a vertical thumbnail -- so reverse the data!
                  for( int y = 0; y < 80; y++ )
                  {
                     for( int x = 0; x < 54; x ++ )
                     {
                     }
                  }
               }
*/

               if(   (longFromCamera(thumbnailData->height) != 54) ||
                     (longFromCamera(thumbnailData->width) != 80) ||
                     (longFromCamera(thumbnailData->type) != 2))
               {
                  logText("ERROR: thumbnail cannot be processed.");
                  result = false;
               }
               else
               {
               // we only display 96x72 bitmaps, but DIGITA cameras
               // send back 80x54 (why??? haven't they heard about
               // something called "aspect ratio"???) so center it
               // in the space that we have available
               CHAR *ptr = &(thumbnailData->data);
               for( int y = 0; y < 54; y++ )
               {
                  for( int x = 0; x < 80; x++ )
                  {
                     // create a 16-bit USHORT with the thumbnail bits
                     USHORT thumbbits = *((USHORT*)&(ptr[(y*80*2)+x*2+0]));

                     // store these thumbnail bits into the object's BITS array
                     // to use as the thumbnail image in the DCITU container
                     obj->bits[((9+53-y)*96*3)+(x+8)*3+0] = 4 * ((thumbbits>>10) & 0x3f);
                     obj->bits[((9+53-y)*96*3)+(x+8)*3+1] = 4 * ((thumbbits>>10) & 0x3f);
                     obj->bits[((9+53-y)*96*3)+(x+8)*3+2] = 4 * ((thumbbits>>10) & 0x3f);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+0] = 8 * ((thumbbits>>0) & 0x1F);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+1] = 8 * ((thumbbits>>0) & 0x1F);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+2] = 8 * ((thumbbits>>0) & 0x1F);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+0] = 8 * ((thumbbits>>5) & 0x1F);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+1] = 8 * ((thumbbits>>5) & 0x1F);
//                     obj->bits[((9+53-y)*96*3)+(x+8)*3+2] = 8 * ((thumbbits>>5) & 0x1F);
                  }
               }
               obj->refresh();
                  }
            }
         delete [] data;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::readSmallThumbnail
 *
 * read the small thumbnail
 *
 *****************************************************************************/
Boolean Camera_Digita::readSmallThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( 0, "WARNING; grayscale thumbnail function not supported" );

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::readImage
 *
 * read the specified image from the camera
 *
 *****************************************************************************/
Boolean Camera_Digita::readImage( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving the image " + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      logText( 0, "opening " + wnd.connectionSettings.dir + obj->name );

      HFILE fileHandle;
      ULONG action;
      APIRET rc;
      // we have the object...now try and open the file locally
      rc = DosOpen(  wnd.connectionSettings.dir+obj->name, // filename
                     &fileHandle             ,  // file handle
                     &action                 ,  // action that DosOpen carried out
                     0                       ,  // new size of file
                     FILE_NORMAL             ,  // new file attributes
                     OPEN_ACTION_CREATE_IF_NEW| // what to do with the file
                     (wnd.connectionSettings.overwriteDuplicates?
                        OPEN_ACTION_REPLACE_IF_EXISTS:OPEN_ACTION_FAIL_IF_EXISTS),
                     OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                     OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                     OPEN_SHARE_DENYWRITE    |  // deny others from write
                     OPEN_ACCESS_WRITEONLY   ,  // open for write
                     (PEAOP2)0               ); // extended attributes
      if( rc )
      {
         logText( "Image file may already exist.", "...ERROR: DosOpen() returned rc==" + IString(rc) );
         // assume that this is actually NOT an error!
         result = true;
      }
      else
      {
         // command 0x42: GetFileData
         GetFileData getFileData = {0};

         getFileData.filename.driveNo = longFromCamera(obj->driveNumber);
         strcpy(getFileData.filename.pathName, obj->path);
         strcpy(getFileData.filename.dosName, obj->nameInCamera);
         getFileData.length =    longFromCamera(mhbs);
         getFileData.fileSize =  longFromCamera(obj->size.asInt());
         ULONG len = 0;

         // loop until we get all the pieces of the file
         do
         {
            // get the file!
            result = sendCommand(0x42, (const CHAR*)&getFileData, sizeof(GetFileData));

            if( result )
            {
               CHAR *data = (CHAR*)0;
               result = readDataPacket(data, len, longFromCamera(getFileData.offset), obj->size.asInt());

               if( result )
               {
                  // write out what we've obtained so far
                  ULONG bytesWritten;

                  // (exclude the first 12 characters; aka PartialTag)
                  len -= 12;

                  // if there are less bytes left in the file than what was returned
                  // to us (the camera automatically pads with null) then we need to
                  // "crop" those trailing characters!
                  if( longFromCamera(getFileData.offset) + len > obj->size.asInt() )
                  {
                     len = obj->size.asInt() - longFromCamera(getFileData.offset);
                  }

                  DosWrite( fileHandle, (PVOID)&(data[12]), len, &bytesWritten );
                  if( bytesWritten != len )
                  {
                     logText( "Unable to write to the file.", "...ERROR: wrote " + IString(bytesWritten) + " of " + IString(len) + " bytes" );
                     result = false;
                  }
               }
               else
               {
                  logText( 0, "ERROR: unable to read data packet!" );
               }

               delete [] data;
            }
            else
            {
               logText( 0, "ERROR: unable to request a portion of the file!" );
            }

            // increment the offset by the mhbs amount
            getFileData.offset = longFromCamera(longFromCamera(getFileData.offset) + len);

         } while( result && (longFromCamera(getFileData.offset) < obj->size.asInt()) );

         // close the file
         DosClose( fileHandle );

         if (result)
         {
            // all went fine during the download

            logText( "Finished transfering file." );
            // mark the image as being transfered by modifying the thumbnail
            obj->addRedLine();

            setFileDateTime( wnd.connectionSettings.dir+obj->name, obj );

            // if the user wants images deleted after transfer...
            if( wnd.connectionSettings.deleteAfterTransfer )
            {
               // ...delete the file
               #if( DEBUG_LOGGING > 0 )
               logText( 0, "...transfer was complete, now delete the file" );
               #endif
               sendMessage( DELETE_IMAGE, number );
            }

            logText( "" );

         }
      }
   }

   if( ! result )
   {
      logText( "Unable to transfer file." );
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::deleteImage
 *
 * delete the specified image from the camera
 *
 *****************************************************************************/
Boolean Camera_Digita::deleteImage( const ULONG number ) const
{
   Boolean result = false;

   logText( "Deleting image #" + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      FileName fileName = {0};
      fileName.driveNo = longFromCamera(obj->driveNumber);
      strcpy(fileName.pathName, obj->path);
      strcpy(fileName.dosName, obj->nameInCamera);

      result = sendCommand(0x43, (const CHAR*)&fileName, sizeof(FileName));

      if( result )
      {
         ULONG len = 0;
         CHAR *data = (CHAR*)0;
         result = readDataPacket(data, len);

         if( result )
         {
            // the image was deleted, so remove it from the container
            wnd.cnr.removeObject( obj );
            delete( obj );
         }

         delete [] data;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::setBaudRate
 *
 * set the baud rate
 *
 *****************************************************************************/
Boolean Camera_Digita::setBaudRate( ULONG baud )
{
   // this camera sets the baud rate in ::openConnection, so
   // this method simply returns "true" and does nothing else
   return true;
}


/*****************************************************************************
 *
 *    Camera_Digita::getCameraInfo
 *
 * get information from the camera (number of images)
 *
 *****************************************************************************/
Boolean Camera_Digita::getCameraInfo()
{
   Boolean result = false;

   /* get a few camera state values -- also ensures we're talking OK! */
   ULONG datalen = 0;
   CHAR *data = (CHAR*)0;

   // command 0x03: GetCameraStatus
   result = sendCommand(0x03);

   if( result )
   {
      result = readDataPacket(data, datalen);

      if( result )
      {
         /* format of the 24 status bytes:
          *    bytes 0 to 7 - system status
          *       byte 00: bit 00 - 07
          *       byte 01: bit 08 - 15
          *       byte 02: bit 16 - 23
          *       byte 03: bit 24 - 32
          *    bytes 8 to 15 - capture status
          *    bytes 16 to 23 - vendor status
          */
         ULONG systemStatus = longFromCamera(*(ULONG*)&(data[0]));

         // bit #26 indicates low batteries
         if( systemStatus & (1<<26) )
         {
            logText( 0, "WARNING: batteries may be low!" );
         }
         // bit #21 indicates a camera error
         if( systemStatus & (1<<21) )
         {
            logText( 0, "WARNING: camera error flag is set!" );
         }
         // bit #16 indicates an external power source
         if( systemStatus & (1<<16) )
         {
            logText( 0, "NOTE: external power source being used" );
         }
      }
      delete [] data;
      data = (CHAR*)0;
   }

   if( result )
   {
      /* set the timeout to longer than the default 10 seconds */

      #pragma pack(1)
      struct TwoIntStruct
      {
         ULONG hostPollTimeOut;
         ULONG CmdRspTimeOut;
      };
      #pragma pack()

      struct TwoIntStruct twoInt = {0};
      twoInt.hostPollTimeOut = longFromCamera(90);

      result = sendCommand(0x91, (CHAR*)&twoInt, sizeof(struct TwoIntStruct));

      if( result )
      {
         result = readDataPacket(data, datalen);
         delete [] data;
         data = (CHAR*)0;
      }
   }

   if( result )
   {
      /* get the mhbs (maximum host buffer size) */

      // command 0x11: GetCameraState
      logText( 0, "- get the \"mhbs\" (maximum host block size)" );
      result = sendCommand(0x11, "mhbs", 4);
      if( result )
      {
         result = readDataPacket(data, datalen);

         if( result && datalen >= 16 )
         {
            /* camera should have sent back 2 items:
             *
             * 1) a ResLength (should be == 1) (byte 0-3)
             * 2) a PNameTypeValueList (byte 4-...)
             *
             *    byte 00-03  resLength (should be == 1)
             *    byte 04-07  name (should be == "mhbs")
             *    byte 08-11  type (should be == 1)
             *    byte 12-15  value (should be the ULONG value for "mhbs")
             *    byte 16-... padding bytes
             */
            mhbs = longFromCamera(*(ULONG*)&(data[12]));
            logText( 0, "camera is using a \"mhbs\" size of " + IString(mhbs) );
         }
         else
         {
            logText( 0, "ERROR: failed to obtain the \"mhbs\"" );
            result = false;
         }

         delete [] data;
         data = (CHAR*)0;
      }
   }

   if( result )
   {
      /* get the number of images in the camera */

      // command 0x44: GetStorageStatus
      logText( 0, "- get the number of images" );
      result = sendCommand(0x44);
      if( result )
      {
         result = readDataPacket(data, datalen);

         if( result )
         {
            /* format of the 12 storage status bytes:
             *
             *    bytes 0 to 3 - TakenCount
             *    bytes 4 to 7 - AvailableCount
             *    bytes 8 to 11 - RawCount (unsupported?)
             */
            ULONG takenCount =      longFromCamera(*(ULONG*)&(data[0]));
            ULONG availableCount =  longFromCamera(*(ULONG*)&(data[4]));

            logText( 0, "number of images: " + IString(takenCount));
            logText( 0, "images available: " + IString(availableCount));

            // remember how many pictures we're dealing with
            numberOfPicsInCard = takenCount;
         }
         delete [] data;
         data = (CHAR*)0;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Digita::getPictureInfo
 *
 * get information on each image in the camera
 *
 *****************************************************************************/
Boolean Camera_Digita::getPictureInfo() const
{
   Boolean result;

   /* get information on each of the images in the camera
    * parameter format:
    *
    *    ListOrder (ULONG):   0 == return information on a single file
    *                         1 == return all files in ascending order
    *                         2 == return all files in descending order
    *    FileNameStruct:      filename to return (when order==0) or blank
    *                         to return information on all files
    */
   GetFileList getFileList          = {0};
   getFileList.listOrder            = longFromCamera(1);
   getFileList.fileName.driveNo     = longFromCamera(1);
   getFileList.fileName.pathName[0] = '/';

   // command 0x40: GetFileList
   ULONG datalen;
   result = sendCommand(0x40, (CHAR*)&getFileList, sizeof(GetFileList));

   if( result )
   {
      CHAR *names = (CHAR*)0; //new CHAR[datalen];

      result = readDataPacket(names, datalen);

      ResFileItem *resFileItem = (ResFileItem*)&(names[4]);

      // create a container objects for each of the pictures in the camera
      wnd.cnr.setRefreshOff();

      for( int i=0; i<numberOfPicsInCard; i++ )
      {
         wnd.cnr.addObject( new ImageObject( longFromCamera(resFileItem[i].driveNo),
                                             resFileItem[i].pathName,
                                             resFileItem[i].dosName,
                                             longFromCamera(resFileItem[i].fileLen)));
      }

      // all container objects have been added, so re-enable the refresh
      wnd.cnr.setRefreshOn().refresh();

      delete [] names;
   }

   return result;
}

