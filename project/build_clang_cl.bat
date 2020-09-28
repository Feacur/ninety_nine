@echo off
chcp 65001

rem make tools available
set VSLANG=1033
pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
call "vcvarsall.bat" x64
popd

rem make tools available
SET PATH=%PATH%;"C:/Program Files/LLVM/bin"

rem options
set compiler=-nologo -WX -W4 -EHa- -GR- -diagnostics:caret
set linker=-nologo -WX

set debug=dummy
if defined debug (
	set compiler=%compiler% -Od -Zi
	set linker=%linker% -debug:full
) else (
	set compiler=%compiler% -O2
	set linker=%linker% -debug:no
)

rem process
set timeStart=%time%
cd ..
if not exist bin mkdir bin
cd bin

rem compile with linking
clang-cl "../project/unity_build.c" -I".." -Fe"ninety_nine.exe" %compiler% -link %linker%

rem compile without linking, then link
rem clang-cl -c "../project/unity_build_engine.c" -I".." %compiler%
rem clang-cl -c "../project/unity_build_sandbox.c" -I".." %compiler%
rem lld-link "unity_build_engine.obj" "unity_build_sandbox.obj" -OUT:"ninety_nine.exe" %linker%

cd ../project
set timeStop=%time%

rem report
echo start: %timeStart%
echo stop:  %timeStop%
