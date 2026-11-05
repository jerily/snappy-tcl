ifndef NAVISERVER
    NAVISERVER  = /usr/local/ns
endif

#
# Module name
#
MOD      =  snappy.so

#
# Objects to build.
#
MODOBJS     = library.o

MODLIBS  += -lsnappy

PROJECT_VERSION := $(shell awk '/^project/ {print $$3}' CMakeLists.txt)
CFLAGS += -DUSE_NAVISERVER -DPROJECT_VERSION=$(PROJECT_VERSION)
CXXFLAGS += $(CFLAGS)

include  $(NAVISERVER)/include/Makefile.module
