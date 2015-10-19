/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/


#pragma once

#include <stdlib.h>

#define GrayTextBlackBground 7
#define RedTextGrayBground 132
#define WhiteTextBlueBground 31

#define kivos_B800width  80
#define kivos_B800height 25
#define kivos_B800BufferSize (2 * kivos_B800width * kivos_B800height)
extern unsigned char kivos_B800[kivos_B800BufferSize];
	/*
	  kivos_B800 je obdoba pameti pristupne na adrese B800:0000
	  v realnem rezimu procesoru IA32
	 */
	

void kivos_InitVGA();
void kivos_DoneVGA();
	/*
	  Zahajeni a ukonceni prace s knihovnou pro graficky/textovy vystup.
	  Respektive nastavi pozadovany rozmer konzole a odstartuje/ukonci jeji prekreslovani z B800.
	*/

void kivos_vgatextout(size_t x, size_t y, char *str, short int attr);
	/*
		Obdoba volani BIOSu pres int 10h, sluzby 13h.
		Ve skutecnosti provede zapis do pameti B800.
	*/