/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/


#include "kbd.h"

#include <Windows.h>
#include <stdio.h>

/*
Nasledujici rutina zaridi, ze se program korektne ukonci i tehdy,
stiskne-li uzivatel Ctr+C nebo mysi prislusne tlacika okna, nebo chce vypnout pocitac, atd.
*/
HANDLE KbdQuitFlag;

BOOL WINAPI CtrlHandlerRoutine(DWORD dwCtrlType) {
	SetEvent(KbdQuitFlag);
	return TRUE;
}


void kivos_InitKbd() {
	KbdQuitFlag = CreateEvent(NULL, TRUE, FALSE, NULL);

	//FlushConsoleInputBuffer(stdcin);
	SetConsoleCtrlHandler(CtrlHandlerRoutine, TRUE);
}

void kivos_DoneKbd() {
	CloseHandle(KbdQuitFlag);
}


int kivos_getch() {
	if (WaitForSingleObject(KbdQuitFlag, 0) == WAIT_OBJECT_0) return EOF;

	/*
	Budeme cekat na jednu ze dvou udalosti:
		1. Uzivatel stisknul klavesu
		2. Anebo jsme dostali nejaky ridici-signal - viz CtrlHandlerRoutine
	*/
	HANDLE stdcin = GetStdHandle(STD_INPUT_HANDLE);

	HANDLE handles[2] = { stdcin, KbdQuitFlag };

	while (1) 

		switch (WaitForMultipleObjects(2, handles, FALSE, INFINITE)) {
			case WAIT_OBJECT_0 + 0: {
										INPUT_RECORD irInBuf;
										DWORD cNumRead;
										if (ReadConsoleInput(
											stdcin,      // input buffer handle 
											&irInBuf,     // buffer to read into 
											1,         // size of read buffer 
											&cNumRead)) {

											if (irInBuf.EventType == KEY_EVENT)
												return irInBuf.Event.KeyEvent.uChar.AsciiChar;											
										}
									}
									break;

			case WAIT_OBJECT_0 + 1: return EOF;
									break;
		}
}