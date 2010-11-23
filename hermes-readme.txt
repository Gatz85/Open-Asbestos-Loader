This code is adapted to launch using syscall 8 from my psgroove payload variant.

This code is experimental and is intended for those who have a 16KB USB dongle and no possibility to launch asbestOS in other form 

You can get an experimental compiled kernel from http://marcansoft.com/transf/dtbImage-20101020.bin or build one (see marcan README)

How it works:

At first, you need to read asbestos.bin and kernel code (i.e from one application. I include a prototype- example in "asbestos_loader" directory 
to load the files from USB devices)

asbestos.bin is allocated in 80000000007FD000 address (LV2 area) (my payload is loaded at 80000000007FF000, remember you)

To work, you must test if syscall 8 is working with this function:

if(sys8_enable(0ULL)<0) goto error;

You can copy asbestos.bin to LV2 so:

// copy asbestOS
	sys8_memcpy(0x80000000007FD000ULL, (uint64_t) asbestos_mem, (uint64_t) asbestos_len);

And run it passing the allocated kernel memry and the size:

// run asbestOS
	sys8_call(0x80000000007FD000ULL, (uint64_t) kernel_len, (uint64_t) kernel_mem);

Internally it disables interrupt for PPE thread 0, patch the decrementer interrupt (0x800000000000900) and waits to capture thread 1 in an internal loop 
mean this interrupt (_thread1_wait in start.S). Thread 1 changes the mode from virtual to real mode, disable interrupts and waits a signal from thread 0
(_thread1_release!=0)

In LV2 virtual address starts at 0x8000000000000000 but in real mode it starts at 0

From thread 0, the Linux kernel is moved from 'kernel_mem' to 0x800000000010000 (DEST_H<<16 in start.S) because you must enter in real mode and you loses
the virtual address focus. In theory, you have 0x7FD000-0x10000= 0x7ED000, 8310784 bytes for Linux kernel (remember you asbestos.bin is allocated in
0x7ED000.)

When Linux kernel is copied it proceed to change thread 0 to the real mode, call lv2_cleanup(), move the Linux kernel from 0x10000 (DEST_H<<16) to 0x0 
using kload() releases thread 1 (it go to 0x100 address, reading _thread1_vector value) and thread 0 go to the same address, 0x100

Remember you this code is experimental and may be, it has some mistakes: I have done what i could ;)

NOTE: Patch for decrementer Interrupt is based in 3.41 firmware (start.S line 35), but surely it works in all


