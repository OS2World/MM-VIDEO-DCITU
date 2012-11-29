/*
 * $Id: //devel/DCITU/ConnectionSettings.CPP#8 $
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

#include "ConnectionSettings.HPP"
#include "GUIWindow.H"
#include "Camera.HPP"


ConnectionSettings::ConnectionSettings( IProfile &profile ) :
   cameraType(             Camera::KODAK_DC210  ),
   advancedComPort(        false                ),
   comPort(                1                    ),
   advancedComPortName(    "\\dev\\com1"        ),
   baudRate(               Camera::BAUD_9600    ),
   extendedBuffer(         EXT_BUFFER_AUTO      ),
   timeout(                3                    ),
   pauseBetweenCommands(   0                    ),
   pauseAfterBaudChange(   200                  ),
   queueSizeIncrease(      100                  ),
   retryPartialTransfers(  true                 ),
   ignoreMemoryCard(       false                ),
   convertKDCtoJPG(        true                 ),
   useRawPath(             false                ),
   rememberNumbering(      true                 ),
   resumeNumberingAt(      0                    ),
   useAlternateStartup(    false                ),
   keepThumbnails(         true                 ),
   dir(                    ".\\"                ),
   deleteFailedTransfers(  true                 ),
   useHighestPriority(     false                ),
   autoConnect(            false                ),
   autoDownloadThumbnails( true                 ),
   autoDownload4bitThumbnails(   true           ),
   autoDownload24bitThumbnails(  true           ),
   autoTransferImages(     false                ),
   overwriteDuplicates(    true                 ),
   deleteAfterTransfer(    true                 ),
   ignoreBreak(            false                ),

   scriptAfterConnecting(  false                ),
   scriptAfterConnectingFg(false                ),
   scriptAfterConnectingName( ""                ),
   scriptAfterTransfer(    false                ),
   scriptAfterTransferFg(  false                ),
   scriptAfterTransferName(""                   ),
   scriptAfterDelete(      false                ),
   scriptAfterDeleteFg(    false                ),
   scriptAfterDeleteName(  ""                   ),
   scriptNoMoreImages(     false                ),
   scriptNoMoreImagesFg(   false                ),
   scriptNoMoreImagesName( ""                   ),
   scriptDisconnect(       false                ),
   scriptDisconnectFg(     false                ),
   scriptDisconnectName(   ""                   ),

   controlModeDTR(         DTR_ENABLE           ),
   controlModeRTS(         RTS_ENABLE           ),
   enableCTS(              false                ),
   enableDSR(              false                ),
   enableDCD(              false                ),
   enableInputDSR(         false                ),
   enableTransmitFlowControl( false             ),
   enableReceiveFlowControl(  false             ),

   profile(                profile              )
{
   // mark all COM ports as unavailable
   for( int i=0; i<9; i++ )
   {
      comPortAvailability[i] = false;
   }

   // are the connection settings in the .INI file?
   if( profile.containsApplication("Connection Settings") )
   {
      // load the values from the .INI file
      profile.setDefaultApplicationName( "Connection Settings" );

      if( profile.containsKeyName( "COM Port" ) )
      {
         comPort              = profile.integerWithKey( "COM Port"               );
         baudRate             = profile.integerWithKey( "BAUD Rate"              );
         dir                  = profile.elementWithKey( "Directory"              );
         deleteAfterTransfer  = profile.integerWithKey( "Delete After Transfer"  );
         overwriteDuplicates  = profile.integerWithKey( "Overwrite Duplicates"   );
         autoConnect          = profile.integerWithKey( "Automatically Connect"  );
         timeout              = profile.integerWithKey( "Timeout"                );
         extendedBuffer       = profile.integerWithKey( "Extended Buffer"        );
         cameraType           = profile.integerWithKey( "Camera Type"            );
      }
      if( profile.containsKeyName( "Alternate Startup" ) )
      {
         // option added at version 1.2h
         useAlternateStartup = profile.integerWithKey( "Alternate Startup"    );
      }
      if( profile.containsKeyName( "Pause Between Commands" ) )
      {
         // option added at version 1.2j
         pauseBetweenCommands = profile.integerWithKey( "Pause Between Commands" );

         // versions prior to 1.8b used to store this value in seconds (between
         // 0 and 30), while newer versions now store in milliseconds (in 50ms
         // steps between 0 and 2000 seems reasonable)
         if( (pauseBetweenCommands > 0) && (pauseBetweenCommands <= 30) )
         {
            pauseBetweenCommands *= 1000;
         }
      }

      if( profile.containsKeyName( "Keep Thumbnails" ) )
      {
         // options added at version 1.4a
         convertKDCtoJPG            = profile.integerWithKey( "Convert KDG to JPG"              );
         keepThumbnails             = profile.integerWithKey( "Keep Thumbnails"                 );
         deleteFailedTransfers      = profile.integerWithKey( "Delete Failed Transfers"         );
         autoDownloadThumbnails     = profile.integerWithKey( "Auto Download Thumbnails"        );
         autoDownload4bitThumbnails = profile.integerWithKey( "Auto Download 4-bit Thumbnails"  );
         autoDownload24bitThumbnails= profile.integerWithKey( "Auto Download 24-bit Thumbnails" );
         autoTransferImages         = profile.integerWithKey( "Auto Transfer Images"            );
      }
      if( profile.containsKeyName( "Advanced COM Port" ) )
      {
         // options added at version 1.4g
         advancedComPort            = profile.integerWithKey( "Advanced COM Port"      );
         advancedComPortName        = profile.elementWithKey( "Advanced COM Port Name" );
      }
      if( profile.containsKeyName( "Script After Connecting" ) )
      {
         // options added at version 1.4j
         scriptAfterConnecting      = profile.integerWithKey( "Script After Connecting"      );
         scriptAfterConnectingName  = profile.elementWithKey( "Script After Connecting Name" );
         scriptAfterTransfer        = profile.integerWithKey( "Script After Transfer"        );
         scriptAfterTransferName    = profile.elementWithKey( "Script After Transfer Name"   );
         scriptAfterDelete          = profile.integerWithKey( "Script After Delete"          );
         scriptAfterDeleteName      = profile.elementWithKey( "Script After Delete Name"     );
         scriptNoMoreImages         = profile.integerWithKey( "Script No More Images"        );
         scriptNoMoreImagesName     = profile.elementWithKey( "Script No More Images Name"   );
         scriptDisconnect           = profile.integerWithKey( "Script Disconnect"            );
         scriptDisconnectName       = profile.elementWithKey( "Script Disconnect Name"       );
      }
      if( profile.containsKeyName( "Ignore Memory Card" ) )
      {
         // options added at version 1.4k
         ignoreMemoryCard           = profile.integerWithKey( "Ignore Memory Card"     );
      }
      if( profile.containsKeyName( "Control Mode DTR" ) )
      {
         // options added at version 1.6a
         controlModeDTR             = profile.integerWithKey( "Control Mode DTR"       );
         controlModeRTS             = profile.integerWithKey( "Control Mode RTS"       );
         enableCTS                  = profile.integerWithKey( "Enable CTS"             );
         enableDSR                  = profile.integerWithKey( "Enable DSR"             );
         enableDCD                  = profile.integerWithKey( "Enable DCD"             );
         enableInputDSR             = profile.integerWithKey( "Input DSR"              );
         enableTransmitFlowControl  = profile.integerWithKey( "Transmit Flow Control"  );
         enableReceiveFlowControl   = profile.integerWithKey( "Receive Flow Control"   );
      }
      if( profile.containsKeyName( "Retry Partial Transfers" ) )
      {
         // options added at version 1.6d
         retryPartialTransfers      = profile.integerWithKey( "Retry Partial Transfers");
         pauseAfterBaudChange       = profile.integerWithKey( "Pause After Baud Change");
      }
      if( profile.containsKeyName( "Use Raw Path" ) )
      {
         // options added at version 1.8a
         useRawPath                 = profile.integerWithKey( "Use Raw Path"           );
         useHighestPriority         = profile.integerWithKey( "Use Highest Priority"   );
      }
      if( profile.containsKeyName( "Queue Size Increase" ) )
      {
         // options added at version 1.8d
         queueSizeIncrease          = profile.integerWithKey( "Queue Size Increase"    );
      }
      if( profile.containsKeyName( "Script After Connecting Fg" ) )
      {
         // options added at version 1.8e
         scriptAfterConnectingFg    = profile.integerWithKey( "Script After Connecting Fg");
         scriptAfterTransferFg      = profile.integerWithKey( "Script After Transfer Fg"  );
         scriptAfterDeleteFg        = profile.integerWithKey( "Script After Delete Fg"    );
         scriptNoMoreImagesFg       = profile.integerWithKey( "Script No More Images Fg"  );
         scriptDisconnectFg         = profile.integerWithKey( "Script Disconnect Fg"      );
      }
      if( profile.containsKeyName( "Remember Numbering" ) )
      {
         // options added at version 1.8h
         rememberNumbering          = profile.integerWithKey( "Remember Numbering"        );
      }
      if( profile.containsKeyName( "Numbering Resumes At" ) )
      {
         // options added at version 1.8h

         /* This item was split off since it is possible for the .INI file to have
          * only this value and nothing else when the user runs DCITU for the first
          * time, hasn't saved the .INI file, transfered some images, and then
          * re-started DCITU.
          */
         resumeNumberingAt          = profile.integerWithKey( "Numbering Resumes At"      );
      }
      if( profile.containsKeyName( "Ignore BREAK" ) )
      {
         // options added at version 1.8hr2
         ignoreBreak                = profile.integerWithKey( "Ignore BREAK"              );
      }
   }

   return;
}


void ConnectionSettings::save()
{
   // save the settings in the profile
   profile.setDefaultApplicationName(   "Connection Settings"                          );
   profile. addOrReplaceElementWithKey( "COM Port",               comPort              ).
            addOrReplaceElementWithKey( "BAUD Rate",              baudRate             ).
            addOrReplaceElementWithKey( "Directory",              dir                  ).
            addOrReplaceElementWithKey( "Delete After Transfer",  deleteAfterTransfer  ).
            addOrReplaceElementWithKey( "Overwrite Duplicates",   overwriteDuplicates  ).
            addOrReplaceElementWithKey( "Automatically Connect",  autoConnect          ).
            addOrReplaceElementWithKey( "Timeout",                timeout              ).
            addOrReplaceElementWithKey( "Extended Buffer",        extendedBuffer       ).
            addOrReplaceElementWithKey( "Camera Type",            cameraType           ).
            // added for version 1.2h
            addOrReplaceElementWithKey( "Alternate Startup",      useAlternateStartup  ).
            // added for version 1.2j
            addOrReplaceElementWithKey( "Pause Between Commands", pauseBetweenCommands ).
            // added for version 1.4a
            addOrReplaceElementWithKey( "Convert KDG to JPG",     convertKDCtoJPG      ).
            addOrReplaceElementWithKey( "Keep Thumbnails",        keepThumbnails       ).
            addOrReplaceElementWithKey( "Delete Failed Transfers",deleteFailedTransfers).
            addOrReplaceElementWithKey( "Auto Download Thumbnails",autoDownloadThumbnails).
            addOrReplaceElementWithKey( "Auto Download 4-bit Thumbnails",autoDownload4bitThumbnails).
            addOrReplaceElementWithKey( "Auto Download 24-bit Thumbnails",autoDownload24bitThumbnails).
            addOrReplaceElementWithKey( "Auto Transfer Images",   autoTransferImages   ).
            // added for version 1.4g
            addOrReplaceElementWithKey( "Advanced COM Port",      advancedComPort      ).
            addOrReplaceElementWithKey( "Advanced COM Port Name", advancedComPortName  ).
            // added for version 1.4j
            addOrReplaceElementWithKey( "Script After Connecting",scriptAfterConnecting).
            addOrReplaceElementWithKey( "Script After Connecting Name",scriptAfterConnectingName).
            addOrReplaceElementWithKey( "Script After Transfer",  scriptAfterTransfer  ).
            addOrReplaceElementWithKey( "Script After Transfer Name",scriptAfterTransferName).
            addOrReplaceElementWithKey( "Script After Delete",    scriptAfterDelete    ).
            addOrReplaceElementWithKey( "Script After Delete Name",scriptAfterDeleteName).
            addOrReplaceElementWithKey( "Script No More Images",  scriptNoMoreImages   ).
            addOrReplaceElementWithKey( "Script No More Images Name",scriptNoMoreImagesName).
            addOrReplaceElementWithKey( "Script Disconnect",      scriptDisconnect     ).
            addOrReplaceElementWithKey( "Script Disconnect Name", scriptDisconnectName ).
            // added for version 1.4k
            addOrReplaceElementWithKey( "Ignore Memory Card",     ignoreMemoryCard     ).
            // added for version 1.6a
            addOrReplaceElementWithKey( "Control Mode DTR",       controlModeDTR       ).
            addOrReplaceElementWithKey( "Control Mode RTS",       controlModeRTS       ).
            addOrReplaceElementWithKey( "Enable CTS",             enableCTS            ).
            addOrReplaceElementWithKey( "Enable DSR",             enableDSR            ).
            addOrReplaceElementWithKey( "Enable DCD",             enableDCD            ).
            addOrReplaceElementWithKey( "Input DSR",              enableInputDSR       ).
            addOrReplaceElementWithKey( "Transmit Flow Control",  enableTransmitFlowControl ).
            addOrReplaceElementWithKey( "Receive Flow Control",   enableReceiveFlowControl ).
            // added for version 1.6d
            addOrReplaceElementWithKey( "Retry Partial Transfers",retryPartialTransfers).
            addOrReplaceElementWithKey( "Pause After Baud Change",pauseAfterBaudChange ).
            // added for version 1.8a
            addOrReplaceElementWithKey( "Use Raw Path",           useRawPath           ).
            addOrReplaceElementWithKey( "Use Highest Priority",   useHighestPriority   ).
            // added for version 1.8d
            addOrReplaceElementWithKey( "Queue Size Increase",    queueSizeIncrease    ).
            // added for version 1.8e
            addOrReplaceElementWithKey( "Script After Connecting Fg",scriptAfterConnectingFg ).
            addOrReplaceElementWithKey( "Script After Transfer Fg",  scriptAfterTransferFg   ).
            addOrReplaceElementWithKey( "Script After Delete Fg",    scriptAfterDeleteFg  ).
            addOrReplaceElementWithKey( "Script No More Images Fg",  scriptNoMoreImagesFg ).
            addOrReplaceElementWithKey( "Script Disconnect Fg",      scriptDisconnectFg   ).
            // added for version 1.8h
            addOrReplaceElementWithKey( "Remember Numbering",     rememberNumbering    ).
            addOrReplaceElementWithKey( "Numbering Resumes At",   resumeNumberingAt    ).
            // added for version 1.8hr2
            addOrReplaceElementWithKey( "Ignore BREAK",           ignoreBreak          );

   return;
}

