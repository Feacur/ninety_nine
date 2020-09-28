@echo off
chcp 65001

rem https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options

set VSLANG=1033
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
call "vcvarsall.bat" x64
popd

set compiler=-nologo -std:c11 -WX -W4 -EHa- -GR- -diagnostics:caret
set compiler_dbg=-Od -JMC -Zi
set compiler_shp=-O2
set linker=-nologo -WX -incremental:no
set linker_dbg=-debug:full

echo ---- BUILD ---- %time%
cd ..
if not exist bin mkdir bin
cd bin

rem compile with linking
cl "../project/unity_build.c" -I "../engine" -Fo"ninety_nine.obj" -Fe"ninety_nine.exe" %compiler% %compiler_dbg% -link %linker%

rem compile without linking, then link
rem cl -c "../project/unity_build.c" -I "../engine" -Fo"ninety_nine.obj" %compiler% %compiler_dbg%
rem link "ninety_nine.obj" -OUT:"ninety_nine.exe" %linker% %linker_dbg%

cd ../project
echo ---- DONE ---- %time%
