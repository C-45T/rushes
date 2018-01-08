#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T12:57:34
#
#-------------------------------------------------

#win32:VERSION = 1.2.3.4 # major.minor.patch.build
#else:VERSION = 1.2.3    # major.minor.patch

VERSION = 0.1.0
#QMAKE_TARGET_COMPANY = company
#QMAKE_TARGET_PRODUCT = product
#QMAKE_TARGET_DESCRIPTION = description
#QMAKE_TARGET_COPYRIGHT = copyright

QT       += core gui sql
QT       += avwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rushes!
TEMPLATE = app

PKGCONFIG += opencv

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
    src/data/database.cpp \
    thirdParty/starrating.cpp \
    src/gui/catalogfilterwidget.cpp \
    src/gui/facetagwidget.cpp \
    src/gui/playerwidget.cpp \
    src/gui/tagswidget.cpp \
    src/gui/jobswidget.cpp \
    src/gui/jobstatuswidget.cpp \
    src/gui/basedialog.cpp \
    src/gui/catalogtreewidget.cpp \
    src/core/catalogfilter.cpp \
    src/core/faces.cpp \
    src/core/ffmpegparser.cpp \
    src/core/jobsmaster.cpp \
    src/core/exportjob.cpp \
    src/core/facedetectionjob.cpp \
    src/data/rush.cpp \
    src/gui/rushinfowidget.cpp \
    src/data/bin.cpp \
    thirdParty/stareditor.cpp \
    thirdParty/flowlayout.cpp \
    src/gui/fileexplorerwidget.cpp \
    src/gui/thumbnailview.cpp \
    src/core/thumbnailrenderer.cpp \
    thirdParty/singleton.cpp \
    src/gui/mediagraphicitem.cpp \
    src/gui/thumbnailscene.cpp \
    src/gui/aboutdialog.cpp \
    src/core/fcpxmlexporter.cpp \
    src/core/onsetdetector.cpp \
    src/data/extract.cpp \
    src/gui/markedslider.cpp \
    src/core/smartselector.cpp \
    src/core/autoeditjob.cpp

HEADERS += \
        src/mainwindow.h \
    src/data/database.h \
    thirdParty/starrating.h \
    src/gui/catalogfilterwidget.h \
    src/gui/facetagwidget.h \
    src/gui/playerwidget.h \
    src/gui/tagswidget.h \
    src/gui/jobswidget.h \
    src/gui/jobstatuswidget.h \
    src/gui/basedialog.h \
    src/gui/catalogtreewidget.h \
    src/core/catalogfilter.h \
    src/core/faces.h \
    src/core/ffmpegparser.h \
    src/core/facedetectionjob.h \
    src/core/jobsmaster.h \
    src/core/exportjob.h \
    src/data/rush.h \
    src/gui/rushinfowidget.h \
    src/data/bin.h \
    thirdParty/stareditor.h \
    thirdParty/flowlayout.h \
    src/gui/fileexplorerwidget.h \
    src/gui/thumbnailview.h \
    src/core/thumbnailrenderer.h \
    thirdParty/singleton.h \
    src/gui/mediagraphicitem.h \
    src/gui/thumbnailscene.h \
    src/gui/aboutdialog.h \
    src/core/fcpxmlexporter.h \
    src/core/onsetdetector.h \
    src/data/extract.h \
    src/gui/markedslider.h \
    src/core/smartselector.h \
    src/core/autoeditjob.h

INCLUDEPATH += src/

# opencv
INCLUDEPATH += "D:/Dev/ThirdParty/opencv-3.3.0/build/include"
LIBS += -L"D:/Dev/ThirdParty/opencv-3.3.0/release_contrib/lib"
LIBS += -lopencv_video330 -lopencv_core330 -lopencv_videoio330 -lopencv_imgproc330 -lopencv_highgui330 \
        -lopencv_ml330 -lopencv_face330 -lopencv_objdetect330 -lopencv_imgcodecs330

# ffmpeg
INCLUDEPATH += "D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/include"
LIBS += -L"D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/lib"
LIBS += -lavutil -lavformat -lavcodec

# essentia
INCLUDEPATH += "D:/Dev/ThirdParty/essentia-2.1_beta3/src"
LIBS += -L"D:/Dev/ThirdParty/essentia-2.1_beta3/build/src"
LIBS += -lessentia


DISTFILES += \
    copyright.template


