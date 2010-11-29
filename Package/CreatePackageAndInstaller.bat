@ECHO off
SET SevenZipPath="C:\Program Files\7-Zip\7z.exe"
Set InnoSetupPath="C:\Program Files\Inno Setup 5\ISCC.exe"

%PHI_PROJECTS%\UpdateVersion\Deploy\UpdateVersion.exe -s %PHI_PROJECTS%\FarrWebMetaSearch\Deploy\FarrWebMetaSearch.dll -t %PHI_PROJECTS%\FarrWebMetaSearch\Deploy\FarrWebMetaSearch.dcupdate -t %PHI_PROJECTS%\FarrWebMetaSearch\Package\versioninfo.xml -t %PHI_PROJECTS%\FarrWebMetaSearch\Package\FarrWebMetaSearch.iss
%SevenZipPath% a Output\FarrWebMetaSearch.zip %PHI_PROJECTS%\FarrWebMetaSearch\Deploy\*.* -r -xr!.svn -tzip -mx7
%InnoSetupPath% FarrWebMetaSearch.iss