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

#include <sys/termios.h>

/****************************************************/

int 
openpt_posix(WIN_TASK *Task, const char *name, int flags, ...)
{
	int result = 0;
	va_list args;
	char path[PATH_MAX] = _PATH_DEV;
	WIN_NAMEIDATA wPath = {0};

	strcat(path, name);
	va_start(args, flags);
	result = __openat(Task, path_win(&wPath, path, flags | O_INODE), flags, args);
	va_end(args);
	return(result);
}

/****************************************************/

int 
term_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	int result = 0;

	if (!vfs_TIOCGWINSZ(Node, WinSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	int result = 0;

	if (!vfs_TIOCSWINSZ(Node, WinSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCFLUSH(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCFLUSH(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCDRAIN(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCDRAIN(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGETA(WIN_VNODE *Node, WIN_TERMIO *Attribs)
{
	int result = 0;

	if (!vfs_TIOCGETA(Node, Attribs)){
		result -= errno_posix(GetLastError());
	}else{
		*Attribs = __Terminals[Node->Index].Attribs;
	}
	return(result);
}
int 
term_TIOCSETA(WIN_TASK *Task, WIN_VNODE *Node, WIN_TERMIO *Attribs)
{
	int result = 0;

	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "termios", 7, Attribs, sizeof(struct termios));
	}
	if (!vfs_TIOCSETA(Node, Attribs, FALSE, FALSE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Attribs = *Attribs;
	}
	return(result);
}
int 
term_TIOCSETAW(WIN_TASK *Task, WIN_VNODE *Node, WIN_TERMIO *Attribs)
{
	int result = 0;

	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "termios", 7, Attribs, sizeof(struct termios));
	}
	if (!vfs_TIOCSETA(Node, Attribs, FALSE, TRUE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Attribs = *Attribs;
	}
	return(result);
}
int 
term_TIOCSETAF(WIN_TASK *Task, WIN_VNODE *Node, WIN_TERMIO *Attribs)
{
	int result = 0;

	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "termios", 7, Attribs, sizeof(struct termios));
	}
	if (!vfs_TIOCSETA(Node, Attribs, TRUE, TRUE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Attribs = *Attribs;
	}
	return(result);
}
int 
term_TIOCSCTTY(WIN_TASK *Task, WIN_VNODE *Node)
{
	int result = 0;
	WIN_DEVICE *pwDevice;

	if (Task->Flags & WIN_PS_CONTROLT){
		result = -EPERM;
	}else if (!vfs_TIOCSCTTY(Task, Node)){
		result -= errno_posix(GetLastError());
	}else{
		pwDevice = DEVICE(Node->DeviceId);
		Node->Index = pwDevice->Index;
		Node->Event = pwDevice->Event;
		Node->FSType = pwDevice->FSType;
	}
	return(result);
}
int 
term_PTMGET(WIN_TASK *Task, WIN_VNODE *Node, WIN_PTMGET *Result)
{
	int result = 0;
	WIN_DEVICE *pwDevice;

	if (!vfs_PTMGET(Node, Result)){
		result -= errno_posix(GetLastError());
	}else{
		/* raw serial device (master) */
		pwDevice = DEVICE(Node->DeviceId);
		Node->Index = pwDevice->Index;
		Node->Event = pwDevice->Event;
		Result->Master = __dup(Task, Node);
		/* controlling terminal (slave) */
		Result->Slave = openpt_posix(Task, Result->SName, 0, 0666);
	}
	return(result);
}
int 
term_TIOCSPGRP(WIN_VNODE *Node, UINT *GroupId)
{
	int result = 0;

	if (!vfs_TIOCSPGRP(Node, *GroupId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGPGRP(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGPGRP(Node, Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGETD(UINT *Result)
{
//	*Result = (__CTTY->Mode.Input & ENABLE_LINE_INPUT);
	return(0);
}
int 
term_TIOCSFLAGS(WIN_VNODE *Node, UINT *Flags)
{
	int result = 0;

	if (!vfs_TIOCSFLAGS(&__Terminals[Node->Index], *Flags)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGFLAGS(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGFLAGS(&__Terminals[Node->Index], Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
term_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
{
	int result = 0;
	WIN_VNODE *pvNode = &Task->Node[fd];

	/* tty(4)
	 */
	if (!pvNode->Access){
		result = -EBADF;
	}else switch (request){
		case TIOCFLUSH:
			result = term_TIOCFLUSH(pvNode);
			break;
		case TIOCDRAIN:
			result = term_TIOCDRAIN(pvNode);
			break;
		case PTMGET:
			result = term_PTMGET(Task, pvNode, va_arg(args, WIN_PTMGET *));
			break;
		case TIOCSCTTY:
			result = term_TIOCSCTTY(Task, pvNode);
			break;
		case TIOCGETA:
			result = term_TIOCGETA(pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCGWINSZ:
			result = term_TIOCGWINSZ(pvNode, va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSWINSZ:
			result = term_TIOCSWINSZ(pvNode, va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSETA:
			result = term_TIOCSETA(Task, pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSETAW:
			result = term_TIOCSETAW(Task, pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSETAF:
			result = term_TIOCSETAF(Task, pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSPGRP:
			result = term_TIOCSPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGPGRP:
			result = term_TIOCGPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGETD:
			result = term_TIOCGETD(va_arg(args, UINT *));
			break;
		case TIOCSFLAGS:		/* ttyflags.exe */
			result = term_TIOCSFLAGS(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGFLAGS:
			result = term_TIOCGFLAGS(pvNode, va_arg(args, UINT *));
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
