; minc.nsi

!include MUI.nsh
!include WinMessages.nsh
!include WinVer.nsh

!define OUTFILE "test-6.1.0.exe"
!define REGFILE "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinC"

Name "MinC"
OutFile ${OUTFILE}
RequestExecutionLevel admin
Unicode True

; Use local drive as default, so the "Browse.." mechanism works
InstallDir "C:\MinC"
; We use this to remember the last chosen directory
InstallDirRegKey HKLM ${REGFILE} "InstallLocation"
InstallDirRegKey HKLM ${REGFILE} "BuildTools"

; Must be BMP3 and 200x57 pixels
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\puf800X689.bmp"
!define MUI_HEADERIMAGE_RIGHT
;do not automatically advance to the "Finish" page:
!define MUI_FINISHPAGE_NOAUTOCLOSE

!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

#PEAddResource "minc-1.1.exe.manifest" "#24" "#2"

#!addplugindir "C:\Program Files\NSIS\Plugins"

Var USERNAME

;LogSet on

;--------------------------------

Section

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Put files there
	File /r 'miniroot'
	File '*.cmd'

	ExecDos::exec /DETAILED 'cacls . /E /R "$USERNAME"'
	ExecDos::exec /DETAILED '.\miniroot\chmod -R 00755 miniroot'

SectionEnd
Section "Kernel" SecKernel

	# Component can not be disabled
	SectionIn RO

	File 'test61.tgz'

	# Make sure tar.exe does not see /dev/tty
	Delete .\dev\*.*

	DetailPrint "Installing kernel..."
	ExecDos::exec /DETAILED '.\install.cmd test61.tgz'

SectionEnd
Section "Test" SecTest
	DetailPrint "Installing Test section..."
SectionEnd
Section
	DetailPrint "Configuring..."
	ExecDos::exec /DETAILED '.\postinstall.cmd osx'
	RMDir /r "$INSTDIR\miniroot"
	Delete "$INSTDIR\*.cmd"
SectionEnd
Function .onInit
	ReadEnvStr $USERNAME USERNAME
	StrCpy $0 0
	IfFileExists $INSTDIR\usr\lib\libc.so +2 0
	StrCpy $0 ${SF_SELECTED}
	SectionSetFlags ${SecTest} $0
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecKernel} "MinC kernel"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

