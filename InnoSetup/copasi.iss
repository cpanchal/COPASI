; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "COPASI"
#define MyAppVersion "0.0.0"
#define MyAppPublisher "copasi.org"
#define MyAppURL "http://www.copasi.org/"
#define MyAppExeName "bin\CopasiUI.exe"
#define MyBuild "0"
#define MyAppId "{{00000000-0000-0000-0000-000000000000}"
#define MyWorkDir "C:\cygwin\home\shoops\environment\win32-icc-32\copasi_dev\RELEASE\InnoSetup"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
ChangesAssociations=true
ChangesEnvironment=true
DefaultDirName={code:DefDirRoot}\{#MyAppPublisher}\{#MyAppName} {#MyAppVersion}
DefaultGroupName={#MyAppName} {#MyAppVersion}
OutputDir={#MyWorkDir}
OutputBaseFilename=Copasi-{#MyBuild}-WIN32
SetupIconFile={#MyWorkDir}\addremov.ico
Compression=lzma/Ultra
SolidCompression=true
InternalCompressLevel=Ultra
;SignTool=Standard sign /f C:\cygwin\home\shoops\environment\qt4\copasi_dev\development\InnoSetup\copasi.pfx /p Koichi01 $f
SignedUninstaller=false
VersionInfoVersion={#MyAppVersion}.0
ShowLanguageDialog=no
UninstallDisplayIcon={app}\share\copasi\icons\Copasi.ico
UninstallDisplayName={#MyAppName} {#MyAppVersion}
PrivilegesRequired=none
VersionInfoCompany=copasi.org

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: ..\setup\copasi\bin\CopasiSE.exe; DestDir: {app}\bin
Source: ..\setup\copasi\bin\CopasiUI.exe; DestDir: {app}\bin
Source: ..\setup\copasi\bin\libmmd.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\svml_dispmd.dll; DestDir: {app}\bin; 
Source: ..\setup\copasi\bin\phonon4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\Qt3Support4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtCore4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtGui4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtNetwork4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtOpenGL4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtSql4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtSvg4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtWebKit4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\QtXml4.dll; DestDir: {app}\bin
Source: ..\setup\copasi\bin\msvcr90.dll; DestDir: {app}\bin; Check: InstallUserRuntime(); 
Source: ..\setup\copasi\bin\msvcp90.dll; DestDir: {app}\bin; Check: InstallUserRuntime(); 
Source: ..\setup\copasi\README.txt; DestDir: {app}
Source: ..\setup\copasi\LICENSE.txt; DestDir: {app}
Source: ..\setup\copasi\share\copasi\config\MIRIAMResources.xml; DestDir: {app}\share\copasi\config
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step6.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step1.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step2.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step3.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step4.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\TutWiz-Step5.html; DestDir: {app}\share\copasi\doc\html
Source: ..\setup\copasi\share\copasi\doc\html\figures\TimeCourseDialog.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ModelSettingsDialog.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\NewPlotAdded.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ObjectBrowserSelection.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ObjectBrowserTree.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\OutputAssistant.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\PlotCurveSelectionDialog.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\PlotDefinition.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\PlotWindow.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ReactionDialog.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ReactionOverview.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ReactionOverviewEmpty.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\share\copasi\doc\html\figures\ReportDefinitionDialog.png; DestDir: {app}\share\copasi\doc\html\figures
Source: ..\setup\copasi\bin\vcredist_x86.exe; DestDir: {app}; Flags: deleteafterinstall
Source: ..\setup\copasi\share\copasi\examples\YeastGlycolysis.gps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\brusselator.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\CircadianClock.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\DimericMWC-stiff.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\Genetic-2000Elo.xml; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\MAPK-HF96-layout.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\Metabolism-2000Poo.xml; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\NF-kappaB.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\examples\Olsen2003_peroxidase.cps; DestDir: {app}\share\copasi\examples
Source: ..\setup\copasi\share\copasi\icons\CopasiDoc.ico; DestDir: {app}\share\copasi\icons
Source: ..\setup\copasi\share\copasi\icons\Copasi.ico; DestDir: {app}\share\copasi\icons


[Icons]
Name: {group}\CopasiUI; Filename: {app}\{#MyAppExeName}; WorkingDir: {userdocs}
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {#MyAppURL}
Name: {group}\License; Filename: {app}\LICENSE.txt
Name: {group}\README; Filename: {app}\README.txt
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; WorkingDir: {userdocs}
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon; WorkingDir: {userdocs}

[Run]
Filename: {app}\vcredist_x86.exe; StatusMsg: Installing Microsoft Visual C++ 2008 Runtime Libraries; Parameters: /q:a; Check: InstallSystemRuntime()

[Dirs]
Name: {app}\bin
Name: {app}\share
Name: {app}\share\copasi
Name: {app}\share\copasi\config
Name: {app}\share\copasi\doc
Name: {app}\share\copasi\doc\html
Name: {app}\share\copasi\doc\html\figures
Name: {app}\share\copasi\examples
Name: {app}\share\copasi\icons

[Registry]
Root: HKCR; SubKey: .cps; ValueType: string; ValueData: COPASI.document; Flags: uninsdeletekey; Check: IsAdminUser
Root: HKCR; SubKey: COPASI.document; ValueType: string; ValueData: COPASI File; Flags: uninsdeletekey; Check: IsAdminUser
Root: HKCR; SubKey: COPASI.document\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\CopasiUI.exe"" ""%1"""; Flags: uninsdeletevalue; Check: IsAdminUser
Root: HKCR; Subkey: COPASI.document\DefaultIcon; ValueType: string; ValueData: {app}\share\copasi\icons\CopasiDoc.ico,-1; Flags: uninsdeletevalue; Check: IsAdminUser
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: COPASIDIR; ValueData: {app}; Check: IsAdminUser
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: expandsz; ValueName: Path; ValueData: "%COPASIDIR%\bin;{olddata}"; Check: UpdateSystemPath
Root: HKLM; Subkey: Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers; ValueType: string; ValueName: "{app}\bin\CopasiUI.exe"; ValueData: "~ WIN7RTM"; Flags: uninsdeletevalue; Check: IsAdminUserAndWindows8
Root: HKLM64; Subkey: Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers; ValueType: string; ValueName: "{app}\bin\CopasiUI.exe"; ValueData: "~ WIN7RTM"; Flags: uninsdeletevalue; Check: IsAdminUserAndWindows8And64

Root: HKCU; SubKey: Software\Classes\.cps; ValueType: string; ValueData: COPASI.document; Flags: uninsdeletekey; Check: IsRegularUser
Root: HKCU; SubKey: Software\Classes\COPASI.document; ValueType: string; ValueData: COPASI File; Flags: uninsdeletekey; Check: IsRegularUser
Root: HKCU; SubKey: Software\Classes\COPASI.document\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\CopasiUI.exe"" ""%1"""; Flags: uninsdeletevalue; Check: IsRegularUser
Root: HKCU; Subkey: Software\Classes\COPASI.document\DefaultIcon; ValueType: string; ValueData: {app}\share\copasi\icons\CopasiDoc.ico,-1; Flags: uninsdeletevalue; Check: IsRegularUser
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: COPASIDIR; ValueData: {app}; Check: IsRegularUser
Root: HKCU; Subkey: Environment; ValueType: expandsz; ValueName: Path; ValueData: "%COPASIDIR%\bin;{olddata}"; Check: UpdateUserPath
Root: HKCU; Subkey: Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers; ValueType: string; ValueName: "{app}\bin\CopasiUI.exe"; ValueData: "~ WIN7RTM"; Flags: uninsdeletevalue; Check: IsRegularUserAndWindows8
Root: HKCU64; Subkey: Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers; ValueType: string; ValueName: "{app}\bin\CopasiUI.exe"; ValueData: "~ WIN7RTM"; Flags: uninsdeletevalue; Check: IsRegularUserAndWindows8And64

[Code]
type
  RunTimeVersion = array [0..3] of Integer;

function IsRunningOn64bit(): Boolean;
begin
  Result := False;  
  try
  if ( '{pf64}' <> '{pf32}') then
  begin
    Result := True;
  end;
  except
  end;
end;

function IsAdminUser(): Boolean;
begin
  Result := (IsAdminLoggedOn or IsPowerUserLoggedOn);
end;

function IsRegularUser(): Boolean;
begin
  Result := not (IsAdminLoggedOn or IsPowerUserLoggedOn);
end;

function isWindows8(): Boolean;
var
  Version: TWindowsVersion;

begin
  GetWindowsVersionEx(Version);

  Result := ((Version.Major = 6) and (Version.Minor = 2));
end;

function IsAdminUserAndWindows8(): Boolean;
begin
  Result := (IsAdminUser() and isWindows8());
end;

function IsAdminUserAndWindows8And64(): Boolean;
begin
  Result := (IsAdminUser() and isWindows8() and IsRunningOn64bit());
end;

function IsRegularUserAndWindows8(): Boolean;
begin
  Result := (IsRegularUser() and isWindows8());
end;

function IsRegularUserAndWindows8And64(): Boolean;
begin
  Result := (IsRegularUser() and isWindows8() and IsRunningOn64bit());
end;

function IsCopasiInSystemPath(): Boolean;
var
  CurrentPath: String;
  Position: Integer;

begin
  Result := False;

  if RegQueryStringValue(HKLM,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', CurrentPath) then
    // Successfully read the value
    begin
      Position := Pos('%COPASIDIR%\bin', CurrentPath);
      if (Position <> 0) then
        // Found an entry to the COPASI binaries in the Path
        begin
          Result := True;
        end;
    end;
end;

function IsCopasiInUserPath(): Boolean;
var
  CurrentPath: String;
  Position: Integer;

begin
  Result := IsCopasiInSystemPath();

  if RegQueryStringValue(HKCU,
    'Environment',
    'Path', CurrentPath) then
    // Successfully read the value
    begin
      Position := Pos('%COPASIDIR%\bin', CurrentPath);
      if (Position <> 0) then
        // Found an entry to the COPASI binaries in the Path
        begin
          Result := True;
        end;
    end;
end;

function UpdateSystemPath(): Boolean;
begin
  Result := (IsAdminUser() and not IsCopasiInSystemPath());
end;

function UpdateUserPath(): Boolean;
begin
  Result := (IsRegularUser() and not IsCopasiInUserPath());
end;

function StrToVersion(str: String): RunTimeVersion;
var
  Version: RunTimeVersion;
  Position: Integer;

begin
  try
    begin
      Position := Pos('.', str);
      Version[0] := StrToInt(Copy(str, 1, Position - 1));
      Delete(str, 1, Position);

      Position := Pos('.', str);
      Version[1] := StrToInt(Copy(str, 1, Position - 1));
      Delete(str, 1, Position);

      Position := Pos('.', str);
      Version[2] := StrToInt(Copy(str, 1, Position - 1));
      Delete(str, 1, Position);

      Version[3] := StrToInt(str);
    end;

  except
    begin
      Version[0] := 0;
      Version[1] := 0;
      Version[2] := 0;
      Version[3] := 0;
    end;
  end;

  Result := Version;
end;

function VersionToStr(Version: RunTimeVersion): String;
begin
  Result := IntToStr(Version[0]) + '.' +
            IntToStr(Version[1]) + '.' +
            IntToStr(Version[2]) + '.' +
            IntToStr(Version[3]);
end;

function CompareVersion(Version1: RunTimeVersion;
                        Version2: RunTimeVersion): Integer;
begin
  if Version1[0] <> Version2[0] then
    Result := Version1[0] - Version2[0]
  else if Version1[1] <> Version2[1] then
    Result := Version1[1] - Version2[1]
  else if Version1[2] <> Version2[2] then
    Result := Version1[2] - Version2[2]
  else
    Result := Version1[3] - Version2[3];
end;

function IsInVersionRange(OldVersionRange: String;
                          Version: RunTimeVersion): Boolean;
var
  LowerBound: RunTimeVersion;
  UpperBound: RunTimeVersion;
  Position: Integer;

begin
  Result := False;
  Position := Pos('-', OldVersionRange);

  if Position <> 0 then
    begin
      LowerBound := StrToVersion(Copy(OldVersionRange, 1, Position - 1));
      UpperBound := StrToVersion(Copy(OldVersionRange, Position + 1, 100));
    end
  else
    begin
      LowerBound := StrToVersion(OldVersionRange);
      UpperBound := LowerBound;
    end;

  if (CompareVersion(LowerBound, Version) <= 0) and
     (CompareVersion(Version, UpperBound) <= 0) then
    begin
      Result := True;
    end;
end;

function IsSuitableSxSInstallation(Policy: AnsiString;
                                   Version: RunTimeVersion;
                                   Var SxSVersion: RunTimeVersion): Boolean;
var
  Position: Integer;
  Position2: Integer;
  OldVersionRange: String;
  SxSVersionStr: String;

begin
  Result := False;
  SxSVersion := Version;

  Position := Pos('oldVersion=', Policy);

  while Position <> 0 do
    begin
      Delete(Policy, 1, Position + 10);
      Position := Pos(' ', Policy);

      OldVersionRange := Copy(Policy, 1, Position - 1);
      OldVersionRange := RemoveQuotes(OldVersionRange);
      Delete(Policy, 1, Position);

      if IsInVersionRange(OldVersionRange, Version) then
        begin
          Position := Pos('newVersion=', Policy);
          Position2 := Pos('oldVersion=', Policy);

          if ((Position <> 0) and
              ((Position < Position2) or (Position2 = 0))) then
             begin
               Delete(Policy, 1, Position + 10);
               Position := Pos('/', Policy);

               SxSVersionStr := Copy(Policy, 1, Position - 1);
               SxSVersionStr := RemoveQuotes(SxSVersionStr);
               SxSVersion := StrToVersion(SxSVersionStr);

               Result := True;
             end;
        end;

      Position := Pos('oldVersion=', Policy);
    end;
end;

function HaveRuntimeDLLs(Architecture: String;
                         Name: String;
                         Key: String;
                         Version: RunTimeVersion;
                         DLLs: TStrings): Boolean;

var
  FindDir: TFindRec;
  Index: Integer;

begin
  Result := False;

  if FindFirst(ExpandConstant('{win}\WinSxS\') +
               Architecture + '_' +
               Name + '_' +
               Key + '_' +
               VersionToStr(Version) + '_*',
               FindDir) then
    begin
      try
        repeat
          if (FindDir.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0 then
            begin
              Result := True;

              // Loop through all DLLs
              for Index := 0 to DLLs.Count - 1 do
                if (not FileExists(ExpandConstant('{win}\winsxs\') +
                                   FindDir.Name + '\' + DLLs[Index])) then
                  Result := False;

            end;
        until not FindNext(FindDir);

      finally
        FindClose(FindDir);
      end;
    end;
end;

function HaveSxSInstallation(Architecture: String;
                             Name: String;
                             Key: String;
                             Version: RunTimeVersion;
                             DLLs: TStrings): Boolean;
var
  VersionStr: String;
  Position: Integer;
  VersionFound: RunTimeVersion;
  FindDir: TFindRec;

begin
  Result := False;

  VersionStr := IntToStr(Version[0]) + '.' +
                IntToStr(Version[1]) + '.' +
                IntToStr(Version[2]) + '.*';

  if FindFirst(ExpandConstant('{win}\WinSxS\') +
               Architecture + '_' +
               Name + '_' +
               Key + '_' +
               VersionStr + '_*',
               FindDir) then
    begin
      try
        repeat
          if (FindDir.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0 then
            begin
              VersionStr := FindDir.Name;
              Delete(VersionStr, 1, Length(Architecture + '_' + Name + '_' + Key + '_'));
              Position := Pos('_', VersionStr);
              VersionStr := Copy(VersionStr, 1, Position - 1);
              VersionFound := StrToVersion(VersionStr);

              if CompareVersion(Version, VersionFound) <= 0 then
                Result := HaveRuntimeDLLs(Architecture, Name, Key, VersionFound, DLLs);
            end;
        until Result or not FindNext(FindDir);

      finally
        FindClose(FindDir);
      end;
    end;

end;

function HaveSxSInstallationXP(Architecture: String;
                               Name: String;
                               Key: String;
                               Version: RunTimeVersion;
                               DLLs: TStrings): Boolean;
var
  FindDir: TFindRec;
  FindFile: TFindRec;
  Policy: AnsiString;
  SxSVersion: RunTimeVersion;

begin
  Result := False;

  if FindFirst(ExpandConstant('{win}\WinSxS\Policies\') +
               Architecture + '_policy.' +
               IntToStr(Version[0]) + '.' +
               IntToStr(Version[1]) + '.' +
               Name + '_' +
               Key + '_*',
               FindDir) then
    begin
      try
        repeat
          if (FindDir.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0 then
            begin
              if FindFirst(ExpandConstant('{win}\WinSxS\Policies\') +
                           FindDir.Name + '\' +
                           IntToStr(Version[0]) + '.' +
                           IntToStr(Version[1]) + '.' +
                           IntToStr(Version[2]) + '.*.policy',
                           FindFile) then
                begin
                  try
                    repeat
                      if (FindFile.Attributes and FILE_ATTRIBUTE_DIRECTORY) = 0 then
                        begin
                          LoadStringFromFile(ExpandConstant('{win}\WinSxS\Policies\') +
						                     FindDir.Name + '\' +
                                             FindFile.Name,
                                             Policy);
                          if IsSuitableSxSInstallation(Policy, Version, SxSVersion) then
                            begin
                              if HaveRuntimeDLLs(Architecture, Name, Key, SxSVersion, DLLs) then
                                Result := True;
                            end;
                        end;
                    until not FindNext(FindFile);

                  finally
                    FindClose(FindFile);
                  end;
                end;
            end;
        until not FindNext(FindDir);

      finally
        FindClose(FindDir);
      end;
    end;
end;

function HaveRuntime(): Boolean;
var
  Architecture: String;
  Name: String;
  Key: String;
  Version: RunTimeVersion;
  DLLs: TStringList;

begin
  Architecture := 'x86'
  Name := 'Microsoft.VC90.CRT'
  Key := '1fc8b3b9a1e18e3b'
  Version := StrToVersion('9.0.30729.1');

  DLLs := TStringList.Create;
  DLLs.Add('MSVCP90.DLL');
  DLLs.Add('MSVCR90.DLL');

  Result := HaveSxSInstallation(Architecture, Name, Key, Version, DLLs);
end;

function InstallSystemRuntime(): Boolean;
begin
  Result := (IsAdminUser() and not HaveRuntime());
end;

function InstallUserRuntime(): Boolean;
begin
  Result := (IsRegularUser() and not HaveRuntime());
end;

function InitializeSetup(): Boolean;
begin
  Result := True;

  RegDeleteValue(HKEY_CURRENT_USER, 'Environment', 'COPASIDIR');
end;

function DefDirRoot(Param: String): String;
begin
  if IsRegularUser() then
    Result := ExpandConstant('{localappdata}')
  else
    Result := ExpandConstant('{pf}')
end;
