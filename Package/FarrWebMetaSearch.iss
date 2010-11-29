[Setup]
AppName=FarrWebMetaSearch
AppVersion=1.1.2
DefaultDirName={reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\Plugins\FarrWebMetaSearch
OutputBaseFilename=FarrWebMetaSearchSetup
Uninstallable=no
Compression=lzma2
SolidCompression=yes

[Files]
Source: "..\Deploy\FarrWebMetaSearch.dll"; DestDir: "{app}"; BeforeInstall: ShutdownFarr
Source: "..\Deploy\FarrWebMetaSearch.ico"; DestDir: "{app}"
Source: "..\Deploy\FarrWebMetaSearch.dcupdate"; DestDir: "{app}"
Source: "..\Deploy\libcurl.dll"; DestDir: "{app}"
Source: "..\Deploy\icons\*.ico"; DestDir: "{app}\icons"
Source: "..\Deploy\searches\*.conf"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.ico"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.html"; DestDir: "{app}\searches"
Source: "..\Deploy\FarrWebMetaSearch.html"; DestDir: "{app}"; Flags: isreadme; AfterInstall: StartFarr 

[Code]
var
  FarrWasRunning: Boolean;

procedure ShutdownFarr();
var
  WindowHandle: HWND;
  FarrPath: String;
  ResultCode: Integer;
begin
  WindowHandle := FindWindowByWindowName('Find and Run Robot');
  if WindowHandle <> 0 then begin
    FarrWasRunning := true;
  
    FarrPath := ExpandConstant('{reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\FindAndRunRobot.exe');
    Exec(FarrPath, '-exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
end;

procedure StartFarr();
var
  FarrPath: String;
  ResultCode: Integer;
begin
  if FarrWasRunning then begin
    FarrPath := ExpandConstant('{reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\FindAndRunRobot.exe');
    Exec(FarrPath, '-show -search "aplugins"', '', SW_SHOW, ewNoWait, ResultCode);
  end;
end;
