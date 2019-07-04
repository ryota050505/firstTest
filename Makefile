NAME	= MyFirstTestGit
IO_FILE = gba_io1

all: $(NAME).mb

crt.o: crt.S
	arm-thumb-elf-as -o crt.o crt.S

$(NAME).s: $(NAME).c gba1.h
	arm-thumb-elf-gcc -S $(NAME).c

$(IO_FILE).s: $(IO_FILE).c gba1.h
	arm-thumb-elf-gcc -S $(IO_FILE).c

$(NAME).o: $(NAME).s
	arm-thumb-elf-as -o $(NAME).o $(NAME).s

$(IO_FILE).o: $(IO_FILE).s
	arm-thumb-elf-as -o $(IO_FILE).o $(IO_FILE).s

$(NAME).out: $(NAME).o $(IO_FILE).o crt.o
	arm-thumb-elf-ld --oformat elf32-littlearm -o $(NAME).out \
	-T display.ls  crt.o $(NAME).o $(IO_FILE).o

$(NAME).mb: $(NAME).out
	arm-thumb-elf-objcopy -O binary $(NAME).out $(NAME).mb

clean:
	rm -rf $(NAME).o $(NAME).s $(NAME).out $(IO_FILE).o $(IO_FILE).s \
	$(NAME).mb $(NAME).BAK crt.o
