*** WORK IN PROGRESS ***


WHAT IS THIS ?

This a highly experimental tiny 8086 operating system.

Features:
  - fully premptive multi-tasking
  - basic scheduler based on task priority
  - event, condition and wait primitives

Drivers:
  - timer
  - serial port with I/O queues

Application:
  - serial echo (for testing)


WHERE IS IT USED ?

Target system is the Advantech SNMP-1000 SBC:
  - R8810 microcontroller (Intel 80188 improved clone)
  - Primary clock @ 20 MHz
  - 512K RAM / 512K Flash
  - Serial port (no FIFO) & 3 timers
  - NE2K compatible Ethernet adapter
  - RTC chip

That target is used to test the code on real & slow HW.

Before burning the code into the target Flash, the code is sanity tested with the `emu86` emulator (built in `advtech` configuration).


HOW TO BUILD ?

Building MON86 requires an IA16 cross toolchain, that you can build with the
scripts from my 'build86' project.

After building the toolchain, just create a link to the 'build86/cross' folder,
then run the './build.sh' script.
