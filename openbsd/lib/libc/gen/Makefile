SOURCES= signal.c errno.c fts.c getprogname.c setprogname.c
OBJECTS= assert.o \
	basename.o clock.o clock_getcpuclockid.o \
	confstr.o ctermid.o ctype_.o \
	dirname.o err.o fnmatch.o \
	errc.o errx.o errlist.o \
	frexp.o fdatasync.o statvfs.o \
	fstab.o ftok.o ftw.o getbsize.o \
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
	telldir.o closedir.o exec.o posix_spawn.o shm_open.o

OBJECTS+= syslog_r.o syslog.o auth_subr.o login_cap.o authenticate.o
#OBJECTS+= _exit.S.o _getlogin.S.o _setlogin.S.o
#OBJECTS+= elf_hash.o

# patched sources
OBJECTS+= signal.c.o errno.c.o fts.c.o getprogname.c.o setprogname.c.o

# MinC replacements
OBJECTS+= __fdnlist.minc.c.o \
	getpwent.minc.c.o getgrent.minc.c.o getgrouplist.minc.c.o \
	setproctitle.minc.c.o

.SUFFIXES:
.DEFAULT:

LIB= ../gen.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.c: ${SRCDIR}/lib/libc/gen/%.c
	@/bin/cp -uv $< $@

%.o: ${SRCDIR}/lib/libc/gen/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/include -o $@ $<

%.c.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

%.S.o: %.S
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../arch/${ARCH} -o $@ $<

source: ${SOURCES}

patch-clean:
	/bin/rm -f ${SOURCES}
