.*
.* $Id: //devel/DCITU/dcitu.ipf#9 $
.*
.* DCITU for OS/2 -- Digital Camera Image Transfer Utility for OS/2
.* Copyright (C) 1997-2000 Stephane Charette, stephanecharette@telus.net
.* Copyright (C) 2000-2002 C Code Run Consulting & Investments, Inc.
.*
.* This program is free software; you can redistribute it and/or modify
.* it under the terms of the GNU General Public License as published by
.* the Free Software Foundation; either version 2 of the License, or at
.* your option, any later version.
.*
.* This program is distributed in the hope that it will be useful,
.* but WITHOUT ANY WARRANTY; without even the implied warranty of
.* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
.* General Public License for more details.
.*

#include "GUIWindow.H"
#include "OptionsCanvas.H"
:userdoc.
:title.Digital Camera Image Transfer Utility for OS/2
.*
.*
.*
:h1 res=WND_OPTIONS.Help for the Options window
:p.The settings in the Options window must be set before you can connect to
your camera.  These settings are temporary unless you chose :hp3.Save
Options:ehp3. from the menubar.
.*
.*
.*
:h2 res=CB_CAMERA_TYPE id=CB_CAMERA_TYPE.Camera Type
:p.This combobox is used to select the digital camera to which a connection
will be made.  At this time, these are the only camera types that are
supported&colon.
:ul compact.
:li.Digita (:hp1.see note below:ehp1.)
:ul compact.
:li.Kodak DC220
:li.Kodak DC260
:li.Kodak DC265
:li.Kodak DC290
:li.Minolta Dimage 1500
:eul.
:li.Kodak DC25 (:hp1.see note below:ehp1.)
:li.Kodak DC120 (:hp1.see note below:ehp1.)
:li.Kodak DC200, DC210 and DC215
:li.Kodak DC240
:li.Kodak DC280
:li.Kodak DC290
:li.Project &apos.O&apos.
:ul compact.
:li.Agfa ePhoto
:li.Epson PhotoPC
:li.Nikon Coolpik
:li.Olympus
:li.Sanyo VPC/DSC
:li.Sierra SD640
:li.Toshiba PDR-2
:eul.
:eul.
:p.If your camera does not appear in this list, and you wish to know if it is
compatible, I suggest you try the following model types&colon.
:ol compact.
:li.Olympus
:li.Digita
:li.Kodak DC240 (with :link reftype=hd refid=CK_USE_RAW_PATH.raw directory path:elink. enabled)
:li.Kodak DC210
:li.Kodak DC120
:li.Kodak DC25
:eol.
:note.This option takes effect the next time a connection is established.
:note.You cannot change the camera type if a connection has already been
established.
:note.The Casio QV digital cameras are no longer supported.
:note.You may need the OS&slash.2 tools contained in the archive KDC_DC2.ZIP if
you are using the Kodak DC25 or Kodak DC120 digital cameras.  The file
KDC_DC2.ZIP can be downloaded free of charge from various locations on the
internet, including
:hp2.ftp&colon.&slash.&slash.hobbes.nmsu.edu&slash.pub&slash.os2&slash.apps&slash.graphics&slash.convert&slash.kdc_dc2.zip:ehp2..
:note.Some thumbnail formats for the DIGITA cameras are not supported.
.*
.*
.*
:h2 res=CK_ADVANCED_COM_PORT id=CK_ADVANCED_COM_PORT.Advanced COM Port
:p.In the default deselected state, the advanced COM port selection is
disabled, forcing you to select a standard COM port such as COM1, etc.
When you select this checkbox, you can then type the true device name to
which your camera is connected, including network serial ports.  Also
see :link reftype=hd refid=EF_COM_PORT.Advanced COM Port Name:elink..
:note.If you are using a normal serial port, you should leave this checkbox
deselected.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=EF_COM_PORT id=EF_COM_PORT.Advanced COM Port Name
:p.If you have a special multi-port serial card installed with the appropriate
OS/2 drivers, you can select a name other than COM.  For example, the
RocketPort card by default installs itself as &bslash.dev&bslash.rck1 through
&bslash.dev&bslash.rckN.
:p.You can also use network serial ports by specifying the NETBIOS name of the
shared serial port.  For example, you could use the shared serial port COM7 on
the computer WORKSTATION by using the name
&bslash.&bslash.WORKSTATION&bslash.COM7.
:note.The :link reftype=hd refid=CK_ADVANCED_COM_PORT.Advanced COM Port:elink.
checkbox must be selected for this option to be available.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=SB_COM_PORT id=SB_COM_PORT.COM Port
:p.This spinbutton is used to select the serial COM port to which the camera
is attached.  Previous versions of DCITU used to scan the COM ports to try
and determine if a serial port was available; however, it was suspected that
this was causing some problems with certain hardware, so all serial COM ports
are now listed regardless of the port&apos.s availability.
:p.:hp2.While all possible serial ports available to OS/2 are listed, please note
that not all of these may be available on your computer!  You should select
the port to which the camera has been attached.:ehp2.
:p.Valid selections include physical serial ports available on your computer,
or ports that have been re-directed using either Peer or IBM Lan Server.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=SB_BAUD_RATE id=SB_BAUD_RATE.Transfer Speed
:p.The transfer rate is the speed at which information is sent from the digital
camera to the computer.  The maximum value you can use for the transfer rate is
determined by the following three factors&colon.
:ol compact.
:li.rate supported by the serial port hardware
:li.rate supported by the camera model
:li.unregistered/registered version of DCITU
:eol.
:p.Most newer computers have built-in serial ports that can easily handle a baud
rate of 115200.  Some older computers without buffered serial ports will only
handle 9600 or 19200 baud.  While OS/2 supports serial transfer rates as high as
230400, this normally requires additional hardware and/or special drivers.
:p.Note that the unregistered version of DCITU will only use speeds of 9600 baud,
regardless of what transfer rate you have selected.  This is the only difference
between the registered and unregistered version of DCITU.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=CB_EXT_BUFFERS id=CB_EXT_BUFFERS.Extended Buffers
:p.Newer serial ports built-in to most computers use extended buffers to
achieve greater reliability at higher transfer rates.  These extended buffers
are sometimes referred to as 16550 UARTs.  Using the :hp1.Extended Buffers:ehp1.
combobox, you can choose to&colon.
:ul compact.
:li.automatically detect and use extended buffering when available
:li.force OS/2 to try and enable extended buffering even if not detected
:li.force OS/2 to disable extended buffering
:eul.
:note.In most cases, the default setting of :hp1.auto-detect:ehp1. is the best
option available.  The other two options should only be used if you are using
special hardware with specific OS/2 drivers that may not correctly detect the
presence of a 16550-type UART.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=SB_TIMEOUT id=SB_TIMEOUT.Timeout Detection
:p.The timeout detection value is used by DCITU to determine the length of time
to wait for the camera to send information.  If this amount of time has elapsed
without the camera sending any information, then DCITU assumes that the camera
has no more information to send, or (depending on the circumstance) that an
error has occured.
:ul.
:li.With the Kodak cameras, this setting can be kept at 5 seconds.
:li.With most :link reftype=hd refid=CB_CAMERA_TYPE.Project &apos.O&apos.:elink.-type
cameras, this setting should be less than 5; a value of 2 or 3 seems to be
sufficient in most cases.
:eul.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=SB_PAUSE_BETWEEN_COMMANDS id=SB_PAUSE_BETWEEN_COMMANDS.Pause Between Commands
:p.Some camera models may encounter errors while trying to transfer or delete multiple
images at once.  This may be caused by DCITU sending commands too quickly to the
digital camera.  If you can transfer or delete a single image, but cannot select
multiple images and transfer or delete them at once, try increasing the number of
milliseconds that DCITU pauses between commands.
:note.See the option :link reftype=hd refid=SB_BAUD_RATE_PAUSE.Pause After
Changing Baud Rate:elink. before adding a pause between commands.
:note.This option takes effect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=SB_BAUD_RATE_PAUSE id=SB_BAUD_RATE_PAUSE.Pause After Changing Baud Rate
:p.To properly synchronize, some cameras need to pause after having changed the
baud rate at which communications with the computer take place.  Prior to
version 1.6e of DCITU, there was fixed pause of 100ms for all of the Kodak-type
cameras, while all others had no pause (0ms).  A minimum of 100ms or 200ms for
almost all camera types will probably help ensure that communications do not
break down.
:note.Setting a too short or too long pause time will cause DCITU and your
camera to either lose synchronization or timeout, possibly requiring you to
reconnect.
:note.This option takes effect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=SB_RETRY_PARTIALS id=SB_RETRY_PARTIALS.Retry Partial Transfers
:p.Some serial port hardware may report incomplete serial byte transfers.  In
these cases, multiple requests must be made to the hardware to transfer a full
block of bytes from the serial port to DCITU.  This behaviour has been observed
on multi-port serial cards, such as the RocketPort and the DigiBoard, as well
as on standard serial ports when Ray Gwinn&apos.s :hp2.SIO.SYS:ehp2. driver is
used instead of IBM&apos.s normal :hp2.COM.SYS:ehp2. driver.
:p.By default, this option should normally be enabled, regardless of the driver
or serial port type being used.
:note.This option takes effect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=CK_IGNORE_MEMORY_CARD id=CK_IGNORE_MEMORY_CARD.Ignore Memory Card
:p.If you do not have an ATA flash memory card in the camera, select this
item to let DCITU know that the calls to initialize the ATA card should be
ignored.
.*
.*
.*
:h2 res=CK_CONVERT_KDC_TO_JPG id=CK_CONVERT_KDC_TO_JPG.Convert KDC to JPG
:p.Compressed .KDC-format files used with the Kodak DC120 camera can be
automatically converted to .JPG-format when this option is selected.  This
conversion is the same as that accomplished by the :hp1.KDC-JPG tool:ehp1..
:note.Uncompressed .KDC files are not converted to .BMP format.  This can
only be done with the command-line conversion tools available from
:hp2.ftp&colon.&slash.&slash.hobbes.nmsu.edu&slash.pub&slash.os2&slash.apps&slash.graphics&slash.convert&slash.kdc_dc2.zip:ehp2..
:note.This option is only available with the Kodak DC120 cameras.
.*
.*
.*
:h2 res=CK_USE_RAW_PATH id=CK_USE_RAW_PATH.Use Raw Directory Path
:p.With the Kodak DC240 and DC280 digital camera, the images are stored in
subdirectories that are grouped together using a combination of numbers and
model identifiers.  Normally, when DC240 or DC280 is selected as the camera
type, only the respective DC240 or DC280 directories are searched for images.
When :hp2.Use Raw Directory Path:ehp2. is selected, all directories found on
the ATA card will be searched.
:note.The Kodak cameras may not allow some non-Kodak JPEG files to be downloaded
even though the file is listed in DCITU.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=CK_REMEMBER_NUMBERING id=CK_REMEMBER_NUMBERING.Remember Last Image Number
:p.Unless this option is selected, DCITU will always use filenames based on a
sequence of numbers beginning with 0 (zero).  This may cause different images
to have the same filename, thus resulting in overwritten files and perhaps
lost images.  When this checkbox is selected, DCITU will never re-use the same
image sequence number until it has reached 999999, at which point the numbers
begin again at 000000.
:p.
However, it should be noted that the same image may be downloaded multiple
times with different names if it isn&apos.t deleted from the camera after
having been downloaded.  This can happen if you connect to your camera,
download an image, disconnect from the camera, reconnect again, and download
the same image.  Because DCITU never re-uses the same filename, the image will
have been downloaded with different filenames.
:note.This option takes effect the next time a connection is established.
:note.This option is only available with the Project &apos.O&apos.-type cameras.
.*
.*
.*
:h2 res=SB_LAST_IMAGE_NUMBER id=SB_LAST_IMAGE_NUMBER.Next Available Number
:p.When a connection is established, DCITU will name the images according to a
counter.  This counter is incremented for every image available on the camera.
To preset the counter value, set the spinbutton to the desired value.  This
option only takes effect when
:link reftype=hd refid=CK_REMEMBER_NUMBERING.Remember Last Image Number:elink.
has been selected.
:note.This option takes effect the next time a connection is established.
:note.This option is only available with the Project &apos.O&apos.-type cameras.
.*
.*
.*
:h2 res=CK_ALTERNATE_STARTUP id=CK_ALTERNATE_STARTUP.Use Alternate Camera Startup
:p.Some :link reftype=hd refid=CB_CAMERA_TYPE.Project &apos.O&apos.:elink.-type
cameras use a different startup sequence when establishing a connection.  If
you cannot get a valid connection, and you&apos.ve already seen the note
concerning the :link reftype=hd refid=SB_TIMEOUT.timeout
detection:elink., then you may want to set this option.  Some digital cameras
(such as the Olympus C2000?) may also require the :link reftype=hd
refid=CK_IGNORE_BREAK.ignore break:elink. option to connect with DCITU.
:note.This option takes effect the next time a connection is established.
:note.This option is only available with the Project &apos.O&apos.-type cameras.
.*
.*
.*
:h2 res=CK_KEEP_THUMBNAILS id=CK_KEEP_THUMBNAILS.Keep Thumbnail Files
:p.When you download thumbnail images from the camera, this creates temporary
image files which can then be deleted, or kept for use as pre-made thumbnails.
Selecting this option will keep these temporary images.
:note.This option is only available with the Project &apos.O&apos.-type cameras.
.*
.*
.*
:h2 res=CK_IGNORE_BREAK id=CK_IGNORE_BREAK.Ignore BREAK Signal
:p.Generally, the :hp2.ignore break:ehp2. option should not be selected.
:p.If DCITU cannot connect to your supported digital camera, then select this
option to skip the BREAK signal.  This signal is sent by DCITU when a
connection is being established to the digital camera; however, some digital
cameras may not expect a BREAK signal, and a connection cannot be established.
:p.Thus far, this behaviour has been observed with an Olympus C2000, though
other cameras -- especially those from the Olympus or "Project O" family --
may also be affected.
:note.For :link reftype=hd refid=CB_CAMERA_TYPE.Project
&apos.O&apos.:elink.-type cameras, please also refer to the :link reftype=hd
refid=CK_ALTERNATE_STARTUP.alternate startup:elink. and :link reftype=hd
refid=SB_TIMEOUT.timeout detection:elink. options.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=ID_HOW_TO_SET_SERIAL_LINE id=ID_HOW_TO_SET_SERIAL_LINE.How to setup the serial line
:p.The serial line settings can greatly affect the communication between your
computer and digital camera.  Unless you have a special camera and&slash.or
serial port adapter that requires a specific setting, you should use the
following defaults&colon.
:ol compact.
:li.DTR control mode&colon. enable
:li.RTS control mode&colon. enable
:li.output handshacking using CTS&colon. not selected
:li.output handshacking using DSR&colon. not selected
:li.output handshacking using DCD&colon. not selected
:li.input sensitivity using DSR&colon. not selected
:li.automatic transmit flow control&colon. not selected
:li.automatic receive flow control&colon. not selected
:eol.
:p.Using the pushbutton "reset to default for selected camera" will
reset the spinbuttons and checkboxes to these default values.
:note.This option takes effect the next time a connection is established.
.*
.*
.*
:h2 res=EF_TRANSFER_DIR id=EF_TRANSFER.Image Transfer Directory
:p.The directory specified will be used to save images when they are being
transferred from the digital camera.  The default is to use the current
working directory.
:note.Starting with version 1.8b, DCITU will actually change the current
drive and directory to the transfer directory to help ensure that user-defined
scripts can be easily called.  It is recommended that you use full path names
versus relative path names to prevent DCITU from confusing the current
drive and directory.  I.e., if DCITU is installed in
:hp2.D&colon.&bslash.software&bslash.dcitu:ehp2. and you wish to have your
images downloaded to :hp2.D&colon.&bslash.software&bslash.my_images:ehp2., the
name :hp2...&bslash.my_images&bslash.:ehp2. is more likely to cause problems
than :hp2.D&colon.&bslash.software&bslash.my_images&bslash.:ehp2..
:note.While UNC path names can be used to save images, DCITU is obviously
unable to change the working drive and directory to a UNC path.  The simplest
solution is to map the UNC path before starting DCITU.  I.e., call :hp2.net
use z: &bslash.&bslash.foo&bslash.images:ehp2. and then set DCITU to use
drive :hp2.z&colon.&bslash.:ehp2..
:note.DCITU will automatically append the trailing backslash to the directory
name.
:note.This option takes effect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=CK_DELETE_FAILED_TRANSFERS id=CK_DELETE_FAILED_TRANSFERS.Delete Failed Image Transfers
:p.If an image transfer is unsuccessful, partial or empty image files can
remain in the image transfer directory.  Selecting this option will delete
files that result from failed image transfers.
.*
.*
.*
:h2 res=CK_USE_HIGHEST_PRIORITY id=CK_USE_HIGHEST_PRIORITY.Use Highest Priority
:p.Starting with version 1.8a of DCITU, the :hp2.Use Highest Priority:ehp2. option
was created to allow DCITU to run at the highest possible priority level.  This
can be used on busy systems to help DCITU prevent incoming serial port characters
from being lost.
:p.Enabling this option will change the GUI thread and DCITU&apos.s serial port service
thread to run at OS/2&apos.s foreground server priority.  Without this option, the
GUI thread runs at normal priority while the serial port service thread runs at
time critical priority.
:p.
From lowest to highest, OS/2&apos.s thread priority is defined as follows:
:ol compact.
:li.idle priority
:li.regular priority
:li.time critical priority
:li.foreground server priority
:eol.
:note.Enabling this option may cause the rest of the system, including OS/2&apos.s Workplace Shell, to seem unresponsive.
:note.This option has no effect until the next time DCITU is started.
.*
.*
.*
:h2 res=CK_CONNECT_ON_STARTUP id=CK_CONNECT_ON_STARTUP.Connect to Camera on Startup
:p.When selected, this option will cause DCITU to automatically attempt to
connect to the camera when first started.  The last-saved options will be
used to determine the camera type, baud rate, and other settings.
:note.This option has no effect until the next time DCITU is started.
.*
.*
.*
:h2 res=CK_AUTO_DOWNLOAD_THUMBNAILS id=CK_AUTO_DOWNLOAD_THUMBNAILS.Auto Download Thumbnails on Connect
:p.When selected, this option ensures that the thumbnails are automatically
downloaded from the camera when a connection is established.
:note.This option is not available with the Kodak DC120 cameras.
.*
.*
.*
:h2 res=CK_4_BIT_THUMBNAIL id=CK_4_BIT_THUMBNAIL.Use 4-bit Thumbnails
:p.When :link reftype=hd refid=CK_AUTO_DOWNLOAD_THUMBNAILS.Auto
Download Thumbnails on Connect:elink. is selected, this option
will enable the transfer of the 4-bit thumbnails.
:note.This option is only available with the Kodak DC210 cameras.
.*
.*
.*
:h2 res=CK_24_BIT_THUMBNAIL id=CK_24_BIT_THUMBNAIL.Use 24-bit Thumbnails
:p.When :link reftype=hd refid=CK_AUTO_DOWNLOAD_THUMBNAILS.Auto
Download Thumbnails on Connect:elink. is selected, this option
will enable the transfer of the 24-bit thumbnails.
:note.This option is not available with the Kodak DC120 cameras.
.*
.*
.*
:h2 res=CK_AUTO_DOWNLOAD_IMAGES id=CK_AUTO_DOWNLOAD_IMAGES.Auto Transfer Images on Connect
:p.When selected, this option will auto download all of the images
from the camera after a connection is established.  If the
:link reftype=hd refid=CK_AUTO_DOWNLOAD_THUMBNAILS.Auto Download
Thumbnails on Connect:elink. option is selected, all of the thumbnails
will be transfered before the images.
.*
.*
.*
:h2 res=CK_OVERWRITE_IMAGES id=CK_OVERWRITE_IMAGES.Overwrite Images with Identical Names
:p.When selected, this option will cause existing files to be overwritten when
images are being transfered.  Without this option being selected, images that
already exist in the transfer directory will not be downloaded.
:note.This option takes effect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=CK_DELETE_IMAGES id=CK_DELETE_IMAGES.Delete Images From Camera after Transfer
:p.When selected, this option will delete from the camera images that were
successfully transfered.
:note.This option is not available with the Kodak DC120 and Kodak DC25 cameras.
:note.This option takes affect the next time an image is downloaded from the
camera.
.*
.*
.*
:h2 res=ID_HOW_TO_USE_SCRIPTS id=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts
:p.To run a script, you must have selected the appropriate checkbox and have
a script name specified in the corresponding entryfield.  If a name is not
specified, or the checkbox is not selected, then the script will not be started.
:p.Names that can be specified as scripts include OS/2 executables and REXX,
Perl, or other scripts.  When the script filename does not end with .EXE
or .COM, you may have to start a secondary shell to run the script.  For example,
if you want to run the script :hp2.MY_REXX_SCRIPT.CMD:ehp2., you must use the
name :hp2.CMD.EXE /C MY_REXX_SCRIPT.CMD:ehp2..
:p.You can use any normal command processor, such as :hp2.4OS2.EXE:ehp2., etc.
The same example would then be :hp2.4OS2.EXE /C MY_REXX_SCRIPT.CMD:ehp2..
:p.Some of the scripts have special variables that are automatically replaced
before the script is called.  If you specify :hp2.%i:ehp2. as a parameter to the
script that gets called after a connection is established, this is automatically
replaced with the number of images in the camera.  If you specify :hp2.%s:ehp2. as
a parameter to the script that gets called when an image is transfered or deleted,
this is automatically replaced with the name of the image.  For example, specifying
:hp2.C&colon.\PMVIEW\PMVIEW.EXE %s:ehp2. as the script to call when an image is transfered will call
PMView with the name of the image file.
:note.Starting with version 1.8b, DCITU will actually change the current
drive and directory to the transfer directory to help ensure that user-defined
scripts can be easily called.
:note.Starting with version 1.8d, DCITU can specify whether to start a script in the
foreground, or minimized and hidden.  This works especially well with command shell
scripts such as REXX or Perl called from :hp2.CMD.EXE:ehp2. or
:hp2.4OS2.EXE:ehp2.  Please see :link reftype=hd refid=ID_HOW_TO_SET_FG_SCRIPTS.How
to Use Foreground Scripts:elink. for additional information.
.*
.*
.*
:h2 res=ID_HOW_TO_SET_FG_SCRIPTS id=ID_HOW_TO_SET_FG_SCRIPTS.How to Use Foreground Scripts
:p.Each script can be run either in the foreground, or background, depending on
the state of the :hp2.foreground:ehp2. checkbox.  When running in the background, the scripts
will be hidden, though still accessible through the OS&slash.2 Window List.
:p.When the :hp2.foreground:ehp2. checkbox is selected, the
script will be run in the foreground; unselecting the checkbox will run the script in
the background.
:note.Many PM applications ignore the visibility flags and start with a default window.
These applications are therefore not affected by the :hp2.foreground:ehp2. checkbox.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=CK_SCRIPT_AFTER_CONNECTION id=CK_SCRIPT_AFTER_CONNECTION.After Connecting to the Camera (checkbox)
:p.Select this checkbox to run a script after a connection is established between DCITU and the camera.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_SCRIPT_AFTER_CONNECTION id=EF_SCRIPT_AFTER_CONNECTION.After Connecting to the Camera (entryfield)
:p.This is the name of the script to run after a connection is established between DCITU and the camera.
:p.The parameter :hp2.%i:ehp2., if specified, is replaced with the number of images.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=CK_SCRIPT_AFTER_TRANSFER id=CK_SCRIPT_AFTER_TRANSFER.After Successful Image Transfer (checkbox)
:p.Select this checkbox to run a script after an image has been transfered.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_SCRIPT_AFTER_TRANSFER id=EF_SCRIPT_AFTER_TRANSFER.After Successful Image Transfer (entryfield)
:p.This is the name of the script to run after an image has been transfered.
:p.The parameter :hp2.%s:ehp2., if specified, is replaced with the name of the image.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=CK_SCRIPT_AFTER_DELETE id=CK_SCRIPT_AFTER_DELETE.After Deleting An Image From the Camera (checkbox)
:p.Select this checkbox to run a script after an image has been deleted.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_SCRIPT_AFTER_DELETE id=EF_SCRIPT_AFTER_DELETE.After Deleting An Image From the Camera (entryfield)
:p.This is the name of the script to run after an image has been deleted.
:p.The parameter :hp2.%s:ehp2., if specified, is replaced with the name of the image.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=CK_SCRIPT_NO_MORE_IMAGES id=CK_SCRIPT_NO_MORE_IMAGES.When No More Images Remain in the Camera (checkbox)
:p.Select this checkbox to run a script after the last images has been deleted.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_SCRIPT_NO_MORE_IMAGES id=EF_SCRIPT_NO_MORE_IMAGES.When No More Images Remain in the Camera (entryfield)
:p.This is the name of the script to run after the last image has been deleted.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=CK_SCRIPT_AFTER_DISCONNECTING id=CK_SCRIPT_AFTER_DISCONNECTING.After Disconnecting from the Camera (checkbox)
:p.Select this checkbox to run a script after disconnecting from the camera.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_SCRIPT_AFTER_DISCONNECTING id=EF_SCRIPT_AFTER_DISCONNECTING.After Disconnecting from the Camera (entryfield)
:p.This is the name of the script to run after disconnecting from the camera.
:note.Also see :link reftype=hd refid=ID_HOW_TO_USE_SCRIPTS.How to Use Scripts:elink..
.*
.*
.*
:h2 res=EF_REGISTER_NAME id=EF_REGISTER_NAME.Name
:p.This is the name that you requested to use with the registration.  This
information will be provided to you along with the registration key once
you register DCITU.
:p.For additional registration information, please see the DCITU web page:
:hp2.http&colon.&slash.&slash.charette.dyn.tj&slash.:ehp2..
.*
.*
.*
:h2 res=EF_REGISTER_KEY id=EF_REGISTER_KEY.Key
:p.This is the registration key required to register DCITU.  This
information will be provided to you once you have registered.
:p.For additional registration information, please see the DCITU web page:
:hp2.http&colon.&slash.&slash.charette.dyn.tj&slash.:ehp2..
.*
.*
.*
:h2 res=PB_REGISTER id=PB_REGISTER.Register
:p.Registration allows you to use transfer rates greater than 9600 baud.  Once
you have registered, you will be sent a key to match the registration name.
:p.This information is required in the :hp1.Name:ehp1. and :hp1.Key:ehp1.
fields before you can use the :hp1.Register:ehp1. pushbutton.
:p.For additional registration information, please see the DCITU web page:
:hp2.http&colon.&slash.&slash.charette.dyn.tj&slash.:ehp2..
.*
.*
.*
:h2 res=PB_OK id=PB_OK.OK
:p.The OK pushbutton will accept the currently selected settings.
.*
.*
.*
:h2 res=PB_CANCEL id=PB_CANCEL.Cancel
:p.The Cancel pushbutton will discard all changes that have been made to the
Options window.
.*
.*
.*
:euserdoc.

