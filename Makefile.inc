DESTDIR=${SYSTEMDRIVE}/minc-release

include ${ROOTDIR}/config.inc

.SUFFIXES:
.DEFAULT:

# PIC default: 0x10000000 (library space)
# PIE default: 0x00400000 (program space)

IMGBASE_LIBDL=		0x0FF80000 # 0.05 Mb
IMGBASE_LIBPOSIX=	0x0FF00000 # 0.30 Mb
IMGBASE_LIBC=		0x0FE00000 # 0.61 Mb
IMGBASE_LIBSOCKET=	0x0FD00000 # 0.16 Mb
IMGBASE_LIBEDIT=	0x0FC00000 # 0.28 Mb
IMGBASE_LIBUTIL=	0x0FB00000 # 0.15 Mb
IMGBASE_LIBCURSES=	0x0FA00000 # 1.20 Mb
IMGBASE_LIBCURL=	0x0F900000 # 0.60 Mb
#LIBPERL_IMGBASE=	0x0F600000 # 3.00 Mb

BUILD= $(shell date +%s)

# In GCC 6.3.0, -ffixed-ebx crashes compilation

CC= gcc
CFLAGS= -ffreestanding -Wl,--disable-auto-import
#CFLAGS+=  -Wimplicit-function-declaration -Werror
CFLAGS+= -Wno-pointer-sign
LDFLAGS= --disable-auto-import
