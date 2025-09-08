OBJECTS=auth_none.o auth_unix.o  authunix_prot.o bindresvport.o \
	clnt_generic.o clnt_perror.o clnt_raw.o clnt_simple.o clnt_tcp.o \
	clnt_udp.o rpc_dtablesize.o get_myaddress.o getrpcent.o getrpcport.o \
	pmap_clnt.o pmap_getmaps.o pmap_getport.o pmap_prot.o \
	pmap_prot2.o rpc_prot.o rpc_commondata.o rpc_callmsg.o \
	svc.o svc_auth.o svc_auth_unix.o svc_raw.o svc_run.o svc_simple.o \
	svc_tcp.o svc_udp.o xdr.o xdr_array.o xdr_float.o xdr_mem.o \
	xdr_rec.o xdr_reference.o xdr_stdio.o
#OBJECTS+= pmap_rmt.o

SHOBJECTS=auth_none.so auth_unix.so  authunix_prot.so bindresvport.so \
	clnt_generic.so clnt_perror.so clnt_raw.so clnt_simple.so clnt_tcp.so \
	clnt_udp.so rpc_dtablesize.so get_myaddress.so getrpcent.so getrpcport.so \
	pmap_clnt.so pmap_getmaps.so pmap_getport.so pmap_prot.so \
	pmap_prot2.so rpc_prot.so rpc_commondata.so rpc_callmsg.so \
	svc.so svc_auth.so svc_auth_unix.so svc_raw.so svc_run.so svc_simple.so \
	svc_tcp.so svc_udp.so xdr.so xdr_array.so xdr_float.so xdr_mem.so \
	xdr_rec.so xdr_reference.so xdr_stdio.so
#SHOBJECTS+= pmap_rmt.so

LIB= ../rpc.a

all: ${OBJECTS} ${SHOBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/rpc/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -o $@ $<

%.so: ${SRCDIR}/lib/libc/rpc/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

