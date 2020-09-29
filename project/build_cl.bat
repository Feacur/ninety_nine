@echo off
chcp 65001

set debug=dummy
set unity_build=dummy

rem https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options
rem https://docs.microsoft.com/en-us/cpp/build/reference/linker-options

rem > PREPARE TOOLS
rem set "PATH=%PATH%;C:/Program Files/LLVM/bin"
rem possible `clang-cl` instead `cl -std:c11`
rem possible `lld-link` instead `link`

set VSLANG=1033
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
call "vcvarsall.bat" x64
popd

rem > OPTIONS
set includes=-I".." -I"../third_party"
set defines=-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX
set libs=user32.lib
set compiler=-nologo -diagnostics:caret -WX -W4 -EHa- -GR- %includes% %defines%
set linker=-nologo -WX -subsystem:console %libs%

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
	cl -std:c11 "../project/unity_build.c" -Fe"ninety_nine.exe" %compiler% -link %linker%
) else ( rem alternatively, compile a set of translation units
	cl -std:c11 -c "../engine/internal/*.c" "../engine/platform_windows/*.c" "../sandbox/*.c" %compiler%
	link "*.obj" -out:"ninety_nine.exe" %linker%
)

cd ../project
set timeStop=%time%

rem > REPORT
echo start: %timeStart%
echo stop:  %timeStop%
