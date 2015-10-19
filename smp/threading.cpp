/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/


#include "threading.h"
#include <process.h>
#include <Windows.h>

/*
   TODO

   Neni udelane pridavani vlaken za behu, tj. zbavit se funkce kivos_RunThreading() a take chybi
   udelat funkci kivos_ExitThread(). Obe funkce by vyzadovaly dalsi event HANDLE Reschedule, ktery
   by vzbudil interni smycku ClockThread, aby bylo mozne naplanovat jine vlakno.  Z toho vyplyva,
   ze TCB by musel obsahovat informaci o stavu vlakna. kivos_ExitThread by tedy tento stav nastavila
   na Terminated, signalizovala Reschedule event a zahajila nekonecnou smycku volani Sleep(0).
   Alternativne by mohla zavolat SuspendThread s tim, ze CLockThread by ji po WaitForMultipleObjects
   uz nevolala v pripade signalizovane udalosti Reschedule.
*/

/*

  Prepinani threadu - lze zavolat GetThreadContext nad vlastnim, bezicim OS threadem, ale vysledek uz
  nebude odpovidat stavu v dobe volani. A zavolat SetThreadContext nad vlastnim, bezicim OS threadem
  nelze - doslo by k nedefinovanemu chovani. Proto je nasledujici kod zjednodusen tim, ze v ClockThread
  dochazi rovnou k planovani a prepinani vlaken. Je to jednodussi, nez emulovat skutecno obsluhu 
  preruseni.  

  Nicmene, emulace skutecne obsluhy preruseni je o dost zajimavajesi a vic cloveka nauci. Nejprve bychom
  definovali tabulku vektoru preruseni - tj. 256 adres funkci obsluh preruseni. Jakmile by doslo k pozadovane
  udalosti, urcilo by se cislo sw preruseni, tj. konkretni obsluhy. Pak bychom udelali SuspendThread(CPUThread)
  a na vrchol zasobniku dali aktualni ip registr. Nasledne bychom ip registr CPUThreadu nastavili na adresu
  ziskanou z tabulky vektoru preruseni a znovu spustili CPUThread. Ten by tak zacal vykonavat obsluhu preruseni.
  Ovsem pozor, stejne jako v realu by nesmel znicit obsah registru volaneho programu. Timto jsme se dostali 
  do bodu, kdy obsluha preruseni uz musi byt napsana v assembleru. Takze proc uz potom neukladat i flags
  a nevracet se z obsluhy instrukci iret?

*/


//#define allocheap	//Pokud je definovano, pak se stack kazdeho naseho vlakna alokuje pres heap, jinak pres malloc

const DWORD ThreadTimeQuantum = 52;

typedef struct {

	CONTEXT OSContext;	//de-facto jenom registry
	HANDLE StackHandle;
	void* Stack;
	/*
	zde by byly nase vlastni veci jako id, priorita, atd.
	*/

	void *param;
	void *id;
} TThreadControlBlock;

HANDLE CPUCore;									/*Handle vlakna simulujiciho jadro procesoru*/
HANDLE QuitFlag;								/*Vlajka, ze koncime*/
size_t CurrentThreadID;							/* Id to Threads*/
size_t ThreadCount = 0;							/*Pocet naseh trahdu*/
TThreadControlBlock Threads[kivos_MaxThreads];	/*nase thready*/
CONTEXT DefaultContext;							/*vychozi hodnoty registru pro kazdy novy thread*/

DWORD WINAPI ClockThread(void* param) {	
	bool doQuit;
	do {
		doQuit = WaitForSingleObject(QuitFlag, ThreadTimeQuantum) == WAIT_OBJECT_0;			
		SuspendThread(CPUCore);

		if (!doQuit) {

			GetThreadContext(CPUCore, &Threads[CurrentThreadID].OSContext);

			CurrentThreadID++;
			if (CurrentThreadID >= ThreadCount) CurrentThreadID = 0;

			SetThreadContext(CPUCore, &Threads[CurrentThreadID].OSContext);			
		}
		else {		
			SetThreadContext(CPUCore, &DefaultContext);
		}	
	
		ResumeThread(CPUCore);		
	} while (!doQuit);

	_endthreadex(0);
	return 0;
}

DWORD WINAPI CPUCoreThread(void* param) {
	_endthreadex(0);
	return 0;
}


#define ThreadStackSize 1024*1024


HANDLE Clock;

/*
  Vytvori novy OS thread, v jeho kontextu se budou stridat nase thready. Tj. vytvori ho pozastaveny
  a ulozi si hodnoty jeho registru jako vychozi - DefaultContext.
*/

void kivos_InitThreading() {
	memset(&DefaultContext, 0, sizeof(DefaultContext));
	CPUCore = (HANDLE)_beginthreadex(NULL, 0, &CPUCoreThread, 0, CREATE_SUSPENDED, NULL);
	
	DefaultContext.ContextFlags = CONTEXT_ALL;
	GetThreadContext(CPUCore, &DefaultContext);	

	QuitFlag = CreateEvent(NULL, TRUE, FALSE, NULL);
}


/*
  Pokud mame misto v tabulce TCB, inicializujeme pro nas novy thread. Tj. 
    1. prekopirujeme vychozi hodnoty registru
	2. Alokujeme zasobnik a do
		2.1 (AMD64/EMT64) do RSP dame jeho vrchol, ale bez 32B ktere rezervujeme pro volani __fastcall a 8B navratove adresy
			parameter se predava v registru RCX, volaci konvence je __fastcall protoze #define WINAPI 
		2.2 (IA32) do ESP dame jeho vrchol, ale bez 4B pro nas parametr a 4B navratove adresy
			parametr se predava zasobnikem, volaci konvence #define WINAPI __stdcall
	3. Do RIP/EIP dame adresu funkce naseho threadu, tj. jeho prvni instrukce
	4. Zapamatujeme si dalsi informace v TCB naseho threadu
	5. A jsme hotovi, tj. oznacime inicializovane TCB za pouzite
*/

bool kivos_AddThreadingProc(LPTHREAD_START_ROUTINE proc, LPVOID lpThreadParameter) {
	if (ThreadCount >= kivos_MaxThreads) return false;

	Threads[ThreadCount].OSContext = DefaultContext;

#ifdef allocheap
	Threads[ThreadCount].StackHandle = HeapCreate(0, ThreadStackSize, 0);	
	Threads[ThreadCount].Stack = HeapAlloc(Threads[ThreadCount].StackHandle, HEAP_NO_SERIALIZE, ThreadStackSize);
#else
	Threads[ThreadCount].Stack = _aligned_malloc(ThreadStackSize, 64);
#endif

	/*
	  TODO

	  Toto je pouze ukazka, ve ktere zadny z nasich threadu neskonci! Vsechny bezi v nekonecne smycce,
	  a proto neni implementovana funkce kivos_ExitThread. Az bude implementovana, bude treba dat na vrchol 
	  zasobniku jeji adresu, aby az nas thread sam dobehne, "vratil se do ni" a tak se korektne ukoncil.
	
	*/


#ifdef _M_X64
	Threads[ThreadCount].OSContext.Rsp = (DWORD64) Threads[ThreadCount].Stack + ThreadStackSize - 32 - sizeof(void*);	//32B je pro __fastcall 
	Threads[ThreadCount].OSContext.Rip = (DWORD64)proc;

	//64-bitova aplikace si preda prvni celociselny paramter v rcx - __fastcall
	Threads[ThreadCount].OSContext.Rcx = (DWORD64)lpThreadParameter;	
#else
	Threads[ThreadCount].OSContext.Esp = (DWORD32)Threads[ThreadCount].Stack + ThreadStackSize - 2*sizeof(void*);	//sizeof(void*) je pro nas parametr
	Threads[ThreadCount].OSContext.Eip = (DWORD32)proc;	

	//32-bitova aplikace si predava parametry na zasobniku - volaci konvence __stdcall
	void* *tmp = (void*)Threads[ThreadCount].OSContext.Esp;
	tmp++;
	*tmp = lpThreadParameter;	
#endif
	//Nase obdoba TLS
	Threads[ThreadCount].param = lpThreadParameter;				
	Threads[ThreadCount].id = &Threads[ThreadCount];


	ThreadCount++;
	return true;
}

/*
  Spustime thread simulujici jadro procesoru a spustime jeho hodiny, tj. IRQ0 na int 01ch, jako vlakno Clock
*/
void kivos_RunThreading() {
	CurrentThreadID = 0;
	SetThreadContext(CPUCore, &Threads[CurrentThreadID].OSContext);
	ResumeThread(CPUCore);
	
	Clock = (HANDLE)_beginthreadex(NULL, 0, &ClockThread, (void*) 123, 0, NULL);
}

/*
	Signalizujeme konec, coz prerusi pripadne cekani vlakna simulujiciho hodiny. To po svem probuzeni
	a se signalizovanou QuitFlag nastavi takovy kontext vlakna simulujiciho jadro procesoru, ktere ho
	slusne ukonci. Pote se vlakno simulujiciho hodiny take samo ukonci. Nakonec provedeme uvolneni pameti.
*/

void kivos_EndThreading() {
	SetEvent(QuitFlag);

	
	HANDLE threads[2] = {Clock, CPUCore};
	WaitForMultipleObjects(2, threads, true, INFINITE);	

	for (size_t i = 0; i < ThreadCount; i++) {
#ifdef allocheap
		HeapFree(Threads[i].StackHandle, HEAP_NO_SERIALIZE, Threads[i].Stack);
		HeapDestroy(Threads[i].StackHandle);		
#else
		_aligned_free(Threads[i].Stack);
#endif
	}

	CloseHandle(QuitFlag);
}

/*
  Jenom cteme polozku z naseho TCB.
*/

void* kivos_GetCurrentThreadParam() {
	return Threads[CurrentThreadID].param;
}

/*
Jenom cteme polozku z naseho TCB.
*/

void* kivos_GetCurrentThreadId() {
	return Threads[CurrentThreadID].id;
}