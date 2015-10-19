/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/


#pragma once

/*
	Inicializuje a ukonci praci s emulovanou klavesnici.
*/
void kivos_InitKbd();
void kivos_DoneKbd();

/*
  Blokujici, vraci ascii kod stisknute klavesy.
  Vraci EOF  pokud uz neni co cist a aplikace by tak mela skoncit.
*/
int kivos_getch();