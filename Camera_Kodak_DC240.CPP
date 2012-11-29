/*
 * $Id: //devel/DCITU/Camera_Kodak_DC240.CPP#4 $
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
#include "Camera_Kodak_DC240.HPP"
#include "Camera_JPEG.HPP"
#include "ImageObject.HPP"
#include "GUIWindow.HPP"


/*****************************************************************************
 *
 *    Camera_Kodak_DC240::Camera_Kodak_DC240
 *
 * empty constructor
 *
 *****************************************************************************/
Camera_Kodak_DC240::Camera_Kodak_DC240( GUIWindow &wnd ) :
   Camera_Kodak( wnd )
{
   // empty constructor
   return;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC240::getCameraTypeName
 *
 * return the name of the camera
 *
 *****************************************************************************/
CHAR *Camera_Kodak_DC240::getCameraTypeName() const
{
   return "Kodak DC240";
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC240::getCameraInfo
 *
 * get some camera image information
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC240::getCameraInfo()
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
         logText( 0,  "...camera type: "     + IString( (int)buffer.cameraType ) );
         logText( 0,  "...battery status: "  + IString( (int)buffer.batteryStatus));
         logText( 0,  "...picture size: "    + IString( (int)buffer.pictureSize) );
         logText( 0,  "...file type: "       + IString( (int)buffer.fileType   ) );
         logText( 0,  "...images on card: "  + IString( numberOfPicsInCard     ) );
         logText( 0,  "...card status: 0b"   + IString( (CHAR)buffer.memoryCardStatus ).c2b() );

         logText( "Setting host packet buffer size." );
         if( sendCommand( 0x2a, 0x02, 0x02 ) )
         {
            if(readResponse(CMD_COMPLETE))
            {
               result = true;
            }
         }
      }
   }

   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC240::getPictureInfo
 *
 * get some information on each picture in the camera
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC240::getPictureInfo(VOID) const
{
   Boolean result = false;

   // get the information for the images on the card
   logText( "Reading directory information." );

   wnd.cnr.setRefreshOff();
   result = readDirectory("\\DCIM\\", false);
   wnd.cnr.setRefreshOn().refresh();
   return result;
}


/*****************************************************************************
 *
 *    Camera_Kodak_DC240::readDirectory
 *
 * find all of the DC240 image directories and files
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC240::readDirectory( IString path, const Boolean lookForFiles ) const
{
   Boolean result;

   // find out the number of files...
   result = sendCommand( 0x99, 1 );
   if( result )
   {
      // ...either *.JPG or *DC240...
      FILEPACKET filePacket = {0};
      IString filename;
      if(lookForFiles)
      {
         filename = IString( path + "*.JPG" );
      }
      else
      {
         if( cameraType == KODAK_DC240 && !wnd.connectionSettings.useRawPath )
         {
            // all DC240 directories are named nnnDC240, where
            // "nnn" is always defined as a 3-digit number
            filename = path + "???DC240";
         }
         else if( cameraType == KODAK_DC280 && !wnd.connectionSettings.useRawPath )
         {
            // all DC280 directories are named nnnDC280, where
            // "nnn" is always defined as a 3-digit number
            filename = path + "???DC280";
         }
         else
         {
            logText( 0, "note: using raw directory path; this may cause strange results" );
            filename = path + "*";
         }
      }
      logText( 0, "Looking for the number of entries that match \"" + filename + "\"" );

      int i;
      int j;

      for( i=0; i<filename.length(); i++ )
      {
         filePacket.name[i] = filename[i+1];
      }
      filePacket.blockOffset = -1;
      filePacket.blockRead = -1;

      result = writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) );

      if( result )
      {
         CHAR buffer[sizeof(FILEINFOTABLE)+2];
         buffer[0] = '\0';
         result = readBuffer( buffer, sizeof(buffer), 258 );

         if( ! result )
         {
            // if we failed to read the correct number of bytes from
            // the camera, then it may be due to the fact that no
            // images were found -- consider this as NOT an error
            result = true;
         }
         else
         {
            // if the number of files...
            USHORT numberOfEntries = shortFromCamera(*((USHORT*)(&(buffer[0]))));
            logText( 0, "-> " + IString(numberOfEntries) + " entries match" );

            // ...is more than 0...
            if( shortFromCamera(numberOfEntries) > 0 )
            {
               // ...then get the full list...
               result = sendCommand( 0x99 );
               if( result )
               {
                  result = writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) );
                  if( result )
                  {
                     ULONG bufferSizeNeeded = numberOfEntries * sizeof(FILEINFOTABLE) + 2;
                     char *dirbuffer = new CHAR[bufferSizeNeeded];
                     result = readBuffer( dirbuffer, bufferSizeNeeded, 258 );
                     if( result )
                     {
                        // ...and for each directory entry...
                        FILEINFOTABLE *fileInfoTable = (FILEINFOTABLE*)&(dirbuffer[2]);
                        for( int i = 0; result && i < numberOfEntries; i++ )
                        {
                           // ...create an object in DCITU's container...
                           IString filename;
                           for( int j = 0; j < 8; j++ )
                           {  // copy the filename
                              filename += fileInfoTable[i].name[j];
                           }
                           filename += '.';
                           for( j = 8; j < 11; j++ )
                           {  // copy the extension
                              filename += fileInfoTable[i].name[j];
                           }

                           if(lookForFiles)
                           {
                              logText( 0, "-> " + filename);
                              wnd.cnr.addObject( new ImageObject(
                                    "\\PCCARD" + path,
                                    filename,
                                    longFromCamera(fileInfoTable[i].size),
                                    shortFromCamera(fileInfoTable[i].date),
                                    shortFromCamera(fileInfoTable[i].time) ) );
                           }
                           else
                           {
                              filename.strip().strip('.');
                              result = readDirectory( path + filename + "\\", true );
                           }
                        }
                     }
                     delete [] dirbuffer;
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
 *    Camera_Kodak_DC240::readLargeThumbnail
 *
 * read the large 24-bit thumbnail
 *
 *****************************************************************************/
Boolean Camera_Kodak_DC240::readLargeThumbnail( const ULONG number ) const
{
   Boolean result = false;

   logText( "Retrieving colour thumbnail for image " + IString(number) + "." );

   IString thumbnailFilename;
   ImageObject *obj = ImageObject::findObject(number);
   if( obj )
   {
      // get some information on the image (including thumbnail)
      if( sendCommand( 0x91 ) )
      {
         FILEPACKET filePacket = {0};

         IString filename(obj->path+obj->nameInCamera);
         logText( 0, "filename == \"" + filename + "\"" );
         for( int i=0; i<filename.length(); i++ )
         {
            filePacket.name[i] = filename[i+1];
         }
         filePacket.blockOffset = -1;
         filePacket.blockRead = -1;

         if( writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) ) )
         {
            IMAGEINFOTABLE imageInfoTable;
            if( readBuffer( (char*)&imageInfoTable, sizeof(IMAGEINFOTABLE), 258 ) )
            {
               ULONG dataSize = longFromCamera(imageInfoTable.thumbnailDataSize);

               logText(0, "type    == " + IString((int)imageInfoTable.cameraType));
               logText(0, "tSize   == " + IString(dataSize));
               logText(0, "tHeight == " + IString(shortFromCamera(imageInfoTable.thumbnailHeight)));
               logText(0, "tWidth  == " + IString(shortFromCamera(imageInfoTable.thumbnailWidth)));

               // tell the camera we want to get the thumbnail
               if( sendCommand( 0x93, 0, 0, 2 ) )
               {
                  // name the thumbnail we need
                  if( writeBuffer( (CHAR*)&filePacket, sizeof(FILEPACKET) ) )
                  {
                     CHAR *buffer = new CHAR[dataSize];
                     if( readBuffer( buffer, dataSize, 514 ) )
                     {
                        thumbnailFilename = wnd.connectionSettings.dir + "T_" + obj->name;  // change the name to start with "T_"

                        HFILE fileHandle;
                        ULONG action;
                        APIRET rc;

                        // now open the file locally
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
                           ULONG bytesWritten;
                           DosWrite( fileHandle, (PVOID)buffer, dataSize, &bytesWritten );

                           if( bytesWritten != dataSize )
                           {
                              logText( "Unable to save the thumbnail file.", "...ERROR: wrote " + IString(bytesWritten) + " of " + IString(dataSize) + " bytes" );
                           }
                           else
                           {
                              logText( "Finished transfering thumbnail." );
                              result = true;
                           }

                           DosResetBuffer( fileHandle );
                           DosClose( fileHandle );
                        }
                     }

                     delete [] buffer;
                  }
               }
            }
         }
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

            if(cinfo.image_width != 160 || cinfo.image_height != 120)
            {
               logText( "Unsuported thumbnail image size (" + IString(cinfo.image_width) + "x" + IString(cinfo.image_height) + ")." );
               result = false;
            }
            else
            {
               // scale the image size by 1/2 to make it fit into 96x72
               cinfo.scale_num = 1;
               cinfo.scale_denom = 2;

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

   return result;
}

