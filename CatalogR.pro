#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T12:57:34
#
#-------------------------------------------------

QT       += core gui sql
QT       += avwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CatalogR
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
        main.cpp \
        mainwindow.cpp \
    database.cpp \
    catalogmodel.cpp \
    catalogfilterwidget.cpp \
    catalogfilter.cpp \
    faces.cpp \
    facetagwidget.cpp \
    videothumbnailgraphicitem.cpp \
    cataloggraphicsscene.cpp \
    proc/ffmpegparser.cpp \
    gui/cataloggraphicsview.cpp \
    catalogwidget.cpp \
    gui/mediainfowidget.cpp \
    mediainfo.cpp \
    gui/playerwidget.cpp \
    thirdParty/starrating.cpp \
    gui/tagswidget.cpp

HEADERS += \
        mainwindow.h \
    database.h \
    catalogmodel.h \
    catalogfilterwidget.h \
    catalogfilter.h \
    faces.h \
    facetagwidget.h \
    videothumbnailgraphicitem.h \
    cataloggraphicsscene.h \
    proc/ffmpegparser.h \
    gui/cataloggraphicsview.h \
    catalogwidget.h \
    gui/mediainfowidget.h \
    mediainfo.h \
    gui/playerwidget.h \
    thirdParty/starrating.h \
    gui/tagswidget.h

# opencv
INCLUDEPATH += "D:/Dev/ThirdParty/opencv-3.3.0/build/include"
LIBS += -L"D:/Dev/ThirdParty/opencv-3.3.0/release_contrib/lib"
LIBS += -lopencv_video330 -lopencv_core330 -lopencv_videoio330 -lopencv_imgproc330 -lopencv_highgui330 \
        -lopencv_ml330 -lopencv_face330 -lopencv_objdetect330 -lopencv_imgcodecs330

# ffmpeg
INCLUDEPATH += "D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/include"
LIBS += -L"D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/lib"
LIBS += -lavutil -lavformat -lavcodec


