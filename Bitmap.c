//************************************************************************
// Bitmap.C
//************************************************************************
#include <avr/pgmspace.h>
#include "Bitmap.h"
#include "sed1335.h"
//##########################################################
#define GLCD_FONT_WIDTH 16
/*
void LoadBitmap(unsigned char *bitmap,int K,int L,int M,int N)
{
uint16_t i, j,by;
for(i=N; i<L; i+=8)				//height,row
		GLCD_WriteCommand(SED1335_MWRITE);
		for(j=M; j<K; j++)		//width,colum
		{
			by=pgm_read_byte(bitmap++);
			GLCD_GraphicGoTo(j, i);
			GLCD_WriteData(by);
		}

}
*/

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void LoadBitmap(unsigned char * bitmap, int x, int y,int width, int height)
{
unsigned char i, j;

	for(j = 0; j < height; j++)
	{
		GLCD_GraphicGoTo(x, y + j);
		GLCD_WriteCommand(SED1335_MWRITE);
		for(i = 0; i < width/GLCD_FONT_WIDTH; i++)
		  {
		  	GLCD_WriteData(GLCD_ReadByteFromROMMemory(bitmap + i + ((width/GLCD_FONT_WIDTH) * j))); 	
		  }
	}
}

/*
void LoadBitmap(unsigned char * bitmap, int x, int y,int width, int height)
{
unsigned char i, j;

	for(j = 0; j < height; j++)
	{
		GLCD_GraphicGoTo(x, y + j);
		for(i = 0; i < width/GLCD_FONT_WIDTH; i++)
		  {
		  	GLCD_WriteDisplayData(pgm_read_byte(bitmap + i + (GLCD_GRAPHIC_AREA * j))); 	
		  }
	}
}

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
