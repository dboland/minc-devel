include ${ROOTDIR}/config.inc

ifndef DESTDIR
	DESTDIR=${SYSTEMDRIVE}/minc-release
endif

.SUFFIXES:
.DEFAULT:

# PIC default: 0x10000000 (library space)
# PIE default: 0x00400000 (program space)

IMGBASE_LIBDL=		0x0FF80000 # (500K)
IMGBASE_LIBPOSIX=	0x0FF00000 # (500K)
IMGBASE_LIBC=		0x0FE00000
IMGBASE_LIBSOCKET=	0x0FD00000
IMGBASE_LIBEDIT=	0x0FC00000
IMGBASE_LIBUTIL=	0x0FB00000
#LIBZ_IMGBASE=		0x0FA00000
IMGBASE_LIBCURL=	0x0F900000
LIBPERL_IMGBASE=	0x0F600000 # (3MB)

BUILD= $(shell date +%s)

# locations for GNU cross-compiler

_PREFIX= /usr/i686-openbsd-pe
_LIBDIR= ${_PREFIX}/lib
_INCDIR= ${_PREFIX}/include

# In GCC 6.3.0, -ffixed-ebx crashes compilation

CC= gcc
CFLAGS= -Wimplicit-function-declaration -Werror -ffreestanding -Wl,--disable-auto-import
LDFLAGS= --disable-auto-import
