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

CFLAGS += -DUSE_NAVISERVER
CXXFLAGS += $(CFLAGS)

include  $(NAVISERVER)/include/Makefile.module