; minc.nsi

!include MUI.nsh
!include WinMessages.nsh
!include WinVer.nsh

!define OUTFILE "buildtools-6.1.0.exe"
; In Vista, all keys are deferred to Software\WOW6432Node\Microsoft
!define REGFILE "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinC"

Name "MinC - Build Tools"
OutFile ${OUTFILE}
RequestExecutionLevel admin
Unicode True
; Use local drive as default, so the "Browse.." mechanism works:
InstallDir "C:\MinC"
; Try the last saved directory
InstallDirRegKey HKLM ${REGFILE} "InstallLocation"
InstallDirRegKey HKLM ${REGFILE} "BuildTools"

; Must be BMP3 and 200x57 pixels
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\puf800X689.bmp"
!define MUI_HEADERIMAGE_RIGHT
; Do not automatically advance to the "Finish" page:
!define MUI_FINISHPAGE_NOAUTOCLOSE

!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE English

;--------------------------------

Section

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Put files there
	File /r 'miniroot'
	File '*.cmd'
	File 'comp61.tgz'

	ExecDos::exec /DETAILED '.\miniroot\chmod -R 00755 miniroot'

	IfFileExists .\usr\lib\libc.so +3 0
	DetailPrint "Installing base libraries..."
	ExecDos::exec /DETAILED '.\install.cmd comp61.tgz'
	Delete 'comp61.tgz'

        # Remember last chosen directory
        WriteRegStr HKLM ${REGFILE} "BuildTools" $INSTDIR

SectionEnd
Section "Curl file transfer" SecCurl
        File 'curl772.tgz'
        DetailPrint "Installing curl 7.72..."
        ExecDos::exec /DETAILED '.\install.cmd curl772.tgz'
SectionEnd
Section "GNU Compiler Collection" SecGCC
	File 'gcc481.tgz'
	DetailPrint "Installing gcc 4.8.1..."
	ExecDos::exec /DETAILED '.\install.cmd gcc481.tgz'
SectionEnd
Section "GNU Binary Utilities" SecBinUtils
	File 'binutils228.tgz'
	DetailPrint "Installing binutils 2.2.8..."
	ExecDos::exec /DETAILED '.\install.cmd binutils228.tgz'
SectionEnd
Section "GNU make" SecMake
	File 'gmake42.tgz'
	DetailPrint "Installing gmake 4.2..."
	ExecDos::exec /DETAILED '.\install.cmd gmake42.tgz'
SectionEnd
Section "Git version control" SecGit
	File 'git231.tgz'
	DetailPrint "Installing git 2.31.1..."
	ExecDos::exec /DETAILED '.\install.cmd git231.tgz'
SectionEnd
Section "Vim editor" SecVim
	File 'vim81.tgz'
	DetailPrint "Installing vim 8.1..."
	ExecDos::exec /DETAILED '.\install.cmd vim81.tgz'
SectionEnd
Section
	DetailPrint "Cleaning up..."
	RMDir /r "$INSTDIR\miniroot"
	Delete "$INSTDIR\*.cmd"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecCurl} "curl - transfer a URL"
!insertmacro MUI_DESCRIPTION_TEXT ${SecGCC} 'gcc - GNU project C and C++ compiler'
!insertmacro MUI_DESCRIPTION_TEXT ${SecBinUtils} 'ld - The GNU linker'
!insertmacro MUI_DESCRIPTION_TEXT ${SecGit} "git - the stupid content tracker"
!insertmacro MUI_DESCRIPTION_TEXT ${SecMake} 'gmake - GNU make utility to maintain groups of programs'
!insertmacro MUI_DESCRIPTION_TEXT ${SecVim} "Vi IMproved, a programmer's text editor"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
