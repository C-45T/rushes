# Rushes!

![Rushes! Screenshot](https://github.com/c-45t/rushes/raw/master/img/rushes_screenshot_0.1.png "Rushes! Screenshot")

Rushes! is a video cataloger application.
It's main purpose is to reference all your rushes, and then add tags, rating, organize them into bins to be able to export them for futur sharing or editing.

Its incredible ambition is to integrate some advanced features :
- aaf or xml export to add integration to video editing software such as Davinci Resolve, Premiere, Final Cut, etc.
- video analysis to automaticly add tags to rushes, like face or scene recognition (portrait, landscape, season, movement type, ...)
- generation of simple editing based on tags and ratings of rushes.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

Rushes uses the following libraries :
- Qt v5.9 (https://www.qt.io/) under GNU Lesser General Public License (LGPL) version 2.1
- QtAv v1.12.0 (http://www.qtav.org/) under GNU Lesser General Public License (LGPL) version 2.1
- FFMpeg v3.3.2 (https://www.ffmpeg.org/) under GNU Lesser General Public License (LGPL) version 2.1
- opencv v3.3.0 (https://opencv.org/) under the BSD License

### Installing

Configure CMAKE to use opencv avec QT:
https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows

If you have the following error : ... windres.exe: unknown option -- W ...
try this : in cmake uncheck ENABLE_PRECOMPILED_HEADERS

If, in the file opencv/sources/modules/videoio/src/cap_dshow.cpp, you have the following error : 'sprintf_instead_use_StringCbPrintfA_or_StringCchPrintfA' was not declared in this scope ...
try this :put the following line: #define NO_DSHOW_STRSAFE, before the line : #include "DShow.h"

Compilation of QtAV lib:
https://github.com/wang-bin/QtAV/wiki/Build-QtAV

TODO...

### Download

Binaries can be downloaded on the [release page](https://github.com/C-45T/rushes/releases)


## Authors

* **Rémy Ruttner** - *Initial work* - 

## License

This project is licensed under the GNU General Public License Version 3 or later.

You can modify or redistribute it under the conditions of these licenses.


