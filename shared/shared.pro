INCLUDEPATH += . ..

with_opencv {     # Needs Ubuntu-Pakage "libopencv-dev"
  DEFINES += WITH_OPENCV
  LIBS += -lopencv_core -lopencv_highgui
}

# Input
HEADERS += \
	../shared/geometrypicker.h \
	../shared/pospicker.h \
        ../shared/imagerectview.h \
        ../shared/imageposview.h \
        ../shared/imageview.h \
        ../shared/image.h \
        ../shared/imagearray.h \
        ../shared/fileimage.h \
        ../shared/cinelerratoc.h

SOURCES += \
	../shared/geometrypicker.cc \
	../shared/pospicker.cc \
        ../shared/imagerectview.cc \
        ../shared/imageposview.cc \
        ../shared/imageview.cc \
        ../shared/image.cc \
        ../shared/imagearray.cc \
        ../shared/fileimage.cc \
        ../shared/cinelerratoc.cc

FORMS += \ 
	../shared/forms/geometrypicker.ui \
	../shared/forms/pospicker.ui
