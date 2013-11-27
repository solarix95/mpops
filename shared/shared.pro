INCLUDEPATH += . ..

with_opencv {     # Needs Ubuntu-Pakage "libopencv-dev"
  DEFINES += WITH_OPENCV
  LIBS += -lopencv_core -lopencv_highgui
}

# Input
HEADERS += \
	../shared/geometrypicker.h \
        ../shared/imageview.h \
        ../shared/image.h \
        ../shared/imagearray.h \
        ../shared/fileimage.h

SOURCES += \
	../shared/geometrypicker.cc \
        ../shared/imageview.cc \
        ../shared/image.cc \
        ../shared/imagearray.cc \
    ../shared/fileimage.cc

FORMS += ../shared/forms/geometrypicker.ui
