[Setup]
AppName=FarrWebMetaSearch
AppVersion=1.6.1
DefaultDirName={reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\Plugins\FarrWebMetaSearch
OutputBaseFilename=FarrWebMetaSearchSetup
Uninstallable=no
Compression=lzma2
SolidCompression=yes

[Files]
Source: "..\Deploy\FarrWebMetaSearch.dll"; DestDir: "{app}"; BeforeInstall: UnloadFarrPlugins
Source: "..\Deploy\FarrWebMetaSearch.ico"; DestDir: "{app}"
Source: "..\Deploy\FarrWebMetaSearch.dcupdate"; DestDir: "{app}"
Source: "..\Deploy\icons\*.ico"; DestDir: "{app}\icons"
Source: "..\Deploy\searches\*.conf"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.ico"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.html"; DestDir: "{app}\searches"
Source: "..\Deploy\FarrWebMetaSearch.html"; DestDir: "{app}"; Flags: isreadme; AfterInstall: ReloadFarrPlugins 

[Code]
const 
  WM_USER = 1024;

var
  WindowHandle: HWND;

procedure UnloadFarrPlugins();
begin
  WindowHandle := FindWindowByWindowName('Find and Run Robot');
  if WindowHandle <> 0 then begin
    // tell FARR to unload plugins
    SendMessage(WindowHandle, WM_USER, 97, 98);
  end;
end;

procedure ReloadFarrPlugins();
begin
  if WindowHandle <> 0 then begin
    // tell FARR to reload plugins
    SendMessage(WindowHandle, WM_USER, 98, 98);
  end;
end;
