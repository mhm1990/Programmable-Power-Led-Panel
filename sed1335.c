/*
 * Author:         Made by Parmar
 * known Problems: none
 * Version:        1.1
 * Description:    Graphic Library for sed1335- (and compatible) based LCDs
 * File name:      sed1335.c
 * 
 */

//-------------------------------------------------------------------------------------------------------
#include "sed1335.h"
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//
// Delay function
//
//-------------------------------------------------------------------------------------------------------
void delay(void)
{
volatile unsigned char i;
for(i = 0; i < (F_CPU/1000000); i++)
  {
  asm("nop");
  }
}

//-------------------------------------------------------------------------------------------------------
//   GLCD_ClearText
//-------------------------------------------------------------------------------------------------------
void GLCD_ClearText(uint16_t x,uint16_t y,uint16_t digit)
	{
	int i;
	GLCD_TextGoTo(x,y);
	GLCD_WriteCommand(SED1335_MWRITE);
	for(i = 0; i < digit; i++)
	GLCD_WriteData(' ');
	}


//-------------------------------------------------------------------------------------------------------
//
// Ports intalization
//
//-------------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{
	SED1335_DATA_DIR = 0xFF;
	SED1335_CONTROL_PORT |= (SED1335_A0 | SED1335_WR | SED1335_RD | SED1335_CS | SED1335_RES); 
	SED1335_CONTROL_DIR |= (SED1335_A0 | SED1335_WR | SED1335_RD | SED1335_CS | SED1335_RES); 
}
//-------------------------------------------------------------------------------------------------------
//
// Display initalization
//
//-------------------------------------------------------------------------------------------------------
void GLCD_Initialize(void)
{
GLCD_InitializePorts();

GLCD_WriteCommand(SED1335_SYSTEM_SET); 
GLCD_WriteData(SED1335_SYS_P1);	
GLCD_WriteData(SED1335_SYS_P2);		
GLCD_WriteData(SED1335_FY);		
GLCD_WriteData(SED1335_CR);		
GLCD_WriteData(SED1335_TCR);	
GLCD_WriteData(SED1335_LF);		
GLCD_WriteData(SED1335_APL);	
GLCD_WriteData(SED1335_APH);	
	
GLCD_WriteCommand(SED1335_SCROLL);   
GLCD_WriteData(SED1335_SAD1L);		
GLCD_WriteData(SED1335_SAD1H);		
GLCD_WriteData(SED1335_SL1);		
GLCD_WriteData(SED1335_SAD2L);		
GLCD_WriteData(SED1335_SAD2H);		
GLCD_WriteData(SED1335_SL2);		
GLCD_WriteData(SED1335_SAD3L);		
GLCD_WriteData(SED1335_SAD3H); 		
GLCD_WriteData(SED1335_SAD4L);		
GLCD_WriteData(SED1335_SAD4H);		
	
GLCD_WriteCommand(SED1335_CSRFORM);
GLCD_WriteData(SED1335_CRX);		
GLCD_WriteData(SED1335_CSRF_P2);		
	
GLCD_WriteCommand(SED1335_CGRAM_ADR);       
GLCD_WriteData(SED1335_SAGL);			
GLCD_WriteData(SED1335_SAGH);				
	
GLCD_WriteCommand(SED1335_CSRDIR_R);      

GLCD_WriteCommand(SED1335_HDOT_SCR);       
GLCD_WriteData(SED1335_SCRD);			
	
GLCD_WriteCommand(SED1335_OVLAY);            
GLCD_WriteData(SED1335_OVLAY_P1);			
			
GLCD_WriteCommand(SED1335_DISP_ON);
GLCD_WriteData(SED1335_FLASH);
}

//-------------------------------------------------------------------------------------------------------
//
// Writes null-terminated string from program memory to display RAM memory
//
//-------------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(const uint8_t * ptr)
{
	return pgm_read_byte(ptr);
}

//-------------------------------------------------------------------------------------------------------
//
// Set (if color==1) or clear (if color==0) pixel on screen
//
//-------------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned int x,unsigned int y, int color)
{
unsigned char tmp = 0;
unsigned int address = SED1335_GRAPHICSTART + (40 * y) + (x/8); 
GLCD_SetCursorAddress(address);

GLCD_WriteCommand(SED1335_MREAD);
tmp = GLCD_ReadData();

if(color)
  tmp |= (1 << (SED1335_FX - (x % 8)));
else
  tmp &= ~(1 << (SED1335_FX - (x % 8)));

GLCD_SetCursorAddress(address);
GLCD_WriteCommand(SED1335_MWRITE);
GLCD_WriteData(tmp);
}
//-------------------------------------------------------------------------------------------------------
//
// Writes null-terminated string to display RAM memory
//
//-------------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * tekst)
{
GLCD_WriteCommand(SED1335_MWRITE);
while(*tekst)
	GLCD_WriteData(*tekst++);
}



//-------------------------------------------------------------------------------------------------------
//
// Writes null-terminated string from program memory to display RAM memory
//
//-------------------------------------------------------------------------------------------------------
/*
void GLCD_WriteTextP(char * tekst)
{
GLCD_WriteCommand(SED1335_MWRITE);
while(GLCD_ReadByteFromROMMemory(tekst))
	GLCD_WriteData(GLCD_ReadByteFromROMMemory(tekst++));
}
*/
void GLCD_WriteTextP(PGM_P str)
{
	GLCD_WriteCommand(SED1335_MWRITE);
	while(pgm_read_byte(str) != 0)
	{ 
		GLCD_WriteData(pgm_read_byte(str));
		str++;
	}
}

//-------------------------------------------------------------------------------------------------------
//
//  set cursor address
//
//-------------------------------------------------------------------------------------------------------
void GLCD_SetCursorAddress(unsigned int address)
{
GLCD_WriteCommand(SED1335_CSRW);
GLCD_WriteData((unsigned char)(address & 0xFF));
GLCD_WriteData((unsigned char)(address >> 8));
}

//-------------------------------------------------------------------------------------------------------
//
// Sets display coordinates
//
//-------------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
GLCD_SetCursorAddress((y * 40) + x);
}
//-------------------------------------------------------------------------------------------------------
//
// Sets graphics coordinates
//
//-------------------------------------------------------------------------------------------------------
void GLCD_GraphicGoTo(unsigned int x, unsigned int y)
{
GLCD_SetCursorAddress(SED1335_GRAPHICSTART + (y * 40) + x/8);
}
//-------------------------------------------------------------------------------------------------------
//
// Clears text area of display RAM memory
//
//-------------------------------------------------------------------------------------------------------
void GLCD_ClearText1(void)
{
int i;
GLCD_TextGoTo(0,0);
GLCD_WriteCommand(SED1335_MWRITE);
for(i = 0; i < 1200; i++)
	GLCD_WriteData(' ');
}

//-------------------------------------------------------------------------------------------------------
//
// Clears graphics area of display RAM memory
//
//-------------------------------------------------------------------------------------------------------

void GLCD_ClearGraphic1(void)
{
unsigned int i;
GLCD_SetCursorAddress(SED1335_GRAPHICSTART);
GLCD_WriteCommand(SED1335_MWRITE);
for(i = 0; i < (40 * 240); i++)
	GLCD_WriteData(0x00);
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
/*
void GLCD_Bitmap(char * bmp, int x, int y, int width, int height)
{
unsigned int i, j;
for(i = 0; i < height ; i++)
	{
	GLCD_GraphicGoTo(x, y+i);
	GLCD_WriteCommand(SED1335_MWRITE);
	for(j = 0; j < width/8; j++)
		GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp+j+(40*i)));
	}
}
*/
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

