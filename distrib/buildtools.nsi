; minc.nsi

!include MUI.nsh
!include WinMessages.nsh
!include WinVer.nsh

!define OUTFILE "buildtools-6.1.0.${VERSION}.exe"

Name "MinC - Build Tools"
OutFile ${OUTFILE}
RequestExecutionLevel admin
Unicode True
InstallDir "%SystemDrive%\MinC"

; Must be BMP3 and 200x57 pixels
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\puf800X689.bmp"
!define MUI_HEADERIMAGE_RIGHT

!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE English

Var SYSTEMDRIVE
Var USERNAME

;LogSet on

;--------------------------------

Function .onInit
        ReadEnvStr $SYSTEMDRIVE SystemDrive
        ReadEnvStr $USERNAME USERNAME
        StrCpy $INSTDIR "$SYSTEMDRIVE\MinC"
FunctionEnd
Section

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Put files there
	File /r 'miniroot'

	ExecDos::exec /DETAILED '.\miniroot\chmod -R 00755 miniroot'

SectionEnd
Section "Base system" SecBase

	# Component can not be disabled
	SectionIn RO

	File 'comp61.tgz'
	File '*.cmd'

	DetailPrint "Installing base libraries..."
	ExecDos::exec /DETAILED '.\install.cmd comp61.tgz'

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
	DetailPrint "Configuring..."
	RMDir /r "$INSTDIR\miniroot"
	Delete "$INSTDIR\*.cmd"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecBase} 'Base libraries'
!insertmacro MUI_DESCRIPTION_TEXT ${SecGCC} 'gcc - GNU project C and C++ compiler'
!insertmacro MUI_DESCRIPTION_TEXT ${SecBinUtils} 'ld - The GNU linker'
!insertmacro MUI_DESCRIPTION_TEXT ${SecGit} "git - the stupid content tracker"
!insertmacro MUI_DESCRIPTION_TEXT ${SecMake} 'gmake - GNU make utility to maintain groups of programs'
!insertmacro MUI_DESCRIPTION_TEXT ${SecVim} "Vi IMproved, a programmer's text editor"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
