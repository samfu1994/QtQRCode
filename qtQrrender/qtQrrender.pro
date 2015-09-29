#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T22:17:11
#
#-------------------------------------------------

QT       += core gui
qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtQrrender
TEMPLATE = app

INCLUDEPATH +=$(OPENCV_HOME)\\include

DEFINES += HAVE_CONFIG_H

DEFINES += QRCORE_USING_QT

LIBS += $(OPENCV_HOME)\\lib\\libopencv_calib3d249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_contrib249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_core249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_features2d249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_flann249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_gpu249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_highgui249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_imgproc249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_legacy249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_ml249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_nonfree249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_objdetect249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_ocl249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_photo249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_stitching249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_superres249.dll.a\
                $(OPENCV_HOME)\\lib\\libopencv_video249.dll.a\
				$(OPENCV_HOME)\\lib\\libopencv_videostab249.dll.a

LIBS +=	$(ZBAR_HOME)\libzbar-0.lib


SOURCES += main.cpp\
        widget.cpp \
    view.cpp \
	cvimage.cpp \
    Core/qrrenderCore.cpp \
    Core/qrencode/bitstream.c \
    Core/qrencode/mask.c \
    Core/qrencode/mmask.c \
    Core/qrencode/mqrspec.c \
    Core/qrencode/qrencode.c \
    Core/qrencode/qrinput.c \
    Core/qrencode/qrspec.c \
    Core/qrencode/rscode.c \
    Core/qrencode/split.c \
    Core/qrcodegen.cpp \
    Core/halftonegen.cpp \
    Core/combine.cpp \
    Core/qrhighquality.cpp \
    test.cpp

HEADERS  += widget.h \
    view.h \
	cvimage.h \
	Core/qrencode/tests/common.h \
    Core/qrencode/tests/decoder.h \
    Core/qrencode/bitstream.h \
    Core/qrencode/config.h \
    Core/qrencode/mask.h \
    Core/qrencode/mmask.h \
    Core/qrencode/mqrspec.h \
    Core/qrencode/qrencode.h \
    Core/qrencode/qrencode_inner.h \
    Core/qrencode/qrinput.h \
    Core/qrencode/qrspec.h \
    Core/qrencode/rscode.h \
    Core/qrencode/split.h \
    Core/ZBar/include/zbar/Decoder.h \
    Core/ZBar/include/zbar/Exception.h \
    Core/ZBar/include/zbar/Image.h \
    Core/ZBar/include/zbar/ImageScanner.h \
    Core/ZBar/include/zbar/Processor.h \
    Core/ZBar/include/zbar/Scanner.h \
    Core/ZBar/include/zbar/Symbol.h \
    Core/ZBar/include/zbar/Video.h \
    Core/ZBar/include/zbar/Window.h \
    Core/ZBar/include/zbar.h \
	Core/qrrenderCore.h \
    Core/qrcodegen.h \
    Core/halftonegen.h \
    Core/combine.h \
    Core/qrhighquality.h

FORMS    +=

RESOURCES += \
    images.qrc

OTHER_FILES +=
