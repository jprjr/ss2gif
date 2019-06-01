!define APPNAME "SS2GIF"
Name "SS2GIF"
Outfile "ss2gif-installer.exe"
RequestExecutionLevel admin
InstallDir "$ProgramFiles\${APPNAME}"

!include LogicLib.nsh
!include MUI.nsh

Function .onInit
SetShellVarContext all
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
    MessageBox mb_iconstop "Administrator rights required!"
    SetErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
    Quit
${EndIf}
FunctionEnd

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

Section
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\Uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SS2GIF"   "DisplayName" "${APPNAME}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SS2GIF"   "UninstallString" "$INSTDIR\Uninstall.exe"
File "ss2gif.exe"
CreateShortCut "$SMPROGRAMS\${APPNAME}.lnk" "$INSTDIR\ss2gif.exe"
SectionEnd


Section "Uninstall"
Delete "$INSTDIR\ss2gif.exe"
RMDir  "$INSTDIR"
Delete "$SMPROGRAMS\${APPNAME}.lnk"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SS2GIF"
Delete "$INSTDIR\Uninstall.exe"
RMDir "$INSTDIR"
SectionEnd


