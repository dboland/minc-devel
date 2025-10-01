#include <stdio.h>

#include "sh.h"

const char const* __E_TYPE[] = {
	"E_NONE",
	"E_PARSE",
	"E_FUNC",
	"E_INCL",
	"E_EXEC",
	"E_LOOP",
	"E_ERRH"
};
const char const* __STATE[] = {
	"PRUNNING",
	"PEXITED",
	"PSIGNALLED",
	"PSTOPPED"
};
const char const* __L_TYPE[] = {
	"LNONE",
	"LRETURN",
	"LEXIT",
	"LERROR",
	"LLEAVE",
	"LINTR",
	"LBREAK",
	"LCONTIN",
	"LSHELL",
	"LAEXPR"
};
const char const* __T_TYPE[] = {
	"TEOF",
	"TCOM",
	"TPAREN",
	"TPIPE",
	"TLIST",
	"TOR",
	"TAND",
	"TBANG",
	"TDBRACKET",
	"TFOR",
	"TSELECT",
	"TCASE",
	"TIF",
	"TWHILE",
	"TUNTIL",
	"TELIF",
	"TPAT",
	"TBRACE",
	"TASYNC",
	"TFUNCT",
	"TTIME",
	"TEXEC",
	"TCOPROC"
};
const char const* __C_TYPE[] = {
	"CNONE",
	"CSHELL",
	"CFUNC",
	"XXXXXX",
	"CEXEC",
	"CALIAS",
	"CKEYWD",
	"CTALIAS"
};
const char const* __X_TYPE[] = {
	"XBASE",
	"XSUB",
	"XARGSEP",
	"XARG",
	"XCOM",
	"XNULLSUB",
	"XSUBMID"
};

char __X_BUF[255];

char *
__X_FLAGS(int flags)
{
	char *buf = __X_BUF;

	*buf = 0;
	if (flags & XEXEC)
		buf += sprintf(buf, "[XEXEC]");
	if (flags & XFORK)
		buf += sprintf(buf, "[XFORK]");
	if (flags & XBGND)
		buf += sprintf(buf, "[XBGND]");
	if (flags & XPIPEI)
		buf += sprintf(buf, "[XPIPEI]");
	if (flags & XPIPEO)
		buf += sprintf(buf, "[XPIPEO]");
	if (flags & XXCOM)
		buf += sprintf(buf, "[XXCOM]");
	if (flags & XPCLOSE)
		buf += sprintf(buf, "[XPCLOSE]");
	if (flags & XCCLOSE)
		buf += sprintf(buf, "[XCCLOSE]");
	if (flags & XERROK)
		buf += sprintf(buf, "[XERROK]");
	if (flags & XCOPROC)
		buf += sprintf(buf, "[XCOPROC]");
	if (flags & XTIME)
		buf += sprintf(buf, "[XTIME]");
	return(__X_BUF);
}

/* Flags for j_waitj() */
#define JW_NONE		0x00
#define JW_INTERRUPT	0x01	/* ^C will stop the wait */
#define JW_ASYNCNOTIFY	0x02	/* asynchronous notification during wait ok */
#define JW_STOPPEDWAIT	0x04	/* wait even if job stopped */

char __JW_BUF[255];

char *
__JW_FLAGS(int flags)
{
	char *buf = __JW_BUF;

	*buf = 0;
	if (flags & JW_INTERRUPT)
		buf += sprintf(buf, "[JW_INTERRUPT]");
	if (flags & JW_ASYNCNOTIFY)
		buf += sprintf(buf, "[JW_ASYNCNOTIFY]");
	if (flags & JW_STOPPEDWAIT)
		buf += sprintf(buf, "[JW_STOPPEDWAIT]");
	return(__JW_BUF);
}
