/*
 * $Id: //devel/DCITU/main.CPP#4 $
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

#include "GUIWindow.H"
#include "GUIWindow.HPP"

#include <iapp.hpp>     // IApplication
#include <iexcbase.hpp> // IException
#include <imsgbox.hpp>  // IMessageBox

int main(int argC, char *argV[])
{
   try
   {
      // keep the arguments
      IApplication::current().setArgs(argC, argV);

      // see what .INI file we should be using
      IString iniFile("DCITU.INI");

      if(argC > 0)
      {
         iniFile = IApplication::current().argv(0).subString(1, IApplication::current().argv(0).lastIndexOfAnyOf("/\\")) + iniFile;
      }

      // look for "/I=" or "/i=" parameter for the .INI file
      if(argC > 1)
      {
         for(int i=1; i<argC; i++)
         {
            if(IString::upperCase(IApplication::current().argv(i)).isLike("/I=*"))
            {
               iniFile = IString::remove(IApplication::current().argv(i), 1, 3);
            }
         }
      }

      GUIWindow *wnd = new GUIWindow(iniFile);
      wnd->setFocus();
      wnd->logText( 0, "running main application message loop" );
      IApplication::current().run();
      delete wnd;
   }
   catch( IException &exception )
   {
      // exception occured -- show the associated text!
      IMessageBox msgBox(  IWindow::desktopWindow() );
      msgBox.  setTitle(   "DCITU Thread 1 Exception Error" ).
               show(       exception.text()        ,
                           IMessageBox::okButton   |
                           IMessageBox::errorIcon  |
                           IMessageBox::moveable   );
   }
   catch( ... )
   {
      IMessageBox msgBox(  IWindow::desktopWindow() );
      msgBox.  setTitle(   "DCITU Thread 1 Exception Error" ).
               show(       "Unknown exception."    ,
                           IMessageBox::defButton1 |
                           IMessageBox::okButton   |
                           IMessageBox::errorIcon  |
                           IMessageBox::moveable   );
   }

   return 0;
}


