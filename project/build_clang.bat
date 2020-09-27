@echo off
chcp 65001

rem https://clang.llvm.org/docs/index.html
rem https://clang.llvm.org/docs/CommandGuide/clang.html
rem https://clang.llvm.org/docs/UsersManual.html
rem https://clang.llvm.org/docs/ClangCommandLineReference.html

SET PATH=%PATH%;"C:/Program Files/LLVM/bin"
set shipping=-O3
set debug=-O0 -g
set common=-std=c99 -Werror -Weverything -fno-exceptions -fno-rtti

echo ---- BUILD ---- %time%
cd ../bin

rem compile with linking, regular tools
clang "../project/unity_build.c" -I"../engine" -o"ninety_nine.exe" %common% %debug%

cd ../project
echo ---- DONE ---- %time%
