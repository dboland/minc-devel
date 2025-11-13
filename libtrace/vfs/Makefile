ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

OBJECTS = VfsFileAttribs.o VfsFileFlags.o VfsPathFlags.o VfsVolumeFlags.o \
	VfsTaskFlags.o VfsInputMode.o VfsNetEvents.o VfsScreenMode.o
OBJECTS += vfs_CAP_CONTROL.o vfs_VNODE.o vfs_NAMEI.o vfs_DEVICE.o vfs_MOUNT.o \
	vfs_STATFS.o vfs_INODE.o vfs_TASK.o vfs_THREAD.o vfs_TTY.o vfs_CONSOLE.o \
	vfs_ACCESS.o vfs_NETFLAGS.o vfs_CFDATA.o

all: ${OBJECTS}
	@echo "done."

%.o: %.c
	@echo -n "$@ "
	@${CC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<
