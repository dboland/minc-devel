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

#include <sys/errno.h>

/****************************************************/

int 
errno_posix(DWORD Error)
{
	int result = ECANCELED;

	switch (Error){
		case ERROR_NO_MORE_FILES:
		case ERROR_SUCCESS:			/* read() and write() operations */
			result = 0;
			break;
		case WSAEINVAL:
		case ERROR_INVALID_PRIMARY_GROUP:
		case ERROR_BAD_ARGUMENTS:		/* 160 */
		case ERROR_NOT_LOCKED:			/* 158: The segment is already unlocked */
		case ERROR_INVALID_PARAMETER:		/* 87: The parameter is incorrect (mailslot_read()) */
			result = EINVAL;
			break;
		case ERROR_NO_SUCH_ALIAS:		/* 1376: The specified local group does not exist. */
		case NERR_GroupNotFound:
		case NERR_UserNotFound:
		case ERROR_NONE_MAPPED:			/* 1332: No mapping between account names and security IDs was done. */
		case NTE_BAD_KEYSET:
		case ERROR_MOD_NOT_FOUND:
		case ERROR_FILE_NOT_FOUND:
		case ERROR_INVALID_NAME:		/* 123: The filename, directory name, or volume label syntax is incorrect */
		case ERROR_PROC_NOT_FOUND:		/* 127: The specified procedure could not be found (mutt.exe) */
		case ERROR_PATH_NOT_FOUND:		/* 3: The system cannot find the file specified (gcc.exe) */
			result = ENOENT;
			break;
		case ERROR_PRIVILEGE_NOT_HELD:
		case ERROR_NOT_ALL_ASSIGNED:		/* 1300: AdjustTokenPrivileges() */
		case ERROR_LOGON_SESSION_EXISTS:
		case ERROR_INVALID_OPERATION:
		case ERROR_ELEVATION_REQUIRED:
			result = EPERM;
			break;
		case ERROR_DIRECTORY:			/* 267: The directory name is invalid */
		case ERROR_BAD_PATHNAME:		/* 161: The specified path is invalid */
			result = ENOTDIR;
			break;
		case ERROR_INSUFFICIENT_BUFFER:
			result = ENOBUFS;
			break;
		case ERROR_NOT_ENOUGH_MEMORY:
			result = ENOMEM;
			break;
		case ERROR_PARAMETER_QUOTA_EXCEEDED:
			result = E2BIG;
			break;
		case ERROR_BUFFER_OVERFLOW:
			result = EOVERFLOW;
			break;
		case ERROR_FILENAME_EXCED_RANGE:
			result = ENAMETOOLONG;
			break;
		case WSAEACCES:
		case ERROR_SQL_ACCESS_DENIED:
		case ERROR_ACCESS_DENIED:
			result = EACCES;
			break;
		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:		/* 183: MoveFileEx() on directory */
			result = EEXIST;
			break;
		case ERROR_DIR_NOT_EMPTY:
			result = ENOTEMPTY;
			break;
		case ERROR_PATH_BUSY:
			result = EISDIR;
			break;
		case ERROR_UNRECOGNIZED_VOLUME:
		case ERROR_BAD_FILE_TYPE:
			result = EFTYPE;
			break;
		case ERROR_BAD_PIPE:			/* 230: The pipe state is invalid */
			result = ENOTCONN;
			break;
		case WSAEBADF:
		case ERROR_INVALID_HANDLE:
			result = EBADF;
			break;
		case ERROR_SHARING_VIOLATION:
			result = ETXTBSY;
			break;
		case ERROR_BAD_EXE_FORMAT:
			result = ENOEXEC;
			break;
		case ERROR_INVALID_FUNCTION:		/* trying to stat DOS devices (COM1, LPT1, CON, AUX) */
		case ERROR_NOT_SUPPORTED:
			result = EOPNOTSUPP;
			break;
		case ERROR_INVALID_THREAD_ID:
			result = ESRCH;
			break;
		case ERROR_NO_DATA:			/* 232: The pipe is being closed (read end closed) */
		case ERROR_PIPE_NOT_CONNECTED:		/* 233: No process is on the other end of the pipe. */
			vfs_raise(WM_SIGNAL, CTRL_PIPE_EVENT, 0);
		case ERROR_BROKEN_PIPE:			/* 109: The pipe has been ended (write end closed) */
			result = EPIPE;
			break;
		case ERROR_PIPE_CONNECTED:		/* 535: There is a process on other end of the pipe */
			result = ESPIPE;
			break;
		case ERROR_WAIT_NO_CHILDREN:
			result = ECHILD;
			break;
		case ERROR_TOO_MANY_OPEN_FILES:
			result = EMFILE;
			break;
		case WSAEFAULT:
		case ERROR_NOACCESS:			/* 998: Invalid access to memory location */
		case ERROR_INVALID_ADDRESS:
			result = EFAULT;
			break;
		case ERROR_NOT_READY:
			result = EBUSY;
			break;
		case WSAEINTR:
		case ERROR_SIGNAL_PENDING:
			result = EINTR;
			break;
		case ERROR_BAD_DEVICE:
			result = ENODEV;
			break;
		case ERROR_DEVICE_NOT_PARTITIONED:
		case ERROR_DEVICE_NOT_AVAILABLE:
			result = ENXIO;
			break;
		case ERROR_TOO_MANY_LINKS:
			result = ELOOP;
			break;
		case ERROR_CTX_NOT_CONSOLE:
			result = ENOTTY;
			break;
		case ERROR_NO_MORE_ITEMS:
		case ERROR_IO_DEVICE:
		case ERROR_HANDLE_EOF:			/* 38: on exit when xterm enabled (vim.exe) */
		case ERROR_INVALID_USER_BUFFER:
		case ERROR_OPERATION_ABORTED:		/* 995: The I/O operation has been aborted because of either a thread exit or an application request. */
			result = EIO;
			break;
		case ERROR_PIPE_BUSY:			/* 231: All pipe instances are busy */
			SetEvent(__PipeEvent);		/* logger.exe */
		case ERROR_MORE_DATA:
		case ERROR_IO_PENDING:
		case ERROR_LOCK_VIOLATION:		/* 33: Another process has locked a portion of the file. */
			result = EAGAIN;
			break;
		case ERROR_NOT_SAME_DEVICE:
		case ERROR_UNABLE_TO_MOVE_REPLACEMENT:
			result = EXDEV;
			break;
		case ERROR_GEN_FAILURE:			/* 31: A device attached to the system is not functioning. */
			result = ENOTBLK;
			break;
		case WSAECONNABORTED:
		case ERROR_CONNECTION_ABORTED:
		case ERROR_NETNAME_DELETED:
			result = ECONNABORTED;
			break;
		case WSAEALREADY:
			result = EALREADY;
			break;
		case WSAEWOULDBLOCK:
			result = EWOULDBLOCK;
			break;
		case WSAETIMEDOUT:
			result = ETIMEDOUT;
			break;
		case WSAECONNRESET:
		case ERROR_PIPE_LISTENING:		/* 536: ReadFile() after DisconnectNamedPipe() */
			result = ECONNRESET;
			break;
		case WSAENOTSOCK:
			result = ENOTSOCK;
			break;
		case WSAEISCONN:
			result = EISCONN;
			break;
		case WSAENOTCONN:
		case ERROR_UNEXP_NET_ERR:		/* 59: WriteFile() on unconnected socket */
			result = ENOTCONN;
			break;
		case WSAEPFNOSUPPORT:
			result = EPFNOSUPPORT;
			break;
		case WSAECONNREFUSED:
			result = ECONNREFUSED;
			break;
		case WSAENOPROTOOPT:
			result = ENOPROTOOPT;
			break;
		case ERROR_BAD_NETPATH:			/* 53: The network path was not found */
		case WSAEHOSTUNREACH:
			result = EHOSTUNREACH;
			break;
		case WSAEAFNOSUPPORT:
			result = EAFNOSUPPORT;
			break;
		case WSAEADDRNOTAVAIL:
			result = EADDRNOTAVAIL;
			break;
		case WSAEINPROGRESS:
			result = EINPROGRESS;
			break;
		case WSAEADDRINUSE:
			result = EADDRINUSE;
			break;
		default:
			WIN_ERR("errno_posix(%d): %s\n", Error, win_strerror(Error));
	}
	return(result);
}
DWORD 
errno_win(VOID)
{
	return(GetLastError());
}
