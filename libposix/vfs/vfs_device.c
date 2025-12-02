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

#include <ddk/ntifs.h>

/* Devices are loaded using OpenBSD's autoconf framework. After executing the 
 * biosboot program, devices are known, but need to be matched with their drivers.
 * The matching process is started by calling cpu_configure() which
 * calls config_rootfound() once, which searches the tree by
 * calling config_search(), which calls the xxx_match() function of all the
 * drivers in its parent class.
 * After that, config_search() calls the xxx_attach() function of the found driver,
 * which calls config_found(), passing a pointer to the xxx_print() function of 
 * the parent device, completing driver initialization.
 */

/****************************************************/

BOOL 
config_init(LPCSTR Name, DWORD FileType, DWORD DeviceType)
{
	WIN_DEVICE *pwDevice = DEVICE(DeviceType);

	pwDevice->FileType = FileType;
	pwDevice->FSType = FS_TYPE_PDO;
	pwDevice->DeviceType = DeviceType;
	pwDevice->DeviceId = DeviceType;
	pwDevice->Flags |= WIN_DVF_CONFIG_READY;
	win_strlcpy(pwDevice->Name, Name, MAX_NAME);
	return(TRUE);
}
BOOL 
config_found(LPCSTR Name, DWORD FileType, WIN_DEVICE *Device)
{
	UINT uiUnit = Device->DeviceId - Device->DeviceType;

	Device->FileType = FileType;
	Device->FSType = FS_TYPE_PDO;
	Device->Flags |= WIN_DVF_CONFIG_READY;
	_itoa(uiUnit, win_stpcpy(Device->Name, Name), 10);
	return(TRUE);
}

/****************************************************/

BOOL 
dull_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_SWD:
			bResult = config_found("swd", WIN_VCHR, Device);
			break;
		case DEV_TYPE_BIOS:
			bResult = config_found("bios", WIN_VCHR, Device);
			break;
		case DEV_TYPE_USBT:
			bResult = config_found("ubt", WIN_VCHR, Device);
			break;
		case DEV_TYPE_NVME:
			bResult = config_found("nvme", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
system_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_ENUM:
			bResult = config_found("enum", WIN_VCHR, Device);
			break;
		case DEV_TYPE_ACPI:
			bResult = config_found("acpi", WIN_VCHR, Device);
			break;
		case DEV_TYPE_PCI:
			bResult = config_found("pci", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
cpu_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_RAMDISK:
			bResult = config_found("rd", WIN_VCHR, Device);
			break;
		case DEV_TYPE_PROCESSOR:
			bResult = config_found("cpu", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
disk_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_SCSI:
			bResult = config_found("scsi", WIN_VBLK, Device);
			break;
		case DEV_TYPE_WDC:
			bResult = config_found("wdc", WIN_VBLK, Device);
			break;
		case DEV_TYPE_USB:
			bResult = config_found("usb", WIN_VBLK, Device);
			break;
		case DEV_TYPE_FDC:
			bResult = config_found("fdc", WIN_VBLK, Device);
			break;
		case DEV_TYPE_AHCI:
			bResult = config_found("ahci", WIN_VBLK, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
ifnet_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_NDIS:
			bResult = config_found("ndis", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_TUNNEL:
			bResult = config_found("gif", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_NIC:
			bResult = config_found("nic", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_LOOPBACK:
			bResult = config_found("lo", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_ETH:
			bResult = config_found("eth", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_WLAN:
			bResult = config_found("wlan", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_PPP:
			bResult = config_found("ppp", WIN_VSOCK, Device);
			break;
		case DEV_TYPE_REMOTE:
			bResult = config_found("smb", WIN_VBLK, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
media_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_MAGTAPE:
			bResult = config_found("mt", WIN_VCHR, Device);
			break;
		case DEV_TYPE_MEDIA:
			bResult = config_found("media", WIN_VCHR, Device);
			break;
		case DEV_TYPE_USBVIDEO:
			bResult = config_found("uvideo", WIN_VCHR, Device);
			break;
		case DEV_TYPE_AUDIO:
			bResult = config_found("audio", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
serial_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_LOG:		/* Vista */
			bResult = config_init("printk", WIN_VCHR, DEV_TYPE_LOG);
			break;
//		case DEV_TYPE_TTY:
//			bResult = config_found("tty", WIN_VCHR, Device);
//			break;
		case DEV_TYPE_PTY:
			bResult = config_found("pty", WIN_VCHR, Device);
			break;
		case DEV_TYPE_COM:
			bResult = config_found("pccom", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
printer_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_QUEUE:
			bResult = config_found("queue", WIN_VCHR, Device);
			break;
		case DEV_TYPE_USBPRINT:
			bResult = config_found("ulpt", WIN_VCHR, Device);
			break;
		case DEV_TYPE_LPT:
			bResult = config_found("parallel", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
display_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_VGA:
			bResult = config_found("vga", WIN_VCHR, Device);
			break;
		default:
			bResult = config_found("wsdisplay", WIN_VCHR, Device);
	}
	return(bResult);
}
BOOL 
keyboard_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_COMKBD:
			bResult = config_found("comkbd", WIN_VCHR, Device);
			break;
		case DEV_TYPE_HIDKBD:
			bResult = config_found("ukbd", WIN_VCHR, Device);
			break;
		default:
			bResult = config_found("wskbd", WIN_VCHR, Device);
	}
	return(bResult);
}
BOOL 
mouse_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_COMMOUSE:
			bResult = config_found("comms", WIN_VCHR, Device);
			break;
		case DEV_TYPE_HIDMOUSE:
			bResult = config_found("ums", WIN_VCHR, Device);
			break;
		default:
			bResult = config_found("wsmouse", WIN_VCHR, Device);
	}
	return(bResult);
}
BOOL 
storage_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_FIXED:
			bResult = config_found("vol", WIN_VBLK, Device);
			break;
		case DEV_TYPE_CDROM:
			bResult = config_found("cd", WIN_VBLK, Device);
			break;
		case DEV_TYPE_FLOPPY:
			bResult = config_found("fd", WIN_VBLK, Device);
			break;
		case DEV_TYPE_SD:
			bResult = config_found("sd", WIN_VBLK, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
usb_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_OHCI:
			bResult = config_found("ohci", WIN_VCHR, Device);
			break;
		case DEV_TYPE_UHCI:
			bResult = config_found("uhci", WIN_VCHR, Device);
			break;
		case DEV_TYPE_UHUB:
			bResult = config_found("uhub", WIN_VCHR, Device);
			break;
		case DEV_TYPE_BTHUB:
			bResult = config_found("bthub", WIN_VCHR, Device);
			break;
		case DEV_TYPE_ITE:
			bResult = config_found("ite", WIN_VCHR, Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
hid_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_EHCI:
			bResult = config_found("ehci", WIN_VCHR, Device);
			break;
		case DEV_TYPE_UHIDEV:
			bResult = config_found("uhidev", WIN_VCHR, Device);
			break;
		default:
			bResult = config_found("hid", WIN_VCHR, Device);
	}
	return(bResult);
}
BOOL 
config_attach(WIN_DEVICE *Device, USHORT Class)
{
	BOOL bResult = TRUE;

	switch (Class){
		case DEV_CLASS_DULL:
			bResult = dull_attach(Device);
			break;
		case DEV_CLASS_CPU:
			bResult = cpu_attach(Device);
			break;
		case DEV_CLASS_DISK:
			bResult = disk_attach(Device);
			break;
		case DEV_CLASS_IFNET:
			bResult = ifnet_attach(Device);
			break;
		case DEV_CLASS_MEDIA:
			bResult = media_attach(Device);
			break;
		case DEV_CLASS_TTY:
			bResult = serial_attach(Device);
			break;
		case DEV_CLASS_PRINTER:
			bResult = printer_attach(Device);
			break;
		case DEV_CLASS_DISPLAY:
			bResult = display_attach(Device);
			break;
		case DEV_CLASS_KEYBOARD:
			bResult = keyboard_attach(Device);
			break;
		case DEV_CLASS_MOUSE:
			bResult = mouse_attach(Device);
			break;
		case DEV_CLASS_STORAGE:
			bResult = storage_attach(Device);
			break;
		case DEV_CLASS_USB:
			bResult = usb_attach(Device);
			break;
		case DEV_CLASS_HID:
			bResult = hid_attach(Device);
			break;
		case DEV_CLASS_SYSTEM:
			bResult = system_attach(Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
