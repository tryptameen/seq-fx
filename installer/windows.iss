#define MyAppName "SeqFX"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "SeqFX"
#define MyAppURL "https://github.com/tryptameen/seq-fx"

[Setup]
AppId={{A1B2C3D4-E5F6-7890-1234-567890ABCDEF}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=Output
OutputBaseFilename=SeqFX-Windows-Installer
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\build\SeqFX_artefacts\Release\Standalone\SeqFX.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\SeqFX_artefacts\Release\VST3\SeqFX.vst3\*"; DestDir: "{commoncf64}\VST3\SeqFX.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\SeqFX.exe"
