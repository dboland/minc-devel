# Building the system

To build the MinC Unix emulator for Windows you need the 
OpenBSD 6.1.0 source code and MinGW:

* https://sourceforge.net/projects/mingw/

I pulled the OpenBSD source code from the read-only GitHub 
repository. There is an article which explains how to pull 
the source for a specific version, but you will need the 
CVS software:

* https://www.openbsd.org/anoncvs.html

However, you can also download the source distribution from
GitHub (196Mb):

* https://github.com/openbsd/src/archive/7692f5f0b8e1ff836d8f3e9dbf51174095f66410.zip

**Note**: create a folder in the root of a diskdrive, named 
'source'. Move the .ZIP file there and extract it.


## Step 1: set up a simple cross-compiler

To build the new kernel and compile the operating system code, 
you would have to build an OpenBSD cross compiler first. This 
can be very laborious and frustrating. 

Instead we will create an isolated, slightly modified instance 
of the MinGW compiler. Let's call it a *poor man's cross-compiler*. 
Your current MinGW installation will be left completely 
untouched.

Open the MSYS terminal, **cd** to your *minc-devel* directory 
and make the *opt* target. All files will be installed in a 
newly created directory, called */opt/minc*:

	make opt

**Note**: you will probably get some error messages here. These 
will guide you to properly set up the 'config.inc' file. Use 
vim to edit it and define the indicated variables.


## Step 2: build a minimal OpenBSD system

The kernel will be built using a combination of vanilla MinGW 
and our *poor man's cross-compiler*, residing in */opt/minc*. 
To test if this all works, make the kernel first:

	make kernel

A minimal OpenBSD system consists of the kernel, the BSD C 
library, the boot program, the Korn shell and some utilities. 
These will be built by the new system itself. To finish 
the build:

	make system

## Step 3: install the system

For this step, you need to be **Administrator**. Close the MSYS 
terminal and open it again as Administrator by right-clicking 
the MSYS icon on your Desktop. Change to your *minc-devel* 
directory and run the *mkroot* command:

	./mkroot.sh

**Note**: By default, MinC will be installed in the **C:\minc-release** 
directory. If you want another install location, uncomment and change 
the *DESTDIR* variable in config.inc. It is not advisable to create 
the MinC root directory in a location like *Program Files*. File 
permissions in this kind of location are unsuitable for OpenBSD 
to run properly.

You now have a working system. Go to the new folder in Windows 
Exporer and open the *sbin* folder. There should be a program 
named *bsd.exe*. You can open a terminal by double-clicking it. 
To test if the new system works, you can run the **uname** 
command:

	uname -a

The result should be similar to the following output:

	OpenBSD dimension.sassenheim.dmz 6.1.0 MINC#20250720 i386

Double-clicking the *bsd.exe* program is not the right way to 
start the MinC terminal. To make it start properly and in your 
home directory, put following lines in a file named 
*minc-release.cmd* on your Desktop:

	@ECHO OFF
	
	CD C:\minc-release\sbin
	START "MinC" bsd.exe -h

The *START* command tells the console to read console settings from
the Windows Registry. The *-h* switch tells MinC to change to 
your */home* directory.

**Note**: the root folder you created seems to have disappeared.
This is because when MinC mounts its root directory, it hides the 
directory from view in Windows Explorer. This is super important 
for preventing recursive directory listings. It is also standard 
practice in Windows for mounted drives. If you want to access 
the MinC root folder in Windows Explorer, type its location in 
the address bar, like *C:\minc-release*.

## Step 4: building the GCC compiler

Building the GNU C Compiler and build tools for MinC is as simple 
as running the *configure* and *make* commands. But in reality it 
is a bit more complicated. You have to provide the OpenBSD headers 
and import libraries yourself, of which the latter have to be built 
first. So you immediately run into the *chicken-and-egg* problem.
Instead, I recommend just installing the pre-compiled build tools 
which you can find in the *Releases* section on GitHub.

If you want to know more about cross-compiling, watch Rob Landley's 
excellent presentation on the topic:

Tutorial: Building the Simplest Possible Linux System - Rob Landley
https://youtu.be/Sk9TatW9ino


Daniel Boland, September, 2025
