# Known Issues

At the time of writing one important feature is not yet implemented 
in the kernel emulation. During fork, the global variable space of 
a program needs to be copied to a separate memory location. Because 
most daemons use this feature, it means that these will not work 
yet. All normal programs in the distribution work fine. They do 
not use this feature.

## Korn Shell

On the other hand, the Korn shell (ksh) does a lot of forking and 
depends heavily on the above mentioned feature. Luckily, I have a 
working *__emutls_get_address()* function in *libgcc.a* to support 
Thread Local Storage (TLS) where needed. This does not solve all 
issues, but it is already enough for normal sh usage. The remaining 
issues with ksh are:

* large 'configure' scripts (GNU autotools) will not run;
* Command Substitution works up to 64 times, then you get: 
/bin/ksh: too many files open in shell

Also, some of the Korn Shell features are dependent on the type of 
terminal used. MinC uses the Windows Console by default. Its 
*ncurses* name in the environment is **interix**. This makes MinC 
compatible as a terminal with **VS Code** and **Windows Terminal**. 
But it also means that Ctrl+Z (**suspend**) does not work, but 
Ctrl+S (XOFF) does. Use this instead to suspend the current 
task. Or just use the **Pause** key.

This also means that Ctrl-V (**paste**) does not work. Use the 
Shift+Insert key combination instead to paste text from the Windows 
Clipboard in any editing program.

Another thing is that one can't interrupt (Ctrl+C) certain running 
scripts. For example, the following line will run forever and 
cannot be interrupted:

	while :; do echo [H]; done

I tested this on Debain Linux and my OpenBSD server. To my surprise 
they both have the same issue. Appearently, this is a feature, not 
a bug.

## Mounting

Mounting is properly implemented, but if you frequently plug and 
unplug USB storage devices, the mount table (/etc/fstab) will need 
to be updated by hand to show the right device assignments. This 
is quite normal for Unix, but they didn't have hot-pluggable devices 
in their days. I am working on a solution which will maintain the 
mount table in semi real-time, like I have done with the /etc/passwd 
and /etc/group tables.

## Networking

Only the "read" part of networking has been implemented, so you 
can run **ifconfig** to show your network configuration, but you 
can't configure the network with it. Likewise, you can run 
**route show** to show your routing table, but adding or deleting 
network routes does not work.

I intend to implement the missing parts but these have a low 
priority at the moment. Create an issue if you need to have one 
of them implemented.

November, 2025
Daniel Boland
