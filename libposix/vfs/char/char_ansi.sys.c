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

#include <wincon.h>

#define VK_MODIFY		0x1F
#define VK_CURSOR		0x2F
#define ANSI_CURSOR(vk)	__ANSI_CURSOR[VK_CURSOR - vk]

static LPCSTR __ANSI_CURSOR[] = {
	"",		/* 0x2F: VK_HELP */
	"\200S",		/* 0x2E: VK_DELETE */
	"\200R",		/* 0x2D: VK_INSERT */
	"",			/* 0x2C: VK_SNAPSHOT */
	"",			/* 0x2B: VK_EXECUTE */
	"",			/* 0x2A: VK_PRINT */
	"",			/* 0x29: VK_SELECT */
	"\200P",		/* 0x28: VK_DOWN */
	"\200M",		/* 0x27: VK_RIGHT */
	"\200H",		/* 0x26: VK_UP */
	"\200K",		/* 0x25: VK_LEFT */
	"\200G",		/* 0x24: VK_HOME */
	"\200O",		/* 0x23: VK_END */
	"\200Q",		/* 0x22: VK_NEXT */
	"\200I",		/* 0x21: VK_PRIOR */
	""		/* 0x20: VK_SPACE */
};

#define VK_WINDOWS		0x5F
#define VK_NUMPAD		0x6F
#define VK_FUNCTION		0x8F
#define ANSI_FUNCTION(vk)	__ANSI_FUNCTION[VK_FUNCTION - vk]

static LPCSTR __ANSI_FUNCTION[] = {
	"",		/* 0x8F: Reserved */
	"",		/* 0x8E: Reserved */
	"",		/* 0x8D: Reserved */
	"",		/* 0x8C: Reserved */
	"",		/* 0x8B: Reserved */
	"",		/* 0x8A: Reserved */
	"",		/* 0x89: Reserved */
	"",		/* 0x88: Reserved */
	"\200",		/* 0x87: VK_F24 */
	"\200",		/* 0x86: VK_F23 */
	"\200",		/* 0x85: VK_F22 */
	"\200",		/* 0x84: VK_F21 */
	"\200",		/* 0x83: VK_F20 */
	"\200",		/* 0x82: VK_F19 */
	"\200",		/* 0x81: VK_F18 */
	"\200",		/* 0x80: VK_F17 */
	"\200",		/* 0x7F: VK_F16 */
	"\200",		/* 0x7E: VK_F15 */
	"\200",		/* 0x7D: VK_F14 */
	"\200",		/* 0x7C: VK_F13 */
	"\200",		/* 0x7B: VK_F12 */
	"\200",		/* 0x7A: VK_F11 */
	"\200",		/* 0x79: VK_F10 */
	"\200",		/* 0x78: VK_F9 */
	"\200",		/* 0x77: VK_F8 */
	"\200",		/* 0x76: VK_F7 */
	"\200",		/* 0x75: VK_F6 */
	"\200",		/* 0x74: VK_F5 */
	"\200",		/* 0x73: VK_F4 */
	"\200",		/* 0x72: VK_F3 */
	"\200",		/* 0x71: VK_F2 */
	"\200;"		/* 0x70: VK_F1 */
};
