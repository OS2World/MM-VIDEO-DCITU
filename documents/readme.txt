 _________________________________________________________________________
!                                                                         !
!  DCITU v1.8j - Digital Camera Image Transfer Utility for OS/2           !
!_________________________________________________________________________!

   This application was written in my spare time from information
   gathered on various WWW sites.

   Previous versions of this tool (once upon a time, back in 1997 when I
   was single and still had lots of free time on my hands) were called
   "DC210" since it was originally written specifically for use with
   Kodak's DC210 camera.  More than 70 digital cameras are now supported,
   not to mention the fact that it now has a graphical user interface,
   on-line context-sensitive help, support for thumbnails, etc...!

 _________________________________________________________________________
!                                                                         !
!  New in this release                                                    !
!_________________________________________________________________________!

   - DCITU is now open source; no other modifications have been made
   - image filenames can be edited directly from the main GUI window
        Note: 1) press ENTER or double-click to change an image's name
              2) an image name cannot be changed while it is downloading

 _________________________________________________________________________
!                                                                         !
!  Quick notes for those who cannot wait to get started                   !
!_________________________________________________________________________!

   - run "DCITU.EXE" to start; no command-line parameters are required
   - verify the options and select your camera type in Settings->Options
   - click on Camera->Connect to get started

 _________________________________________________________________________
!                                                                         !
!  Contact information                                                    !
!_________________________________________________________________________!

   The author of DCITU can be contacted at stephanecharette@telus.net.

   Note that DCITU is no longer supported, though I will make every
   attempt to help out anyone who has questions.  I would like to
   personally thank everyone who registered DCITU in the last 5 years.
   
   DCITU's web page is at http://charette.dyn.tj/.

   Please consult the web site for up-to-date information, including the
   full list of cameras and models that are known to work with DCITU.  I
   will try to keep the web site up and operational as long as I can.

 _________________________________________________________________________
!                                                                         !
!  OS/2 revisions supported                                               !
!_________________________________________________________________________!

   DCITU should work with any version of OS/2 starting with v3.0.

 _________________________________________________________________________
!                                                                         !
!  Camera models supported                                                !
!_________________________________________________________________________!

   DCITU was written for use with the following cameras:

   - Kodak DC220, Kodak DC260, Kodak DC265, Minolta Dimage:
      1) select "Digita" as the camera type
      2) thumbnails are only in grayscale (this is being worked on)
      3) some thumbnails may not work (this is being worked on)
      4) camera must be set to "connect" mode

   - Kodak DC240 & Kodak DC280
      1) camera must be set to "connect" mode

   - Kodak DC200, Kodak DC210 & Kodak DC215:
      1) infra-red transfers not supported
      2) camera must be set to "connect" mode

   - Kodak DC120:
      1) albums not supported; images must be saved/moved to "NO ALBUM"
      2) thumbnails are not supported
      3) image delete is not supported
      4) attach cable, slide cover to "ON/OPEN", click "FILE->CONNECT"
      5) there is a bug if more than 12 images are in the camera (this
         is being worked on)

   - Kodak DC25:
      1) also download "KDC_DC2.ZIP" to convert .dc2 image files
      2) supports both internal and external memory image files
      3) image delete is not supported
      4) date and time not supported

   - Agfa ePhoto, Epson PhotoPC, Nikon, Olympus, Sanyo VPC/DSC, Sierra
     SD640 & Toshiba PDR-2 (cameras known as Project/Protocol "O")
      1) some models may require a 2 or 3 second timeout period
      2) some models may require the "alternate startup" to be selected
      3) some models may require a pause between commands
      4) date and time not supported

   - Casio:
      1) these cameras are no longer supported

 _________________________________________________________________________
!                                                                         !
!  Use of this software                                                   !
!_________________________________________________________________________!

   This application is fairly straight-forward to use.

   Basically:

   1) make certain your camera is plugged in using the serial cable
   2) turn on the camera
   3) set the camera to serial connection mode (if applicable)
   4) run "DCITU.EXE"
   5) verify the settings in the options notebook
   6) click on "OK" to close the options notebook
   7) select "Connect" from the menubar


   +--NOTE TO LAN SERVER AND OS/2 PEER USERS-----------------------------+
   !                                                                     !
   !  If you mount a serial port over the network, you can download      !
   !  images onto a computer while the camera is hooked up somewhere     !
   !  else on the network.  For example, on my home network, I have the  !
   !  camera hooked up to serial port 1 of one particular computer while !
   !  I connect to a Peer redirected COM port from a second computer.    !
   !                                                                     !
   +---------------------------------------------------------------------+

   Starting with version 1.8a, a new command-line switch has been added
   to allow loading the .INI settings from a specific drive, directory,
   or .INI file.  Use the parameter  /i=x:\foo\bar.ini  to specify the
   file to use.  One such example would be:

      dcitu.exe /i=c:\apps\public\myfile.ini

   Starting with version 1.8d, a new setting in the .INI file has been
   included which is *not* viewable with the DCITU options notebook.
   Open the DCITU.INI file using an OS/2 INI editor such as REGEDIT2.EXE
   and view the setting called "Queue Size Increase".  This value, which
   defaults to "100", dictates how the message queue should be increased.
   This number *must* be larger than the number of images that DCITU
   deals with.

 _________________________________________________________________________
!                                                                         !
!  Distribution and registration                                          !
!_________________________________________________________________________!

   This application used to be distributed as freeware/shareware.

   As of version 1.8j, DCITU is now "open source".  There are a number
   of reasons for this, many of which can be guessed.  My decision to
   make DCITU "open source" is based on:

   1) lack of support for OS/2; IBM no longer provides OS/2, nor the
      C++ compiler and library used to create DCITU (VisualAge C++ and
      the Open Class Library -- OCL -- is still the best compiler and
      library I have ever used; in my opinion, IBM made a very large
      mistake in killing both OS/2 and the best commercial compiler
      then available for OS/2 and Windows)

   2) diminished user install base; the small number of OS/2 users
      remaining does not warrant spending any additional time and
      effort working on OS/2 tools such as DCITU

   3) limited source of income; over the 5 year lifespan of DCITU, I
      estimate that DCITU has earned me about $6 per hour, which isn't
      anywhere near enough money to allow me to purchase new cameras
      for testing (1500 hours, 300 registered users @ $30 per user)

 _________________________________________________________________________
!                                                                         !
!  Disclaimer                                                             !
!_________________________________________________________________________!

   I AM NOT RESPONSIBLE FOR ANY DAMAGES THAT CAN OCCUR FROM USING THIS
   APPLICATION.  YOU ACCEPT WHATEVER RISKS INVOLVED, INCLUDING BUT NOT
   LIMITED TO, DAMAGES TO BOTH YOUR COMPUTER AND YOUR DIGITAL CAMERA.

   On a lighter note:  I am not aware of any damages that _could_ occur
   from using this application.  I have been using it on my own systems
   with a Kodak DC260, Kodak DC240, Kodak DC210, and an Olympus D220, all
   without problems.  Other camera models were tested by various people
   contacted through the internet.

 _________________________________________________________________________
!                                                                         !
!  Legal notice of contributions                                          !
!_________________________________________________________________________!

   Kodak DC220 & DC260 camera support is based in part on documentation
   made available on the DIGITA digital camera operating system and by
   initial programming efforts by David LaRue from david.larue@acm.org

   Kodak DC280 camera support is based in part on Eastman Kodak's
   "DC280 Host Interface Specification" documentation at www.kodak.com

   Kodak DC240 camera support is based in part on Eastman Kodak's
   "DC240 Host Interface Specification" documentation at www.kodak.com

   Kodak DC200, DC210 & DC215 camera support is based in part on Eastman
   Kodak's "DC210 Host Interface Specification" documentation at
   www.kodak.com

   Kodak's DC120 .KDC file format support is based in part on
   documentation from www.hamrick.com/dc120

   Kodak's DC20/25 camera support is based in part on documentation
   from home.t-online.de/home/Oliver.Hartmann/dc20prot.htm

   "Project O" camera support is based in part on documentation
   from www.best.com/~rudnicki/olycmd.html

   Thumbnail support for "Project O", Kodak DC240 & Kodak DC280 cameras
   is based in part on the work of the Independent JPEG Group at
   ftp.uu.net/graphics/jpeg

 _________________________________________________________________________
!                                                                         !
!  End of README.TXT                                                      !
!_________________________________________________________________________!


