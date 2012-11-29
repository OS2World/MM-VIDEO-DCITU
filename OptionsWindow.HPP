/*
 * $Id: //devel/DCITU/OptionsWindow.HPP#6 $
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

#ifndef OptionsWindow_HPP
#define OptionsWindow_HPP

#include <icheckbx.hpp> // ICheckBox
#include <icmdhdr.hpp>  // ICommandHandler
#include <icolor.hpp>   // IColor
#include <icombobx.hpp> // IComboBox
#include <iedithdr.hpp> // IEditHandler
#include <ientryfd.hpp> // IEntryField
#include <iflytext.hpp> // IFlyText
#include <iflyhhdr.hpp> // IFlyOverHelpHandler
#include <ifocshdr.hpp> // IFocusHandler
#include <iframe.hpp>   // IFrameWindow
#include <ihelp.hpp>    // IHelpWindow
#include <imcelcv.hpp>  // IMultiCellCanvas
#include <inotebk.hpp>  // INotebook
#include <iprofile.hpp> // IProfile
#include <ipushbut.hpp> // IPushButton
#include <iradiobt.hpp> // IRadioButton
#include <iselhdr.hpp>  // ISelectHandler
#include <isetcv.hpp>   // ISetCanvas
#include <ispinnum.hpp> // INumericSpinButton
#include <ispintxt.hpp> // ITextSpinButton
#include <istattxt.hpp> // IStatictext
#include <istring.hpp>  // IString
#include <isysmenu.hpp> // ISystemMenu

#include "GUIWindow.hpp"


class OptionsWindow :
   public IFrameWindow,
   protected ICommandHandler,
   protected IFocusHandler,
   protected ISelectHandler,
   protected IEditHandler
{
   public:
      /* default constructor creates the "Options" notebook window
       */
      OptionsWindow( GUIWindow &ownerWindow );

      /* destructor removes the event handlers
       */
      ~OptionsWindow( VOID );

      /*
       * run the initialization on a second thread,
       * after the construction has been completed
       */
      void initialize(void);

      /* inherited from ISelectHandler; required to determine when
       * the camera type has been changed, or when a checkbutton has
       * been selected
       */
      virtual Boolean enter( IControlEvent &event );
      virtual Boolean selected( IControlEvent &event );

      /* inherited from ICommandHandler; required to process the
       * pushbutton messages from OK, CANCEL, and REGISTER
       */
      virtual Boolean systemCommand( ICommandEvent &event );
      virtual Boolean command( ICommandEvent &event );

      /* inherited from IFocusHandler; used to validate the "DIR" field when a
       * new directory is specified
       */
      virtual Boolean lostFocus( IControlEvent &event );

      /* inherited from IEditHandler; used to validate the "DIR" field when a
       * new directory is specified
       */
      virtual Boolean edit(IControlEvent &event);

      /* setup certain contols depending on the state of registration
       */
      VOID analyzeRegistration(VOID);

      /* reset all of the serial line controls to default values
       */
      VOID resetDefaultSerialLine(VOID);

      /* validate the transfer path; show the warning if the path is invalid
       */
      VOID validatePath(VOID);

      // instance variables
      IFlyText             flyText;
      IFlyOverHelpHandler  flyOverHelpHandler;
      Boolean              initializationDone;

      GUIWindow         &wnd;
      IMultiCellCanvas  cnvMain;
      INotebook         notebook;
      IFrameWindow      wndConnection;
      IFrameWindow      wndCamera;
      IFrameWindow      wndDirectory;
      IFrameWindow      wndPreference;
      IFrameWindow      wndScripts;
      IFrameWindow      wndRegister;
      IFrameWindow      wndSerial;

      // controls used on the "Connection" notebook page
      IComboBox         cbCameraType;
      ICheckBox         ckAdvancedCOMPort;
      ITextSpinButton   sbCOMPort;
      IEntryField       efCOMPort;
      ITextSpinButton   sbBaudRate;
      IComboBox         cbExtBuffers;
      ITextSpinButton   sbTimeout;
      ITextSpinButton   sbPause;
      ITextSpinButton   sbPauseAfterBaudChange;
      ITextSpinButton   sbRetryPartialTransfers;

      // controls used on the "Camera" notebook page
      ICheckBox         ckIgnoreMemoryCard;
      ICheckBox         ckConvertKDCToJPG;
      ICheckBox         ckRawPath;
      ICheckBox         ckRememberNumbering;
      IStaticText       txtNumbering;
      INumericSpinButton sbNumbering;
      ICheckBox         ckAlternateStartup;
      ICheckBox         ckKeepThumbnails;
      ICheckBox         ckIgnoreBreak;

      // controls used on the "Serial Line" notebook page
      ITextSpinButton   sbDTR;
      ITextSpinButton   sbRTS;
      ICheckBox         ckCTS;
      ICheckBox         ckDSR;
      ICheckBox         ckDCD;
      ICheckBox         ckInputDSR;
      ICheckBox         ckTransmitFlowControl;
      ICheckBox         ckReceiveFlowControl;
      IPushButton       pbResetSerial;

      // controls used on the "Transfer" notebook page
      IEntryField       efDir;
      IStaticText       txtDir;
      IStaticText       txtRelativePath;
      ICheckBox         ckDeleteFailedTransfer;

      // controls used on the "Preferences" notebook page
      ICheckBox         ckHighestPriority;
      ICheckBox         ckConnectOnStartup;
      ICheckBox         ckAutoDownloadThumbnails;
      ICheckBox         ck4bitThumbnails;
      ICheckBox         ck24bitThumbnails;
      ICheckBox         ckAutoDownloadImages;
      ICheckBox         ckOverwriteImages;
      ICheckBox         ckDeleteImages;

      // controls used on the "Scripts" notebook page
      ICheckBox         ckScriptAfterConnecting;
      ICheckBox         ckScriptAfterConnectingFg;
      IEntryField       efScriptAfterConnecting;
      ICheckBox         ckScriptAfterTransfer;
      ICheckBox         ckScriptAfterTransferFg;
      IEntryField       efScriptAfterTransfer;
      ICheckBox         ckScriptAfterDelete;
      ICheckBox         ckScriptAfterDeleteFg;
      IEntryField       efScriptAfterDelete;
      ICheckBox         ckScriptNoMoreImages;
      ICheckBox         ckScriptNoMoreImagesFg;
      IEntryField       efScriptNoMoreImages;
      ICheckBox         ckScriptDisconnect;
      ICheckBox         ckScriptDisconnectFg;
      IEntryField       efScriptDisconnect;

      // controls used on the "Register" notebook page
      IEntryField       efRegisterName;
      IEntryField       efRegisterKey;
      IPushButton       pbRegister;

      // canvas and controls used at the bottom of the notebook
      ISetCanvas        cnvPB;
      IPushButton       pbOk;
      IPushButton       pbCancel;
      IPushButton       pbHelp;

      IHelpWindow       helpWindow;
      ConnectionSettings &connectionSettings;
};

#endif

