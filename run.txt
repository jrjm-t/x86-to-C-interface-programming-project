del program.exe
nasm -f win64 asmMCO2.asm -o asmMCO2.obj
gcc -c MCO2.c -o c_MCO2.obj -m64 -std=c99
gcc c_MCO2.obj asmMCO2.obj -o program.exe -m64
program.exe
pause