#define MyAppName "R-Tool"
#define MyAppVersion GetEnv("RTool_VERSION")
#if MyAppVersion == ""
#define MyAppVersion "0.1.0"
#endif
#define MyAppPublisher "Ranze"
#define MyAppURL "https://github.com/take-tech/r-tool"
#define SourceVst3 GetEnv("RTOOL_VST3_DIR")
#if SourceVst3 == ""
#define SourceVst3 "..\\..\\build\\RTool_artefacts\\Release\\VST3\\R-Tool.vst3"
#endif

[Setup]
AppId={{A1E2F3B4-5C6D-7E8F-9A0B-1C2D3E4F5A6B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commoncf}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
OutputBaseFilename=R-Tool-{#MyAppVersion}-Windows-x64-Setup
OutputDir=..\..\build\packages
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
UninstallDisplayName={#MyAppName}

[Files]
Source: "{#SourceVst3}\*"; DestDir: "{commoncf}\VST3\R-Tool.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[InstallDelete]
Type: filesandordirs; Name: "{commoncf}\VST3\R-Tool.vst3"
