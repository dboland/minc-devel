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

OBJECTS+= atexit.c.o

#OBJECTS+= ecvt.o gcvt.o malloc.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/stdlib/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -I${SRCDIR}/sys -I../include -o $@ $<

%.c.o: %.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/stdlib -o $@ $<


