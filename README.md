# MinC Is Not Cygwin

<img src="MinC.png" width="448" align="right">

MinC was written to help children at vocational education 
learn Linux without the hassle of virtualization. It runs 
on Windows NT, version 2000/XP/2003/Vista/7/8/10/11.

MinC has a tiny kernel emulation (285Kb) which runs on 
Windows. The rest of the software was taken verbatim from 
OpenBSD, version 6.1. This means that by installing MinC, 
you run OpenBSD on your Windows machine at native speed.

MinC is already pretty much functional for normal usage, but 
there are still some issues which need to be solved. Read 
about them in the [ISSUES.md](ISSUES.md) document.

If you want to build the MinC system yourself, read the
[BUILD.md](BUILD.md) document. For downloading the 
installer and more information go to:

https://minc.commandlinerevolution.nl/english/home.html

## Why developers use MinC over WSL/Cygwin

* It's a Unix environment, without the need for a whole GNU 
userland. I appreciate GNU's efforts but I greatly prefer 
BSD and Solaris style environments to them.
* It's more than just a userland port. Since we have the 
*bsd.dll* providing the PTYs and process management etc., 
it makes it to where depending on Windows for some things 
is less necessary. I especially appreciate the fact that 
unlike Cygwin, we don't need to have .exe or .dll on every 
single PE file.
* Like mentioned on the home page, the programs are where 
the data is. Makes it easier than WSL which transfers files 
over a 9P connection and runs an entire Linux kernel in a 
Hyper-V VM.
* Integration with the NT kernel makes using network 
utilities like **ifconfig** and **route** incredibly nice, 
because I am more used to Unix networking utilities so being 
able to see network info and other stuff like ACPI devices 
and whatnot in a format I am familiar with is really nice.
* Compatibility with older Windows versions. I daily-drive 
Windows 7 on my laptop because of better HW compat and a nicer 
graphical interface to use. Having MinC work on older platforms 
is incredibly nice for this, and that's one of the biggest 
parts of why I like it. Cygwin nuked support for older Windows 
recently, so having something similar but even better is 
really good.
* Forgot to mention, **ifconfig** showing other NDIS devices 
that aren't just Ethernet or WLAN are awesome, as I have a 
WWAN Sierra modem so that being detected as ndis14 in **ifconfig** 
is super cool.

## Contribute

If you want to contribute to the MinC project, these are 
the rules of engagement:

1. **When in Rome do as the Romans do**. Don't contribute 
code because you don't like the other guys/gals coding style. 
You are not going to make friends with that. Like you yourself, 
every coder has their own style. So try to copy the coding 
style of the project you are contributing to. You might 
even learn something.

2. **Don't fix it if it ain't broke**. Please do contribute, 
but only if there is an actual bug to fix. And even then: don't 
start with a pull request. State the issue or proposal first 
in the *Issues* section.

3. **Eat your own dogfood**. I got that one from Dave Cuttler,
architect of Windows NT. Please use MinC before, while and after 
you contribute code. It is really satisfying seeing you own 
code run.

4. **Don't break userspace**. This one has been made famous by Linus 
Torvalds, creator of Linux. It means that the kernel has to keep 
supporting software that was created in the past. It is also known 
as 'backward compatibility' and is the reason why Microsoft Windows 
is still the most used operating system in the world.
