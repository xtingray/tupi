[Setup]
AppName=Tupi
AppVersion=0.2.9
AppPublisher=MaeFloresta
AppPublisherURL=http://www.maefloresta.com/
DefaultDirName={pf}\Tupi
DefaultGroupName=Tupi 2D Magic
OutputDir=c:\devel\installer
ChangesEnvironment=yes
ChangesAssociations=yes
; Uncomment this line for 64bit installer
; ArchitecturesInstallIn64BitMode=x64

[Dirs]
Name: "{app}\bin"
Name: "{app}\bin\platforms"
Name: "{app}\bin\imageformats"
Name: "{app}\bin\mediaservice"
Name: "{app}\bin\audio"
Name: "{app}\data"
Name: "{app}\data\html"
Name: "{app}\data\html\css"
Name: "{app}\data\html\examples"
Name: "{app}\data\html\images"
Name: "{app}\data\html\pages"
Name: "{app}\data\palettes"
Name: "{app}\data\storyboard"
Name: "{app}\data\themes"
Name: "{app}\data\themes\default"
Name: "{app}\data\themes\defaul\config"
Name: "{app}\data\themes\default\cursors"
Name: "{app}\data\themes\default\icons"
Name: "{app}\data\themes\default\images"
Name: "{app}\data\themes\dark"
Name: "{app}\data\themes\dark\config"
Name: "{app}\data\themes\dark\cursors"
Name: "{app}\data\themes\dark\icons"
Name: "{app}\data\themes\dark\images"
Name: "{app}\data\translations"
Name: "{app}\data\xml"
Name: "{app}\data\xml\ca"
Name: "{app}\data\xml\cs"
Name: "{app}\data\xml\da"
Name: "{app}\data\xml\de"
Name: "{app}\data\xml\en"
Name: "{app}\data\xml\es"
Name: "{app}\data\xml\fr"
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
Source: "bin\imageformats\*"; DestDir: "{app}\bin\imageformats"
Source: "bin\mediaservice\*"; DestDir: "{app}\bin\mediaservice"
Source: "bin\audio\*"; DestDir: "{app}\bin\audio"
Source: "data\html\css\*"; DestDir: "{app}\data\html\css"
Source: "data\html\examples\*"; DestDir: "{app}\data\html\examples"
Source: "data\html\images\*"; DestDir: "{app}\data\html\images"
Source: "data\html\pages\*"; DestDir: "{app}\data\html\pages"
Source: "data\palettes\*"; DestDir: "{app}\data\palettes"
Source: "data\storyboard\*"; DestDir: "{app}\data\storyboard"
Source: "data\themes\default\config\*"; DestDir: "{app}\data\themes\default\config"
Source: "data\themes\default\cursors\*"; DestDir: "{app}\data\themes\default\cursors"
Source: "data\themes\default\icons\*"; DestDir: "{app}\data\themes\default\icons"
Source: "data\themes\default\images\*"; DestDir: "{app}\data\themes\default\images"
Source: "data\themes\dark\config\*"; DestDir: "{app}\data\themes\dark\config"
Source: "data\themes\dark\cursors\*"; DestDir: "{app}\data\themes\dark\cursors"
Source: "data\themes\dark\icons\*"; DestDir: "{app}\data\themes\dark\icons"
Source: "data\themes\dark\images\*"; DestDir: "{app}\data\themes\dark\images"
Source: "data\translations\*"; DestDir: "{app}\data\translations"
Source: "data\xml\ca\*"; DestDir: "{app}\data\xml\ca"
Source: "data\xml\cs\*"; DestDir: "{app}\data\xml\cs"
Source: "data\xml\da\*"; DestDir: "{app}\data\xml\da"
Source: "data\xml\de\*"; DestDir: "{app}\data\xml\de"
Source: "data\xml\en\*"; DestDir: "{app}\data\xml\en"
Source: "data\xml\es\*"; DestDir: "{app}\data\xml\es"
Source: "data\xml\fr\*"; DestDir: "{app}\data\xml\fr"
Source: "data\xml\gl\*"; DestDir: "{app}\data\xml\gl"
Source: "data\xml\pt\*"; DestDir: "{app}\data\xml\pt"
Source: "data\xml\ru\*"; DestDir: "{app}\data\xml\ru"
Source: "data\xml\uk\*"; DestDir: "{app}\data\xml\uk"
Source: "plugins\*"; DestDir: "{app}\plugins"
Source: "lib\*"; DestDir: "{app}\lib"
Source: "lib\qt5\*"; DestDir: "{app}\lib\qt5"
Source: "lib\libav\*"; DestDir: "{app}\lib\libav"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_HOME"; ValueData: "{app}"; Flags: uninsdeletevalue
; Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\lib;{app}\lib\qt5;{app}\lib\libav";
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_SHARE"; ValueData: "{app}\data"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "TUPI_PLUGIN"; ValueData: "{app}\plugins"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".tup"; ValueType: string; ValueName: ""; ValueData: "Tupi2DMagic"; Flags: uninsdeletevalue 
Root: HKCR; Subkey: "Tupi2DMagic"; ValueType: string; ValueName: ""; ValueData: "Tupi 2D Magic"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Tupi2DMagic\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\tupi.ico,0"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Tupi2DMagic\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\Tupi.vbs"" ""%1"""; Flags: uninsdeletekey

[Icons]
Name: "{group}\Tupi"; Filename: "{app}\bin\Tupi.vbs"; IconFilename: "{app}\bin\tupi.ico"
Name: "{commondesktop}\Tupi"; Filename: "{app}\bin\Tupi.vbs"; IconFilename: "{app}\bin\tupi.ico"; Comment: "Tupi 2D Magic"