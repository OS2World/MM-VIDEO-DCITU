/*
 * $Id: //devel/DCITU/GUIWindow.H#10 $
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

#ifndef GUIWindow_H
#define GUIWindow_H

/*
 * Version history for the DC210 Image Transfer Utility (later renamed to "DCITU")
 *
 * v1.0  First working version, text-mode, allows the download of filenames from camera, SC, 1997Dec23
 * v1.1  Downloads both filenames and file sizes from the camera, SC, 1997Dec24
 * v1.2  Completed full file download.  First version released on the internet, SC, 1997Dec26
 * v1.3  Allows for the change of transfer rate, SC, 1997Dec29
 * v1.4  First working graphical (OCL) version, SC, 1997Dec29-30
 * v1.5  Ported connection framework from v1.2 to graphical shell, SC, 1997Dec31
 * v1.6  Implemented custom container for memory bitmaps, SC, 1998Jan1-3
 * v1.7  Completed large and small thumbnails & baud rate, SC, 1998Jan4
 * v1.7a Implemented image transfer, SC, 1998Jan4
 * v1.8  Bit-tracked the DATE and TIME format, SC, 1998Jan5-6
 * v1.8a Included user-registration code, SC, 1998Jan6-7
 * v1.8b Added selection menus, SC, 1998Jan8
 * v1.8c Added delete menu, SC, 1998Jan8
 * v1.8d Added container menu, SC, 1998Jan9
 * v1.8e Added exception handling to thread 3, debug info and critical sections, SC, 1998Jan10
 * v1.8f Fixed error with short registration names not recognized, SC, 1998Jan18
 * v1.8g Keep the image date/time for use with the downloaded file, SC, 1998Jan18
 * v1.8h Added timeout spinbutton, SC, 1998Jan21
 * v1.8i Added extended buffer override spinbutton, SC, 1998Jan23
 *
 * code for the DC210 application frozen at v1.8i, SC, 1998Jan24
 * renamed application to Digital Camera Image Transfer Utility (DCITU), SC, 1998Jan24
 *
 * v1.0  Started re-working code to use connection inheritance model, SC, 1998Jan24-26
 * v1.0a First working version (Kodak DC210) using new code structure, SC, 1998Jan26
 * v1.1  Added preliminary support for Kodak DC50 and Kodak DC120, SC, 1998Jan27-28
 * v1.1a Resolved timing issues in the following situations:
 *       - between the "Find Port" and "Connect" thread
 *       - when the debug window is deleted while debug information is being added
 *       - between the main window and the debug window, SC, 1998Jan30
 * v1.1b Changed DC120 to use 0x4a(2), 0x55 & 0x99 instead of 0x4a(1), SC, 1998Jan30-31
 * v1.1c Added .KDC file conversion for the DC120, SC, 1998Feb01
 * v1.1d Various work on the DC120 support, SC, 1998Feb02
 * v1.2  Preliminary work for the "Project O" cameras (Epson, Olympus, etc), SC, 1998Feb04-05
 * v1.2a Added "read/write word" for the "Project O" cameras, SC, 1998Feb06-07
 * v1.2b Fixed bug in KDC->JPG which used to slow conversion (Kodak DC120), SC, 1998Feb07
 * v1.2c Fixed checksum error in "Project O" cameras, SC, 1998Feb08
 * v1.2d Implemented "delete image" and "transfer image" for the "Project O" cameras, SC, 1998Feb10
 * v1.2e New init code, and removed double "power down" code for "Project O" cameras, SC, 1998Feb15
 * v1.2f Added loop to purge extra characters in queue when closing port, SC, 1998Feb15, 1998Feb18
 * v1.2g Re-wrote power up function for "Project O" cameras, SC, 1998Feb20
 * v1.2h Added alternate power up function, worked on checksum, SC, 1998Feb22
 * v1.2i Fixed reversed packet buffers during image transfer, SC, 1998Feb23
 * v1.2j Added option to pause between commands, SC, 1998Feb25
 * v1.2k First public release with "Project O" support included, SC, 1998Feb26
 * v1.3  Added context-sensitive help for the Options window, SC, 1998Feb27
 * v1.3a Fixed image renumbering bug with "Project O" cameras, SC, 1998Feb27
 * v1.3b Changed registration check in options window, SC, 1998Mar04
 * v1.3c Moved addRedLine() to ImageObject and added addBlueLine(), SC, 1998Mar05
 * v1.3d Added code to write "Project O" thumbnails as seperate files, SC, 1998Mar05
 * v1.3e Enabled caching for .LOG file, second attempt to download thumbnails, SC, 1998Mar10
 * v1.3er2 Added "power up" and "power down" around thumbnail transfers, SC, 1998Mar10
 * v1.3er3 Re-disabled caching (again) for the .LOG file, SC, 1998Mar10
 * v1.3f Fixed bug where deleting an image twice produced a SYS3175, SC, 1998Mar11
 * v1.3g Removed scan for COM ports and added BMT Micro information, SC, 1998Mar12
 * v1.3h Fixed bug with "sleep between commands" (seconds, not milliseconds!), SC, 1998Mar14
 * v1.3i Added alternate close method for Warp 3 and earlier, SC, 1998Mar18,19
 * v1.3j Added handshaking detection code for preliminary test, SC, 1998Mar19
 * v1.3k Added code to reset handshaking and flow control, SC, 1998Mar22
 * v1.3l First version to correctly download Project O thumbnails, SC, 1998Mar23-24
 * v1.3m Included IJG code to process JPEG thumbnails from Project O cameras, SC, 1998Mar25
 * v1.3n Public release of stable code from v1.3m, SC, 1998Mar25
 * v1.4a Re-write of the options notebook, SC, 1998Apr18-21, 1998May2-3
 * v1.4b Added code to support auto-download of thumbnails and images, SC, 1998May6
 * v1.4c Added message purge feature, SC, 1998May6
 * v1.4d Rewrote help file for use with the new options notebook, SC, 1998May12
 * v1.4e Various small bits of functionality:
 *       - added flyover help to the main window
 *       - added flyover help to the options window
 *       - added "delete failed transfers" functionality to images
 *       - added "keep thumbnail files" functionality for "Project O" cameras
 *       - added "convert KDC to JPG" functionality for Kodak DC120 cameras, SC, 1998May16
 * v1.4f Added extra code to purge characters in queue when checksum error occur, SC, 1998May19
 * v1.4g Added advanced COM port selection & fixed DCBINFO problem, SC, 1998May25
 * v1.4h Added "purge" menu item with same functionality as the latch button, SC, 1998May27
 * v1.4i Public release of stable code from v1.4h, SC, 1998June1
 * v1.4j Added "Script" functionality, SC, 1998June2-3, 13
 * v1.4k Added "ignore memory card" for some Kodak cameras without flash cards, SC, 1998July6
 * v1.5  Added support for Kodak DC25, 1998July6-12
 * v1.6  Added preliminary support for Casio QV10, 1998July18-26
 * v1.6a Added new notebook page for fine tuning of the serial line, SC, 1998July29-Aug02
 * v1.6b Changed handling for closing the debug window, SC, 1998Aug8
 * v1.6c Changed a few small things to create a new official version, including:
 *       - added new on-line help in the options notebook
 *       - added path verification in options notebook
 *       - temporarily removed beta code for DC50 and Casio QV10, SC, 1998Oct11
 * v1.6d Added option to retry partial transfers, SC, 1998Oct20-30
 * v1.6e Added option to pause after a change of baud rate, SC, 1998Nov11
 * v1.7  Started support for the DIGITA cameras, SC & DL, 1998Dec13
 * v1.7a Beta release with fix for SYS3175 when exiting, SC, 1998Dec31
 * v1.7b First working version with DIGITA support, SC, 1999Jan10-31
 * v1.7c Added "delete" and "thumbnails" for DIGITA cameras, SC, 1999Feb3-6
 * v1.7d Added "Minolta Dimage" as DIGITA-compatible camera, SC, 1999Feb24
 * v1.8  Added "Kodak DC240" and fixed thumbnail problem with new Olympus cameras, SC, 1999July17-25
 * v1.8a Added DC200/210/215 distinction, added support for the new Kodak DC280, SC, 1999August25
 * v1.8b Various small changes:
 *       - first version under CVS
 *       - changed "pause between commands" to msecs
 *       - fixed loading help file when dcitu is loaded for a different directory
 *       - change to the transfer directory while running
 *       - added pauses between commands for "Project O", SC, 1999Sept02-04
 * v1.8c Various small changes:
 *       - Project 'O' cameras now support 230400 rates (do any of the cameras?)
 *       - added 2 new fields to the statusbar, SC, 1999Sept13-14
 * v1.8d Various small changes:
 *       - Project 'O' cameras now ignore "Power Up/Down" if additional commands are queued
 *       - scripts are now started minimized, hidden, and zorder'd behind DCITU
 *       - added option to increase the camera thread's queue size, SC, 1999Sept19
 * v1.8e Various small changes:
 *       - ALT-F4 can now be used to close options notebook
 *       - input focus fixed for options notebook
 *       - added FG/BG capability for each script
 *       - show message when transfer drive/directory is invalid
 *       - select ALL/NONE/TOGGLE now works on second thread, SC, 1999Sept26-27,Oct04
 * v1.8f Various small changes:
 *       - added BREAK for Project 'O' cameras to reset on connection
 *       - added some more "pause between commands"
 *       - changed layout on the "scripts" page
 * v1.8g Various small fixes to the Project 'O' camera and IJG jpeg library, SC, 1999Nov10
 * v1.8h Image filenames are now editable, SC, 2000Feb5-6,15,27
 * v1.8hr2 Added option to skip sending BREAK signal, SC, 2000Mar01
 * v1.8i - fixed crashes due to items missing from .INI file
 *       - added warning about relative path names
 *       - removed double "power down" instances in Project O cameras
 *       - pause before downloading thumbnails & images on connect
 *       - updated .IPF documentation, SC, 2000May7
 * v1.8j Open source version; full download speed enabled, SC, 2002Sep20
 */

// debug_logging is used in CAMERA.CPP (and a few other isolated places) to
// display every byte read from and written to the COM port; this should be
// set to 0 (zero) for production versions to speed up execution time
//
// NOTE: while debug_logging had a turn at fame, I've decided to change the
// way debugging works -- now, the base Camera class checks to see if the
// debugging window is enabled, and skips part of the code if it is
// currently closed.  This will prevent me from having to distribute a
// separate "debug" compile to certain customers when they have a problem,
// while maintaining a fast executable when the debug window is disabled.
//
// Leave DEBUG_LOGGING set to 1.
#define DEBUG_LOGGING         1

// string constants
#define STR_TITLE             "DCITU for OS/2"
#define STR_VERSION_MAJOR     "1"
#define STR_VERSION_MINOR     "8"
#define STR_VERSION_REV       "j [GPL'd]"
#define STR_DATE              "2002 September 20"
#define STR_EMAIL             "stephanecharette@telus.net, http://charette.dyn.tj/"
#define STR_NAME              "Stephane Charette"
#define STR_REG_KEY           "\x02\x6A\x12\x1A\x11\x5D\x97\x62\xA4\x13\xCE\x7D\x83\x47\xCC\x36\x8D\x1B"

// resource ids
#define WND_RCID              1000
#define CNV_MAIN_RCID         1001
#define CNV_STATUS_RCID       1002
#define SLIDER_RCID           1003
#define CNR_RCID              1004
#define EF_STATUS_RCID        1005
#define EF_BAUD_RCID          1006
#define EF_SELECTED_RCID      1007
#define PB_PURGE_RCID         1008

// menubar ids
#define MB_CONNECT_RCID       1050
#define MB_DISCONNECT_RCID    1051
#define MB_TRANSFER_RCID      1052
#define MB_OPTIONS_RCID       1053
#define MB_SAVE_OPTIONS_RCID  1054
#define MB_OUTPUT_RCID        1055
#define MB_ABOUT_RCID         1056
#define MB_THUMBNAIL_RCID     1057
#define MB_COLOUR_TN_RCID     1058
#define MB_GRAYSCALE_TN_RCID  1059
#define MB_SELECT_RCID        1060
#define MB_SELECT_ALL_RCID    1061
#define MB_SELECT_NONE_RCID   1062
#define MB_SELECT_TOGGLE_RCID 1063
#define MB_DELETE_RCID        1064
#define MB_PURGE_RCID         1065
#define POPUP_MENU_RCID       1066
#define MB_MOVE_FOCUS_RCID    1067

// debug window ids
#define DEBUG_WND_RCID        1100
#define DEBUG_LB_RCID         1101

// help identifiers
#define HELPTABLE_RCID        1500
#define HELP_OPTIONS_SUBTABLE 1501

#endif

