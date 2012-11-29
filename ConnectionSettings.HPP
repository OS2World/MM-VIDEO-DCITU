/*
 * $Id: //devel/DCITU/ConnectionSettings.HPP#6 $
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

#ifndef ConnectionSettings_HPP
#define ConnectionSettings_HPP

#include <iprofile.hpp> // IProfile
#include <istring.hpp>  // IString
#include <os2.h>


class ConnectionSettings
{
   public:

      enum EXTENDED_BUFFER_MODE
      {
         // possible states of the extended buffers
         EXT_BUFFER_AUTO= 0,
         EXT_BUFFER_ON  = 1,
         EXT_BUFFER_OFF = 2
      };

      enum DTR_MODE
      {
         // possible states of the DTR serial control line
         DTR_DISABLE    = 0,
         DTR_ENABLE     = 1,
         DTR_HANDSHAKE  = 2
      };

      enum RTS_MODE
      {
         // possible states of the RTS serial control line
         RTS_DISABLE    = 0,
         RTS_ENABLE     = 1,
         RTS_HANDSHAKE  = 2,
         RTS_TOGGLE     = 3
      };

      /* default constructor checks the DCITU.INI file to see if any settings
       * have been previously saved, and loads them; if the .INI file does
       * not contain any settings, then the default values are used
       */
      ConnectionSettings( IProfile &profile );

      /* saves the current settings to the DCITU.LOG file -- called from the
       * menubar option on the main GUI window
       */
      void save( void );

      // instance variables
      USHORT   cameraType;
      Boolean  advancedComPort;
      USHORT   comPort;
      IString  advancedComPortName;
      ULONG    baudRate;
      USHORT   extendedBuffer;
      USHORT   timeout;
      USHORT   pauseBetweenCommands;
      USHORT   pauseAfterBaudChange;
      SHORT    queueSizeIncrease;
      Boolean  retryPartialTransfers;
      Boolean  ignoreMemoryCard;
      Boolean  convertKDCtoJPG;
      Boolean  useRawPath;
      Boolean  rememberNumbering;
      ULONG    resumeNumberingAt;
      Boolean  useAlternateStartup;
      Boolean  keepThumbnails;
      IString  dir;
      Boolean  deleteFailedTransfers;
      Boolean  useHighestPriority;
      Boolean  autoConnect;
      Boolean  autoDownloadThumbnails;
      Boolean  autoDownload4bitThumbnails;
      Boolean  autoDownload24bitThumbnails;
      Boolean  autoTransferImages;
      Boolean  overwriteDuplicates;
      Boolean  deleteAfterTransfer;
      Boolean  comPortAvailability[9];
      Boolean  ignoreBreak;

      Boolean  scriptAfterConnecting;
      Boolean  scriptAfterConnectingFg;
      IString  scriptAfterConnectingName;
      Boolean  scriptAfterTransfer;
      Boolean  scriptAfterTransferFg;
      IString  scriptAfterTransferName;
      Boolean  scriptAfterDelete;
      Boolean  scriptAfterDeleteFg;
      IString  scriptAfterDeleteName;
      Boolean  scriptNoMoreImages;
      Boolean  scriptNoMoreImagesFg;
      IString  scriptNoMoreImagesName;
      Boolean  scriptDisconnect;
      Boolean  scriptDisconnectFg;
      IString  scriptDisconnectName;

      USHORT   controlModeDTR;
      USHORT   controlModeRTS;
      Boolean  enableCTS;
      Boolean  enableDSR;
      Boolean  enableDCD;
      Boolean  enableInputDSR;
      Boolean  enableTransmitFlowControl;
      Boolean  enableReceiveFlowControl;

      IProfile profile;
};

#endif

