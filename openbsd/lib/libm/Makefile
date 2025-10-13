ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

LIB= libm.a
ARCH=i387
ULIBDIR= /usr/lib

OBJECTS=b_exp__D.o b_log__D.o b_tgamma.o \
	e_acos.o e_acosf.o e_acosh.o e_acoshf.o e_asin.o e_asinf.o \
	e_atan2.o e_atan2f.o e_atanh.o e_atanhf.o e_cosh.o e_coshf.o e_exp.o \
	e_expf.o e_fmod.o e_fmodf.o e_hypot.o e_hypotf.o e_j0.o e_j0f.o \
	e_j1.o e_j1f.o e_jn.o e_jnf.o e_lgamma_r.o e_lgammaf_r.o e_log.o \
	e_log10.o e_log10f.o e_log2.o e_log2f.o e_logf.o e_pow.o e_powf.o \
	e_rem_pio2.o \
	e_rem_pio2f.o e_remainder.o e_remainderf.o e_scalb.o e_scalbf.o \
	e_sinh.o e_sinhf.o e_sqrt.o e_sqrtf.o \
	fenv.arch.o \
	k_cos.o k_cosf.o k_rem_pio2.o k_rem_pio2f.o k_sin.o k_sinf.o \
	k_tan.o k_tanf.o \
	s_lround.o s_lroundf.o s_llround.o s_llroundf.o \
	s_asinh.o s_asinhf.o s_atan.o s_atanf.o s_cabsf.o \
	s_cargf.o \
	s_catanf.o \
	s_cbrt.o s_cbrtf.o s_ccosf.o \
	s_ccoshf.o s_ceil.o \
	s_ceilf.o s_cexpf.o s_clogf.o \
	s_copysign.o s_copysignf.o s_cos.o s_cosf.o s_cpowf.o \
	s_csinf.o \
	s_csinhf.o s_csqrtf.o s_ctanf.o \
	s_ctanhf.o s_erf.o s_erff.o s_exp2.o s_exp2f.o s_expm1.o s_expm1f.o \
	s_fabsf.o s_fma.o s_fmaf.o \
	s_floor.o s_floorf.o s_frexpf.o s_ilogb.o s_ilogbf.o \
	s_log1p.o \
	s_log1pf.o s_logb.o s_logbf.o s_llrint.o s_llrintf.o s_lrint.o \
	s_lrintf.o s_modff.o s_nan.o s_nearbyint.o s_nextafter.o \
	s_nextafterf.o s_nexttowardf.o s_remquo.o s_remquof.o s_rint.o \
	s_rintf.o \
	s_scalbn.o s_scalbnf.o s_signgam.o s_significand.o \
	s_significandf.o \
	s_sin.o s_sinf.o s_tan.o s_tanf.o s_tanh.o s_tanhf.o s_trunc.o \
	s_truncf.o w_drem.o w_dremf.o w_gamma.o w_gamma_r.o w_gammaf.o \
	w_gammaf_r.o w_lgamma.o w_lgammaf.o

#OBJECTS+=s_cacoshf.o s_casinf.o s_casinhf.o s_catanhf.o s_cacosf.o 

# LONG_SRCS:
#OBJECTS+=e_acosl.o e_asinl.o e_atan2l.o \
	s_cabsl.o s_cacoshl.o s_cacosl.o s_cargl.o \
	s_casinhl.o s_casinl.o s_catanhl.o s_catanl.o s_ccoshl.o \
	s_ccosl.o s_cexpl.o s_cimagl.o s_clogl.o s_conjl.o s_cpowl.o \
	s_cprojl.o s_creall.o s_csinhl.o s_csinl.o s_csqrtl.o \
	s_ctanhl.o s_ctanl.o \
	e_remainderl.o \
	e_sqrtl.o \
	polevll.o s_atanl.o \
	s_copysignl.o s_cosl.o s_fabsl.o \
	s_fmal.o s_fmaxl.o s_fminl.o s_frexpl.o s_ilogbl.o \
	s_llrintl.o s_llroundl.o s_logbl.o s_lrintl.o \
	s_lroundl.o \
	s_roundl.o s_scalbnl.o s_sinl.o \
	s_tanl.o

OBJECTS+=s_rintl.o

all: ${OBJECTS} ${LIB}

%.o: ${SRCDIR}/lib/libm/src/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/libm/src/ld80 -o $@ $<

%.arch.o: ${SRCDIR}/lib/libm/arch/${ARCH}/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.d: %
	CFLAGS="${CFLAGS}" ${MAKE} -C $< XGCC="${XGCC}" SRCDIR="${SRCDIR}" ARCH="${ARCH}"

${LIB}:
	@echo "done."
	@echo "Creating static library $@"
	@ar cru $@ ${OBJECTS}

install: ${DESTDIR}
	${INSTALL} -m ${BINMODE} libm.a ${DESTDIR}${ULIBDIR}/

install-local:
	/bin/cp -uv ${LIB} ${LIBDIR}/

lib-clean:
	/bin/rm -f ${LIB}

all-clean: lib-clean
	/bin/rm -f *.o

clean: lib-clean
