
//**************************************************
// ***** HEADER FILE : touch.h ******
//**************************************************
#ifndef _EEPROM_H_
#define _EEPROM_H_

void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);

void read_schedule_m(unsigned int startBlock);
void read_schedule_d(unsigned int startBlock);
void add_schedule_d(unsigned int addrestostore);
void add_schedule_m(unsigned int addrestostore);
#endif
