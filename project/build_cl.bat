@echo off
chcp 65001

rem https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options

set VSLANG=1033
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
call "vcvarsall.bat" x64
popd
set shipping=-O2
set debug=-Od -Zi -Zf -JMC
set common=-std:c11 -nologo -WX -W4 -EHa- -GR- -diagnostics:caret

rem rem release: 
rem rem debug: 

echo ---- BUILD ---- %time%
cd ../bin

rem compile with linking
cl "../project/unity_build.c" -I "../engine" -Fo"ninety_nine.obj" -Fe"ninety_nine.exe" %common% %debug%

rem compile without linking, then link
rem cl "../project/unity_build.c" -I "../engine" -Fo"ninety_nine.obj" %common% %debug%
rem link "ninety_nine.obj" -OUT:"ninety_nine.exe" -nologo

cd ../project
echo ---- DONE ---- %time%
