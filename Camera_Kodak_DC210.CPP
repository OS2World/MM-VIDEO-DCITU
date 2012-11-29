/*
 * $Id: //devel/DCITU/Camera_Kodak_DC210.CPP#4 $
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
#include "Camera_Kodak_DC210.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"


/*****************************************************************************
 *
 *    Camera_Kodak_DC210::Camera_Kodak_DC210
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_Kodak_DC210::Camera_Kodak_DC210( GUIWindow &wnd ) :
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
CHAR *Camera_Kodak_DC210::getCameraTypeName() const
{
   return "Kodak DC210";
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC210::getCameraInfo
 *
 * get some camera image information
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC210::getCameraInfo()
{
   Boolean result = false;

   logText( "Retrieving camera information." );
   if( sendCommand( 0x7f ) )
   {
      CAMERAINFOTABLE buffer; // this structure is camera-specific!

      if( readBuffer( (CHAR*)&buffer, sizeof(CAMERAINFOTABLE), 258 ) )
      {
         numberOfPicsInMem = 0;
         numberOfPicsInCard = shortFromCamera(buffer.picturesInCard);
         logText( 0,  "...camera type: "   + IString( (int)buffer.cameraType ) );
         logText( 0,  "...picture size: "  + IString( (int)buffer.pictureSize) );
         logText( 0,  "...file type: "     + IString( (int)buffer.fileType   ) );
         logText( 0,  "...images on card: "+ IString( numberOfPicsInCard     ) );
         logText( 0,  "...camera mode: "   + IString( (int)buffer.cameraMode ) );
         logText( 0,  "...card status: 0b" + IString( (CHAR)buffer.memoryCardStatus ).c2b() );

         result = true;
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC210::getPictureInfo
 *
 * get some information on each picture in the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC210::getPictureInfo() const
{
   Boolean result = false;

   // get the information for the images on the card
   logText( "Retrieving image information." );

   if( sendCommand( 0x4a ) )
   {
      // the first two bytes in the response from a 0x4a command is the number
      // of records that will be returned; thus, the buffer must be created
      // large enough to accomodate all of the entries, plus two bytes
      int bufferSize = sizeof(FILEINFOTABLE)*numberOfPicsInCard + 2;
      CHAR *nameBuffer = new CHAR[bufferSize];

      if( readBuffer( nameBuffer, bufferSize, 258 ) )
      {
         // create a FILEINFOTABLE pointer and get it to point to byte 0 of the
         // first record (remember that the first 2 bytes of the buffer is the
         // number of records that will be returned...something we already know)
         FILEINFOTABLE *fileInfoTable = (FILEINFOTABLE*)&(nameBuffer[2]);

         // create a container objects for each of the pictures in the camera
         wnd.cnr.setRefreshOff();

         for( int i=0; i<numberOfPicsInCard; i++ )
         {
            wnd.cnr.addObject( new ImageObject( "\\PCCARD\\DCIMAGES\\",
                                                IString(fileInfoTable[i].name).subString(1,8)+
                                                "." +
                                                IString(fileInfoTable[i].name).subString(9,3),
                                                longFromCamera(fileInfoTable[i].size),
                                                shortFromCamera(fileInfoTable[i].date),
                                                shortFromCamera(fileInfoTable[i].time) ) );
         }
         // all container objects have been added, so re-enable the refresh
         wnd.cnr.setRefreshOn().refresh();
         result = true;
      }
      delete [] nameBuffer;
   }
   return result;
}

