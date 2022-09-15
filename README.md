# DESCRIPTION

FreqVoltageXYSweep is a Win32 application providing control of Stanford
Research Systems lock-in amplifiers via a National Instruments USB-to-GPIB
(IEEE488.2) interface cable. The software was created by Connor D. Pierce while
a Ph.D. student at the University of Illinois.

With the exception of the National Instruments header file "ni4882.h", this
software is licensed under the terms of the MIT License, a copy of which may be
found in the file "LICENSE".


# COMPILING THIS SOFTWARE

This software was created using the Orwell Dev-C++ IDE. The easiest way to build
this software is to download and install Dev-C++, then load the
FreqVoltageXYSweep.dev project file.

## REQUIRED LIBRARIES

This software requires National Instruments drivers in order to interface with
the NI GPIB card. These drivers can be downloaded from
https://www.ni.com/en-us/support/downloads/drivers/download.ni-488-2.html and
must be installed prior to building the FreqVoltageXYSweep software. The default
installation location for these drivers is
"C:/Program Files (x86)/National Instruments". Note that if you install the
drivers in a different location, you will need to change the location for the
includes, libraries, and linker in the Dev-C++ settings.


# SUPPORTED OPERATING SYSTEMS

This software has only been tested on Windows 10.
