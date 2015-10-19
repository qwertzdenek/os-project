/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/


#include "vga.h"
#include <process.h>
#include <Windows.h>

unsigned char kivos_B800[kivos_B800BufferSize];

//const unsigned char * const kivos_endofB800 =kivos_ B800 + kivos_B800BufferSize;


/*
   Pouze pretransformujeme nas B800 do formatu, ktery lze pouzit s funkci WriteConsoleOuputA
   s niz ho zobrazime.
*/
void RefreshVGA() {
	COORD characterBufferSize = { kivos_B800width, kivos_B800height };
	COORD characterPosition = { 0, 0 };
	SMALL_RECT consoleWriteArea = { 0, 0, kivos_B800width - 1, kivos_B800height - 1 };

	CHAR_INFO buf[kivos_B800width * kivos_B800height];	

	for (size_t i = 0; i < kivos_B800width * kivos_B800height; i++) {
		buf[i].Char.AsciiChar = (char)kivos_B800[i * 2];
		buf[i].Attributes = (char)kivos_B800[i * 2 + 1];
	}
	WriteConsoleOutputA(GetStdHandle(STD_OUTPUT_HANDLE), buf, characterBufferSize, characterPosition, &consoleWriteArea);
}

HANDLE VGADAC;				//Handle vlakna, ktere nam simuluje cinnost VGA DAC
HANDLE VGAPowerOff;			//Signal pro ukonceni vlakna VGADAC


/*
	Dokud neobdrzime signal k ukonceni, s 25fps simulovany DAC prekresluje obrazovku.
*/
DWORD WINAPI VGADACThread(void* param) {
	do {
		RefreshVGA();		
	} while (WaitForSingleObject(VGAPowerOff, 40) != WAIT_OBJECT_0);
									//25 fps

	_endthreadex(0);

	return 0;
}



void kivos_InitVGA() {
	HANDLE hConOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	SMALL_RECT r;
	COORD c;
	hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConOut, &csbi);		

	r.Left = 0;
	r.Top = 0;
	r.Right = kivos_B800width - 1;
	r.Bottom = kivos_B800height - 1;
	SetConsoleWindowInfo(hConOut, TRUE, &r);

	c.X = kivos_B800width;
	c.Y = kivos_B800height;
	SetConsoleScreenBufferSize(hConOut, c);

	kivos_vgatextout(39, 0, "KIV/OS ukazka vlastniho planovace threadu", WhiteTextBlueBground);
	kivos_vgatextout(39, 1, "VGA/EGA/CGA BIOS mode 03h KIV/OS Emulator", WhiteTextBlueBground);

	VGAPowerOff = CreateEvent(NULL, TRUE, FALSE, NULL);
	VGADAC = (HANDLE)_beginthreadex(NULL, 0, &VGADACThread, 0, 0, NULL);
}

void kivos_DoneVGA() {
	SetEvent(VGAPowerOff);
	WaitForSingleObject(VGADAC, INFINITE);
	CloseHandle(VGAPowerOff);
}

void kivos_vgatextout(size_t x, size_t y, char *str, short int attr) {
	unsigned char *pos = kivos_B800 + x * 2 + y*kivos_B800width * 2;
	while (*str) {// & (pos<(kivos_B800+kivos_B800BufferSize))) {
		*pos = *str;
		str++;
		pos++;
		*pos = (unsigned char) attr;	//atribut
		pos++;		
	}
}