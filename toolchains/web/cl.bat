@echo off

set llvm=C:\Users\sgree\Downloads\LLVM\build\Release\bin

%llvm%\clang++ -emit-llvm --target=wasm32-unknown-unknown-elf -nostdlib -fno-exceptions -S %*
