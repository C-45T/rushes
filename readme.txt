Configurer CMAKE pour utiliser opencv avec QT:
https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows

Compilation QtAV
https://github.com/wang-bin/QtAV/wiki/Build-QtAV

If you have the following error : ... windres.exe: unknown option -- W ...

try this : in cmake uncheck ENABLE_PRECOMPILED_HEADERS

If, in the file opencv/sources/modules/videoio/src/cap_dshow.cpp, you have the following error : 'sprintf_instead_use_StringCbPrintfA_or_StringCchPrintfA' was not declared in this scope ...

try this :put the following line: #define NO_DSHOW_STRSAFE, before the line : #include "DShow.h"


Erreur à l'execution :
Utiliser Environnement de compilation et
VLC_PLUGIN_PATH définit à D:\Dev\ThirdParty\vlc-2.2.1\plugins