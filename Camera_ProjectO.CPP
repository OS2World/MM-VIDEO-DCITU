/*
 * $Id: //devel/DCITU/Camera_ProjectO.CPP#9 $
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

#include <iexcbase.hpp> // IInvalidRequest
#include "Camera_ProjectO.HPP"
#include "Camera_JPEG.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"


// these static system codes are single byte values used to synchronize with the camera
const CHAR Camera_ProjectO::CMD_INIT   = 0x00;  // initialization byte
const CHAR Camera_ProjectO::CMD_DONE   = 0x05;  // action complete
const CHAR Camera_ProjectO::CMD_ACK    = 0x06;  // positive acknowledgement
const CHAR Camera_ProjectO::CMD_ERROR  = 0x11;  // unable to execute command
const CHAR Camera_ProjectO::CMD_NAK    = 0x15;  // negative acknowledgement
const CHAR Camera_ProjectO::CMD_TERM   = 0xff;  // termination byte


/*****************************************************************************
 *
 *    Camera_ProjectO::Camera_ProjectO
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_ProjectO::Camera_ProjectO( GUIWindow &wnd ) :
   Camera( wnd )
{
   // empty constructor
   return;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::getCameraTypeName
 *
 * return the name of the camera
 *
 *****************************************************************************/
CHAR *Camera_ProjectO::getCameraTypeName() const
{
   return "Project 'O'";
}


/*****************************************************************************
 *
 *    Camera_ProjectO::openConnection
 *
 * overloaded from Camera::openConnection to use a baud rate of 19200
 *
 *****************************************************************************/
Boolean Camera_ProjectO::openConnection( const ULONG defaultSpeed )
{
   // ignore the parameter and force it to use BAUD_19200
   Boolean result = Camera::openConnection( BAUD_19200 );

   if(result)
   {
      result = sendBreak(500);
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::powerUp
 *
 * send a "power up" command to the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::powerUp()
{
   Boolean result = false;

   if(poweredUp)
   {
      // we're already powered up -- skip the rest
      result = true;
   }
   else
   {
      logText( "Talking to camera.", "sending \"power up\" command" );

      //purgeInputQueue();

      // send a CMD_INIT to wake up the camera
      if( sendCommand( CMD_INIT ) )
      {
         CHAR byte;
         Boolean signatureByteFlag = false;

         // keep looping while the camera is sending back information
         while( readBuffer(&byte,1) )
         {
            if( byte == 0x15 )
            {
               // this is the signature byte we need
               logText( 0, "found power up signature byte" );
               signatureByteFlag = true;

               IThread::current().sleep(500);
               purgeInputQueue();
               break;
            }
         }

         if( signatureByteFlag )
         {
            // if alternate startup is selected, start with a baud rate of 0
            if( wnd.connectionSettings.useAlternateStartup )
            {
               logText( 0, "using alternate camera startup sequence" );
               setBaudRate( BAUD_0 );
               purgeInputQueue();
            }

            if( setBaudRate( wnd.connectionSettings.baudRate ) )
            {
               // connection is established
               logText( 0, "power up procedure complete" );
               poweredUp = true;
               result = true;
            }
            else
            {
               logText( 0, "...ERROR: power up procedure failed on BAUD RATE setting" );
            }
         }
         else
         {
            logText( 0, "...ERROR: power up signature byte not found" );
         }
      }
   }

   if( ! result )
   {
      logText( "Unable to connect to camera." );
   }


   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::powerDown
 *
 * send a "power down" command to the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::powerDown()
{
   Boolean result = false;

   if((wnd.pbPurge.text().asInt() > 1) && (!cancelRequested))
   {
      // skip the power down process since we
      // have additional commands to handle
      result = true;
   }
   else
   {
      if( poweredUp )
      {
         logText( 0, "sending \"power down\" command" );

         if( setBaudRate( BAUD_0 ) )
         {
            // connection is powered down
            logText( 0, "" );
            poweredUp = false;
            result = true;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::requiresPowerUpDown
 *
 * specifies that this camera need the "power up" and "power down" command
 *
 *****************************************************************************/
Boolean Camera_ProjectO::requiresPowerUpDown() const
{
   return true;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::writeWord
 *
 * write a word to the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::writeWord( const UCHAR wordReg, const USHORT value, const Boolean startup ) const
{
   // convert the USHORT value we want to write into a ULONG
   ULONG number = (ULONG)value;
   // call the proper ::writeWord() method
   return writeWord( wordReg, number, startup );
}


/*****************************************************************************
 *
 *    Camera_ProjectO::writeWord
 *
 * write a word to the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::writeWord( const UCHAR wordReg, const ULONG value, const Boolean startup ) const
{
   Boolean result = false;

   CHAR command[12];
   command[0] = 0x1b;               // <- escape
   command[1] = startup?0x53:0x43;  // <- read/write command
   command[2] = 0x06;               // <- len of data (LSB)
   command[3] = 0x00;               // <- len of data (MSB)
   command[4] = 0x00;               // <- write register command
   command[5] = wordReg;            // <- register to which we want to write
   command[6] = ((CHAR*)&value)[0]; // <- word to write (LSB)
   command[7] = ((CHAR*)&value)[1]; // <- word to write
   command[8] = ((CHAR*)&value)[2]; // <- word to write
   command[9] = ((CHAR*)&value)[3]; // <- word to write (MSB)

   USHORT checksum = (USHORT)command[4] +
                     (USHORT)command[5] +
                     (USHORT)command[6] +
                     (USHORT)command[7] +
                     (USHORT)command[8] +
                     (USHORT)command[9];
   command[10]= ((CHAR*)&checksum)[0]; // <- checksum (LSB)
   command[11]= ((CHAR*)&checksum)[1]; // <- checksum (MSB)

   logText( 0, "sending command to write word \"" + IString(value) + "\" to register 0x" + IString((int)wordReg).d2x() );

   if( writeBuffer( command, 12 ) )
   {
      if( readResponse( CMD_ACK ) )
      {
         result = true;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::readWord
 *
 * read a word from the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::readWord( const UCHAR wordReg, USHORT &answer ) const
{
   // create a temporary ULONG variable
   ULONG number;
   // call the proper ::readWord() method
   Boolean result = readWord( wordReg, number );
   // convert the answer into a USHORT
   answer = (USHORT)number;
   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::readWord
 *
 * read a word from the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::readWord( const UCHAR wordReg, ULONG &answer ) const
{
   Boolean result = false;
   answer = 0;

   CHAR command[8];
   command[0] = 0x1b;      // <- escape
   command[1] = 0x43;      // <- read/write command
   command[2] = 0x02;      // <- len of data (LSB)
   command[3] = 0x00;      // <- len of data (MSB)
   command[4] = 0x01;      // <- read register command
   command[5] = wordReg;   // <- register to read
   command[6] = wordReg+1; // <- checksum (LSB)
   command[7] = 0x00;      // <- checksum (MSB)

   logText( 0, IString("sending command to read word from register 0x") + IString((int)wordReg).d2x() );

   if( writeBuffer( command, 8 ) )
   {
      CHAR buffer[4];

      // read the packet header
      if( readBuffer( &(buffer[0]), 4 ) )
      {
         if(buffer[0] == 0x03 &&
            buffer[1] == 0x00 &&
            buffer[2] == 0x04 &&
            buffer[3] == 0x00 )
         {
            // read the word
            CHAR *ptr = (CHAR*)&answer;
            if( readBuffer( ptr, 4 ) )
            {
               // read the checksum
               if( readBuffer( &(buffer[0]), 2 ) )
               {
                  // compare the checksum (2 bytes) against the four bytes from "answer"
                  USHORT checksum = *( (USHORT*)&(buffer[0]) );
                  if( checksum == ptr[0] + ptr[1] + ptr[2] + ptr[3] )
                  {
                     if( writeResponse( CMD_ACK ) )
                     {
                        logText( 0, "...word: " + IString(answer) );
                        result = true;
                     }
                  }
                  else
                  {
                     logText( 0, "ERROR; checksum does not match!" );
                  }
               }
            }
         }
         else
         {
            logText( 0, "ERROR; buffer ="
                        "= 0x" + IString((int)buffer[0]).d2x() +
                        ", 0x" + IString((int)buffer[1]).d2x() +
                        ", 0x" + IString((int)buffer[2]).d2x() +
                        ", 0x" + IString((int)buffer[3]).d2x() );
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::sendAction
 *
 * send an action command to the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::sendAction( const UCHAR action ) const
{
   Boolean result = false;

   CHAR command[9];
   command[0] = 0x1b;               // <- escape
   command[1] = 0x43;               // <- read/write command
   command[2] = 0x03;               // <- len of data (LSB)
   command[3] = 0x00;               // <- len of data (MSB)
   command[4] = 0x02;               // <- send action command
   command[5] = action;             // <- action to perform (LSB)
   command[6] = 0x00;               // <- action to perform (MSB)
   command[7] = 0x02+action;        // <- checksum (LSB)
   command[8] = 0x00;               // <- checksum (MSB)

   logText( 0, IString("sending action #") + IString((int)action) );

   if( writeBuffer( command, 9 ) )
   {
      if( readResponse( CMD_ACK ) )
      {
         if( readResponse( CMD_DONE ) )
         {
            result = true;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::readLargeThumbnail
 *
 * read the large 24-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_ProjectO::readLargeThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving the thumbnail " + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   IString thumbnailFilename;
   if( obj )
   {
      thumbnailFilename = wnd.connectionSettings.dir + "t" + IString(obj->originalNumber).rightJustify(7,'0') + ".jpg";

      logText( 0, "opening " + thumbnailFilename );

      HFILE fileHandle;
      ULONG action;
      APIRET rc;
      // we have the object...now try and open the file locally
      rc = DosOpen(  thumbnailFilename       ,  // filename
                     &fileHandle             ,  // file handle
                     &action                 ,  // action that DosOpen carried out
                     0                       ,  // new size of file
                     FILE_NORMAL             ,  // new file attributes
                     OPEN_ACTION_CREATE_IF_NEW| // what to do with the file
                     OPEN_ACTION_REPLACE_IF_EXISTS,
                     OPEN_FLAGS_FAIL_ON_ERROR|  // errors reported via return code
                     OPEN_FLAGS_SEQUENTIAL   |  // all access will be sequential
                     OPEN_SHARE_DENYWRITE    |  // deny others from write
                     OPEN_ACCESS_WRITEONLY   ,  // open for write
                     (PEAOP2)0               ); // extended attributes
      if( rc )
      {
         logText( "Thumbnail file could not be opened.", "...ERROR: DosOpen() returned rc==" + IString(rc) );
      }
      else
      {
         // select the image
         if( writeWord( 0x04, obj->cameraNumber ) )
         {
            // get the thumbnail size
            ULONG bufferSize;
            if( readWord( 0x0d, bufferSize ) )
            {
               // send the command to read the selected thumbnail
               CHAR command[8];
               command[0] = 0x1b;   // <- escape
               command[1] = 0x43;   // <- read/write command
               command[2] = 0x02;   // <- len of data (LSB)
               command[3] = 0x00;   // <- len of data (MSB)
               command[4] = 0x04;   // <- read data command
               command[5] = 0x0f;   // <- register from which to read
               command[6] = 0x13;   // <- checksum (LSB)
               command[7] = 0x00;   // <- checksum (MSB)

               if( writeBuffer( command, 8 ) )
               {
                  CHAR  *bufferData    = new CHAR[bufferSize];
                  CHAR   packetHeader[2];
                  USHORT packetSize;
                  CHAR  *packetData    = new CHAR[2048];
                  USHORT packetChecksum;
                  USHORT checksum;
                  int    bufferOffset  = 0;
                  int    packetOffset  = 0;

                  result = true;
                  while( result && bufferOffset<bufferSize )
                  {
                     // update the slider control
                     wnd.slideTo( bufferOffset, bufferSize );
                     logText( "Transfered " + IString(bufferOffset) + " of " + IString(bufferSize) + " bytes." );

                     result = readBuffer( packetHeader, 2 );
                     if( result )
                     {
                        result = readBuffer( (CHAR*)&packetSize, 2 );
                     }
                     if( result )
                     {
                        result = readBuffer( packetData, packetSize );
                     }
                     if( result )
                     {
                        result = readBuffer( (CHAR*)&packetChecksum, 2 );
                     }
                     if( result )
                     {
                        // verify the checksum
                        checksum = 0;
                        for( int i=0; i<packetSize; i++ )
                        {
                           checksum += packetData[i];
                        }
                        if( checksum == packetChecksum )
                        {
                           result = writeResponse( CMD_ACK );
                        }
                        else
                        {
                           logText( 0, "ERROR: checksum does not match (" + IString(checksum) + ", " + IString(packetChecksum) + ")" );
                           writeResponse( CMD_ERROR );
                           result = false;
                        }
                     }
                     if( result )
                     {
                        // this packet is ok, copy it to the buffer
                        for( int i=0; i<packetSize; i++ )
                        {
                           bufferData[bufferOffset++] = packetData[i];
                        }
                     }
                  }

                  wnd.slideTo( 0 );
                  logText( 0, "...finished reading " + IString(bufferOffset) + " of " + IString(bufferSize) );

                  if( result )
                  {
                     // the image was downloaded correctly, now write it out to disk
                     ULONG bytesWritten;
                     DosWrite( fileHandle, (PVOID)bufferData, bufferSize, &bytesWritten );
                     if( bytesWritten != bufferSize )
                     {
                        logText( "Unable to save the thumbnail file.", "...ERROR: wrote " + IString(bytesWritten) + " of " + IString(bufferSize) + " bytes" );
                     }
                     else
                     {
                        logText( "Finished transfering thumbnail." );
                        result = true;
                     }
                  }
                  delete [] bufferData;
                  delete [] packetData;
               }
            }
         }
         DosResetBuffer( fileHandle );
         DosClose( fileHandle );
      }
   }

   if( ! result )
   {
      logText( "Unable to transfer thumbnail." );
   }
   else
   {
      FILE *infile = (FILE*)0;

      try
      {
         // the following code segment was taken from the IJG library
         struct jpeg_decompress_struct cinfo;
         struct jpeg_error_mgr jerr;
         JSAMPARRAY buffer;
         int row_stride;

         cinfo.err = jpeg_std_error(&jerr);

         // setup our own JPEG error catch routine
         jerr.error_exit = jpegLibraryError;

         jpeg_create_decompress(&cinfo);

         infile = fopen( thumbnailFilename, "rb" );
         if( ! infile )
         {
            logText( "Unable to open JFIF/JPEG thumbnail." );
         }
         else
         {
            logText( "Creating thumbnail bitmap." );

            jpeg_stdio_src(&cinfo, infile);
            jpeg_read_header(&cinfo, TRUE);

            if(cinfo.image_width == 160 && cinfo.image_height == 120)
            {
               // we need to cut the image size down to something we can use
               cinfo.scale_num = 1;
               cinfo.scale_denom = 2;
            }
            else if(cinfo.image_width == 80 && cinfo.image_height == 60)
            {
               // nothing to do -- all is well
            }
            else
            {
               logText( "Unsuported thumbnail image size (" + IString(cinfo.image_width) + "x" + IString(cinfo.image_height) + ")." );
               result = false;
            }
            if(result)
            {
               jpeg_start_decompress(&cinfo);

               row_stride = cinfo.output_width * cinfo.output_components;
               buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

               // the thumbnail is only 80x60, while it should be 96x72...so skip some blank space
               int y=6;
               while( cinfo.output_scanline < cinfo.output_height )
               {
                  jpeg_read_scanlines(&cinfo, buffer, 1);

                  for( int x=0; x<(80*3); x+=3 )
                  {
                     // the bytes received are RGB, while the thumbnails are BGR
                     obj->bits[((71-y)*(96*3))+x+0+(8*3)] = buffer[0][x+2];
                     obj->bits[((71-y)*(96*3))+x+1+(8*3)] = buffer[0][x+1];
                     obj->bits[((71-y)*(96*3))+x+2+(8*3)] = buffer[0][x+0];
                  }
                  y++;
               }
               obj->refresh();

               jpeg_finish_decompress(&cinfo);
               jpeg_destroy_decompress(&cinfo);
               // end of code segment taken from the IJG library
            }
         }
      }
      catch(IInvalidRequest &exception)
      {
         // something about the JPEG failed to load!
         logText("Failed to load thumbnail.", "possible reason: unsupported thumbnail format?");
         result = false;
      }

      if( infile )
      {
         fclose( infile );
      }
   }

   if( (!wnd.connectionSettings.keepThumbnails) || (!result) )
   {
      // delete the temporary thumbnail file
      if( obj )
      {
         DosDelete(thumbnailFilename);
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::readSmallThumbnail
 *
 * read the small thumbnail
 *
 *****************************************************************************/
Boolean Camera_ProjectO::readSmallThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( 0, "WARNING; grayscale thumbnail function not supported" );

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::readImage
 *
 * read the specified image from the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::readImage( const ULONG number ) const
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
         logText( "Image file already exists.",
                  "...ERROR: DosOpen() returned rc==" + IString(rc) );
         // assume that this is actually NOT an error!
         result = true;
      }
      else
      {
         // select the image
         if( writeWord( 0x04, obj->cameraNumber ) )
         {
            // send the command to read the selected image
            CHAR command[8];
            command[0] = 0x1b;   // <- escape
            command[1] = 0x43;   // <- read/write command
            command[2] = 0x02;   // <- len of data (LSB)
            command[3] = 0x00;   // <- len of data (MSB)
            command[4] = 0x04;   // <- read data command
            command[5] = 0x0e;   // <- register from which to read
            command[6] = 0x12;   // <- checksum (LSB)
            command[7] = 0x00;   // <- checksum (MSB)

            if( writeBuffer( command, 8 ) )
            {
               ULONG bufferSize = obj->size.asUnsigned();
               CHAR *bufferData = new CHAR[bufferSize];
               CHAR packetHeader[2];
               USHORT packetSize;
               CHAR *packetData = new CHAR[2048];
               USHORT packetChecksum;
               USHORT checksum;
               int bufferOffset = 0;
               int packetOffset = 0;

               result = true;
               while( result && bufferOffset<bufferSize )
               {
                  // update the slider control
                  wnd.slideTo( bufferOffset, bufferSize );
                  logText( "Transfered " + IString(bufferOffset) + " of " + IString(bufferSize) + " bytes." );

                  result = readBuffer( packetHeader, 2 );
                  if( result )
                  {
                     result = readBuffer( (CHAR*)&packetSize, 2 );
                  }
                  if( result )
                  {
                     result = readBuffer( packetData, packetSize );
                  }
                  if( result )
                  {
                     result = readBuffer( (CHAR*)&packetChecksum, 2 );
                  }
                  if( result )
                  {
                     // verify the checksum
                     checksum = 0;
                     for( int i=0; i<packetSize; i++ )
                     {
                        checksum += packetData[i];
                     }
                     if( checksum == packetChecksum )
                     {
                        result = writeResponse( CMD_ACK );
                     }
                     else
                     {
                        logText( 0, IString("ERROR: checksum does not match (")+IString(checksum)+IString(", ")+IString(packetChecksum)+IString(")") );
                        writeResponse( CMD_ERROR );
                        result = false;
                     }
                  }
                  if( result )
                  {
                     // this packet is ok, copy it to the buffer
                     for( int i=0; i<packetSize; i++ )
                     {
                        bufferData[bufferOffset++] = packetData[i];
                     }
                  }
               }

               wnd.slideTo( 0 );
               logText( 0, "...finished reading " + IString(bufferOffset) + " of " + IString(bufferSize) );

               if( result )
               {
                  // the image was downloaded correctly, now write it out to disk
                  ULONG bytesWritten;
                  DosWrite( fileHandle, (PVOID)bufferData, bufferSize, &bytesWritten );
                  if( bytesWritten != bufferSize )
                  {
                     logText( "Unable to save the file.",
                              "...ERROR: wrote " + IString(bytesWritten) + " of " + IString(bufferSize) + " bytes" );
                  }
                  else
                  {
                     logText( "Finished transfering file." );
                     // mark the image as being transfered by modifying the thumbnail
                     obj->addRedLine();
                     result = true;
                  }
               }
               delete [] bufferData;
               delete [] packetData;
            }
         }
         DosClose( fileHandle );
         if( result )
         {
            setFileDateTime( wnd.connectionSettings.dir+obj->name, obj );

            // if the user wants images deleted after transfer...
            if( wnd.connectionSettings.deleteAfterTransfer )
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
 *    Camera_ProjectO::deleteImage
 *
 * delete the specified image from the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::deleteImage( const ULONG number ) const
{
   Boolean result = false;

   logText( "Deleting image #" + IString(number) + " from the camera." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      // select the image to delete
      if( writeWord( 0x04, obj->cameraNumber ) )
      {
         // send the delete action command
         if( sendAction( 0x07 ) )
         {
            // the image was deleted -- renumber the remaining image objects
            ULONG oldNumber = obj->cameraNumber;
            wnd.cnr.removeObject( obj );
            delete( obj );
            ImageObject::renumberFrom( oldNumber );
            result = true;
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::setBaudRate
 *
 * set the baud rate
 *
 *****************************************************************************/
Boolean Camera_ProjectO::setBaudRate( ULONG baud )
{
   Boolean result = false;

   ULONG commandValue;

   switch( baud )
   {
      case BAUD_0:
      {
         // this case is used when powering down
         commandValue = 0x00;
         baud = BAUD_19200;
         break;
      }
      case BAUD_230400:
      {
         commandValue = 0x06;
         break;
      }
      case BAUD_115200:
      {
         commandValue = 0x05;
         break;
      }
      case BAUD_57600:
      {
         commandValue = 0x04;
         break;
      }
      case BAUD_38400:
      {
         commandValue = 0x03;
         break;
      }
      case BAUD_19200:
      {
         commandValue = 0x02;
         break;
      }
      default:
      {
         commandValue = 0x01;
         baud = BAUD_9600;
         break;
      }
   }

   logText( 0, "setting camera connection speed to " + IString(baud) );

   if( writeWord( 0x11, commandValue, true ) )
   {
      logText( 0, "...camera accepted request for new connection speed" );

      // now set the port on the computer to the required baud rate
      if( Camera::setBaudRate( baud ) )
      {
         result = true;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::getCameraInfo
 *
 * get information from the camera (number of images)
 *
 *****************************************************************************/
Boolean Camera_ProjectO::getCameraInfo()
{
   Boolean result = false;

   logText( "Retrieving camera information." );

   numberOfPicsInMem  = 0;
   numberOfPicsInCard = 0;

   // see if we need to pause between commands
   if(wnd.connectionSettings.pauseBetweenCommands)
   {
      logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
      IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
   }
   if( readWord( 0x0a, numberOfPicsInMem ) )
   {
      result = true;
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_ProjectO::getPictureInfo
 *
 * get information on each image in the camera
 *
 *****************************************************************************/
Boolean Camera_ProjectO::getPictureInfo() const
{
   Boolean result = true;

   // create a container objects for each of the pictures in the camera
   wnd.cnr.setRefreshOff();
   for( int i=1; i<=numberOfPicsInMem; i++ )
   {
      // get the image information
      ULONG imageSize = 0;

      // see if we need to pause between commands
      if(wnd.connectionSettings.pauseBetweenCommands)
      {
         logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
         IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
      }
      // select the image
      if( writeWord( 0x04, (ULONG)i ) )
      {
         // see if we need to pause between commands
         if(wnd.connectionSettings.pauseBetweenCommands)
         {
            logText( 0, "(pause for " + IString(wnd.connectionSettings.pauseBetweenCommands) + ")" );
            IThread::current().sleep(wnd.connectionSettings.pauseBetweenCommands);
         }
         // get the image size
         if( readWord( 0x0c, imageSize ) )
         {
            // create a container object for this image
            logText( 0, "...adding image #" + IString(i) + " with a size of " + IString(imageSize) );
            wnd.cnr.addObject( new ImageObject( (ULONG)i, imageSize ) );
         }
         else
         {
            // could not get the image size
            logText( 0, "ERROR; could not determine size of image #" + IString(i) );
         }
      }
      else
      {
         // could not select the image
         logText( 0, "ERROR; could not select image #" + IString(i) );
      }
   }

   // all container objects have been added, so re-enable the refresh
   wnd.cnr.setRefreshOn().refresh();

   return result;
}

