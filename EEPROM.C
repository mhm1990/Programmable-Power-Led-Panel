#include "EEPROM.h"
// #include ""

//-------------------------------------------------------------------------------------------------
// EEPROM_write
//-------------------------------------------------------------------------------------------------
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEWE))
;
/* Set up address and data registers */
EEAR = uiAddress;
EEDR = ucData;
/* Write logical one to EEMWE */
EECR |= (1<<EEMWE);
/* Start eeprom write by setting EEWE */
EECR |= (1<<EEWE);
}
//-------------------------------------------------------------------------------------------------
// EEPROM_read
//-------------------------------------------------------------------------------------------------
unsigned char EEPROM_read(unsigned int uiAddress)
{
/* Wait for completion of previous write */
while(EECR & (1<<EEWE))
;
/* Set up address register */
EEAR = uiAddress;
/* Start eeprom read by writing EERE */
EECR |= (1<<EERE);
/* Return data from data register */
return EEDR;
}

void add_schedule_d(unsigned int addrestostore)
{
	EEPROM_write(addrestostore,inten);
}

void add_schedule_m(unsigned int addrestostore)
{
	
	EEPROM_write(addrestostore++,frm_hour);
	EEPROM_write(addrestostore++,frm_minute);
	EEPROM_write(addrestostore++,to_hour);
	EEPROM_write(addrestostore,to_minute);
	
}

void read_schedule_d(unsigned int startBlock)
{	
	intensity=EEPROM_read(startBlock);
}

void read_schedule_m(unsigned int startBlock)
{	
	from_hour=EEPROM_read(startBlock++);    				
	from_minute=EEPROM_read(startBlock++);				
	to_hr=EEPROM_read(startBlock++);    				
	to_min=EEPROM_read(startBlock++);	
}

