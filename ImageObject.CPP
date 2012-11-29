/*
 * $Id: //devel/DCITU/ImageObject.CPP#4 $
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

#define INCL_WINWINDOWMGR

#include "ImageObject.HPP"
#include "GUIWindow.HPP"
#include <os2.h>


// static variables
int         ImageObject::offset        = 0;
ULONG       ImageObject::uniqueNumber  = 0;
GUIWindow  *ImageObject::wnd           = 0;


/*****************************************************************************
 *
 *    ImageObject::ImageObject
 *
 * constructor used by the Kodak DC120 & DC210 cameras
 *
 *****************************************************************************/
ImageObject::ImageObject( CHAR *path, CHAR *name, ULONG size, USHORT date, USHORT time, Boolean card, Boolean memory ) :
   IContainerObject( ),
   path(    path     ),
   name(    name     ),
   size(    size     ),
   onCard(  card     ),
   inMemory(memory   ),
   driveNumber(0     )
{
   // figure out the date from the value specified (ATA-type date format)
   day   = (date   ) & 0x1f;          // bits 0-4
   month = (date>>5) & 0x0f;          // bits 5-8
   year  =((date>>9) & 0x7f) + 1980;  // bits 9-15
   this->date = IDate(day, (IDate::Month)month, year).asString("%d %b %Y");

   // figure out the time from the value specified (ATA-type time format)
   secs = (time<<1) & 0x3f;           // bits 0-4 multiplied by 2 (bits 0-5)
   mins = (time>>5) & 0x3f;           // bits 5-10
   hour = (time>>11)& 0x1f;           // bits 11-15
   this->time = ITime(hour, mins, secs).asString("%X");

   // initialize the bits used to draw the image
   for( int i=0; i<(96*72*3); i++ )
   {
      bits[i] = 204;
   }

   // remember the number of this image -- based on the uniqueNumber static variable
   cameraNumber = uniqueNumber ++;
   originalNumber = cameraNumber;

   // allow direct editing of this object (to change the image name)
   enableDataUpdate();
   nameInCamera = name;

   return;
}


/*****************************************************************************
 *
 *    ImageObject::ImageObject
 *
 * constructor used by the Digita cameras
 *
 *****************************************************************************/
ImageObject::ImageObject( const ULONG driveNo, const CHAR *path, const CHAR *name, const ULONG size ) :
   IContainerObject( ),
   driveNumber(   driveNo  ),
   path(          path     ),
   name(          name     ),
   size(          size     ),
   onCard(        true     ),
   inMemory(      false    )
{
   // remember the number of this image -- based on the uniqueNumber static variable
   cameraNumber = uniqueNumber ++;
   originalNumber = cameraNumber;

   // set the default date & time
   day   =        IDate::today().dayOfMonth();
   month = (int)  IDate::today().monthOfYear();
   year  =        IDate::today().year();
   date  =        IDate::today().asString("%d %b %Y");

   secs  =        ITime::now().seconds();
   mins  =        ITime::now().minutes();
   hour  =        ITime::now().hours();
   time  =        ITime::now().asString("%X");

   // initialize the bits used to draw the image
   for( int i=0; i<(96*72*3); i++ )
   {
      bits[i] = 204;
   }

   // allow direct editing of this object (to change the image name)
   enableDataUpdate();
   nameInCamera = name;

   return;
}


/*****************************************************************************
 *
 *    ImageObject::ImageObject
 *
 * constructor used by the Project 'O' and Casio cameras
 *
 *****************************************************************************/
ImageObject::ImageObject( ULONG number, ULONG size ) :
   IContainerObject(       ),
   cameraNumber(  number   ),
   originalNumber(number   ),
   path(          ""       ),
   size(          size     ),
   onCard(        false    ),
   inMemory(      true     ),
   driveNumber(   0        )
{
   // should we use ever-increasing numbers?
   if(wnd->connectionSettings.rememberNumbering)
   {
      name = IString(wnd->connectionSettings.resumeNumberingAt).rightJustify(8, '0');

      // now remember the new counter value so we can be certain to never re-use the same number
      if(wnd->connectionSettings.resumeNumberingAt == 999999)
         wnd->connectionSettings.resumeNumberingAt = 0;
      else
         wnd->connectionSettings.resumeNumberingAt++;
   }
   else
   {
      // generate a filename from the image number
      name = IString(number).rightJustify(8, '0');
   }

   if( size == 0 )
   {
      name += ".cam";
   }
   else
   {
      name += ".jpg";
   }

   day   =        IDate::today().dayOfMonth();
   month = (int)  IDate::today().monthOfYear();
   year  =        IDate::today().year();
   date  =        IDate::today().asString("%d %b %Y");

   secs  =        ITime::now().seconds();
   mins  =        ITime::now().minutes();
   hour  =        ITime::now().hours();
   time  =        ITime::now().asString("%X");

   // initialize the bits used to draw the image
   for( int i=0; i<(96*72*3); i++ )
   {
      bits[i] = 204;
   }

   // allow direct editing of this object (to change the image name)
   enableDataUpdate();
   nameInCamera = name;

   return;
}


/*****************************************************************************
 *
 *    ImageObject::ImageObject
 *
 * constructor used by the Kodak DC25 camera
 *
 *****************************************************************************/
ImageObject::ImageObject( ULONG number, ULONG size, Boolean card, Boolean memory ) :
   IContainerObject(       ),
   cameraNumber(  number   ),
   originalNumber(number   ),
   path(          ""       ),
   size(          size     ),
   onCard(        card     ),
   inMemory(      memory   ),
   driveNumber(   0        )
{
   // generate a filename from the image number
   name = card?"c":"m";
   name += IString(number).rightJustify(7,'0') + ".dc2";

   day   =        IDate::today().dayOfMonth();
   month = (int)  IDate::today().monthOfYear();
   year  =        IDate::today().year();
   date  =        IDate::today().asString("%d %b %Y");

   secs  =        ITime::now().seconds();
   mins  =        ITime::now().minutes();
   hour  =        ITime::now().hours();
   time  =        ITime::now().asString("%X");

   // initialize the bits used to draw the image
   for( int i=0; i<(96*72*3); i++ )
   {
      bits[i] = 204;
   }

   // allow direct editing of this object (to change the image name)
   enableDataUpdate();
   nameInCamera = name;

   return;
}


void ImageObject::handleCursoredChange( IContainerControl *container, Boolean acquired )
{
   // force the bitmap to redraw itself
   if( imageRect.height() == 0 )
   {
      // if a refresh rectangle doesn't exist, then refresh the entire object
      refresh();
   }
   else
   {
      // otherwise, refresh only the rectangle (prevents the CnrCtl from flashing)
      RECTL refreshRect = imageRect.expandedBy(IPair(0,4)).movedBy(IPair(0,offset)).asRECTL();
      WinInvalidateRect( container->handle(), &refreshRect, true );
   }
   return;
}


void ImageObject::handleOpen(IContainerControl *container)
{
   wnd->logText( 0, "...editing image name for " + name + " (number " + IString(originalNumber) + ")" );
   container->editObject(this, &wnd->col2);

   return;
}


void ImageObject::addRedLine()
{
   for( int i=0; i<96*2; i++ )
   {
      // draw 2 red lines at the bottom of the thumbnail
      bits[i*3+0] = 0;
      bits[i*3+1] = 0;
      bits[i*3+2] = 255;
   }
   refresh();

   return;
}


void ImageObject::addBlueLine()
{
   for( int i=0; i<96*2; i++ )
   {
      // draw 2 blue lines at the bottom of the thumbnail
      bits[i*3+0] = 255;
      bits[i*3+1] = 0;
      bits[i*3+2] = 0;
   }
   refresh();

   return;
}


void ImageObject::setRefreshOffset( int offset )
{
   // remember the offset by which the refresh rectangle has to be moved
   ImageObject::offset = offset;
   return;
}


ImageObject *ImageObject::findObject( ULONG number )
{
   // find the object with the specified dcitu number

   wnd->logText( 0, "...looking for object number " + IString(number) );

   ImageObject *obj = (ImageObject*)0;

   // create an object cursor to search through all of the objects
   IContainerControl::ObjectCursor cursor( wnd->cnr, IContainerObject::none );
   for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
   {
      obj = (ImageObject*)cursor.current();
      if( obj->originalNumber == number )
      {
         // this is the object we want
         break;
      }
      else
      {
         // forget about this object
         obj = (ImageObject*)0;
      }
   }

   if( obj )
   {
      wnd->logText( 0, "...found object number " + IString(obj->originalNumber) + " (using camera number " + IString(obj->cameraNumber) + ")" );
   }
   else
   {
      wnd->logText( 0, "WARNING: could not find object number " + IString(number) );
   }

   return obj;
}


Boolean ImageObject::renumberFrom( ULONG number )
{
   Boolean result = true;

   // this renumber will not change the original numbering, only the camera numbering

   ImageObject *obj = (ImageObject*)0;

   // create an object cursor to search through all of the objects
   IContainerControl::ObjectCursor cursor( wnd->cnr, IContainerObject::none );
   for( cursor.setToFirst(); cursor.isValid(); cursor.setToNext() )
   {
      obj = (ImageObject*)cursor.current();
      if( obj->cameraNumber >= number )
      {
         // renumber this object
         obj->cameraNumber -=1;
      }
   }

   return result;
}

