@echo off
chcp 65001

rem https://clang.llvm.org/docs/index.html
rem https://clang.llvm.org/docs/CommandGuide/clang.html
rem https://clang.llvm.org/docs/UsersManual.html
rem https://clang.llvm.org/docs/ClangCommandLineReference.html

rem make tools available
SET PATH=%PATH%;"C:/Program Files/LLVM/bin"

rem options
set target=ninety_nine
set compiler=-std=c99 -Werror -Weverything -fno-exceptions -fno-rtti

set debug=dummy
if defined debug (
	set compiler=%compiler% -O0 -g
) else (
	set compiler=%compiler% -O3
)

rem process
set timeStart=%time%
cd ..
if not exist bin mkdir bin
cd bin

rem compile with linking, regular tools
clang "../project/unity_build.c" -I"../engine" -o"%target%.exe" %compiler%

cd ../project
set timeStop=%time%

rem report
echo start: %timeStart%
echo stop:  %timeStop%
