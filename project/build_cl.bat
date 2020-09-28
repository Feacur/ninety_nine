@echo off
chcp 65001

set debug=dummy
set unity_build=dummy

rem https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options
rem https://docs.microsoft.com/en-us/cpp/build/reference/linker-options

rem > PREPARE TOOLS
rem set "PATH=%PATH%;C:/Program Files/LLVM/bin"

set VSLANG=1033
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
call "vcvarsall.bat" x64
popd

rem > OPTIONS
set compiler=-nologo -std:c11 -WX -W4 -EHa- -GR- -diagnostics:caret
set linker=-nologo -WX

if defined debug (
	set compiler=%compiler% -Od -Zi
	set linker=%linker% -debug:full
) else (
	set compiler=%compiler% -O2
	set linker=%linker% -debug:no
)

rem > COMPILE AND LINK
set timeStart=%time%
cd ..
if not exist bin mkdir bin
cd bin

if defined unity_build (
	cl "../project/unity_build.c" -I".." -Fe"ninety_nine.exe" %compiler% -link %linker%
) else ( rem alternatively, compile a set of translation units
	cl -c "../project/unity_build_engine.c" "../project/unity_build_sandbox.c" -I".." %compiler%
	link "unity_build_engine.obj" "unity_build_sandbox.obj" -out:"ninety_nine.exe" %linker%
	rem cl -c "../engine/*.c" "../sandbox/*.c" -I".." %compiler%
	rem link "*.obj" -out:"ninety_nine.exe" %linker%
)

cd ../project
set timeStop=%time%

rem > REPORT
echo start: %timeStart%
echo stop:  %timeStop%
