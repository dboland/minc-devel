OBJECTS= base64.o freeaddrinfo.o gai_strerror.o \
	getnetbyaddr.o getnetbyname.o getnetent.o \
	getpeereid.o getproto.o getprotoent.o getprotoname.o \
	getservbyname.o getservbyport.o getifaddrs.o \
	if_nameindex.o inet_addr.o \
	inet_lnaof.o inet_makeaddr.o inet_neta.o inet_netof.o inet_network.o \
	inet_net_ntop.o inet_net_pton.o inet_ntoa.o inet_ntop.o inet_pton.o \
	res_comp.o res_data.o rcmd.o rcmdsh.o rresvport.o \
	res_debug_syms.o ruserok.o \
	res_random.o ethers.o vars6.o
OBJECTS+= getservent.c.o linkaddr.c.o if_indextoname.c.o if_nametoindex.c.o
LIBC= send.o recv.o herror.o

# IPv6
OBJECTS+=ip6opt.o rthdr.o vars6.o

SHOBJECTS= base64.so freeaddrinfo.so gai_strerror.so \
	getnetbyaddr.so getnetbyname.so getnetent.so \
	getpeereid.so getproto.so getprotoent.so getprotoname.so \
	getservbyname.so getservbyport.so getifaddrs.so \
	if_nameindex.so inet_addr.so \
	inet_lnaof.so inet_makeaddr.so inet_neta.so inet_netof.so inet_network.so \
	inet_net_ntop.so inet_net_pton.so inet_ntoa.so inet_ntop.so inet_pton.so \
	res_comp.so res_data.so rcmd.so rcmdsh.so rresvport.so \
	res_debug_syms.so ruserok.so \
	res_random.so ethers.so vars6.so
SHOBJECTS+= getservent.c.so linkaddr.c.so if_indextoname.c.so if_nametoindex.c.so
LIBC= send.so recv.so herror.so

SHOBJECTS+=ip6opt.so rthdr.so vars6.so

LIB= ../net.a

all: ${OBJECTS} ${SHOBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/net/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -o $@ $<

%.c.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -I${ROOTDIR}/include -o $@ $<

%.so: ${SRCDIR}/lib/libc/net/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.c.so: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<
