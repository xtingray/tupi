[Setup]
AppName=Tupi
AppVersion=0.2-6
DefaultDirName={pf}\Tupi
DefaultGroupName=Tupi Open 2D Magic
OutputDir=c:\devel\installer
ChangesEnvironment=yes
ChangesAssociations=yes

[Dirs]
Name: "{app}\bin"
Name: "{app}\bin\platforms"
Name: "{app}\data"
Name: "{app}\data\help"
Name: "{app}\data\help\css"
Name: "{app}\data\help\en"
Name: "{app}\data\help\es"
Name: "{app}\data\help\examples"
Name: "{app}\data\help\gl"
Name: "{app}\data\help\images"
Name: "{app}\data\help\ru"
Name: "{app}\data\palettes"
Name: "{app}\data\storyboard"
Name: "{app}\data\themes"
Name: "{app}\data\themes\default"
Name: "{app}\data\themes\default\cursors"
Name: "{app}\data\themes\default\icons"
Name: "{app}\data\themes\default\images"
Name: "{app}\data\translations"
Name: "{app}\data\xml"
Name: "{app}\data\xml\ca"
Name: "{app}\data\xml\cs"
Name: "{app}\data\xml\da"
Name: "{app}\data\xml\de"
Name: "{app}\data\xml\en"
Name: "{app}\data\xml\es"
Name: "{app}\data\xml\gl"
Name: "{app}\data\xml\pt"
Name: "{app}\data\xml\ru"
Name: "{app}\data\xml\uk"
Name: "{app}\plugins"
Name: "{app}\lib"
Name: "{app}\lib\qt5"
Name: "{app}\lib\libav"

[Files]
Source: "bin\*"; DestDir: "{app}\bin"
Source: "bin\platforms\*"; DestDir: "{app}\bin\platforms"
Source: "data\help\css\*"; DestDir: "{app}\data\help\css"
Source: "data\help\en\*"; DestDir: "{app}\data\help\en"
Source: "data\help\es\*"; DestDir: "{app}\data\help\es"
Source: "data\help\examples\*"; DestDir: "{app}\data\help\examples"
Source: "data\help\gl\*"; DestDir: "{app}\data\help\gl"
Source: "data\help\images\*"; DestDir: "{app}\data\help\images"
Source: "data\help\ru\*"; DestDir: "{app}\data\help\ru"
Source: "data\palettes\*"; DestDir: "{app}\data\palettes"
Source: "data\storyboard\*"; DestDir: "{app}\data\storyboard"
Source: "data\themes\default\cursors\*"; DestDir: "{app}\data\themes\default\cursors"
Source: "data\themes\default\icons\*"; DestDir: "{app}\data\themes\default\icons"
Source: "data\themes\default\images\*"; DestDir: "{app}\data\themes\default\images"
Source: "data\translations\*"; DestDir: "{app}\data\translations"
Source: "data\xml\ca\*"; DestDir: "{app}\data\xml\ca"
Source: "data\xml\cs\*"; DestDir: "{app}\data\xml\cs"
Source: "data\xml\da\*"; DestDir: "{app}\data\xml\da"
Source: "data\xml\de\*"; DestDir: "{app}\data\xml\de"
Source: "data\xml\en\*"; DestDir: "{app}\data\xml\en"
Source: "data\xml\es\*"; DestDir: "{app}\data\xml\es"
Source: "data\xml\gl\*"; DestDir: "{app}\data\xml\gl"
Source: "data\xml\pt\*"; DestDir: "{app}\data\xml\pt"
Source: "data\xml\ru\*"; DestDir: "{app}\data\xml\ru"
Source: "data\xml\uk\*"; DestDir: "{app}\data\xml\uk"
Source: "plugins\*"; DestDir: "{app}\plugins"
Source: "lib\*"; DestDir: "{app}\lib"
Source: "lib\qt5\*"; DestDir: "{app}\lib\qt5"
Source: "lib\libav\*"; DestDir: "{app}\lib\libav"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_HOME"; ValueData: "{app}"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\lib;{app}\lib\qt5;{app}\lib\libav"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_SHARE"; ValueData: "{app}\data"
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_PLUGIN"; ValueData: "{app}\plugins"
Root: HKCR; Subkey: ".tup"; ValueType: string; ValueName: ""; ValueData: "TupiOpen2DMagic"; Flags: uninsdeletevalue 
Root: HKCR; Subkey: "TupiOpen2DMagic"; ValueType: string; ValueName: ""; ValueData: "Tupi Open 2D Magic"; Flags: uninsdeletekey
Root: HKCR; Subkey: "TupiOpen2DMagic\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\tupi.ico,0" 
Root: HKCR; Subkey: "TupiOpen2DMagic\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\tupi.exe"" ""%1""" 

[Icons]
Name: "{group}\Tupi"; Filename: "{app}\bin\tupi.exe"; IconFilename: "{app}\bin\tupi.ico"

