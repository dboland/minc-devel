# MinC Is Not Cygwin

MinC was written to help children at vocational education 
learn Linux without the hassle of virtualization. It runs 
on Windows NT, version 2000/XP/2003/Vista/7/8/10/11.

MinC has a tiny kernel emulation (285Kb) which runs on 
Windows. The rest of the software was taken verbatim from 
OpenBSD, version 6.1. This means that by installing MinC, 
you run OpenBSD on your Windows machine at native speed.

![MinC](MinC.png)

If you want to build the MinC system yourself, read the
BUILD.md document.

For downloading the installer and more information go 
to:

https://minc.commandlinerevolution.nl/english/home.html

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

3. **Eat you own dogfood**. I got that one from Dave Cuttler,
architect of Windows NT. Please use MinC before, while and after 
you contribute code. It is really satisfying seeing you own 
code run.
