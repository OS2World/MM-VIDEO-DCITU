/*
 * $Id: //devel/DCITU/Camera_Kodak.CPP#4 $
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

#include "Camera_Kodak.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"
#include <os2.h>


// these static system codes are single byte values used to synchronize with the camera
const CHAR Camera_Kodak::CMD_COMPLETE= 0x00; // camera to host -- after a command is completed
const CHAR Camera_Kodak::CMD_ACK     = 0xd1; // camera to host -- after a request has been received
const CHAR Camera_Kodak::CMD_CORRECT = 0xd2; // BOTH directions, if the checksum is ok
const CHAR Camera_Kodak::CMD_NAK     = 0xe1; // camera to host -- command was not recognized
const CHAR Camera_Kodak::CMD_ERROR   = 0xe2; // camera to host -- when?
const CHAR Camera_Kodak::CMD_ILLEGAL = 0xe3; // BOTH directions, if the checksum is not ok (send again)
const CHAR Camera_Kodak::CMD_CANCEL  = 0xe4; // HOST to camera -- sent to cancel outstanding command
const CHAR Camera_Kodak::CMD_BUSY    = 0xf0; // camera to host -- try command again in 1 second (DC210 ONLY)



/*****************************************************************************
 *
 *    Camera_Kodak::Camera_Kodak
 *
 * call the constructor for the class from which we inherited
 *
 *****************************************************************************/
Camera_Kodak::Camera_Kodak( GUIWindow &wnd ) :
   Camera( wnd )
{
   return;
}



/*****************************************************************************
 *
 *    Camera_Kodak::openConnection
 *
 * setup the camera connection
 *
 *****************************************************************************/
Boolean Camera_Kodak::openConnection( const ULONG defaultSpeed )
{
   Boolean result = Camera::openConnection(defaultSpeed);
   if( result )
   {
      result = sendBreak(300);
      // give the cameras time to recuperate after the break
      IThread::current().sleep(500 + wnd.connectionSettings.pauseAfterBaudChange);
      purgeInputQueue();
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::readBuffer
 *
 * read a number of bytes from the camera to a memory buffer
 *
 * NOTE: packets from the camera to the host are as follows:
 *
 *    [ packet control byte      ]  <== ONLY FOR THE DC210, and must be == 1!
 *    [ data...variable length   ]
 *    [ checksum (xor all data)  ]
 *
 * thus, the DC25, DC50 and DC120 have packets that are 1 byte less than the DC210
 *
 *****************************************************************************/
Boolean Camera_Kodak::readBuffer( CHAR *buffer, const ULONG size, const ULONG packetSize ) const
{
   Boolean result = false;

   logText( 0, "reading " + IString(size) + " bytes in " + IString(packetSize) + "-byte packets..." );

   // check the packet size
   if( ( cameraType==KODAK_DC240 ||
         cameraType==KODAK_DC280 ) &&
       ( packetSize==18    ||
         packetSize==258   ||
         packetSize==514   ) )
   {
      result = true;
   }
   else
   if( ( cameraType==KODAK_DC200 ||
         cameraType==KODAK_DC210 ||
         cameraType==KODAK_DC215 ) &&
       ( packetSize==18    ||
         packetSize==258   ||
         packetSize==514   ||
         packetSize==1026  ) )
   {
      result = true;
   }
   else
   if( ( cameraType==KODAK_DC120 ||
       //cameraType==KODAK_DC50  ||
         cameraType==KODAK_DC25  ) &&
       ( packetSize==17    ||
         packetSize==257   ||
         packetSize==513   ||
         packetSize==1025  ) )
   {
      result = true;
   }
   /*else
   if( ( cameraType==KODAK_DC50 ) &&
       ( packetSize==256 ) )
   {
      result = true;
   }*/

   // was an illegal packet size specified
   if( ! result )
   {
      logText( 0, "ERROR: illegal packet size specified" );
   }
   else
   {
      // create the buffer that will be used to read each packet
      CHAR *packetBuffer = new CHAR[packetSize];
      CHAR *packetData;
      ULONG packetDataSize;

      if(  cameraType==KODAK_DC200 ||
           cameraType==KODAK_DC210 ||
           cameraType==KODAK_DC215 ||
           cameraType==KODAK_DC240 ||
           cameraType==KODAK_DC280 )
      {
         // on a DC210 and DC240, the data starts at byte 1,
         // not byte 0 because of the packet control byte
         packetData = &(packetBuffer[1]);
         packetDataSize = packetSize - 2;
      }
      else
      {
         // on the DC25, DC50 and DC120, the data starts at byte 0
         packetData = &(packetBuffer[0]);
         packetDataSize = packetSize - 1;
      }

      // keep an index into the buffer that was specified
      ULONG bufferOffset = 0;

      // loop until an error occurs, or until we've read all the bytes to fill the buffer
      while( result && (bufferOffset<size) )
      {
         if( size > packetSize )
         {
            // if multiple reads will be necessary, then update the slider control
            wnd.slideTo( bufferOffset, size );
            logText( "Transfered " + IString(bufferOffset) + " of " + IString(size) + " bytes." );
         }
         logText( 0, "...reading packet" );
         result = Camera::readBuffer( packetBuffer, packetSize );
         if( result )
         {
            // the correct number of bytes were read...now handle the full buffer!

            CHAR reply;

            // if this is a DC210 or DC240, check the packet control byte
            if( cameraType==KODAK_DC200 ||
                cameraType==KODAK_DC210 ||
                cameraType==KODAK_DC215 ||
                cameraType==KODAK_DC240 ||
                cameraType==KODAK_DC280 )
            {
               if( packetBuffer[0] != 0x01 )
               {
                  logText( 0, "...WARNING: packet control byte is 0x" + IString((int)packetBuffer[0]).d2x() + " instead of 0x01" );
               }
            }

            CHAR checksum = 0;
            // verify the checksum for this packet
            for( int i=0; i<packetDataSize; i++ )
            {
               checksum ^= packetData[i];
            }
            if( checksum != packetData[packetDataSize] )
            {
               logText( "...packet failed checksum; requesting again..." );
               purgeInputQueue();
               reply = CMD_ILLEGAL;
            }
            else
            {
               // packet was received correctly!
               reply = CMD_CORRECT;
            }

            // send our reply to the camera
            result = writeResponse( reply );

            // does this packet have information that we want to keep?
            if( reply == CMD_CORRECT )
            {
               // loop until this packet is copied, or until we've read the entire buffer
               for( i = 0; (i<packetDataSize)&&(bufferOffset<size); i ++ )
               {
                  buffer[bufferOffset++] = packetData[i];
               }
            }
         }
      }  // while loop

      delete [] packetBuffer;

      if( size > packetSize )
      {
         // if the slider was used, then reset it to 0
         wnd.slideTo( 0 );
         logText( 0, "...finished reading " + IString(bufferOffset) + " of " + IString(size) );
      }

      // was the buffer read without error?
      if( result )
      {
         // camera should now indicate that the command is complete
         result = readResponse( CMD_COMPLETE );
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::writeBuffer
 *
 * write the specified bytes to the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak::writeBuffer( const CHAR *buffer, const ULONG size ) const
{
   Boolean result = false;

   logText( 0, "writing " + IString(size) + "-byte packet to camera" );
   if( size != 58 )
   {
      logText( 0, "...ERROR: only 58-byte buffers are acceptable" );
   }
   else
   {
      // only 60-byte buffers can be written to the camera; the other
      // buffer sizes are not currently supported by this application
      CHAR packetBuffer[60];

      packetBuffer[0] = 0x80;   // first byte (packet control byte) is hard-coded as 0x80
      packetBuffer[59] = 0;     // last byte (checksum byte) will be defined later

      // copy the buffer & calculate the checksum
      for( int i=0; i<size; i++ )
      {
         packetBuffer[i+1] = buffer[i];  // copy the byte
         packetBuffer[59] ^= buffer[i];  // update the checksum byte
      }

      CHAR reply = CMD_ILLEGAL;
      result = true;

      // loop as long as the camera tells us that the checksum did not match
      while( result && (reply==CMD_ILLEGAL) )
      {
         result = Camera::writeBuffer( packetBuffer, size+2 );

         // if the write was successful...
         if( result )
         {
            // ...then wait for a response
            readResponse( CMD_CORRECT, &reply );

            if( reply==CMD_ILLEGAL )
            {
               logText( 0, "...checksum error; resending packet..." );
            }
         }
      }
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::sendCommand
 *
 * send an 8-byte command to the camera
 *
 * NOTE: commands for the Kodak line of cameras are 8 sequential bytes, with
 *       the command code in byte 0, parameters in byte 2-5, and a terminal
 *       byte with the value 0x1a (bytes 1 and 6 and set to 0)
 *
 *****************************************************************************/
Boolean Camera_Kodak::sendCommand( const CHAR p0, const CHAR p2, const CHAR p3, const CHAR p4, const CHAR p5, const CHAR p6, const CHAR specialP1 ) const
{
   CHAR command[8];
   command[0] = p0;
   command[1] = specialP1; // a few DC120 commands use P1 as a parameter
   command[2] = p2;
   command[3] = p3;
   command[4] = p4;
   command[5] = p5;
   command[6] = p6;
   command[7] = 0x1a;

   logText( 0, "sending command 0x" + IString((int)p0).d2x() );

   // send the command
   Boolean result = Camera::writeBuffer( &(command[0]), 8 );

   // if the send command was successful...
   if( result )
   {
      // ...then wait for ACK to be sent back
      result = readResponse( CMD_ACK );
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::readLargeThumbnail
 *
 * read the large 24-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_Kodak::readLargeThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving colour thumbnail for image " + IString(number) + "." );

   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      if( sendCommand( 0x93, 0, 0, 1 ) )
      {
         FILEPACKET filePacket;
         for( int i=0; i<sizeof(FILEPACKET); i++ )
         {
            ((char*)(&filePacket))[i] = 0;
         }

         IString filename = obj->path+obj->nameInCamera;
         logText( 0, "filename == \"" + filename + "\"" );
         for( i=0; i<filename.length(); i++ )
         {
            filePacket.name[i] = filename[i+1];
         }
         filePacket.blockOffset = -1;
         filePacket.blockRead = -1;

         if( writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) ) )
         {
            CHAR *buffer = new CHAR[96*72*3];

            // read the bytes
            if( readBuffer( &(buffer[0]), 96*72*3, 1026 ) )
            {
               // copy the new thumbnail, reversing the rows top-bottom and mapping RGB:BGR
               for( int y=0; y<72; y++ )
               {
                  for( int x=0; x<(96*3); x+=3 )
                  {
                     // the bytes received are RGB, while the thumbnails are BGR
                     obj->bits[((71-y)*(96*3))+x+0] = buffer[(y*96*3)+x+2];
                     obj->bits[((71-y)*(96*3))+x+1] = buffer[(y*96*3)+x+1];
                     obj->bits[((71-y)*(96*3))+x+2] = buffer[(y*96*3)+x+0];
                  }
               }
               obj->refresh();
               result = true;
            }
            delete [] buffer;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::readSmallThumbnail
 *
 * read the small 4-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_Kodak::readSmallThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving grayscale thumbnail for image " + IString(number) + "." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      if( sendCommand( 0x93 ) )
      {
         FILEPACKET filePacket;
         for( int i=0; i<sizeof(FILEPACKET); i++ )
         {
            ((CHAR*)(&filePacket))[i] = 0;
         }

         IString filename = obj->path+obj->nameInCamera;
         logText( 0, "filename == \"" + filename + "\"" );
         for( i=0; i<filename.length(); i++ )
         {
            filePacket.name[i] = filename[i+1];
         }
         filePacket.blockOffset = -1;
         filePacket.blockRead = -1;

         if( writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) ) )
         {
            CHAR *buffer = new CHAR[96*72/2];

            // read the bytes
            if( readBuffer( &(buffer[0]), 96*72/2, 1026 ) )
            {
               // copy the new thumbnail, reversing the rows top-bottom
               for( int y=0; y<72; y++ )
               {
                  for( int x=0; x<(96/2); x++ )
                  {
                     CHAR low  = buffer[(y*96/2)+x] & 0x0F;
                     CHAR high = buffer[(y*96/2)+x] >> 4;

                     obj->bits[((71-y)*(96*3))+(x*6)+0] = (low*16)+low;
                     obj->bits[((71-y)*(96*3))+(x*6)+1] = (low*16)+low;
                     obj->bits[((71-y)*(96*3))+(x*6)+2] = (low*16)+low;
                     obj->bits[((71-y)*(96*3))+(x*6)+3] = (high*16)+high;
                     obj->bits[((71-y)*(96*3))+(x*6)+4] = (high*16)+high;
                     obj->bits[((71-y)*(96*3))+(x*6)+5] = (high*16)+high;
                  }
               }
               obj->refresh();
               result = true;
            }
            delete [] buffer;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::readImage
 *
 * retrieve an image from the camera
 *
 * NOTE: the DC120 and DC210 cameras specify the images to download using
 *       an internal camera image name, while the DC25 camera specifies
 *       the image using a simple image number starting at offset 1
 *
 *****************************************************************************/
Boolean Camera_Kodak::readImage( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving the image " + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
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
         logText( "Image file already exists.", "...ERROR: DosOpen() returned rc==" + IString(rc) );
         // assume that this is actually NOT an error!
         result = true;
      }
      else
      {
         if( cameraType==KODAK_DC25 )
         {
            // the DC25 camera select the image as follows:
            USHORT number = obj->cameraNumber;

            // the command is different if the image is in memory or on the card
            if( obj->onCard )
            {
               // number for card-based images were incremented by 500 when the
               // container object was created, so decrement by the same amount
               number -= 500;
               result = sendCommand( 0x61, number>>8, number&0xff );
            }
            else
            {
               result = sendCommand( 0x51, number>>8, number&0xff );
            }
         }
         else
         {
            // the DC120, DC210 and DC240 camera select the image as follows:
            if( sendCommand( 0x9a ) )
            {
               FILEPACKET filePacket = {0};
               IString filename(obj->path + obj->nameInCamera);
               logText( 0, "filename == \"" + filename + "\"" );

               for( int i=0; i<filename.length(); i++ )
               {
                  filePacket.name[i] = filename[i+1];
               }
               filePacket.blockOffset = -1;
               filePacket.blockRead = -1;

               result = writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) );
            }
         }

         // if everything is OK, then start downloading the image
         if( result )
         {
            result = false;
            CHAR *buffer = new CHAR[obj->size.asUnsigned()];

            // read the bytes
            int packetSize = 0;
            if(cameraType==KODAK_DC240 ||
               cameraType==KODAK_DC280 )
            {
               packetSize = 514;
            }
            else if( cameraType==KODAK_DC200 ||
                     cameraType==KODAK_DC210 ||
                     cameraType==KODAK_DC215 )
            {
               packetSize = 514;
            }
            else if( cameraType==KODAK_DC120 )
            {
               packetSize = 513;
            }
            else if( cameraType==KODAK_DC25 )
            {
               packetSize = 1025;
            }

            if( readBuffer( &(buffer[0]), obj->size.asUnsigned(), packetSize ) )
            {
               // write the file
               ULONG bytesWritten;
               DosWrite( fileHandle, (PVOID)&(buffer[0]), obj->size.asUnsigned(), &bytesWritten );
               if( bytesWritten != obj->size.asUnsigned() )
               {
                  logText( "Unable to save the file.", "...ERROR: wrote " + IString(bytesWritten) + " of " + IString(obj->size.asUnsigned()) + " bytes" );
               }
               else
               {
                  logText( "Finished transfering file." );
                  // mark the image as being transfered by modifying the thumbnail
                  obj->addRedLine();
                  result = true;
               }
            }
            delete [] buffer;
         }
         DosClose( fileHandle );
         if( result )
         {
            setFileDateTime( wnd.connectionSettings.dir+obj->name, obj );

            if( IString::upperCase(obj->nameInCamera).isLike("*.KDC" ))
            {
               if( wnd.connectionSettings.convertKDCtoJPG )
               {
                  // convert from .KDC to .JPG or .BMP
                  convertFromKDC( wnd.connectionSettings.dir+obj->name, obj->size.asUnsigned() );
               }
            }

            // if the user wants images deleted after transfer...
            if( wnd.connectionSettings.deleteAfterTransfer && (cameraType!=KODAK_DC120 && cameraType!=KODAK_DC25) )
            {
               // ...delete the file
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
 *    Camera_Kodak::convertFromKDC
 *
 * convert .KDC files to .JPG
 *
 *****************************************************************************/
Boolean Camera_Kodak::convertFromKDC( const CHAR *filename, const ULONG size ) const
{
   Boolean result = false;
   IString fname( filename );
   IString filebase = fname.subString(1, fname.length()-4);
   logText( 0, "converting " + fname );

   // most of the following code was adapted from the KDC_FILE utility;
   // only compressed .KDC files are supported here -- uncompressed KDC
   // files still have to be converted using the KDC2BMP.EXE tool

   #define KDC_HEADER_SIZE 15680

   HFILE sourceHandle = (HFILE)0;
   HFILE targetHandle = (HFILE)0;
   CHAR *sourceBuffer = (CHAR*)0;
   ULONG bytesRead;
   ULONG bytesWritten;
   ULONG action;
   APIRET rc;

   // open the source file
   rc = DosOpen(  filebase+IString(".kdc"),  // filename
                  &sourceHandle           ,  // file handle
                  &action                 ,  // action that DosOpen carried out
                  0                       ,  // new size of file
                  FILE_NORMAL             ,  // new file attributes
                  OPEN_ACTION_FAIL_IF_NEW |  // what to do with the file
                  OPEN_ACTION_OPEN_IF_EXISTS,
                  OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                  OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                  OPEN_SHARE_DENYWRITE    |  // deny others from write
                  OPEN_ACCESS_READONLY    ,  // open for write
                  (PEAOP2)0               ); // extended attributes

   if( rc )
   {
      logText( 0, "ERROR: could not open \"" + filebase + ".kdc\"" );
   }
   else
   {
      // file was opened correctly

      sourceBuffer = new CHAR[KDC_HEADER_SIZE];

      // read the header
      DosRead( sourceHandle, sourceBuffer, KDC_HEADER_SIZE, &bytesRead );
      if( bytesRead != KDC_HEADER_SIZE )
      {
         logText( 0, "ERROR: could not read header from source file" );
      }
      else
      {
         // we've read the file header correctly
         result = true;
      }
   }

   //if (strcmp(hdr+470, "Kodak DC120 ZOOM Digital Camera"))
   //{
   //   fprintf(stderr, "Error: not a DC120 .kdc file\n");
   //   return;
   //}

   if( result )
   {
      if( sourceBuffer[707] == 7 )
      {
         // this is a compressed .KDC file -- convert to .JPG

         static CHAR JPGHeader[] = {
                              0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46,
                              0x49, 0x46, 0x00, 0x01, 0x02, 0x01, 0x00, 0x64,
                              0x00, 0x4B, 0x00, 0x00 };

         logText( "Converting .KDC file to .JPG." );

         IThread::current().setPriority( IApplication::regular, 0 );

         // create the new .JPG file
         rc = DosOpen(  filebase+IString(".jpg"),  // filename
                        &targetHandle           ,  // file handle
                        &action                 ,  // action that DosOpen carried out
                        0                       ,  // new size of file
                        FILE_NORMAL             ,  // new file attributes
                        OPEN_ACTION_CREATE_IF_NEW |// what to do with the file
                        OPEN_ACTION_REPLACE_IF_EXISTS,   // ...
                        OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                        OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                        OPEN_SHARE_DENYWRITE    |  // deny others from write
                        OPEN_ACCESS_WRITEONLY   ,  // open for write
                        (PEAOP2)0               ); // extended attributes
         if( rc )
         {
            logText( 0, IString("ERROR: could not open \"")+filebase+IString(".jpg\"") );
            result = false;
         }
         else
         {
            // the .JPG file is open, so start with the standard JPEG header
            DosWrite( targetHandle, JPGHeader, sizeof(JPGHeader), &bytesWritten );
            if( bytesWritten != sizeof(JPGHeader) )
            {
               logText( 0, "ERROR: could not write jpeg header" );
               result = false;
            }
            else
            {
               // write the actual file
               CHAR readBuffer[2];
               CHAR writeBuffer[2];
               CHAR c;
               ULONG counter = 0;

               // skip the next two bytes (included in jpeg header)
               DosRead( sourceHandle, readBuffer, 2, &bytesRead );

               // read-write the jpeg file, reversing every two bytes
               while( bytesRead == 2 )
               {
                  // read two bytes
                  DosRead( sourceHandle, readBuffer, 2, &bytesRead );
                  if( bytesRead == 2 )
                  {
                     // swap the two bytes
                     c = readBuffer[0];
                     writeBuffer[0] = readBuffer[1];
                     writeBuffer[1] = c;

                     // write the two bytes
                     DosWrite( targetHandle, writeBuffer, 2, &bytesWritten );

                     counter +=2;
                     if( counter % 1000 == 0 )
                     {
                        // every 1000 bytes, update the slider
                        wnd.slideTo( counter, size - KDC_HEADER_SIZE );
                     }
                  }

               }
            }
         }

         // reset the thread priority
         IThread::current().setPriority( IApplication::timeCritical, 31 );
      }
      else
      {
         // unrecognized .KDC format
         logText( 0, "ERROR; unrecognized .KDC compression" );
      }
   }

   if( sourceHandle )
   {
      DosClose( sourceHandle );
   }

   if( targetHandle )
   {
      DosClose( targetHandle );
   }

   if( sourceBuffer )
   {
      delete [] sourceBuffer;
   }

   // reset the slider
   wnd.slideTo( 0 );

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::deleteImage
 *
 * delete an image from the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak::deleteImage( const ULONG number ) const
{
   Boolean result = false;

   logText( "Deleting the image " + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      if( sendCommand( 0x9d ) )
      {
         FILEPACKET filePacket = {0};
         IString filename(obj->path + obj->nameInCamera);
         logText( 0, "filename == \"" + filename + "\"" );

         for( int i=0; i<filename.length(); i++ )
         {
            filePacket.name[i] = filename[i+1];
         }
         filePacket.blockOffset = -1;
         filePacket.blockRead = -1;

         if( writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) ) )
         {
            // the camera should reply with CMD_COMPLETE
            if( readResponse(CMD_COMPLETE) )
            {
               wnd.cnr.removeObject( obj );
               delete( obj );
               result = true;
            }
         }
      }
   }
   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::openCard
 *
 * open the ATA memory card and set the appropriate flag
 *
 *****************************************************************************/
Boolean Camera_Kodak::openCard()
{
   Boolean result = true;

   logText( 0, "opening ATA card" );

   // has the option been set to bypass the memory card?
   if( cameraType!=KODAK_DC280 &&
       cameraType!=KODAK_DC240 &&
       cameraType!=KODAK_DC215 &&
       cameraType!=KODAK_DC210 &&
       cameraType!=KODAK_DC200 &&
       wnd.connectionSettings.ignoreMemoryCard )
   {
      logText( 0, "...ignore flag has been set, bypassing ATA card" );
      cardOpened = false;
   }
   else
   {
      result = sendCommand( 0x96 );

      if( result )
      {
         result = readResponse( CMD_COMPLETE );
         if( result )
         {
            cardOpened = true;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::closeCard
 *
 * close the ATA memory card if it has been opened
 *
 *****************************************************************************/
Boolean Camera_Kodak::closeCard()
{
   Boolean result = true;

   logText( 0, "closing ATA card" );

   if( ! cardOpened )
   {
      logText( 0, "...ATA card is not opened, bypassing close command" );
   }
   else
   {
      result = sendCommand( 0x97 );

      if( result )
      {
         result = readResponse( CMD_COMPLETE );
         if( result )
         {
            cardOpened = false;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak::setBaudRate
 *
 * set the communication baud rate
 *
 *****************************************************************************/
Boolean Camera_Kodak::setBaudRate( ULONG baud )
{
   Boolean result = false;

   CHAR p2, p3;

   switch( baud )
   {
      case BAUD_230400:
      {
         p2 = 0x23;
         p3 = 0x04;
         break;
      }
      case BAUD_115200:
      {
         p2 = 0x11;
         p3 = 0x52;
         break;
      }
      case BAUD_57600:
      {
         p2 = 0x57;
         p3 = 0x60;
         break;
      }
      case BAUD_38400:
      {
         p2 = 0x38;
         p3 = 0x40;
         break;
      }
      case BAUD_19200:
      {
         p2 = 0x19;
         p3 = 0x20;
         break;
      }
      default:
      {
         baud = 9600;
         p2 = 0x96;
         p3 = 0x00;
         break;
      }
   }

   logText( "Setting camera connection speed to " + IString(baud) + "." );

   if( sendCommand( 0x41, p2, p3 ) )
   {
      logText( 0, "...camera accepted request for new connection speed" );

      // now set the port on the computer to the required baud rate
      if( Camera::setBaudRate( baud ) )
      {
         // note that unlike other commands, this one does not end with CMD_COMPLETE

         currentBaudRate = baud;
         purgeInputQueue();
         result = true;
      }
   }

   return result;
}

