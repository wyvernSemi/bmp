
[Setup]
AppName=bmp
AppVerName=BMP_1_0_0
DefaultDirName={src}\bmp
DisableProgramGroupPage=yes
OutputBaseFilename=setup_bmp_1_0_0

[Dirs]
Name: "{app}\src"
Name: "{app}\objs"

[Files]
Source:"bmp.iss";                          DestDir: "{app}"
                                           
Source:"Release\bmp.exe";                  DestDir: "{app}"
                                           
Source:"bitmaps.sln";                      DestDir: "{app}"
Source:"bitmaps.vcxproj";                  DestDir: "{app}"
Source:"bitmaps.vcxproj.filters";          DestDir: "{app}"
Source:"makefile";                         DestDir: "{app}"
                                            
Source:"src\*.c";                          DestDir: "{app}\src"
Source:"src\*.h";                          DestDir: "{app}\src"
Source:"..\include\general.h";             DestDir: "{app}\src"

Source:"Bitmap Doc\bmp_utility.htm";       DestDir: "{app}"; DestName: "README.html"; Flags: isreadme
