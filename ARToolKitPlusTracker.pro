################################
#
# QMake definitions for simple example
#

TEMPLATE = lib

TARGET   = osgart_artoolkitplus_tracker_b

QMAKE_CLEAN = $$(ARTKP)/bin/osgart_artoolkitplus_tracker_b*

LIBS += -L$$(ARTKP)/lib -lARToolKitPlus

INCLUDEPATH += $$(MIXINC)
INCLUDEPATH += $$(ARTKP)/include

debug {
  OBJECTS_DIR     = $$(ARTKP)/ARTKPT/build/linux/debug
}

release {
  OBJECTS_DIR     = $$(ARTKP)/ARTKPT/build/linux/release
}


DESTDIR  = $$(ARTKP)/bin

debug {
  message("Building ARTKTP in debug mode ...")
}

release {
  message("Building ARTKTP in release mode ...")
}

SOURCES += $$files(src/*.cpp)
HEADERS += $$files(src/*.h) src/MultiMarker

target.path = ""/$$PREFIX/bin
INSTALLS += target

################################
