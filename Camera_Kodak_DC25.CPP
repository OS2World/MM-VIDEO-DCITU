/*
 * $Id: //devel/DCITU/Camera_Kodak_DC25.CPP#4 $
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

#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#include "Camera_Kodak_DC25.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"
#include <os2.h>


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::Camera_Kodak_DC25
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_Kodak_DC25::Camera_Kodak_DC25( GUIWindow &wnd ) :
   Camera_Kodak( wnd )
{
   // empty constructor
   return;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::openConnection
 *
 * overloaded from Camera::openConnection to set the port to 8/even/1
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::openConnection( const ULONG defaultSpeed )
{
   Boolean result = Camera_Kodak::openConnection();

   if( result )
   {
      // the port was opened, reset it to 8/even/1 (instead of default 8/none/1)
      logText( 0, "...resetting serial line control to 8/even/1..." );
      UCHAR ucParm[3];
      ucParm[0] = 8; // 8==8 data bits
      ucParm[1] = 2; // 2==even parity
      ucParm[2] = 0; // 0==1 stop bit
      ULONG ulParmLen = 3;
      APIRET rc = DosDevIOCtl( portHandle    ,  // filehandle obtained from DosOpen()
                        IOCTL_ASYNC          ,  // device category
                        ASYNC_SETLINECTRL    ,  // device-specific function
                        (PVOID)&ucParm       ,  // pointer to parameter buffer
                        (ULONG)ulParmLen     ,  // length of parameter buffer
                        (PULONG)&ulParmLen   ,  // pointer to length of parameter buffer
                        (PVOID)0             ,  // pointer to data buffer
                        (ULONG)0             ,  // max length of data buffer
                        (PULONG)0            ); // pointer to length of data buffer
      if( rc )
      {
         logText( 0, IString("...ERROR: rc==")+IString(rc) );
         result = false;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::getCameraTypeName
 *
 * return the name of the camera
 *
 *****************************************************************************/
CHAR *Camera_Kodak_DC25::getCameraTypeName() const
{
   return "Kodak DC25";
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::getCameraInfo
 *
 * get some camera image information
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::getCameraInfo()
{
   Boolean result = false;

   logText( "Retrieving camera information." );
   if( sendCommand( 0x7f ) )
   {
      CAMERAINFOTABLE buffer; // this structure is camera-specific!

      if( readBuffer( (CHAR*)&buffer, sizeof(CAMERAINFOTABLE), 257 ) )
      {
         numberOfPicsInMem = buffer.takenNormPicsInMem + buffer.takenHighPicsInMem;
         numberOfPicsInCard = buffer.takenNormPicsInCard + buffer.takenHighPicsInCard;
         logText( 0,  "...camera type: "     + IString( (int)buffer.cameraType ) );
         logText( 0,  "...images in memory: "+ IString( numberOfPicsInMem      ) );
         logText( 0,  "...images on card: "  + IString( numberOfPicsInCard     ) );

         result = true;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::getPictureInfo
 *
 * get some information on each picture in the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::getPictureInfo() const
{
   Boolean result = true;

   // get the information for the images on the card
   logText( "Retrieving image information." );
   char *buffer = new char[1024];

   // get the images on the card
   if( wnd.connectionSettings.ignoreMemoryCard )
   {
      logText( 0, "...bypassing access to the memory card" );
   }
   else
   {
      // create a container object for each of the pictures in the memory card
      wnd.cnr.setRefreshOff();
      for( int i=1; result && i<=numberOfPicsInCard; i++ )
      {
         // create a container object for this image
         // NOTE: images on the memory card are numbered starting from 501
         //       instead of 1 so we can distinguish between images on the
         //       card and images in memory
         IThread::current().sleep(100);   // slow things down for this camera
         if( sendCommand( 0x65, i>>8, i&0xff ) )
         {
            if( readBuffer( buffer, 1024, 1025 ) )
            {
               // at byte 659 is a 4-byte integer with the image size
               ULONG *sizePtr = (ULONG*)&(buffer[658]);
               ULONG size = longFromCamera(*sizePtr);
               logText( 0, "...adding image #c" + IString(i+500) + ", " + IString(size) + " bytes" );
               wnd.cnr.addObject( new ImageObject( (ULONG)i+500, size, true, false ) );
            }
            else
            {
               result = false;
            }
         }
         else
         {
            result = false;
         }
      }

      // all container objects have been added, so re-enable the refresh
      wnd.cnr.setRefreshOn().refresh();
   }

   if( result )
   {
      // create a container object for each of the pictures in the camera memory
      wnd.cnr.setRefreshOff();
      for( int i=1; result && i<=numberOfPicsInMem; i++ )
      {
         // create a container object for this image
         IThread::current().sleep(100);   // slow things down for this camera
         if( sendCommand( 0x55, i>>8, i&0xff ) )
         {
            if( readBuffer( buffer, 256, 257 ) )
            {
               // at byte 9 is a 4-byte integer with the image size
               ULONG *sizePtr = (ULONG*)&(buffer[8]);
               ULONG size = longFromCamera(*sizePtr);
               logText( 0, "...adding image #m" + IString(i) + ", " + IString(size) + " bytes" );
               wnd.cnr.addObject( new ImageObject( (ULONG)i, size, false, true ) );
            }
            else
            {
               result = false;
            }
         }
         else
         {
            result = false;
         }
      }

      delete [] buffer;

      // all container objects have been added, so re-enable the refresh
      wnd.cnr.setRefreshOn().refresh();
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::deleteImage
 *
 * the image delete functionality is not supported on the DC25
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::deleteImage( const ULONG number ) const
{
   logText( 0, "WARNING: image delete not supported (DC25)" );
   return false;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::openCard
 *
 * open the ATA memory card and set the appropriate flag
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::openCard()
{
   // the Kodak::openCard() method doesn't work with the DC25 camera,
   // so use the default Camera::openCard()
   return Camera::openCard();
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::closeCard
 *
 * close the ATA memory card if it has been opened
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::closeCard()
{
   // the Kodak::closeCard() method doesn't work with the DC25 camera,
   // so use the default Camera::closeCard()
   return Camera::closeCard();
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC25::readLargeThumbnail
 *
 * read the large 24-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC25::readLargeThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving colour thumbnail for image " + IString(number) + "." );

   // find the object with the specified number
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      // the command to get the thumbnail is different depending on whether
      // the thumbnail is on the flash memory card, or in internal memory
      if( obj->onCard )
      {
         // number for card-based images were incremented by 500 when the
         // container object was created, so decrement by the same amount
         ULONG tmpNumber = number - 500;
         result = sendCommand( 0x66, tmpNumber>>8, tmpNumber&0xff );
      }
      else
      {
         result = sendCommand( 0x56, number>>8, number&0xff );
      }

      if( result )
      {
         // the camera sends fifteen 1024-byte blocks,
         // composed of an 80x60 array of RGB values
         CHAR *buffer = new CHAR[15*1024];

         // read the bytes
         if( readBuffer( buffer, 15*1024, 1025 ) )
         {
            // copy the new thumbnail, reversing the rows top-bottom
            for( int y=0; y<60; y++ )
            {
               for( int x=0; x<80; x++ )
               {
                  // since the thumbnail is less than 96x72, we need to
                  // skip 8 columns on each side and 6 rows on the top
                  // and the bottom (also reverse RGB to BGR for OS/2)
                  obj->bits[(71-6-y)*96*3+(x+8)*3+2] = buffer[y*80*3+x*3+0];
                  obj->bits[(71-6-y)*96*3+(x+8)*3+1] = buffer[y*80*3+x*3+1];
                  obj->bits[(71-6-y)*96*3+(x+8)*3+0] = buffer[y*80*3+x*3+2];
               }
            }
            obj->refresh();
            result = true;
         }
         delete [] buffer;
      }
   }

   return result;
}

