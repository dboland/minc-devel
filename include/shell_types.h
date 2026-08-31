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

typedef struct _SHELL_LINK_HEADER {
	DWORD HeaderSize;
	CLSID LinkCLSID;
	DWORD LinkFlags;
	DWORD FileAttributes;
	FILETIME CreationTime;
	FILETIME AccessTime;
	FILETIME WriteTime;
	DWORD FileSize;
	DWORD IconIndex;
	DWORD ShowCommand;
	USHORT HotKey;
	USHORT Reserved1;
	DWORD Reserved2;
	DWORD Reserved3;
} SHELL_LINK_HEADER;

/* LinkFlags */

#define HasLinkTargetIDList		0x00000001
#define HasLinkInfo			0x00000002
#define HasName				0x00000004
#define HasRelativePath			0x00000008
#define HasWorkingDir			0x00000010
#define HasArguments			0x00000020
#define HasIconLocation			0x00000040
#define IsUnicode			0x00000080
#define ForceNoLinkInfo			0x00000100
#define HasExpString			0x00000200
#define RunInSeparateProcess		0x00000400
#define HasDarwinID			0x00001000
#define RunAsUser			0x00002000
#define HasExpIcon			0x00004000
#define NoPidlAlias			0x00008000
#define RunWithShimLayer		0x00020000
#define ForceNoLinkTrack		0x00040000
#define EnableTargetMetadata		0x00080000
#define DisableLinkPathTracking		0x00100000
#define DisableKnownFolderTracking	0x00200000
#define DisableKnownFolderAlias		0x00400000
#define AllowLinkToLink			0x00800000
#define UnaliasOnSave			0x01000000
#define PreferEnvironmentPath		0x02000000
#define KeepLocalIDListForUNCTarget	0x04000000

typedef struct _LINK_INFO {
	DWORD LinkInfoSize;
	DWORD LinkInfoHeaderSize;
	DWORD LinkInfoFlags;
	DWORD VolumeIDOffset;
	DWORD LocalBasePathOffset;
	DWORD CommonNetworkRelativeLinkOffset;
	DWORD CommonPathSuffixOffset;
} LINK_INFO;

/* LinkInfoFlags */

#define VolumeIDAndLocalBasePath		0x1
#define CommonNetworkRelativeLinkAndPathSuffix	0x2

typedef struct _VOLUME_ID {
	DWORD VolumeIDSize;
	DWORD DriveType;
	DWORD DriveSerialNumber;
	DWORD VolumeLabelOffset;
} VOLUME_ID;

typedef struct _COMMON_NETWORK_RELATIVE_LINK {
	DWORD CommonNetworkRelativeLinkSize;
	DWORD CommonNetworkRelativeLinkFlags;
	DWORD NetNameOffset;
	DWORD DeviceNameOffset;
	DWORD NetworkProviderType;
} COMMON_NETWORK_RELATIVE_LINK;

/* CommonNetworkRelativeLinkFlags */

#define ValidDevice	0x1#define ValidNetType	0x2
INT shell_wcstombs(LPSTR Destination, LPCWSTR Source, INT Size);
