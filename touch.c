#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "touch.h"
#include "sed1335.h"

#define SED1335_MWRITE 0x42


//-------------------------------------------------------------------------------------------------------
//   Initialise touch screen
//-------------------------------------------------------------------------------------------------------
void initialise_touch()
{
		DDRF &=~(_BV(TOUCH_X_PO)|_BV(TOUCH_Y_PO)|_BV(TOUCH_X_NE)|_BV(TOUCH_Y_NE));
}
//-------------------------------------------------------------------------------------------------------
//   InitADC
//-------------------------------------------------------------------------------------------------------
void InitADC()
	{	
	ADMUX=((1<<REFS0)|(1<<REFS1));										// For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(7<<ADPS0);
	}
//-------------------------------------------------------------------------------------------------------
//  ReadADC 
//-------------------------------------------------------------------------------------------------------
uint16_t ReadADC(uint8_t ch)
	{
   	uint16_t idelay;													//Select ADC Channel ch must be 0-7
   	ch=ch&0b00000111;
   	ADMUX|=ch;
   	ADCSRA|=(1<<ADSC);													//Start Single conversion
    while(!(ADCSRA & (1<<ADIF)));										//Wait for conversion to complete
    ADCSRA|=(1<<ADIF);													//Clear ADIF by writing one to it
	idelay = ADCL;
	idelay += (ADCH << 8);
    return(idelay);
	}

//-------------------------------------------------------------------------------------------------------
//   read_y_cord
//-------------------------------------------------------------------------------------------------------
void read_y_cord()
	{
	InitADC();
	_delay_ms(1);
	DDRF|=(_BV(TOUCH_Y_PO)|_BV(TOUCH_Y_NE));
	_delay_ms(1);
	PORTF|=_BV(TOUCH_Y_NE);
	PORTF&=~_BV(TOUCH_Y_PO);
	DDRF&=~(_BV(TOUCH_X_PO)|_BV(TOUCH_X_NE));
	_delay_ms(1);
	
	RH1=ReadADC(1);
	ADCSRA&=~(1<<ADEN);

	GLCD_TextGoTo(0,28);
	GLCD_WriteString("RH1 : ");

	display_int(6,28,RH1,4);
//	lcd_int_display(20,28,RH1);
	}
//-------------------------------------------------------------------------------------------------------
//   read_x_cord
//-------------------------------------------------------------------------------------------------------
void read_x_cord()
	{
	InitADC();
	_delay_ms(1);
	DDRF|=(_BV(TOUCH_X_PO)|_BV(TOUCH_X_NE));
	_delay_ms(1);
	PORTF|=_BV(TOUCH_X_PO);
	PORTF&=~_BV(TOUCH_X_NE);
	DDRF&=~(_BV(TOUCH_Y_PO)|_BV(TOUCH_Y_NE));
	_delay_ms(1);
	
	RH2=ReadADC(3);
	ADCSRA&=~(1<<ADEN);
	GLCD_TextGoTo(0,27);
	GLCD_WriteString("RH2 : ");
	display_int(6,27,RH2,4);
//	lcd_int_display(25,28,RH2);
	}
//-------------------------------------------------------------------------------------------------------
//   touch_value
//-------------------------------------------------------------------------------------------------------
void touch_value()
	{
	RH1=1023;
	RH2=1023;
	read_x_cord();
	_delay_ms(10);
	
	read_y_cord();
	_delay_ms(10);
	}
