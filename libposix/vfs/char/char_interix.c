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
	"\177",		/* 0x2E: VK_DELETE (^?): ncurses 6.1.20181013 (was \e[M) */
	"\e[L",		/* 0x2D: VK_INSERT */
	"",		/* 0x2C: VK_SNAPSHOT */
	"",		/* 0x2B: VK_EXECUTE */
	"",		/* 0x2A: VK_PRINT */
	"",		/* 0x29: VK_SELECT */
	"\e[B",		/* 0x28: VK_DOWN */
	"\e[C",		/* 0x27: VK_RIGHT */
	"\e[A",		/* 0x26: VK_UP */
	"\e[D",		/* 0x25: VK_LEFT */
	"\e[H",		/* 0x24: VK_HOME */
	"\e[F",		/* 0x23: VK_END (\e[U actually) */
	"\e[T",		/* 0x22: VK_NEXT */
	"\e[S",		/* 0x21: VK_PRIOR */
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
	"\eFO",		/* 0x87: VK_F24 */
	"\eFN",		/* 0x86: VK_F23 */
	"\eFM",		/* 0x85: VK_F22 */
	"\eFL",		/* 0x84: VK_F21 */
	"\eFK",		/* 0x83: VK_F20 */
	"\eFJ",		/* 0x82: VK_F19 */
	"\eFI",		/* 0x81: VK_F18 */
	"\eFH",		/* 0x80: VK_F17 */
	"\eFG",		/* 0x7F: VK_F16 */
	"\eFF",		/* 0x7E: VK_F15 */
	"\eFE",		/* 0x7D: VK_F14 */
	"\eFD",		/* 0x7C: VK_F13 */
	"\eFC",		/* 0x7B: VK_F12 */
	"\eFB",		/* 0x7A: VK_F11 */
	"\eFA",		/* 0x79: VK_F10 */
	"\eF9",		/* 0x78: VK_F9 */
	"\eF8",		/* 0x77: VK_F8 */
	"\eF7",		/* 0x76: VK_F7 */
	"\eF6",		/* 0x75: VK_F6 */
	"\eF5",		/* 0x74: VK_F5 */
	"\eF4",		/* 0x73: VK_F4 */
	"\eF3",		/* 0x72: VK_F3 */
	"\eF2",		/* 0x71: VK_F2 */
	"\eF1"		/* 0x70: VK_F1 */
};
