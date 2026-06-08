; Luna Plus Reborn — Windows NSIS installer template
; Build on Windows: makensis LunaPlusReborn.nsi
; Requires staged files in dist/windows/ (see package_release.sh)

!define APP_NAME "LUNA Plus Reborn"
!define APP_VERSION "1.1.0"
!define PUBLISHER "LUNA Reborn Team"
!define INSTALL_DIR "$PROGRAMFILES64\\${APP_NAME}"
!define DIST "dist\\windows"

Name "${APP_NAME} ${APP_VERSION}"
OutFile "LunaPlusReborn-${APP_VERSION}-setup.exe"
InstallDir "${INSTALL_DIR}"
RequestExecutionLevel admin

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
  SetOutPath "$INSTDIR"
  File /r "${DIST}\\*.*"
  WriteUninstaller "$INSTDIR\\Uninstall.exe"
  CreateDirectory "$SMPROGRAMS\\${APP_NAME}"
  CreateShortcut "$SMPROGRAMS\\${APP_NAME}\\${APP_NAME}.lnk" "$INSTDIR\\LunaPlusClient.exe"
  CreateShortcut "$SMPROGRAMS\\${APP_NAME}\\Uninstall.lnk" "$INSTDIR\\Uninstall.exe"
  CreateShortcut "$DESKTOP\\${APP_NAME}.lnk" "$INSTDIR\\LunaPlusClient.exe"
  WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LunaPlusReborn" \
    "DisplayName" "${APP_NAME}"
  WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LunaPlusReborn" \
    "UninstallString" "$INSTDIR\\Uninstall.exe"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\\Uninstall.exe"
  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\\${APP_NAME}"
  Delete "$DESKTOP\\${APP_NAME}.lnk"
  DeleteRegKey HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LunaPlusReborn"
SectionEnd
