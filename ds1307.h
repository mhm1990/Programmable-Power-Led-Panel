#ifndef DS1307_H
#define DS1307_H

#include <avr/io.h>

uint8_t DS1307Read(uint8_t address,uint8_t *data);
uint8_t DS1307Write(uint8_t address,uint8_t data);
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_writeSingleBlock (unsigned int addrestostore);
unsigned char EEPROM_readSingleBlock(unsigned int startBlock);
void add_schedule_d(unsigned int addrestostore);
void add_schedule_m(unsigned int addrestostore);
void read_schedule_d(unsigned int startBlock);
void read_schedule_m(unsigned int startBlock);


#endif
