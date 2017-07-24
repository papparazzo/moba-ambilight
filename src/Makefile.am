bin_PROGRAMS = moba-ambilight moba-ambilightd
moba_ambilightd_SOURCES = daemon.cpp bridge.cpp handler.cpp processdata.cpp
moba_ambilightd_LDADD = -lwiringPi /usr/local/lib/libMoBaCommon.a

moba_ambilight_SOURCES = client.cpp
moba_ambilight_LDADD = /usr/local/lib/libMoBaCommon.a

LDFLAGS += -L/usr/local/lib -pthread
ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
