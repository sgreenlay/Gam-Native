@echo off

:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="-o" (
    set output=%~2
    SHIFT
) ELSE (
    set input=%~1 %input%
)
SHIFT
GOTO parse
:endparse

set llvm=C:\Users\sgree\Downloads\LLVM\build\Release\bin
set binaryen=C:\Users\sgree\Downloads\Binaryen\build\bin

%llvm%\llvm-link -o %output%.ll %input%
%llvm%\llc -march=wasm32 -o %output%.s %output%.ll
%binaryen%\s2wasm --allocate-stack 1000000 -o %output%.wast %output%.s
%binaryen%\wasm-as -o %output% %output%.wast
