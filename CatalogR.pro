#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T12:57:34
#
#-------------------------------------------------

#win32:VERSION = 1.2.3.4 # major.minor.patch.build
#else:VERSION = 1.2.3    # major.minor.patch

VERSION = 0.0.9
#QMAKE_TARGET_COMPANY = company
#QMAKE_TARGET_PRODUCT = product
#QMAKE_TARGET_DESCRIPTION = description
#QMAKE_TARGET_COPYRIGHT = copyright

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
    data/database.cpp \
    data/mediainfo.cpp \
    thirdParty/starrating.cpp \
    gui/catalogfilterwidget.cpp \
    gui/facetagwidget.cpp \
    gui/videothumbnailgraphicitem.cpp \
    gui/cataloggraphicsscene.cpp \
    gui/cataloggraphicsview.cpp \
    gui/catalogwidget.cpp \
    gui/mediainfowidget.cpp \
    gui/playerwidget.cpp \
    gui/tagswidget.cpp \
    gui/jobswidget.cpp \
    gui/jobstatuswidget.cpp \
    gui/basedialog.cpp \
    gui/catalogtreewidget.cpp \
    core/catalogmodel.cpp \
    core/catalogfilter.cpp \
    core/faces.cpp \
    core/ffmpegparser.cpp \
    core/jobsmaster.cpp \
    core/exportjob.cpp \
    core/importjob.cpp \
    core/facedetectionjob.cpp

HEADERS += \
        mainwindow.h \
    data/database.h \
    data/mediainfo.h \
    thirdParty/starrating.h \
    gui/catalogfilterwidget.h \
    gui/facetagwidget.h \
    gui/videothumbnailgraphicitem.h \
    gui/cataloggraphicsscene.h \
    gui/cataloggraphicsview.h \
    gui/catalogwidget.h \
    gui/mediainfowidget.h \
    gui/playerwidget.h \
    gui/tagswidget.h \
    gui/jobswidget.h \
    gui/jobstatuswidget.h \
    gui/basedialog.h \
    gui/catalogtreewidget.h \
    core/catalogmodel.h \
    core/catalogfilter.h \
    core/faces.h \
    core/ffmpegparser.h \
    core/importjob.h \
    core/facedetectionjob.h \
    core/jobsmaster.h \
    core/exportjob.h

# opencv
INCLUDEPATH += "D:/Dev/ThirdParty/opencv-3.3.0/build/include"
LIBS += -L"D:/Dev/ThirdParty/opencv-3.3.0/release_contrib/lib"
LIBS += -lopencv_video330 -lopencv_core330 -lopencv_videoio330 -lopencv_imgproc330 -lopencv_highgui330 \
        -lopencv_ml330 -lopencv_face330 -lopencv_objdetect330 -lopencv_imgcodecs330

# ffmpeg
INCLUDEPATH += "D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/include"
LIBS += -L"D:/Dev/ThirdParty/ffmpeg-3.3.2-desktop-MINGW32/lib"
LIBS += -lavutil -lavformat -lavcodec


