#include "ps4.h"
#define SPOOF_FW 0x505003100
                 
////Credits to zerofo
//https://github.com/zerofo/ps4-sdk-spoofer

///////* examples *\\\\\\\\

//Thanks to psdevwiki 
//https://www.psdevwiki.com/ps4/SCEI_PS4_SDK

//PS4 FW 0x8008041000 (8.00)
//PS4 FW 0x6720001000 (6.72)
//PS4 FW 0x7020001000 (7.02)
//PS4 FW 0x4740001000 (4.74)
//PS4 FW 0x4550.01100 (4.55)
//PS4 FW 0x4050001000 (4.05)
//PS4 FW 0x5050031000 (5.05)
//PS4 FW 0x9600079000 (9.60)
//PS4 FW 0x1760001000 (1.76)
//PS4 FW 0x3550001000 (3.55)

int _main(struct) thread *td) {
    UNUSED(td);
	
	initkernel();
	initlibc();
	
	jailbreak();
	sdk_spoofer(SPOOF_FW);
	
	initsysutil();
	
	printf_notification("firmware spoofed to 5.05 by zerofo,n00d");
	
	
	return 0;
	
}
