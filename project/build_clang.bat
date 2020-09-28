@echo off
chcp 65001

set debug=dummy
set unity_build=dummy

rem https://clang.llvm.org/docs/index.html
rem https://clang.llvm.org/docs/CommandGuide/clang.html
rem https://clang.llvm.org/docs/UsersManual.html
rem https://clang.llvm.org/docs/ClangCommandLineReference.html
rem https://lld.llvm.org/windows_support.html
rem https://docs.microsoft.com/en-us/cpp/build/reference/linker-options

rem > PREPARE TOOLS
set "PATH=%PATH%;C:/Program Files/LLVM/bin"

if not defined auto_linking (
	set VSLANG=1033
	pushd "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build"
	call "vcvarsall.bat" x64
	popd
)

rem > OPTIONS
set compiler=-std=c99 -Werror -Weverything -fno-exceptions -fno-rtti
set linker=-nologo -WX

if defined debug (
	set compiler=%compiler% -O0 -g
	set linker=%linker% -debug:full
) else (
	set compiler=%compiler% -O3
	set linker=%linker% -debug:no
)

rem > COMPILE AND LINK
set timeStart=%time%
cd ..
if not exist bin mkdir bin
cd bin

if defined unity_build (
	clang "../project/unity_build.c" -I".." -o"ninety_nine.exe" %compiler%
) else ( rem alternatively, compile a set of translation units
	clang -c "../project/unity_build_engine.c" "../project/unity_build_sandbox.c" -I".." %compiler%
	lld-link "unity_build_engine.o" "unity_build_sandbox.o" libcmt.lib -out:"ninety_nine.exe" %linker%
	rem clang -c "../engine/*.c" "../sandbox/*.c" -I".." %compiler%
	rem lld-link "*.o" libcmt.lib -out:"ninety_nine.exe" %linker%
)

cd ../project
set timeStop=%time%

rem > REPORT
echo start: %timeStart%
echo stop:  %timeStop%
