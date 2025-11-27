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

/* Device classes (sys/device.h) */

#define DEV_CLASS_DULL		0x0000			/* generic, no special info */
#define DEV_CLASS_CPU		0x0100
#define DEV_CLASS_DISK		0x0200
#define DEV_CLASS_IFNET		0x0300
#define DEV_CLASS_TAPE		0x0400
#define DEV_CLASS_MEDIA		DEV_CLASS_TAPE
#define DEV_CLASS_TTY		0x0500
#define DEV_CLASS_SERIAL	DEV_CLASS_TTY
#define DEV_CLASS_PRINTER	0x0600
#define DEV_CLASS_DISPLAY	0x0700
#define DEV_CLASS_KEYBOARD	0x0800
#define DEV_CLASS_MOUSE		0x0900
#define DEV_CLASS_STORAGE	0x0A00
#define DEV_CLASS_USB		0x0B00
#define DEV_CLASS_HID		0x0C00
#define DEV_CLASS_SYSTEM	0x0D00
#define DEV_CLASS_MAX		14

/* Hardware buses */

#define DEV_BUS_MAIN		64
#define DEV_BUS_BIOS		DEV_BUS_MAIN
#define DEV_BUS_APM		56			/* Advanced Power Management interface bus */
#define DEV_BUS_ACPI		DEV_BUS_APM		/* Advanced Configuration and Power Interface bus */
#define DEV_BUS_PCI		40			/* Peripheral Component Interconnect bus */
#define DEV_BUS_SATA		DEV_BUS_PCI		/* Serial Advanced Technology Attachment bus */
#define DEV_BUS_ISA		32			/* Industry Standard Architecture bus */
#define DEV_BUS_FDC		DEV_BUS_ISA		/* Floppy Disk controller bus */
#define DEV_BUS_UART		24			/* Universal Asynchronous Receiver-Transmitter bus */
#define DEV_BUS_USB		DEV_BUS_UART		/* Universal Serial Bus */
#define DEV_BUS_IDE		16			/* Integrated Disk Electronics bus */
#define DEV_BUS_WDC		DEV_BUS_IDE		/* Hard Disk Controller bus */
#define DEV_BUS_SCSI		8			/* Small Computer System Interface bus  */
#define DEV_BUS_HID		DEV_BUS_SCSI		/* Human Interface Device bus */

/* System device types */

#define DEV_TYPE_SWD		(DEV_CLASS_DULL)
#define DEV_TYPE_BIOS		(DEV_CLASS_DULL + DEV_BUS_ACPI)
#define DEV_TYPE_USBT		(DEV_CLASS_DULL + DEV_BUS_USB)	/* USB Bluetooth adapter */
#define DEV_TYPE_NVME		(DEV_CLASS_DULL + DEV_BUS_PCI)

#define DEV_TYPE_ENUM		(DEV_CLASS_SYSTEM)
#define DEV_TYPE_PCI		(DEV_CLASS_SYSTEM + DEV_BUS_PCI)	/* PCI controller device */
#define DEV_TYPE_ACPI		(DEV_CLASS_SYSTEM + DEV_BUS_ACPI)

#define DEV_TYPE_MEM		(DEV_CLASS_CPU + 1)		/* physical memory device */
#define DEV_TYPE_KMEM		(DEV_CLASS_CPU + 2)		/* kernel memory device (libposix.dll) */
#define DEV_TYPE_NULL		(DEV_CLASS_CPU + 3)		/* null byte write device */
#define DEV_TYPE_PORT		(DEV_CLASS_CPU + 4)		/* I/O port access */
#define DEV_TYPE_ZERO		(DEV_CLASS_CPU + 5)		/* null byte read device */
#define DEV_TYPE_URANDOM	(DEV_CLASS_CPU + 6)		/* random generator pseudo device */
#define DEV_TYPE_RANDOM		(DEV_CLASS_CPU + 7)		/* crypto random generator pseudo device */
#define DEV_TYPE_ROUTE		(DEV_CLASS_CPU + 8)		/* message routing pseudo device */
#define DEV_TYPE_STDIN		(DEV_CLASS_CPU + 9)		/* standard stream pseudo device */
#define DEV_TYPE_STDOUT		(DEV_CLASS_CPU + 10)		/* standard stream pseudo device */
#define DEV_TYPE_STDERR		(DEV_CLASS_CPU + 11)		/* standard stream pseudo device */
#define DEV_TYPE_SWAP		(DEV_CLASS_CPU + 12)		/* disk swap pseudo device */
#define DEV_TYPE_RAMDISK	(DEV_CLASS_CPU + 24)		/* Random Access Memory disk device (rd*) */
#define DEV_TYPE_PROCESSOR	(DEV_CLASS_CPU + 32)		/* Central Processing Unit */

//#define DEV_TYPE_ROOT		(DEV_CLASS_DISK)		/* Root mount point */
#define DEV_TYPE_AHCI		(DEV_CLASS_DISK + DEV_BUS_SATA)	/* Serial ATA Advanced Host Controller Interface */
#define DEV_TYPE_FDC		(DEV_CLASS_DISK + DEV_BUS_FDC)	/* Floppy Disk Controller */
#define DEV_TYPE_USB		(DEV_CLASS_DISK + DEV_BUS_USB)	/* USB storage controller */
#define DEV_TYPE_WDC		(DEV_CLASS_DISK + DEV_BUS_WDC)	/* WD100x compatible hard disk controller */
#define DEV_TYPE_SCSI		(DEV_CLASS_DISK + DEV_BUS_SCSI)	/* Small Computer System Interface controller */

#define DEV_TYPE_NDIS		(DEV_CLASS_IFNET)
#define DEV_TYPE_TUNNEL		(DEV_CLASS_IFNET + 16)		/* Tunnel type encapsulation */
#define DEV_TYPE_LOOPBACK	(DEV_CLASS_IFNET + 20)		/* loop-back network pseudo device */
#define DEV_TYPE_PPP		(DEV_CLASS_IFNET + 24)		/* Point-To-Point network device */
#define DEV_TYPE_ETH		(DEV_CLASS_IFNET + 28)		/* Ethernet network device */
#define DEV_TYPE_WLAN		(DEV_CLASS_IFNET + 36)		/* IEEE80211 wireless network device */
#define DEV_TYPE_NIC		(DEV_CLASS_IFNET + DEV_BUS_PCI)	/* Network Interface Card */
#define DEV_TYPE_REMOTE		(DEV_CLASS_IFNET + DEV_BUS_MAIN)	/* Server Message Block storage */

#define DEV_TYPE_MEDIA		(DEV_CLASS_MEDIA)
#define DEV_TYPE_USBVIDEO	(DEV_CLASS_MEDIA + DEV_BUS_USB)
#define DEV_TYPE_AUDIO		(DEV_CLASS_MEDIA + DEV_BUS_PCI)
#define DEV_TYPE_MAGTAPE	(DEV_CLASS_MEDIA + DEV_BUS_SCSI)

#define DEV_TYPE_TTY		(DEV_CLASS_TTY)
#define DEV_TYPE_CONSOLE	(DEV_CLASS_TTY + 1)		/* system console master device */
#define DEV_TYPE_PTM		(DEV_CLASS_TTY + 2)		/* serial multiplex device */
#define DEV_TYPE_LOG		(DEV_CLASS_TTY + 3)		/* boot time output device (printf for kernel) */
#define DEV_TYPE_INPUT		(DEV_CLASS_TTY + 4)		/* generic input device */
#define DEV_TYPE_SCREEN		(DEV_CLASS_TTY + 5)		/* generic output device */
#define DEV_TYPE_PTY		(DEV_CLASS_TTY + 8)		/* pseudo serial master device */
#define DEV_TYPE_COM		(DEV_CLASS_TTY + DEV_BUS_ISA)	/* EIA RS232 serial master device */

#define DEV_TYPE_QUEUE		(DEV_CLASS_PRINTER)
#define DEV_TYPE_USBPRINT	(DEV_CLASS_PRINTER + DEV_BUS_USB)
#define DEV_TYPE_LPT		(DEV_CLASS_PRINTER + DEV_BUS_ISA)

#define DEV_TYPE_WSDISPLAY	(DEV_CLASS_DISPLAY)
#define DEV_TYPE_VGA		(DEV_CLASS_DISPLAY + DEV_BUS_PCI)

#define DEV_TYPE_WSKBD		(DEV_CLASS_KEYBOARD)
#define DEV_TYPE_COMKBD		(DEV_CLASS_KEYBOARD + DEV_BUS_ISA)
#define DEV_TYPE_USBKBD		(DEV_CLASS_KEYBOARD + DEV_BUS_USB)
#define DEV_TYPE_HIDKBD		(DEV_CLASS_KEYBOARD + DEV_BUS_HID)

#define DEV_TYPE_WSMOUSE	(DEV_CLASS_MOUSE)
#define DEV_TYPE_COMMOUSE	(DEV_CLASS_MOUSE + DEV_BUS_ISA)
#define DEV_TYPE_USBMOUSE	(DEV_CLASS_MOUSE + DEV_BUS_USB)
#define DEV_TYPE_HIDMOUSE	(DEV_CLASS_MOUSE + DEV_BUS_HID)

#define DEV_TYPE_FIXED		(DEV_CLASS_STORAGE)
#define DEV_TYPE_CDROM		(DEV_CLASS_STORAGE + DEV_BUS_WDC)	/* CDROM storage */
#define DEV_TYPE_REMOVABLE	(DEV_CLASS_STORAGE + DEV_BUS_USB)	/* Hard Disk storage */
#define DEV_TYPE_FLOPPY		(DEV_CLASS_STORAGE + DEV_BUS_FDC)	/* Floppy Disk storage */
#define DEV_TYPE_SD		(DEV_CLASS_STORAGE + DEV_BUS_SCSI)	/* SCSI Disk storage */
#define DEV_TYPE_ROOT		(DEV_CLASS_STORAGE + DEV_BUS_MAIN)	/* Root mount point */

#define DEV_TYPE_OHCI		(DEV_CLASS_USB)			/* USB Open Host Controller Interface (USB 1.1) */
#define DEV_TYPE_UHCI		(DEV_CLASS_USB + DEV_BUS_PCI)	/* USB Universal Host Controller Interface (USB 1.0) */
#define DEV_TYPE_UHUB		(DEV_CLASS_USB + DEV_BUS_USB)	/* USB Hub adapter */
#define DEV_TYPE_BTHUB		(DEV_CLASS_USB + DEV_BUS_ISA)	/* USB Bluetooth hub */
#define DEV_TYPE_ITE		(DEV_CLASS_USB + DEV_BUS_ACPI)	/* ITE Chipset */

#define DEV_TYPE_EHCI		(DEV_CLASS_HID + DEV_BUS_PCI)	/* USB Enhanced Host Controller Interface (USB 2.0) */
#define DEV_TYPE_UHIDEV		(DEV_CLASS_HID + DEV_BUS_USB)	/* USB Human Interface Device */
