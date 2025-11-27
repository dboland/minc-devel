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

#include <uvm/uvm_param.h>	/* VM_LOADAVG */
#include <uvm/uvm_extern.h>	/* struct uvmexp */

/****************************************************/

int 
sysctl_KERN(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case KERN_CLOCKRATE:		/* mcount() */
			result = kern_KERN_CLOCKRATE((struct clockinfo *)oldp);
			break;
		case KERN_RAWPARTITION:	/* GNU conftest.exe */
			*(int *)oldp = 0;
			break;
		case KERN_MAXPARTITIONS:
			*(int *)oldp = MAXPARTITIONS;
			break;
		case KERN_SAVED_IDS:		/* vim.exe */
			*(int *)oldp = 1;
			break;
		case KERN_JOB_CONTROL:
			*(int *)oldp = 1;
			break;
		case KERN_MAXVNODES:
			*(int *)oldp = OPEN_MAX;
			break;
		case KERN_HOSTNAME:
			result = kern_KERN_HOSTNAME((char *)oldp, oldlenp, (const char *)newp, newlen);
			break;
		case KERN_DOMAINNAME:
			result = kern_KERN_DOMAINNAME((char *)oldp, oldlenp, (const char *)newp, newlen);
			break;
		case KERN_OSTYPE:
			win_strlcpy(oldp, "OpenBSD", *oldlenp);
			break;
		case KERN_OSRELEASE:
			win_strlcpy(oldp, RELEASE, *oldlenp);
			break;
		case KERN_OSVERSION:
			win_strlcpy(win_stpcpy(oldp, "MINC#"), VERSION, *oldlenp - 5);
			break;
		case KERN_ARGMAX:
			*(int *)oldp = MAX_ARGBUF - (MAX_ARGBUF % __Globals->PageSize);
			break;
		case KERN_MSGBUFSIZE:	/* dmesg.exe */
			result = msgbuf_KERN_MSGBUFSIZE((int *)oldp, oldlenp);
			break;
		case KERN_MSGBUF:
			result = msgbuf_KERN_MSGBUF((char *)oldp, oldlenp);
			break;
		case KERN_PROC:
			result = kern_KERN_PROC(name, oldp, oldlenp);
			break;
		case KERN_PROC_ARGS:
			result = kern_KERN_PROC_ARGS(name, oldp, oldlenp);
			break;
		case KERN_FSCALE:	/* The kernel fixed-point scale factor (ps.exe) */
			*(int *)oldp = FSCALE;
			break;
		case KERN_CCPU:		/* The scheduler exponential decay value (ps.exe) */
			*(int *)oldp = __CCPU;
			break;
		case KERN_CPTIME:	/* the number of ticks spent by the system (top.exe) */
			result = kern_KERN_CPTIME((long *)oldp);
			break;
		case KERN_CPTIME2:	/* ticks per CPU (top.exe) */
			result = kern_KERN_CPTIME2(name[2], (u_int64_t *)oldp);
			break;
		case KERN_VERSION:
			result = kern_KERN_VERSION(oldp, *oldlenp);
			break;
		case KERN_SECURELVL:
			result = kern_KERN_SECURELVL((int *)oldp, (int *)newp);
			break;
		case KERN_TTYCOUNT:
			*(int *)oldp = WIN_TTY_MAX;
			break;
		case KERN_FILE:		/* fstat.exe */
			result = file_KERN_FILE(name, oldp, oldlenp);
			break;
		case KERN_ARND:		/* makewhatis.exe */
			result = kern_KERN_ARND((char *)oldp, *(size_t *)oldlenp);
			break;
		case KERN_BOOTTIME:	/* w.exe */
			result = kern_KERN_BOOTTIME((struct timeval *)oldp);
			break;
		case KERN_HOSTID:	/* alpine.exe */
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

void 
sysctl_HW_DISKNAMES(char *buf, size_t size)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_STORAGE);
	int unit = 0;
	char name[MAX_NAME];
	char *sep = "";
	int len;

	while (unit < WIN_UNIT_MAX){
		if (pwDevice->Flags){
			if (size < MAX_NAME){
				break;
			}else if (disk_HW_DISKNAMES(pwDevice, name)){
				len = sprintf(buf, "%s%s", sep, name);
				buf += len;
				size -= len;
				sep = ",";
			}
		}
		unit++;
		pwDevice++;
	}
}
int 
sysctl_HW(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case HW_MACHINE:
			win_strlcpy(oldp, MACHINE, *oldlenp);
			break;
		case HW_PAGESIZE:
			*(int *)oldp = __Globals->PageSize;
			break;
		case HW_PHYSMEM:	/* gcc (collect2.exe) */
			*(int *)oldp = win_HW_PHYSMEM();
			break;
		case HW_PHYSMEM64:
			*(quad_t *)oldp = win_HW_PHYSMEM64();
			break;
		case HW_USERMEM:
			*(int *)oldp = win_HW_USERMEM();
			break;
		case HW_NCPU:
			*(int *)oldp = win_HW_NCPU();
			break;
		case HW_DISKNAMES:
			sysctl_HW_DISKNAMES((char *)oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_VM_LOADAVG(struct loadavg *load, size_t size)
{
	/* Not sure what this should be.
	 * Let OpenBSD figure it out (top.exe).
	 */
	load->ldavg[0] = 1 * FSCALE;	/* 1 min. */
	load->ldavg[1] = 1 * FSCALE;	/* 5 min. */
	load->ldavg[2] = 1 * FSCALE;	/* 15 min. */
	load->fscale = FSCALE;
	return(0);
}
int 
sysctl_VM_UVMEXP(struct uvmexp *uvm, size_t size)
{
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};
	DWORD dwPageSize;
	DWORDLONG dwlTotal, dwlAvail, dwlUsed;
	SYSTEM_INFO sInfo;

	win_bzero(uvm, size);
	GetSystemInfo(&sInfo);
	GlobalMemoryStatusEx(&msInfo);
	dwPageSize = sInfo.dwPageSize;

	uvm->pagemask = sInfo.dwActiveProcessorMask;

	dwlTotal = msInfo.ullTotalPhys;
	dwlAvail = msInfo.ullAvailPhys;
	dwlUsed = dwlTotal - dwlAvail;

	uvm->active = (dwlUsed + dwPageSize - 1) / dwPageSize;
	uvm->inactive = (dwlAvail + dwPageSize - 1) / dwPageSize;

	dwlTotal = msInfo.ullTotalPageFile;
	dwlAvail = msInfo.ullAvailPageFile;
	dwlUsed = dwlTotal - dwlAvail;

	uvm->nswapdev = 1;
	uvm->swpages = (dwlTotal + dwPageSize - 1) / dwPageSize;
	uvm->swpginuse = (dwlUsed + dwPageSize - 1) / dwPageSize;
	uvm->swpgonly = uvm->swpginuse;

	dwlTotal = msInfo.ullTotalVirtual;
	dwlAvail = msInfo.ullAvailVirtual;
	dwlUsed = dwlTotal - dwlAvail;

	uvm->pagesize = dwPageSize;
	uvm->npages = (dwlTotal + dwPageSize - 1) / dwPageSize;
	uvm->free = (dwlAvail + dwPageSize - 1) / dwPageSize;

	return(0);
}
int 
sysctl_VM(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case VM_LOADAVG:
			result = sysctl_VM_LOADAVG((struct loadavg *)oldp, *oldlenp);
			break;
		case VM_MAXSLP:			/* time to be blocked before being swappable (ps.exe) */
			*(int *)oldp = 0;
			break;
		case VM_UVMEXP:			/* statistics about the UVM memory management system (top.exe) */
			result = sysctl_VM_UVMEXP((struct uvmexp *)oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_NET_RT(const int *name, void *buf, size_t *size)
{
	int result = 0;

	/* net/route.c */

	switch (name[4]){
		case NET_RT_DUMP:	/* 1 */
			result = route_NET_RT_DUMP(buf, size);
			break;
		case NET_RT_FLAGS:	/* 2 */
			result = route_NET_RT_FLAGS(name, buf, size);
			break;
		case NET_RT_IFLIST:	/* 3 */
			result = route_NET_RT_IFLIST(buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET_INET(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case IPPROTO_IP:	/* netinet/ip.c */
			result = ip_NET_INET_IP(name, buf, size);
			break;
		case IPPROTO_TCP:	/* netinet/tcp.c */
			result = tcp_NET_INET_TCP(name, buf, size);
			break;
		case IPPROTO_UDP:	/* netinet/udp.c */
			result = udp_NET_INET_UDP(name, buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET_INET6(const int *name, void *buf, size_t *size)
{
	int result = 0;

	/* netinet6/in6.c */

	switch (name[2]){
		case IPPROTO_IPV6:	/* 41 */
			result = ip6_NET_INET6_IPV6(name, buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case PF_ROUTE:
			result = sysctl_NET_RT(name, oldp, oldlenp);
			break;
		case PF_INET:
			result = sysctl_NET_INET(name, oldp, oldlenp);
			break;
		case PF_INET6:
			result = sysctl_NET_INET6(name, oldp, oldlenp);
			break;
		case PF_KEY:
			*oldlenp = 0;		/* no IPSEC (netstat.exe) */
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_VFS_BCACHESTAT(struct bcachestats *bcstats, size_t size)
{
	win_bzero(bcstats, size);
	return(0);
}
int 
sysctl_VFS_GENERIC(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case VFS_BCACHESTAT:		/* nbytes in VNODE cache (top.exe) */
			result = sysctl_VFS_BCACHESTAT((struct bcachestats *)buf, *size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_VFS(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case VFS_GENERIC:
			result = sysctl_VFS_GENERIC(name, oldp, oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

#if 0
int 
sysctl_MACHDEP_CPU_BIOS(const int *name, char **buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case BIOS_DEV:
			*(dev_t *)buf = DEV_TYPE_PORT;
			break;
		case BIOS_DISKINFO:
			result = -ENXIO;
			break;
		case BIOS_CKSUMLEN:
		case BIOS_MAXID:
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_MACHDEP(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case CPU_CONSDEV:
			*(dev_t *)oldp = DEV_TYPE_CONSOLE;
			break;
		case CPU_BIOS:
			result = sysctl_MACHDEP_CPU_BIOS(name, (char **)oldp, oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
#endif

/****************************************************/

int 
sysctl_USER_NAMEI(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[2]){
		case NAMEI_TTY:		/* tty.c */
			result = tty_USER_NAMEI_TTY(name[3], oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_USER(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case USER_PWD:		/* pwd.c */
			result = pwd_USER_PWD(name, oldp, oldlenp);
			break;
		case USER_GRP:		/* grp.c */
			result = grp_USER_GRP(name, oldp, oldlenp);
			break;
		case USER_NAMEI:
			result = sysctl_USER_NAMEI(name, oldp, oldlenp, newp, newlen);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sys___sysctl(call_t call, const int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	/* sysctl(3)
	 */
	if (namelen < 2 || namelen >= CTL_MAXNAME){
		result = -EINVAL;
	}else switch(name[0]){
		case CTL_KERN:
			result = sysctl_KERN(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_HW:
			result = sysctl_HW(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VM:
			result = sysctl_VM(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_NET:
			result = sysctl_NET(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VFS:
			result = sysctl_VFS(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_MACHDEP:
//			result = sysctl_MACHDEP(name, oldp, oldlenp, newp, newlen);
			result = -ENXIO;
			break;
		case CTL_USER:
			result = sysctl_USER(name, oldp, oldlenp, newp, newlen);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
