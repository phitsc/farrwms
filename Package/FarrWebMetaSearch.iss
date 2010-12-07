[Setup]
AppName=FarrWebMetaSearch
AppVersion=1.6.1
DefaultDirName={reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\Plugins\FarrWebMetaSearch
OutputBaseFilename=FarrWebMetaSearchSetup
Uninstallable=no
Compression=lzma2
SolidCompression=yes

[Files]
Source: "..\Deploy\CloseApp2.dll"; DestDir: "{tmp}"; Flags: onlyifdoesntexist
Source: "..\Deploy\FarrWebMetaSearch.dll"; DestDir: "{app}";
Source: "..\Deploy\FarrWebMetaSearch.ico"; DestDir: "{app}"
Source: "..\Deploy\FarrWebMetaSearch.dcupdate"; DestDir: "{app}"
Source: "..\Deploy\icons\*.ico"; DestDir: "{app}\icons"
Source: "..\Deploy\searches\*.conf"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.ico"; DestDir: "{app}\searches"
Source: "..\Deploy\searches\*.html"; DestDir: "{app}\searches"
Source: "..\Deploy\FarrWebMetaSearch.html"; DestDir: "{app}"; Flags: isreadme

[Code]
// importing DLL functions from closeapp2.dll
procedure SendMsgUnloadPlugins_OnInstall(farrApplicationName: String);
external 'SendMsgUnloadPlugins@files:CloseApp2.dll cdecl setuponly';

procedure SendMsgLoadPlugins_OnInstall(farrApplicationName: String);
external 'SendMsgLoadPlugins@files:CloseApp2.dll cdecl setuponly';

//
procedure CurStepChanged(CurStep: TSetupStep);
begin
    case CurStep of
        ssInstall: SendMsgUnloadPlugins_OnInstall('FindAndRunRobot.exe');
        ssPostInstall: SendMsgLoadPlugins_OnInstall('FindAndRunRobot.exe');
    end;
end;