OBJECTS= assert.o \
	basename.o clock.o clock_getcpuclockid.o \
	confstr.o ctermid.o ctype_.o \
	dirname.o err.o fnmatch.o \
	errc.o errx.o errlist.o \
	frexp.o fdatasync.o statvfs.o \
	fstab.o ftok.o fts.c.o ftw.o getbsize.o \
	getttyent.o getusershell.o glob.o \
	isctype.o isfdtype.o isfinite.o isinf.o \
	isnan.o isnormal.o signbit.o lockf.o \
	nftw.o pause.o popen.o psignal.o \
	pw_dup.o readpassphrase.o scandir.o \
	setjmperr.o setmode.o \
	siginterrupt.o siglist.o signame.o \
	sigsetops.o sleep.o strtofflags.o \
	time.o times.o timezone.o getlogin.o \
	tolower_.o vis.o unvis.o \
	ttyslot.o toupper_.o usleep.o \
	utime.o valloc.o wait.o wait3.o waitpid.o \
	warn.o warnc.o warnx.o \
	pwcache.o daemon.o fpclassify.o getcap.o getmntinfo.o \
	nice.o alarm.o sysctl.o getcwd.o devname.o sysconf.o \
	getpagesize.o getloadavg.o getdomainname.o setdomainname.o \
	gethostname.o sethostname.o uname.o isatty.o initgroups.o disklabel.o \
	getnetgrent.o ttyname.o ualarm.o raise.o nlist.o \
	vwarn.o vwarnc.o vwarnx.o verr.o verrc.o verrx.o \
	opendir.o readdir.o readdir_r.o rewinddir.o seekdir.o dirfd.o \
	telldir.o closedir.o exec.o

OBJECTS+= getpwent.c.o getgrent.c.o setproctitle.c.o setprogname.c.o \
	getprogname.c.o signal.c.o getgrouplist.c.o errno.c.o
#OBJECTS+= _exit.S.o _getlogin.S.o _setlogin.S.o
OBJECTS+= syslog_r.o syslog.o auth_subr.o login_cap.o authenticate.o

#OBJECTS+= elf_hash.o posix_spawn.o shm_open.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/gen/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -I../include -o $@ $<

%.c.o: %.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -I${ROOTDIR}/include -o $@ $<

%.S.o: %.S
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../arch/${ARCH} -o $@ $<
