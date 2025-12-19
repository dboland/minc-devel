OBJECTS = a64l.o atoi.o atof.o atol.o atoll.o bsearch.o \
	cfree.o getenv.o random.o \
	getsubopt.o heapsort.o imaxabs.o imaxdiv.o l64a.o llabs.o \
	lldiv.o lsearch.o reallocarray.o merge.o posix_pty.o \
	qsort.o radixsort.o setenv.o strtoimax.o \
	strtol.o strtoll.o strtonum.o strtoul.o strtoull.o strtoumax.o \
	system.o tfind.o tsearch.o _rand48.o drand48.o erand48.o jrand48.o \
	lcong48.o lrand48.o mrand48.o nrand48.o seed48.o srand48.o qabs.o \
	qdiv.o _Exit.o exit.o abort.o rand.o realpath.o \
	div.o hcreate.o getopt_long.o
OBJECTS+= ecvt.o gcvt.o abs.o
#OBJECTS+= malloc.o

# MinC replacements
OBJECTS+= atexit.minc.c.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -I${SRCDIR}/lib/libc/gdtoa
CFLAGS+= -I${SRCDIR}/lib/libc/arch/${ARCH}/gdtoa

LIB= ../stdlib.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/stdlib/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/include -o $@ $<

%.c.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<


