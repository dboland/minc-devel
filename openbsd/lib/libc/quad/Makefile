OBJECTS=adddi3.o anddi3.o ashldi3.o ashrdi3.o cmpdi2.o divdi3.o fixdfdi.o \
	fixsfdi.o fixunsdfdi.o fixunssfdi.o floatdidf.o floatdisf.o \
	floatunsdidf.o iordi3.o lshldi3.o lshrdi3.o moddi3.o muldi3.o \
	negdi2.o notdi2.o qdivrem.o subdi3.o ucmpdi2.o udivdi3.o umoddi3.o \
	xordi3.o
KOBJECTS=adddi3.o anddi3.o ashldi3.o ashrdi3.o cmpdi2.o divdi3.o iordi3.o \
	lshldi3.o lshrdi3.o moddi3.o muldi3.o negdi2.o notdi2.o qdivrem.o \
	subdi3.o ucmpdi2.o udivdi3.o umoddi3.o xordi3.o

.SUFFIXES:
.DEFAULT:

LIB= ../quad.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/quad/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

