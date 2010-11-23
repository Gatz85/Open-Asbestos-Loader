/*  main.cpp - AsbestOS loader

Copyright (C) 2010  MrGatz85 <gatz85@gmail.com
Copyright (C) 2010  Francisco Muñoz "hermes" <www.elotrolado.net>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

And i want to say thanks to Ceal,Marcan32,KaKaRoToKS,Hermes,ooPo,AerialX,Mathieulh,Aaron (the guy who started PSL1GHT)
and who ever i missed. with out any of yall i wont have been able to do what i do. 
Thats have fun and use my toys the way i want

*/

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/timer.h>
#include <lv2/process.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "netdebug.h"
#include "stage2.bin.h"
#define SYSCALL_PEEK		6
#define SYSCALL_POKE		7
#define SYSCALL_TRASH 		11

// lv2 retail 3.41
#define LV2_MEMCPY		0x800000000007C01C
#define ASBESTOS_BASE		0x80000000007FD000



//Standerd peek/poke calls
static void poke(u64 address, u64 value);
static u64 peek(u64 address);





int main(int argc, const char* argv[])
{
  
	

	//File Vars
	FILE *fp=NULL;
	int kernel_len;
	char *kernel_mem= NULL;

	//Lets define our function
	u64 opd = peek(LV2_SYSCALL_TABLE + 8 * SYSCALL_TRASH);
	

	//Ok this Starts the Debug over TCP run 'nc -l 4000' set your ip in netdebug.h
	int s;
	s = tcp_debug();		
	
	//had this here to test debug quickly	
	//if (s < 0)
	//goto error;



//We are now loading the kernel from a file off USB first then HDD 
//if both fail we goto error, tried to load the kernel into a header file it was 30 meg 
//right size (i think )but was much larger then i wanted to have in the program

printX("trying to open the dtbImage.ps3.bin from USB!\n", s);	
	fp = fopen("/dev_usb/dtbImage.ps3.bin", "r");
		if(!fp) {
			printX("failed trying to open the dtbImage.ps3.bin from USB!\n", s);
			printX("trying to open the dtbImage.ps3.bin from HDD!\n", s);
				fp = fopen("/dev_hdd0/game/LOAD00000/USRDIR/dtbImage.ps3.bin", "r");
			if(!fp)
				{
				printX("failed trying to open the dtbImage.ps3.bin from HDD ERROR!\n", s);
				goto error;
				}
		}
printX("dtbImage.ps3.bin Loaded from a drive!\n", s);
	fseek(fp, 0, SEEK_END);
	kernel_len= ftell(fp);
	printX("trying to malloc dtbImage.ps3.bin!\n", s);
	kernel_mem= (char *) malloc(kernel_len+32);
	if(!kernel_mem) 
		{
		printX("failed trying to malloc dtbImage.ps3.bin ERROR!\n", s);
		goto error;
		}

	fseek(fp, 0, SEEK_SET);
	if((int) fread((void *) kernel_mem, 1, kernel_len, fp) != kernel_len) 
		{
		printX("dtbImage.ps3.bin in memory does not match file ERROR!\n", s);
		 goto error;		
		}
	fclose(fp);
	printX("dtbImage.ps3.bin Loaded all good!\n", s);
	//OK bins loaded and ready for useage!!

	//Take a break
	lv2TimerSleep(1);


	printX("Starting to copying Stage2 to memory!\n", s);
	printX("copying stage2 to memory out of header!\n", s);
		//here here poke memcpy to our function to copy stage2 into memory
		poke(opd, LV2_MEMCPY);
		Lv2Syscall3(SYSCALL_TRASH, ASBESTOS_BASE, (u64)stage2_bin, sizeof(stage2_bin));
	printX("Starting to Run AsbestOS!\n", s);
	printX("Calling Stage2 Good Luck!\n", s);
		poke(opd, ASBESTOS_BASE);
		Lv2Syscall2(SYSCALL_TRASH, (u64)kernel_len, (u64)kernel_mem);
			

error:
//If you dont know by now you should not program
	printX("And Error happen!!!\n", s);
	printX("Debug Now shutting Down!!!\n", s);
	printX("We had a ERROR sorry try again!\n", s);
	tcp_shutdown(s);

	if(kernel_mem) free(kernel_mem);
	
	if(fp) fclose(fp);



}
//SysCalls
static void poke(u64 address, u64 value)
{
	Lv2Syscall2(SYSCALL_POKE, address, value);
}


 
static u64 peek(u64 address)
{
	return Lv2Syscall1(SYSCALL_PEEK, address);
}



