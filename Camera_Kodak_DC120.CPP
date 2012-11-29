/*
 * $Id: //devel/DCITU/Camera_Kodak_DC120.CPP#4 $
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

#include <istring.hpp>  // IString
#include "Camera_Kodak_DC120.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"


/*****************************************************************************
 *
 *    Camera_Kodak_DC120::Camera_Kodak_DC120
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_Kodak_DC120::Camera_Kodak_DC120( GUIWindow &wnd ) :
   Camera_Kodak( wnd )
{
   // empty constructor
   return;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC210::getCameraTypeName
 *
 * return the name of the camera
 *
 *****************************************************************************/
CHAR *Camera_Kodak_DC120::getCameraTypeName() const
{
   return "Kodak DC120";
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC120::getCameraInfo
 *
 * get some camera image information
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC120::getCameraInfo()
{
   Boolean result = false;

   logText( "Retrieving camera information." );
   if( sendCommand( 0x7f ) )
   {
      CAMERAINFOTABLE buffer; // this structure is camera-specific!

      if( readBuffer( (CHAR*)&buffer, sizeof(CAMERAINFOTABLE), 257 ) )
      {
         numberOfPicsInMem = shortFromCamera(buffer.picturesInMemory);
         numberOfPicsInCard = shortFromCamera(buffer.picturesInCard);
         logText( 0,  IString( "...camera type: "     ) + IString( (int)buffer.cameraType ) );
         logText( 0,  IString( "...images in memory: ") + IString( numberOfPicsInMem      ) );
         logText( 0,  IString( "...images on card: "  ) + IString( numberOfPicsInCard     ) );
         logText( 0,  IString( "...card status: 0b"   ) + IString( (CHAR)buffer.memoryCardStatus ).c2b() );

         result = true;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC120::getPictureInfo
 *
 * get some information on each picture in the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC120::getPictureInfo() const
{
   Boolean result = true;

   // get the information for the images on the card
   logText( "Retrieving image information." );

   // get the images on the card
   if( wnd.connectionSettings.ignoreMemoryCard )
   {
      logText( 0, "...bypassing access to the memory card" );
   }
   else if( sendCommand( 0x4a, 0, 0, 0, 0, 0, 1 ) )  // see reference; P1 used as parameter in this case!
   {
      result = false;
      logText( 0, "...getting information on images on the card" );
      // the first two bytes in the response from a 0x4a command is the number
      // of records that will be returned; thus, the buffer must be created
      // large enough to accomodate all of the entries, plus two bytes
      int bufferSize = sizeof(FILEINFOTABLE)*numberOfPicsInCard + 2;
      CHAR *nameBuffer = new CHAR[bufferSize];
      nameBuffer[0] = (CHAR)0;
      nameBuffer[1] = (CHAR)0;

      if( readBuffer( &(nameBuffer[0]), bufferSize, 257 ) )
      {
         int *numberOfPictures = (int*)nameBuffer;
         logText( 0, IString("-> camera returned ") +
                     IString( shortFromCamera(*numberOfPictures) ) +
                     IString(" images") );

         // create a FILEINFOTABLE pointer and get it to point to byte 0 of the
         // first record (remember that the first 2 bytes of the buffer is the
         // number of records that will be returned...something we already know)
         FILEINFOTABLE *fileInfoTable = (FILEINFOTABLE*)&(nameBuffer[2]);

         // create a container objects for each of the pictures in the camera
         wnd.cnr.setRefreshOff();

         for( int i=0; i<numberOfPicsInCard; i++ )
         {
            logText( 0, "-> filename == \"" + IString(fileInfoTable[i].name).subString(1,11) + "\"" );
            logText( 0, "-> size == " + IString(longFromCamera(fileInfoTable[i].size)) );

            wnd.cnr.addObject( new ImageObject( "\\PCCARD\\DC120IMG\\",
                                                IString(fileInfoTable[i].name).subString(1,8).strip()+
                                                "." +
                                                IString(fileInfoTable[i].name).subString(9,3).strip(),
                                                longFromCamera(fileInfoTable[i].size),
                                                shortFromCamera(fileInfoTable[i].date),
                                                shortFromCamera(fileInfoTable[i].time) ) );
         }
         // all container objects have been added
         result = true;
      }
      delete [] nameBuffer;
   }

   if( result )
   {
      result = false;
      logText( 0, "...getting information on images in memory" );
      // get the images in memory
      if( sendCommand( 0x4a ) )
      {
         // the first two bytes in the response from a 0x4a command is the number
         // of records that will be returned; thus, the buffer must be created
         // large enough to accomodate all of the entries, plus two bytes
         int bufferSize = sizeof(FILEINFOTABLE)*numberOfPicsInMem + 2;
         CHAR *nameBuffer = new CHAR[bufferSize];
         nameBuffer[0] = (CHAR)0;
         nameBuffer[1] = (CHAR)0;

         if( readBuffer( &(nameBuffer[0]), bufferSize, 257 ) )
         {
            int *numberOfPictures = (int*)nameBuffer;
            logText( 0, "-> camera returned " + IString( shortFromCamera(*numberOfPictures) ) + " images" );

            // create a FILEINFOTABLE pointer and get it to point to byte 0 of the
            // first record (remember that the first 2 bytes of the buffer is the
            // number of records that will be returned...something we already know)
            FILEINFOTABLE *fileInfoTable = (FILEINFOTABLE*)&(nameBuffer[2]);

            // create a container objects for each of the pictures in the camera
            for( int i=0; i<numberOfPicsInMem; i++ )
            {
               logText( 0, "-> filename == \"" + IString(fileInfoTable[i].name).subString(1,11) + "\"" );
               logText( 0, "-> size ==" + IString(longFromCamera(fileInfoTable[i].size)) );

               wnd.cnr.addObject( new ImageObject( "\\MEMORY\\",
                                                   IString(fileInfoTable[i].name).subString(1,8).strip() +
                                                   "." +
                                                   IString(fileInfoTable[i].name).subString(9,3).strip(),
                                                   longFromCamera(fileInfoTable[i].size),
                                                   shortFromCamera(fileInfoTable[i].date),
                                                   shortFromCamera(fileInfoTable[i].time),
                                                   false,      // card images
                                                   true ) );   // memory images
            }
            // all container objects have been added, so re-enable the refresh
            wnd.cnr.setRefreshOn().refresh();
            result = true;
         }
         delete [] nameBuffer;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC120::deleteImage
 *
 * the image delete functionality is not supported on the DC120
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC120::deleteImage( const ULONG number ) const
{
   logText( 0, "WARNING: image delete not supported (DC120)" );
   return false;
}

