/*
 * $Id: //devel/DCITU/ImageObject.HPP#4 $
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

#ifndef ImageObject_HPP
#define ImageObject_HPP

#include <icnrctl.hpp>  // IContainerControl
#include <icnrobj.hpp>  // IContainerObject
#include <idate.hpp>    // IDate
#include <irect.hpp>    // IRectangle
#include <istring.hpp>  // IString
#include <itime.hpp>    // ITime

#include <os2.h>


// class prototype
class GUIWindow;


class ImageObject :
   public IContainerObject
{
   friend class IContainerColumn;

   public:
      /* constructor that takes the following set of variables:
       * 1) the path needed to access the image in the camera;
       * 2) filename needed to access the image in and out of the camera;
       * 3) size of the image, in bytes;
       * 4) date of the image;
       * 5) time of the image;
       * 6) if the image is on the card (default to true);
       * 7) if the image is in memory (default to false);
       * note that this constructor is mostly used with the Kodak-type of cameras
       * that provide much information about the images
       */
      ImageObject( CHAR *path, CHAR *name, ULONG size, USHORT date, USHORT time, Boolean card=true, Boolean memory=false );

      /* constructor that takes the following set of variables:
       * 1) the drive number needed to access the image in the camera
       * 2) the path needed to access the image in the camera
       * 3) filename needed to access the image in and out of the camera
       * 4) size of the image, in bytes
       * that that this constructor is mostly used with the Digita-based cameras
       */
      ImageObject( const ULONG driveNo, const CHAR *path, const CHAR *name, const ULONG size );

      /* constructor that takes the following set of variables:
       * 1) image number;
       * 2) image size;
       * note that this constructor is mostly used with the Project 'O' cameras
       * that provide virtually no information about the images
       */
      ImageObject( ULONG number, ULONG size );

      /* constructor that takes the following set of variables:
       * 1) image number
       * 2) image size, in bytes
       * 3) if the image is on the card
       * 4) if the image is in memory
       * note that this constructor is mostly used with the Kodak DC25 camera
       */
      ImageObject( ULONG number, ULONG size, Boolean card, Boolean memory );

      /* method called when the cursor object changes in the container; this
       * is needed to redraw the cursor emphasis since our container is not
       * a "standard" OCL container
       */
      virtual void handleCursoredChange( IContainerControl *container, Boolean acquired );

      /* method called when ENTER or double-click events takes place; start
       * direct-editing the object when this happens
       */
      virtual void handleOpen( IContainerControl *container );

      /* add a red line to the bottom of the thumbnail -- called after an image
       * has been downloaded
       */
      virtual void addRedLine( void );

      /* add a blue line to the bottom of the thumbnail -- called just before an
       * image has been downloaded
       */
      virtual void addBlueLine( void );

      // instance variables
      CHAR bits[96*72*3];  // bits that make up the thumbnail (96x72, in 24-bit colour)
      int day;
      int month;
      int year;
      int secs;
      int mins;
      int hour;
      ULONG driveNumber;// drive number used by the camera
      IString path;     // path used to access the image
      IString name;     // name used to access the image
      IString nameInCamera;// name used internally to the camera (i.e., Kodak)
      IString size;     // size of the image (in bytes, as IString)
      IString date;     // date the image was taken
      IString time;     // time the image was taken
      Boolean onCard;   // if the image is on the ATA card
      Boolean inMemory; // if the image is in memory
      IRectangle imageRect;   // rectangle used when refreshing the thumbnail
      ULONG cameraNumber;     // unique number used by the camera to track images
      ULONG originalNumber;   // unique number used by DCITU to track images

      // static variables and methods

      /* method used to set the refresh offset of each thumbnail within the
       * container window; it seems that the default font causes some type
       * of "offset" to be added to each container object
       */
      static void setRefreshOffset( int offset );

      /* find and return the container object that has the same unique original
       * number specificer as that of "number"; if no object matches, then a
       * null object is returned
       */
      static ImageObject *findObject( ULONG number );

      /* renumbers (subtracts 1 from the number) all images with numbers that
       * are equal to or greater than the value specified (i.e., if called
       * with parameter "6", then 6 would become 5, 7 would become 6, etc...)
       */
      static Boolean renumberFrom( ULONG number );

      static int offset;         // offset used by setRefreshOffset()
      static ULONG uniqueNumber; // static counter to keep unique numbers
      static GUIWindow *wnd;     // pointer to the main window
};

#endif

