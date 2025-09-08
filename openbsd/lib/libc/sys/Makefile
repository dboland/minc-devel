OBJECTS= __get_tcb.o __getcwd.o __semctl.o __set_tcb.o \
	__sysctl.o __tfork.o __threxit.o __thrsigdivert.o __thrsleep.o __thrwakeup.o \
	access.o acct.o adjfreq.o adjtime.o break.o chdir.o chflags.o \
	chmod.o chown.o chroot.o clock_getres.o clock_gettime.o clock_settime.o close.o \
	closefrom.o dup.o dup2.o execve.o faccessat.o fchdir.o \
	fchflags.o fchmod.o fchmodat.o fchown.o fchownat.o fcntl.o fhopen.o fhstat.o \
	fhstatfs.o flock.o fpathconf.o fstat.o fstatat.o fstatfs.o fsync.o \
	ftruncate.o futimens.o futimes.o getdents.o getdtablecount.o getegid.o \
	geteuid.o getfh.o getfsstat.o getgid.o getgroups.o getitimer.o \
	getpgid.o getpgrp.o getpid.o getppid.o getpriority.o getresgid.o \
	getresuid.o getrlimit.o getrtable.o getrusage.o getsid.o \
	getthrid.o gettimeofday.o getuid.o ioctl.o issetugid.o kevent.o \
	kill.o kqueue.o ktrace.o lchown.o link.o linkat.o lseek.o lstat.o \
	madvise.o mincore.o minherit.o mkdir.o mkdirat.o mkfifo.o mkfifoat.o mknod.o \
	mknodat.o mlock.o mlockall.o mmap.o mount.o mprotect.o mquery.o msgctl.o \
	msgget.o msgrcv.o msgsnd.o msync.o munlock.o munlockall.o munmap.o nanosleep.o \
	nfssvc.o open.o openat.o pathconf.o pipe.o poll.o ppoll.o pread.o preadv.o \
	profil.o pselect.o ptrace.o pwrite.o pwritev.o quotactl.o read.o readlink.o \
	readlinkat.o readv.o reboot.o rename.o renameat.o revoke.o \
	rmdir.o sched_yield.o select.o semget.o semop.o setegid.o \
	seteuid.o setgid.o setgroups.o setitimer.o setpgid.o setpriority.o \
	setregid.o setresgid.o setresuid.o setreuid.o setrlimit.o setrtable.o setsid.o \
	settimeofday.o setuid.o shmat.o shmctl.o shmdt.o shmget.o \
	sigaction.o sigaltstack.o sigpending.o sigprocmask.o sigreturn.o \
	sigsuspend.o stat.o statfs.o swapctl.o symlink.o \
	symlinkat.o sync.o sysarch.o truncate.o \
	umask.o unlink.o unlinkat.o unmount.o utimensat.o utimes.o \
	utrace.o wait4.o write.o writev.o

OBJECTS+= semctl.c.o
OBJECTS+= socket.o connect.o accept.o recvfrom.o sendto.o sendmsg.o recvmsg.o \
	socketpair.o shutdown.o getsockopt.o setsockopt.o bind.o listen.o \
	getpeername.o getsockname.o

#OBJECTS+= exit.o getlogin.o setlogin.o
#OBJECTS+=  __syscall.o

.SUFFIXES:
.DEFAULT:

LIB= ../sys.a

all: source ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: %.S
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../arch -o $@ $<

%.c.o: ${SRCDIR}/lib/libc/sys/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../arch -o $@ $<

source:
	@syscall.sh
