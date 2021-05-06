# SYS86 project
# Global Makefile

CC = ia16-elf-gcc
# ES only for segment please!
CFLAGS = -Os -ffreestanding -mcmodel=tiny -mprotected-mode -Wall

AS = ia16-elf-as

LD = ia16-elf-ld
LDFLAGS = -T raw.ld

EXE = sys86.bin

OBJS= \
	entry.o \
	lib.o \
	arch.o \
	task.o \
	int-dev.o \
	int-arch.o \
	int-proc.o \
	timer.o \
	queue.o \
	serial.o \
	system.o \
	main.o \
	trace-far.o \
	# end of list

EXE2 = stub.bin

OBJS2 = \
	stub.o
	# end of list

.PHONY : all clean test

all: $(EXE) $(EXE2)

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -M -o $(EXE) $(OBJS) > sys86.map

$(EXE2): $(OBJS2)
	$(LD) $(LDFLAGS) -M -o $(EXE2) $(OBJS2) > stub.map

clean:
	rm -f $(EXE) $(EXE2) *.o sys86.map stub.map test-emu-out.txt

test: test-emu

test-emu:
	../emu86/emu86 -w 0x10000 -f sys86.bin -x 0x1000:0x0 -w 0x20000 -f stub.bin &
	sleep 1
	../emu86/pcat -t 1000 -d 1 `cat emu86.pts` < test-sys86-in.txt > test-emu-out.txt
	diff test-sys86-ref.txt test-emu-out.txt

test-sbc:
	stty /dev/ttyUSB0 -echo
	./test-mon86-write 1000 0 ../sys86/sys86.bin
	./test-mon86-write 2000 0 ../sys86/stub.bin
	sudo ../emu86/pcat /dev/ttyUSB0
