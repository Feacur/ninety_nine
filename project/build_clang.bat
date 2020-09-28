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
set includes=-I".." -I"../third_party"
set defines=-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX
set libs=user32.lib
set compiler=-Werror -Weverything -fno-exceptions -fno-rtti %includes% %defines%
set linker=-nologo -WX -subsystem:console

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
	clang -std=c99 "../project/unity_build.c" -o"ninety_nine.exe" %compiler% -Wl,%libs: =,% -Xlinker -subsystem:console
) else ( rem alternatively, compile a set of translation units
	clang -std=c99 -c "../engine/internal/*.c" "../engine/platform_windows/*.c" "../sandbox/*.c" %compiler%
	lld-link "*.o" libcmt.lib -out:"ninety_nine.exe" %libs% %linker%
)

cd ../project
set timeStop=%time%

rem > REPORT
echo start: %timeStart%
echo stop:  %timeStop%
