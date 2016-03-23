
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mosaic
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    msavedialog.cpp

HEADERS  += mainwindow.h \
    msavedialog.h

FORMS    += mainwindow.ui

INCLUDEPATH += .\release\opencv
INCLUDEPATH += .\release\opencv2

win32:CONFIG(debug,debug|release) {
LIBS += -LD:\OPENCV\opencv\build\x86\vc12\lib \
-lopencv_ml249d\
-lopencv_calib3d249d\
-lopencv_contrib249d\
-lopencv_core249d\
-lopencv_features2d249d\
-lopencv_flann249d\
-lopencv_gpu249d\
-lopencv_highgui249d\
-lopencv_imgproc249d\
-lopencv_legacy249d\
-lopencv_objdetect249d\
-lopencv_ts249d\
-lopencv_video249d\
-lopencv_nonfree249d\
-lopencv_ocl249d\
-lopencv_photo249d\
-lopencv_stitching249d\
-lopencv_superres249d\
-lopencv_videostab249d\
} else {
win32:LIBS += -L.\release\
    -lopencv_core249 \
    -lopencv_highgui249 \
    -lopencv_imgproc249 \
    -lopencv_features2d249 \
    -lopencv_calib3d249
}

LIBS += -LC:\WINDOWS\SYSTEM32\pthreadGC2.dll
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS *= -fopenmp
LIBS += -lgomp -lpthread

RESOURCES += \
    icon.qrc
