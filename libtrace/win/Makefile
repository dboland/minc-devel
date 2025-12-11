ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

LIB= ../win.a
OBJECTS = WinFlagName.o WinAceFlags.o WinFileAccess.o WinSecurityFlags.o \
	WinGroupAttribs.o WinPrivilegeAttribs.o
OBJECTS += win_ACL.o win_SID.o win_TOKEN.o win_SECURITY_DESCRIPTOR.o \
	win_TOKEN_PRIVILEGES.o win_TOKEN_GROUPS.o win_SID_RIGHTS.o win_SYSTEM_INFO.o

all: ${LIB}
	@echo "done."

${LIB}: ${OBJECTS}
	@ar cru $@ ${OBJECTS}

%.o: %.c ${ROOTDIR}/include/win_types.h
	@echo -n "$@ "
	@${CC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<
