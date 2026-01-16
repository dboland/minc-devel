; minc.nsi

!include MUI.nsh
!include WinMessages.nsh
!include WinVer.nsh

!define OUTFILE "minc-6.1.0.exe"
; In Vista, all keys are deferred to Software\WOW6432Node\Microsoft
!define REGFILE "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinC"

Name "MinC"
OutFile ${OUTFILE}
RequestExecutionLevel admin
Unicode True
; Use local drive as default, so the "Browse.." mechanism works:
InstallDir "C:\MinC"
; Try the last saved directory
InstallDirRegKey HKLM ${REGFILE} "InstallLocation"

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

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE English

Var USERNAME

;--------------------------------

Function .onInit
	ReadEnvStr $USERNAME USERNAME
FunctionEnd
Section

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Put files there
	File /r 'miniroot'

	ExecDos::exec /DETAILED 'cacls $INSTDIR /E /R "$USERNAME"'
	ExecDos::exec /DETAILED '.\miniroot\chmod -R 00755 miniroot'

SectionEnd
Section "Uninstall"

	# Set output path to the installation directory.
	SetOutPath "$INSTDIR\.."
	# Put file there
	File 'uninstall.cmd'

	DetailPrint "Removing /usr..."
	ExecDos::exec /DETAILED '.\uninstall.cmd usr'
	DetailPrint "Removing /var..."
	ExecDos::exec /DETAILED '.\uninstall.cmd var'
	DetailPrint "Removing /bin..."
	ExecDos::exec /DETAILED '.\uninstall.cmd bin'
	DetailPrint "Removing /etc..."
	ExecDos::exec /DETAILED '.\uninstall.cmd etc'
	DetailPrint "Removing /dev..."
	ExecDos::exec /DETAILED '.\uninstall.cmd dev'
	DetailPrint "Removing /root..."
	ExecDos::exec /DETAILED '.\uninstall.cmd root'
	DetailPrint "Removing /tmp..."
	ExecDos::exec /DETAILED '.\uninstall.cmd tmp'
	DetailPrint "Removing /mnt..."
	ExecDos::exec /DETAILED '.\uninstall.cmd mnt'

	# Remove files and uninstaller
	RMDir /r "$INSTDIR"
	Delete ".\minc.exe"
	Delete ".\bsd.dll"

	# Remove registry keys
	DeleteRegKey HKLM ${REGFILE}

	# Remove shortcuts
	Delete "$DESKTOP\Console.lnk"
	Delete .\uninstall.cmd

SectionEnd
Section "Base system" SecBase

	# Component can not be disabled
	SectionIn RO

	File 'base61.tgz'
	File '*.cmd'

	# Make sure tar.exe does not see /dev/tty
	Delete .\dev\*.*
	# Legacy
	Delete .\sbin\minc.exe
	Delete .\sbin\bsd.exe
	Delete .\sbin\libposix-6.1.0.dll

	DetailPrint "Installing base system..."

	# Write the uninstall keys for Windows
	WriteRegStr HKLM ${REGFILE} "DisplayName" "MinC Unix emulator"
	WriteRegStr HKLM ${REGFILE} "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM ${REGFILE} "InstallSource" "$INSTDIR"
;	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinC" "SourceDir" "$INSTDIR"
;	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MinC" "EstimatedSize" 1048576
	WriteRegStr HKLM ${REGFILE} "Publisher" "BOLAND Automatisering"
	WriteRegStr HKLM ${REGFILE} "ProductVersion" "6.1"
	WriteRegStr HKLM ${REGFILE} "DisplayVersion" "6.1.0"
	WriteRegDWORD HKLM ${REGFILE} "NoModify" 1
	WriteRegDWORD HKLM ${REGFILE} "NoRepair" 1

	ExecDos::exec /DETAILED '.\install.cmd base61.tgz'

	WriteUninstaller "$INSTDIR\sbin\uninstall.exe"
	WriteRegStr HKLM ${REGFILE} "UninstallString" "$INSTDIR\sbin\uninstall.exe"

SectionEnd
Section "Base libraries" SecComp
	File 'comp61.tgz'
	DetailPrint "Installing base libraries..."
	ExecDos::exec /DETAILED '.\install.cmd comp61.tgz'
SectionEnd
Section "Nano editor" SecNano
	File 'nano64.tgz'
	DetailPrint "Installing nano 6.4..."
	ExecDos::exec /DETAILED '.\install.cmd nano64.tgz'
SectionEnd
Section "Lynx web browser" SecLynx
	File 'lynx28.tgz'
	DetailPrint "Installing lynx 2.8..."
	ExecDos::exec /DETAILED '.\install.cmd lynx28.tgz'
SectionEnd
Section "wget downloader" SecWGet
	File 'wget118.tgz'
	DetailPrint "Installing wget 1.18..."
	ExecDos::exec /DETAILED '.\install.cmd wget118.tgz'
SectionEnd
Section "Unzip .zip files" SecUnzip
	File 'unzip552.tgz'
	DetailPrint "Installing unzip 5.52..."
	ExecDos::exec /DETAILED '.\install.cmd unzip552.tgz'
SectionEnd
Section "Unzip .bzip2 files" SecBZip2
	File 'bzip210.tgz'
	DetailPrint "Installing bzip2 1.0.6..."
	ExecDos::exec /DETAILED '.\install.cmd bzip210.tgz'
SectionEnd
Section "BitchX IRC client" SecBitchX
	File 'bitchx12.tgz'
	DetailPrint "Installing BitchX 1.2.2..."
	ExecDos::exec /DETAILED '.\install.cmd bitchx12.tgz'
SectionEnd
Section "EMail mailer" SecEMail
	File 'email32.tgz'
	DetailPrint "Installing email 3.2.3..."
	ExecDos::exec /DETAILED '.\install.cmd email32.tgz'
SectionEnd
Section "HEX Editor" SecHexEdit
	File 'hexedit12.tgz'
	DetailPrint "Installing hexedit 1.2.13..."
	ExecDos::exec /DETAILED '.\install.cmd hexedit12.tgz'
SectionEnd
Section "Unzip .xz files" SecXZ
	File 'xz52.tgz'
	DetailPrint "Installing xz 5.2.3..."
	ExecDos::exec /DETAILED '.\install.cmd xz52.tgz'
SectionEnd
Section "Mutt e-mail reader" SecMutt
	File 'mutt20.tgz'
	DetailPrint "Installing mutt 2.0.7..."
	ExecDos::exec /DETAILED '.\install.cmd mutt20.tgz'
SectionEnd
Section "ImageMagick" SecMagick
	File 'magick710.tgz'
	DetailPrint "Installing ImageMagick 7.1.0-6..."
	ExecDos::exec /DETAILED '.\install.cmd magick710.tgz'
SectionEnd
Section "Bind tools" SecBind
	File 'bind93.tgz'
	DetailPrint "Installing Bind DNS tools 9.3.4..."
	ExecDos::exec /DETAILED '.\install.cmd bind93.tgz'
SectionEnd
Section "SASL authentication library" SecSasl
	File 'sasl2128.tgz'
	DetailPrint "Installing Sasl 2.1.28..."
	ExecDos::exec /DETAILED '.\install.cmd sasl2128.tgz'
SectionEnd
Section

;	File 'vista.tgz'
;	${If} ${AtMostWin2003}
;		Delete 'vista.tgz'
;	${Else}
;		ExecDos::exec /DETAILED '.\install.cmd vista.tgz'
;	${EndIf}

	DetailPrint "Configuring..."
	ExecDos::exec /DETAILED '.\postinstall.cmd monokai'
	RMDir /r "$INSTDIR\miniroot"
	Delete "$INSTDIR\*.cmd"

	# 'Start In' will be set to $OUTDIR
	CreateShortcut "$DESKTOP\Console.lnk" "$OUTDIR\sbin\terminal.cmd" "" "$OUTDIR\minc.exe" 0

SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecBase} "The OpenBSD/i386 6.1 base binary distribution"
!insertmacro MUI_DESCRIPTION_TEXT ${SecComp} "The OpenBSD/i386 6.1 base libraries distribution"
!insertmacro MUI_DESCRIPTION_TEXT ${SecNano} "GNU nano -- an enhanced clone of the Pico text editor"
!insertmacro MUI_DESCRIPTION_TEXT ${SecLynx} "lynx - a general purpose distributed information browser for the World Wide Web"
!insertmacro MUI_DESCRIPTION_TEXT ${SecWGet} "Wget - The non-interactive network downloader"
!insertmacro MUI_DESCRIPTION_TEXT ${SecUnzip} "unzip - list, test and extract compressed files in a ZIP archive"
!insertmacro MUI_DESCRIPTION_TEXT ${SecBZip2} "bzip2, bunzip2 - a block-sorting file compressor, v1.0.6"
!insertmacro MUI_DESCRIPTION_TEXT ${SecEMail} "email - Encrypted SMTP email via Command line"
!insertmacro MUI_DESCRIPTION_TEXT ${SecHexEdit} "hexedit - view and edit files in hexadecimal or in ASCII"
!insertmacro MUI_DESCRIPTION_TEXT ${SecXZ} "xz - Compress or decompress .xz and .lzma files"
!insertmacro MUI_DESCRIPTION_TEXT ${SecMutt} "mutt - The Mutt Mail User Agent"
!insertmacro MUI_DESCRIPTION_TEXT ${SecBitchX} "BitchX - The Ultimate IRC Client"
!insertmacro MUI_DESCRIPTION_TEXT ${SecMagick} "magick - convert between image formats as well as resize an image and more"
;!insertmacro MUI_DESCRIPTION_TEXT ${SecPerl} "perl - The Perl 5 language interpreter"
!insertmacro MUI_DESCRIPTION_TEXT ${SecBind} "Bind DNS tools"
!insertmacro MUI_DESCRIPTION_TEXT ${SecSasl} "SASL - SASL authentication library"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

