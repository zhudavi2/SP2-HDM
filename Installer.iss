; -- HDM.iss --
; Inno Setup 5.6.1 Unicode
; Copyright D. Z. (BlasterMillennia)

[Setup]
AppName=SuperPower 2 Human Development Mod
AppVersion=10.7
DefaultDirName={pf}\Steam\steamapps\common\SuperPower 2\MODS\SP2
OutputBaseFilename=Install {#SetupSetting("AppName")} V{#SetupSetting("AppVersion")}
OutputDir=.
UsePreviousAppDir=no

[Languages]
Name: "English"; MessagesFile: "compiler:Default.isl"
Name: "Armenian"; MessagesFile: "compiler:Languages\Armenian.islu"
Name: "BrazilianPortuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "Catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "Corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "Czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "Danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "Dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "Finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "French"; MessagesFile: "compiler:Languages\French.isl"
Name: "German"; MessagesFile: "compiler:Languages\German.isl"
Name: "Greek"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "Hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "Hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "Italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "Japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "Nepali"; MessagesFile: "compiler:Languages\Nepali.islu"
Name: "Norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "Polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "Portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "ScottishGaelic"; MessagesFile: "compiler:Languages\ScottishGaelic.isl"
Name: "SerbianCyrillic"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "SerbianLatin"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
Name: "Slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "Spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "Turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "Ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Files]
Source: "Client\Release\client.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Client\hdm_cfg_client.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "Server\Release\Server.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Server\hdm_cfg_server.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme

