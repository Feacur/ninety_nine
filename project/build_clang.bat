@echo off
chcp 65001

rem https://clang.llvm.org/docs/index.html
rem https://clang.llvm.org/docs/CommandGuide/clang.html
rem https://clang.llvm.org/docs/UsersManual.html
rem https://clang.llvm.org/docs/ClangCommandLineReference.html

SET PATH=%PATH%;"C:/Program Files/LLVM/bin"
set compiler=-std=c99 -Werror -Weverything -fno-exceptions -fno-rtti
set compiler_dbg=-O0 -g
set compiler_shp=-O3

echo ---- BUILD ---- %time%
cd ..
if not exist bin mkdir bin
cd bin

rem compile with linking, regular tools
clang "../project/unity_build.c" -I"../engine" -o"ninety_nine.exe" %compiler% %compiler_dbg%

cd ../project
echo ---- DONE ---- %time%
