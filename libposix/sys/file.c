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

#include <sys/file.h>

/****************************************************/

void *
kfile_posix(WIN_TASK *Task, WIN_VNODE *Node, struct kinfo_file *buf)
{
	WIN_VATTR wStat = {0};
	struct stat info;
	WCHAR szBuffer[MAX_PATH];
	HANDLE hResult = NULL;
	wchar_t *command = win_basename(__Strings[Task->TaskId].Command);

	/* Below stat_posix() tracing will depend on Task's PS flags.
	 */
	win_bzero(buf, sizeof(struct kinfo_file));
	buf->f_flag = FFLAGS(flflags_posix(Node));	/* SHORT: flags (see fcntl.h) */
	buf->f_type = WIN_DTYPE_VNODE;			/* INT: descriptor type */
	buf->v_type = Node->FileType;			/* ENUM: vnode type */
	buf->p_pid = Task->TaskId;			/* PID_T: process id */
	buf->fd_fd = Node->FileId;			/* INT: descriptor number */
	buf->v_tag = WIN_VT_NTFS;			/* ENUM: type of underlying data */
	buf->p_uid = rid_posix(&Task->UserSid);		/* UID_T: process credentials */
	buf->p_gid = rid_posix(&Task->GroupSid);	/* GID_T: process credentials */
	buf->p_tid = Task->ThreadId;			/* PID_T: thread id */
	win_wcstombs(buf->p_comm, command, KI_MAXCOMLEN);
	if (vfs_fstat(Node, &wStat)){
		stat_posix(Task, &info, &wStat);
		buf->va_fsid = info.st_dev;		/* DEV_T: filesystem device */
		buf->va_rdev = info.st_rdev;		/* DEV_T: raw device */
		buf->va_mode = info.st_mode;		/* MODE_T: file access mode and type */
		buf->va_fileid = info.st_ino;		/* LONG: file id */
		buf->f_uid = info.st_uid;		/* UID_T: descriptor credentials */
		buf->f_gid = info.st_gid;		/* GID_T: descriptor credentials */
		buf->va_size = info.st_size;
	}else{
		WIN_ERR("vfs_fstat(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
		vfs_ktrace(L"kfile_posix", STRUCT_VNODE, Node);
	}
	return(buf + 1);
}

/****************************************************/

void *
file_KERN_FILE_BYPID(WIN_TASK *Task, void *buf, size_t *size)
{
	size_t result = 0;
	int fd = 0;
	WIN_VNODE *pwNode = Task->Node;

	while (fd < OPEN_MAX){
		if (pwNode->Access){
			if (buf){
				buf = kfile_posix(Task, pwNode, buf);
			}
			result += sizeof(struct kinfo_file);
		}
		pwNode++;
		fd++;
	}
	*size += result;
	return(buf);
}
int 
file_KERN_FILE(const int *name, void *buf, size_t *size)
{
	int result = 0;
	pid_t pid = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[pid];
	DWORD dwRemain;

	*size = 0;
	while (pid < CHILD_MAX){
		if (pwTask->Flags && pwTask->State != WIN_SZOMB){
			buf = file_KERN_FILE_BYPID(pwTask, buf, size);
		}
		pwTask++;
		pid++;
	}
	return(0);
}
