/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <sys/signal.h>

#define BIT_SIGKILL		0x00000100
#define BIT_SIGURG		0x00008000
#define BIT_SIGSTOP		0x00010000
#define BIT_SIGTSTP		0x00020000
#define BIT_SIGCONT		0x00040000
#define BIT_SIGCHLD		0x00080000
#define BIT_SIGTTIN		0x00100000
#define BIT_SIGTTOU		0x00200000
#define BIT_SIGIO		0x00400000
#define BIT_SIGWINCH		0x08000000
#define BIT_SIGINFO		0x10000000
#define BIT_SIGTHR		0x80000000	/* null signal (child detaching) */

#define SIGMASK_STOP	(BIT_SIGSTOP | BIT_SIGTSTP | BIT_SIGTTIN | BIT_SIGTTOU | BIT_SIGCONT)
#define SIGMASK_IGNORE	(BIT_SIGWINCH | BIT_SIGCHLD | BIT_SIGURG | BIT_SIGINFO | BIT_SIGIO)

typedef void (*action_t)(int, siginfo_t *, void *);

static const 
DWORD __SIG_WIN[NSIG] = {
	0,
	CTRL_LOGOFF_EVENT,
	CTRL_C_EVENT,
	CTRL_QUIT_EVENT,
	CTRL_ILLEGAL_INSTRUCTION_EVENT,
	CTRL_TRAP_EVENT,
	CTRL_BREAK_EVENT,
	CTRL_EMULATOR_EVENT,
	CTRL_DIVIDE_BY_ZERO_EVENT,
	CTRL_CLOSE_EVENT,		/* closing Console window */
	CTRL_BUS_EVENT,			/* 10 */
	CTRL_ACCESS_VIOLATION_EVENT,
	CTRL_INVALID_ARGUMENT_EVENT,
	CTRL_PIPE_EVENT,
	CTRL_TIMER_EVENT,
	CTRL_SHUTDOWN_EVENT,
	CTRL_URGENT_EVENT,
	CTRL_STOP_EVENT,
	CTRL_TERMINAL_STOP_EVENT,
	CTRL_CONTINUE_EVENT,
	CTRL_CHILD_EVENT,		/* 20 */
	CTRL_BACKGROUND_READ_EVENT,
	CTRL_BACKGROUND_WRITE_EVENT,
	CTRL_IO_EVENT,
	CTRL_EXCEDED_CPUTIME_EVENT,
	CTRL_EXCEDED_FILE_SIZE_EVENT,
	CTRL_VTIMER_EVENT,
	CTRL_PROFILING_EVENT,
	CTRL_SIZE_EVENT,
	CTRL_INFO_EVENT,
	CTRL_USER1_EVENT,		/* 30 */
	CTRL_USER2_EVENT,
	CTRL_DETACH_EVENT
};
static const 
int __SIG_POSIX[NSIG] = {
	SIGINT,
	SIGABRT,		/* sent by abort() */
	SIGKILL,		/* cannot be caught or ignored */
	SIGSEGV,
	SIGILL,
	SIGHUP,
	SIGTERM,		/* default of kill command */
	SIGFPE,
	SIGWINCH,
	SIGCHLD,
	SIGALRM,	/* 10 */
	SIGTHR,
	SIGQUIT,		/* SIGTERM with coredump */
	SIGPIPE,
	SIGTSTP,		/* Software flow control (XON/XOFF) */
	SIGVTALRM,		/* SIGALRM in program time */
	SIGURG,
	SIGUSR1,
	SIGUSR2,
	SIGSYS,
	SIGEMT,		/* 20 */
	SIGBUS,
	SIGINFO,
	SIGSTOP,		/* Motor stop feature (CuriousMarc, 2018) */
	SIGCONT,
	SIGTTIN,
	SIGTTOU,
	SIGIO,
	SIGTRAP,
	SIGXCPU,
	SIGXFSZ,	/* 30 */
	SIGPROF,
	0
};

/****************************************************/

ucontext_t *
context_posix(ucontext_t *ucontext, CONTEXT *Context)
{
	ucontext->sc_edi = Context->Edi;
	ucontext->sc_esi = Context->Esi;
	ucontext->sc_ebp = Context->Ebp;
	ucontext->sc_ebx = Context->Ebx;
	ucontext->sc_edx = Context->Edx;
	ucontext->sc_ecx = Context->Ecx;
	ucontext->sc_eax = Context->Eax;
	ucontext->sc_eip = Context->Eip;
	ucontext->sc_esp = Context->Esp;
	ucontext->sc_cs = Context->SegCs;
	ucontext->sc_es = Context->SegEs;
	ucontext->sc_ds = Context->SegDs;
	ucontext->sc_fs = Context->SegFs;
	ucontext->sc_gs = Context->SegGs;
	ucontext->sc_ss = Context->SegSs;
	return(ucontext);
}
int 
sigproc_default(WIN_TASK *Task, int signum)
{
	sigset_t sigbit = sigmask(signum);
	sigset_t mask = 0;

	/* Return value must be -1 because non-lethal signals
	 * are discarded by default, see signal(2).
	 */
	if (sigbit & SIGMASK_STOP){
		Task->Status = _WSTOPPED;
		SetEvent(__Interrupt);
	}else if (sigbit & ~SIGMASK_IGNORE){
		Task->Status = signum;
		__exit(Task, 127);
	}
	return(-1);
}
int 
sigproc_posix(WIN_TASK *Task, int signum, ucontext_t *ucontext)
{
	int result = 0;
	struct sigaction *sigaction = (void *)&Task->Action[signum];
	sig_t handler = sigaction->sa_handler;
	int flags = sigaction->sa_flags;
	action_t action = sigaction->sa_sigaction;
	siginfo_t info = {0};
	sigset_t sigbit = sigmask(signum);

	info.si_signo = signum;
	info.si_errno = Task->Error;
	info.si_pid = Task->TaskId;
	info.si_uid = rid_posix(&Task->UserSid);
	info.si_status = Task->Status;
	info.si_addr = (void *)ucontext->sc_eip;
	if (Task->TracePoints & KTRFAC_PSIG){
		ktrace_PSIG(Task, signum, handler, &info);
	}
//	if (flags & SA_RESTART){		/* supersedes SIG_IGN */
//		result = -1;
//	}
	/* This will hang during boot.
	 */
//	if (Task->ProcMask & sigbit){
//		Task->Pending = __SIG_WIN[signum];
//		result = -1;
//	}else 
	if (handler == SIG_DFL){		/* terminate, if applicable */
		result = sigproc_default(Task, signum);
	}else if (handler == SIG_ERR){		/* don't terminate, but interrupt system call */
		Task->Status = (Task->Error * 0x100) + signum;
	}else if (handler == SIG_IGN){		/* don't terminate, don't interrupt */
		result = -1;
	}else if (flags & SA_SIGINFO){
		action(signum, &info, ucontext);
	}else{
		handler(signum);
	}
	return(result);
}
BOOL CALLBACK 
sigproc_win(DWORD CtrlType, CONTEXT *Context)
{
	BOOL bResult = FALSE;
	ucontext_t ucontext = {0};
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!sigproc_posix(pwTask, __SIG_POSIX[CtrlType], context_posix(&ucontext, Context))){
		bResult = TRUE;
	}
	return(bResult);
}
int
kill_GRP(WIN_TASK *Task, DWORD GroupId, int sig)
{
	int result = 0;

	if (!vfs_kill_GRP(GroupId, WM_COMMAND, __SIG_WIN[sig], Task->TaskId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int
kill_SYS(WIN_TASK *Task, int sig)
{
	int result = 0;

	if (!vfs_kill_SYS(Task->TaskId, WM_COMMAND, __SIG_WIN[sig], Task->TaskId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/************************************************************/

int 
sys_sigaction(call_t call, int signum, const struct sigaction *act, struct sigaction *oldact)
{
	int result = 0;
	struct sigaction *action;
	struct sigaction tmp = {0};
	WIN_TASK *pwTask = call.Task;

	if (signum < 0 || signum >= NSIG){
		return(-EINVAL);
	}else{
		action = (PVOID)&pwTask->Action[signum];
	}
	if (oldact){
		*oldact = *action;
	}
	if ((sig_t)act > SIG_IGN){
		tmp = *act;
	}else{
		tmp.sa_handler = (sig_t)act;
	}
	if (pwTask->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(pwTask, "sigaction", 9, &tmp, sizeof(struct sigaction));
	}
	switch (signum){
		case SIGKILL:
		case SIGSTOP:
			result = -EINVAL;
			break;
		default:
			*action = tmp;
	}
	return(result);
}
int 
sys_sigprocmask(call_t call, int how, const sigset_t *restrict set, sigset_t *restrict oldset)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	sigset_t curset = pwTask->ProcMask;
	sigset_t newset;

	if (oldset){
		*oldset = curset;
	}
	if (how & ~SIG_SETMASK){
		result = -EINVAL;
	}else if (!set){
		result = -EFAULT;
	}else{
		newset = *set & ~(BIT_SIGKILL | BIT_SIGSTOP);
		if (how == SIG_BLOCK){		// 1
			curset |= newset;
		}else if (how == SIG_UNBLOCK){	// 2
			curset &= ~newset;
		}else if (how == SIG_SETMASK){	// 3
			curset = newset;
		}
		pwTask->ProcMask = curset;
	}
	return(result);
}
int 
sys_sigsuspend(call_t call, const sigset_t *mask)
{
	int result = 0;

	if (!mask){
		result = -EFAULT;
	}else if (!vfs_sigsuspend(call.Task, mask)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_sigpending(call_t call, sigset_t *set)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!set){
		result = -EFAULT;
	}else{
		*set = pwTask->Pending;
	}
	return(result);
}
int 
sys_kill(call_t call, pid_t pid, int sig)
{
	int result = 0;
	ucontext_t ucontext = {0};
	WIN_TASK *pwTask = call.Task;

	if (sig >= NSIG){
		result = -EINVAL;
	}else if (pid == -1){
		result = kill_SYS(pwTask, sig);
	}else if (pid < 0){
		result = kill_GRP(pwTask, -pid, sig);
	}else if (!sig){
		if (pid_win(pid) == 0){
			result = -ESRCH;
		}
	}else if (!pid){
		result = kill_GRP(pwTask, pwTask->GroupId, sig);
	}else if (pid >= CHILD_MAX){
		result = -EINVAL;
	}else if (!vfs_kill_PID(pid_win(pid), WM_COMMAND, __SIG_WIN[sig], pwTask->TaskId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
