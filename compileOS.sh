dd if=/dev/zero of=diskc.img bs=512 count=1000
dd if=bootload of=diskc.img conv=notrunc

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

bcc -ansi -c -o shell_c.o shell.c
as86 -o userlib.o userlib.asm
ld86 -o shellz -d shell_c.o  userlib.o

bcc -ansi -c -o tstpr1.o tstpr1.c
ld86 -d -o tstpr1 tstpr1.o userlib.o

bcc -ansi -c -o tstpr2.o tstpr2.c
ld86 -d -o tstpr2 tstpr2.o userlib.o

bcc -ansi -c -o number.o number.c
ld86 -d -o number number.o userlib.o

bcc -ansi -c -o letter.o letter.c
ld86 -d -o letter letter.o userlib.o


./loadFile kernel
./loadFile shellz
./loadFile tstpr1
./loadFile tstpr2
./loadFile message.txt
./loadFile number
./loadFile letter
