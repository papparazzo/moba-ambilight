bin_PROGRAMS = \
    moba-ambilight \
    moba-ambilightd

moba_ambilightd_SOURCES = \
    processdatawobble.cpp \
    processdataplain.cpp \
    processdatahold.cpp \
    processdata.cpp \
    bridge.cpp \
    bankcolorvalues.cpp \
    handler.cpp \
    daemon.cpp

moba_ambilightd_LDADD = \
    -lwiringPi \
    /usr/local/lib/libmobacommon.a

moba_ambilight_SOURCES = \
    client.cpp

moba_ambilight_LDADD = \
    /usr/local/lib/libmobacommon.a

LDFLAGS += -L/usr/local/lib -pthread

ACLOCAL_AMFLAGS = -I m4
CLEANFILES = *~
