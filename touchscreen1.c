#include <avr/io.h>
#define F_CPU 12000000UL
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include "sed1335.h"
#include "sed1335.c"
#include "Bitmap.h"
#include "Bitmap.c"
#include "I2C.h"
#include "I2C.c"
#include "ds1307.h"
#include "ds1307.c"
#include <avr/interrupt.h>

#include "touch.c"
#include "touch.h"

uint8_t to_hour=0,to_minute=0,frm_hour=0,frm_minute=0,from_hour=0,from_minute=0,to_hr=0,to_min=0,set_date_flag=0,sdk=0,d=0,m=0,y=0,inten=0;
unsigned int intensity=0,brightness=0;
int pos=0,month_flag=0,date_flag=0,day_flag=0,s_flag=0,frm_flag=0,to_flag=0,int_flag=0,t_flag=0,sch_flag=0,scrol=0,from_sign_flag=0,to_sign_flag=0;
int c[6],manual_mode=0,auto_mode=0,moon_flag=0,auto_run_flag=0,manual_run_flag=0,mun=0;


#include "EEPROM.c"
#include "EEPROM.h"

#define GLCD_FONT_WIDTH 16
#define Date_Starting_Address 50
#define Month_Starting_Address 0
#define Auto_Date_Address 150
#define Auto_Month_Address 100
#define Useless_Location 300
void invertBitmap(int x, int y,int width, int height);
void GLCD_WriteData(unsigned char dataToWrite);
void GLCD_WriteCommand(unsigned char commandToWrite);
unsigned char GLCD_ReadData(void);
void lcd_int(unsigned int var);
void display_int(uint16_t a,uint16_t b,uint16_t x,uint8_t y);
void GLCD_ClearGraphic(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
void lcd_int_display(unsigned int a,unsigned int b,uint16_t var);
void port_init(void);
void init_devices(void);
void display_date(void);
void display_time(void);
void display_inten(void);
void keypad_view(void);
void keypad1(void);
void date_wise(void);
void day_wise(void);
void month_wise(void);
void display_digit(void);
void InitPWM(void);
void SetPWMOutput(uint16_t duty);
//--


//-------------------------------------------------------------------------------------------------
// variables for Setting page
//-------------------------------------------------------------------------------------------------

// flags..
uint8_t reset_total_bundle_flag=0;

// variables..
unsigned int reset_total_bundle=0;

//-------------------------------------------------------------------------------------------------------
//Function	: To convert the unsigned long value of memory into 
//            text string and send to UART
//Arguments	: 1. unsigned char flag. If flag is HIGH, memory will be displayed in KBytes, else in Bytes. 
//			  2. unsigned long memory value
//return	: none
//-------------------------------------------------------------------------------------------------------
void f_displayMemory (float f_memory)
{
  char memoryString[] = "       "; //19 character long string for memory display
  unsigned char i;
  unsigned long int memory=0;

  memory = f_memory*100;
  for(i=7; i>0; i--) //converting freeMemory into ASCII string
  {
  	if(i==5) 
	{
	   memoryString[i-1] = '.';  
	   i--;
	}
    memoryString[i-1] = (memory % 10) | 0x30;
	memory /= 10;
	if(memory == 0) 
		break;
  }
  GLCD_WriteString(memoryString);
}

//-------------------------------------------------------------------------------------------------------
//   GLCD_WriteData 
//-------------------------------------------------------------------------------------------------------

void invertBitmap(int x, int y,int width, int height)
{
unsigned char i, j;
uint8_t data[40];

	for(j = 0; j < height; j++)
	{
		GLCD_GraphicGoTo(x, (y + j));
		for(i = 0; i < width/GLCD_FONT_WIDTH; i++)
		{
			GLCD_WriteCommand(SED1335_MREAD);
		  	data[i]=GLCD_ReadData();
			data[i] = ~data[i];
		}
		//GLCD_GraphicGoTo(x, y + j);
		for(i = 0; i < width/GLCD_FONT_WIDTH; i++)
		{
			GLCD_WriteCommand(SED1335_MWRITE);
			GLCD_WriteData(data[i]); 	
		}
	}
}


//-------------------------------------------------------------------------------------------------------
//   GLCD_WriteData 
//-------------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
	{
	SED1335_DATA_PORT = dataToWrite;
	SED1335_DATA_DIR = 0xFF;
	SED1335_CONTROL_PORT &= ~(SED1335_CS | SED1335_A0 | SED1335_WR);
	asm("nop");
	SED1335_CONTROL_PORT |= (SED1335_CS | SED1335_A0 | SED1335_WR);
	}

//-------------------------------------------------------------------------------------------------------
//   GLCD_WriteCommand
//-------------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite)
	{
	SED1335_DATA_PORT = commandToWrite;
	SED1335_DATA_DIR = 0xFF;
	SED1335_CONTROL_PORT &= ~(SED1335_CS |SED1335_WR);
	asm("nop");
	SED1335_CONTROL_PORT |= (SED1335_CS | SED1335_WR);
	}

//-------------------------------------------------------------------------------------------------------
//   GLCD_ReadData        
//-------------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
	{
	unsigned char tmp;
	SED1335_CONTROL_PORT &= ~(SED1335_CS | SED1335_RD);
	asm("nop");
	SED1335_DATA_DIR = 0x00;
	asm("nop");
	tmp =  SED1335_DATA_PIN;
	SED1335_CONTROL_PORT |= (SED1335_CS | SED1335_RD);
	return tmp;
	}


//-------------------------------------------------------------------------------------------------------
//   lcd_int
//-------------------------------------------------------------------------------------------------------
void lcd_int(unsigned int var)
	{
	char buffer1[7];
	itoa(var, buffer1, 10);												// CONVERT INTEGER INTO STRING 
    GLCD_WriteString(buffer1);											// PUT CONVERTED STRING TO DISPLAY 
	}

//-------------------------------------------------------------------------------------------------------
//   display_int
//-------------------------------------------------------------------------------------------------------
void display_int(uint16_t a,uint16_t b,uint16_t x,uint8_t y)
{
	GLCD_TextGoTo(a,b);
	uint8_t ca[10];
	int i=0,j=10;
	for(i=0;i<y;i++)
	{
		ca[i]=x%j;
		x=x/10;
	}
	for(i=y-1;i>=0;i--)
	{
		lcd_int(ca[i]);
	}
}

//-------------------------------------------------------------------------------------------------------
//   GLCD_ClearGraphic
//-------------------------------------------------------------------------------------------------------
void GLCD_ClearGraphic(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	for(int j = 0; j < height; j++)
	{
		GLCD_GraphicGoTo(x, y + j);
		GLCD_WriteCommand(SED1335_MWRITE);
		for(int i = 0; i < width/GLCD_FONT_WIDTH; i++)
		{
			GLCD_WriteData(0x00); 	
	  	}
	}
}

//-------------------------------------------------------------------------------------------------------
// lcd int display
//-------------------------------------------------------------------------------------------------------
void lcd_int_display(unsigned int a,unsigned int b,uint16_t var)
	{
	char buffer[7];
	itoa(var, buffer, 10);												// convert interger into string 
   	GLCD_TextGoTo(a,b);
	GLCD_WriteString(buffer);											// put converted string to display 
	}


void rtc_initialise(void)
{
	I2CInit();

	#define CH 7

	uint8_t temp;
	DS1307Read(0x00,&temp);

	//Clear CH Bit
	temp&=(~(1<<CH));

	DS1307Write(0x00,temp);

	//Set 24 Hour Mode
	DS1307Read(0x02,&temp);

	//Set 24 Hour BIT
	temp|=(0b00000000);

	//Write Back to DS1307
	DS1307Write(0x02,temp);
}

void clear_message()
{
	GLCD_ClearText(2,26,38);
	GLCD_TextGoTo(2,26);
}

void clear_text()
{
	GLCD_ClearText(19,7,10);
	GLCD_TextGoTo(19,7);
}


//-------------------------------------------------------------------------------------------------------
//    initialise devices
//-------------------------------------------------------------------------------------------------------
void init_devices(void)
{
 	cli();  //all interrupts disabled
// port_init();
// spi_init();
 	initialise_touch();
 	GLCD_Initialize();
	GLCD_ClearGraphic1();
	GLCD_ClearText1();
	rtc_initialise();

 	MCUCR = 0x00;
 
 //all peripherals are now initialized
}

void rtc_view(void)
{
	int k,l,v,z,s;
	char Time[9];	//hh:mm:ss
	char Date[9];	//dd:mm:yy
	//Now Read and format time
	uint8_t data;
		
	DS1307Read(0x00,&data);
	
	Time[8]='\0';
	Time[7]=(data & 0b00001111);
	Time[6]=((data) & 0b01110000>>4);
	Time[5]=':';

	DS1307Read(0x01,&data);

	Time[4]=(data & 0b00001111);
	Time[3]=((data & 0b01110000)>>4);
	Time[2]=':';

	DS1307Read(0x02,&data);

	Time[1]=(data & 0b00001111);
	Time[0]=((data & 0b00110000)>>4);

	DS1307Read(0x06,&data);
	Date[8]='\0';
	Date[7]=(data & 0b00001111);
	Date[6]=((data & 0b11110000)>>4);
	Date[5]=':';

	DS1307Read(0x05,&data);

	Date[4]=(data & 0b00001111);
	Date[3]=((data & 0b01110000)>>4);
	Date[2]=':';

	DS1307Read(0x04,&data);
	sdk=data;
//	display_int(2,13,data,4);
//	_delay_ms(5000);
	Date[1]=(data & 0b00001111);
	Date[0]=((data & 0b00110000)>>4);
	
	if((Time[0]==0)&&(Time[1]==0)&&(Time[3]==0)&&(Time[4]==0)&&(Time[6]==0)&&(Time[7]==0))
	{
		s=EEPROM_read(400);
		s++;
		EEPROM_write(400,s);
	}

	for(k=0;k<5;k++)
	{
		if((k==2))
		{
			LoadBitmap(dot,216,119,16,16);
		}
		else
		{
			z=184+(16*k);
			if(Time[k]==0)
			{
				LoadBitmap(zero,z,119,16,16);
			}
			else if(Time[k]==1)
			{
				LoadBitmap(one,z,119,16,16);
			}
			else if(Time[k]==2)
			{
				LoadBitmap(two,z,119,16,16);
			}
			else if(Time[k]==3)
			{
				LoadBitmap(three,z,119,16,16);
			}
			else if(Time[k]==4)
			{
				LoadBitmap(four,z,119,16,16);
			}
			else if(Time[k]==5)
			{
				LoadBitmap(five,z,119,16,16);
			}
			else if(Time[k]==6)
			{
				LoadBitmap(six,z,119,16,16);
			}
			else if(Time[k]==7)
			{
				LoadBitmap(seven,z,119,16,16);
			}
			else if(Time[k]==8)
			{
				LoadBitmap(eight,z,119,16,16);
			}
			else if(Time[k]==9)
			{
				LoadBitmap(nine,z,119,16,16);
			}
		}
	}

	for(l=0;l<8;l++)
	{
		if((l==2)||(l==5))
		{
			LoadBitmap(slash,(160+(l*16)),51,16,16);
		}
		else
		{
			v=160+(16*l);
			if(Date[l]==0)
			{
				LoadBitmap(zero,v,51,16,16);
			}
			else if(Date[l]==1)
			{
				LoadBitmap(one,v,51,16,16);
			}
			else if(Date[l]==2)
			{
				LoadBitmap(two,v,51,16,16);
			}
			else if(Date[l]==3)
			{
				LoadBitmap(three,v,51,16,16);
			}
			else if(Date[l]==4)
			{
				LoadBitmap(four,v,51,16,16);
			}
			else if(Date[l]==5)
			{
				LoadBitmap(five,v,51,16,16);
			}
			else if(Date[l]==6)
			{
				LoadBitmap(six,v,51,16,16);
			}
			else if(Date[l]==7)
			{
				LoadBitmap(seven,v,51,16,16);
			}
			else if(Date[l]==8)
			{
				LoadBitmap(eight,v,51,16,16);
			}
			else if(Date[l]==9)
			{
				LoadBitmap(nine,v,51,16,16);
			}
		}
	}
}

void  SetPWMOutput(uint16_t duty)
{
	
	ICR1 = 65535;
	ICR3 = 65535;
	
	OCR1A = duty; // out at OC1A 
    OCR1B = duty; // out at OC1B
 	OCR1C = duty; // out at OC1C

	OCR3A = duty; // out at OC3A 
    OCR3B = duty; // out at OC3B
 	OCR3C = duty; // out at OC3C
}

void InitPWM(void)
{
	/*
	TCCR0 - Timer Counter Control Register (TIMER0)
	-----------------------------------------------
	BITS DESCRIPTION
	
	NO: 	NAME   DESCRIPTION
	--------------------------
	BIT 7 : FOC0   Force Output Compare 	  [SET to 0]
	BIT 6 : WGM00  Wave form generartion mode [SET to 0]
	BIT 5 : COM01  Compare Output Mode        [SET to 1]
	BIT 4 : COM00  Compare Output Mode        [SET to toggle]
	BIT 3 : WGM01  Wave form generartion mode [SET to 1]
	BIT 2 : CS02   Clock Select               [SET to 0]
	BIT 1 : CS01   Clock Select               [SET to 0]
	BIT 0 : CS00   Clock Select               [SET to 1]

	The above settings are for
	--------------------------
	Timer Clock = CPU Clock (No Prescalling)
	Mode        = Fast PWM
	PWM Output  = Non Inverted

	*/

/*	TCCR0|=(1<<WGM01)|(1<<CS00);
	TCCR0&=~(1<<COM00);

*/
	//setting PWM-Ports as output 
    DDRB |= (1 << PB4)|(1 << PB7)|(1 << PB6)|(1 << PB5); 
    

	// Fast PWM 16-Bit mode(Timer 1) 
	TCCR1A |= (1 << WGM11)|(1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1); 
    TCCR1A &= ~(1 << COM1A0);
	TCCR1A &= ~(1 << COM1B0);
    TCCR1A &= ~(1 << COM1C0); 
	TCCR1B |= (1 << CS10)|(1 << WGM13)|(1 << WGM12);
	TCCR1C |= (1<<FOC1A);

	//setting PWM-Ports as output
	DDRE |= (1 << PE3)|(1 << PE4)|(1 << PE5);
	
	// Fast PWM 16-Bit mode(Timer 3) 
    TCCR3A |= (1 << WGM31)|(1<<COM3A1)|(1<<COM3B1)|(1<<COM3C1); 
    TCCR3A &= ~(1 << COM3A0);
	TCCR3A &= ~(1 << COM3B0);
    TCCR3A &= ~(1 << COM3C0); 
    TCCR3B |= (1 << CS30)|(1 << WGM33)|(1 << WGM32);
	TCCR3C |= (1<<FOC3A);
	
}



/**********************************************************************
	program for variation of LED intensity
***********************************************************************/
void led_glow(void)
{
	//display_int(2,15,manual_run_flag,2);
	//display_int(2,16,auto_run_flag,2);
	if(manual_run_flag==1||auto_run_flag==1)
	{
		uint8_t r_data=0,run_hour=0,run_minute=0,r_month=0;
			
	//InitPWM();
		
		r_data= ((( sdk & 0b11110000)>>4)*10)+(sdk & 0b00001111);
		
		if(manual_run_flag==1)
		{
			read_schedule_d(Date_Starting_Address+r_data);
		}
		else if(auto_run_flag==1)
		{
		//	display_int(2,15,Auto_Date_Address,4);
		//	_delay_ms(5000);
			read_schedule_d(Auto_Date_Address+(r_data-1));
			//display_int(2,18,(Auto_Date_Address+(r_data-1)),5);
		//	_delay_ms(5000);
		}
		//	display_int(2,21,intensity,3);
			
			brightness= (655 * intensity);
			
			//_delay_ms(1000);
			//display_int(10,20,brightness,5);
			
			
			DS1307Read(0x05,&r_month);
			DS1307Read(0x02,&run_hour);
			DS1307Read(0x01,&run_minute);
			
			r_month= ((( r_month & 0b00010000)>>4)*10)+(r_month & 0b00001111);
			run_hour= ((( run_hour & 0b00110000)>>4)*10)+(run_hour & 0b00001111);
			run_minute= ((( run_minute & 0b01110000)>>4)*10)+(run_minute & 0b00001111);
			if(r_month==1)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+0);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+0);
				}
				
			}
			else if(r_month==2)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+4);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+4);
				}
				
			}
			else if(r_month==3)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+8);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+8);
				}
				
			}
			else if(r_month==4)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+12);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+12);
				}
				
			}
			else if(r_month==5)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+16);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+16);
				}
			}
			else if(r_month==6)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+20);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+20);
				}
			}
			else if(r_month==7)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+24);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+24);
				}
			}
			else if(r_month==8)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+28);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+28);
				}
			}
			else if(r_month==9)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+32);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+32);
				}
			}
			else if(r_month==10)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+36);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+36);
				}
			}
			else if(r_month==11)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+40);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+40);
				}
			}
			else if(r_month==12)
			{
				if(manual_run_flag==1)
				{
					read_schedule_m(Month_Starting_Address+44);
				}
				else if(auto_run_flag==1)
				{
					read_schedule_m(Auto_Month_Address+44);
				}
				
			}
			if((run_hour>=from_hour)&&(run_minute>=from_minute))
				{
											
					if(run_hour<to_hr)
					{
							//display_int(4,13,intensity,5);
							InitPWM();
							SetPWMOutput(brightness);
					}
					else if(run_hour==to_hr)
					{
						if(run_minute<to_min)
						{
							//display_int(4,13,intensity,5);
							InitPWM();
							SetPWMOutput(brightness);
						}							
					}
				}
			if((run_hour>=to_hr)&&(run_minute>=to_min))
				{
					auto_run_flag=0;
					manual_run_flag=0;
					SetPWMOutput(0);
					EEPROM_write(401,auto_run_flag);
					EEPROM_write(402,manual_run_flag);
				}		
	}	
		
}

/**************************************************************************
	Function to display moon
**************************************************************************/
void display_moon(void)
{
	int s;
	s=EEPROM_read(400);
	if(s==1)
	LoadBitmap(m1,16,68,88,88);
	else if(s==2)
	LoadBitmap(m2,16,68,88,88);
	else if(s==3)
	LoadBitmap(m3,16,68,88,88);
	else if(s==4)
	LoadBitmap(m4,16,68,88,88);
	else if(s==5)
	LoadBitmap(m5,16,68,88,88);
	else if(s==6)
	LoadBitmap(m6,16,68,88,88);
	else if(s==7)
	LoadBitmap(m7,16,68,88,88);
	else if(s==8)
	LoadBitmap(m8,16,68,88,88);
	else if(s==9)
	LoadBitmap(m9,16,68,88,88);
	else if(s==10)
	LoadBitmap(m10,16,68,88,88);
	else if(s==11)
	LoadBitmap(m11,16,68,88,88);
	else if(s==12)
	LoadBitmap(m12,16,68,88,88);
	else if(s==13)
	LoadBitmap(m13,16,68,88,88);
	else if(s==14)
	LoadBitmap(m14,16,68,88,88);
	else if(s==15)
	LoadBitmap(m15,16,68,88,88);
	else if(s==16)
	LoadBitmap(m15,16,68,88,88);
	else if(s==17)
	LoadBitmap(m16,16,68,88,88);
	else if(s==18)
	LoadBitmap(m17,16,68,88,88);
	else if(s==19)
	LoadBitmap(m18,16,68,88,88);
	else if(s==20)
	LoadBitmap(m19,16,68,88,88);
	else if(s==21)
	LoadBitmap(m20,16,68,88,88);
	else if(s==22)
	LoadBitmap(m21,16,68,88,88);
	else if(s==23)
	LoadBitmap(m22,16,68,88,88);
	else if(s==24)
	LoadBitmap(m23,16,68,88,88);
	else if(s==25)
	LoadBitmap(m24,16,68,88,88);
	else if(s==26)
	LoadBitmap(m25,16,68,88,88);
	else if(s==27)
	LoadBitmap(m26,16,68,88,88);
	else if(s==28)
	LoadBitmap(m27,16,68,88,88);
	else if(s==29)
	LoadBitmap(m28,16,68,88,88);
	else if(s==30)
	LoadBitmap(m1,16,68,88,88);
	else if(s==31)
	{
		s=1;
		EEPROM_write(400,s);
	}
}

// Auto mode schedule
void auto_mode_schedule(void)
{
	//uint16_t f;
	EEPROM_write(100,11);
	EEPROM_write(101,0);
	EEPROM_write(102,19);
	EEPROM_write(103,0);
	EEPROM_write(104,11);
	EEPROM_write(105,0);
	EEPROM_write(106,19);
	EEPROM_write(107,0);
	EEPROM_write(108,11);
	EEPROM_write(109,0);
	EEPROM_write(110,19);
	EEPROM_write(111,0);
	EEPROM_write(112,11);
	EEPROM_write(113,0);
	EEPROM_write(114,16);
	EEPROM_write(115,50);
	EEPROM_write(116,11);
	EEPROM_write(117,0);
	EEPROM_write(118,20);
	EEPROM_write(119,20);
	EEPROM_write(120,11);
	EEPROM_write(121,0);
	EEPROM_write(122,17);
	EEPROM_write(123,0);
	EEPROM_write(124,11);
	EEPROM_write(125,0);
	EEPROM_write(126,17);
	EEPROM_write(127,0);
	EEPROM_write(128,11);
	EEPROM_write(129,0);
	EEPROM_write(130,17);
	EEPROM_write(131,0);
	EEPROM_write(132,11);
	EEPROM_write(133,0);
	EEPROM_write(134,17);
	EEPROM_write(135,0);
	EEPROM_write(136,11);
	EEPROM_write(137,0);
	EEPROM_write(138,17);
	EEPROM_write(139,0);
	EEPROM_write(140,11);
	EEPROM_write(141,0);
	EEPROM_write(142,17);
	EEPROM_write(143,0);
	EEPROM_write(144,11);
	EEPROM_write(145,0);
	EEPROM_write(146,17);
	EEPROM_write(147,0);
// Intensity

	EEPROM_write(150,10);
	EEPROM_write(151,20);
	EEPROM_write(152,30);
	EEPROM_write(153,40);
	EEPROM_write(154,50);
	EEPROM_write(155,60);
	EEPROM_write(156,70);
	EEPROM_write(157,80);
	EEPROM_write(158,90);
	EEPROM_write(159,100);
	EEPROM_write(160,95);
	EEPROM_write(161,85);
	EEPROM_write(162,75);
	EEPROM_write(163,65);
	EEPROM_write(164,55);
	EEPROM_write(165,45);
	EEPROM_write(166,35);
	EEPROM_write(167,25);
	EEPROM_write(168,15);
	EEPROM_write(169,5);
	EEPROM_write(160,25);
	EEPROM_write(171,35);
	EEPROM_write(172,45);
	EEPROM_write(173,55);
	EEPROM_write(174,25);
//	f=EEPROM_read(174);
//	display_int(2,19,f,3);
//	_delay_ms(5000);	
	EEPROM_write(175,100);
	EEPROM_write(176,85);
	EEPROM_write(177,95);
	EEPROM_write(178,100);
	EEPROM_write(179,85);
	EEPROM_write(180,35);
}


// display date

void display_date(void)
{
	d=0;
	m=0;
	y=0;
	mun=0;
	if(pos==6)
	{
		d=(c[0]*10)+c[1];
		m=(c[2]*10)+c[3];
		y=(c[4]*10)+c[5];
	}
	else if(pos==5)
	{
		d=(c[0]*10)+c[1];
		m=(c[2]*10)+c[3];
		y=(c[4]*10);	
	}
	else if(pos==4)
	{
		d=(c[0]*10)+c[1];
		m=(c[2]*10)+c[3];
		y=0;
	}
	else if(pos==3)
	{
		d=(c[0]*10)+c[1];
		m=(c[2]*10)+(0*c[3]);
		y=0;
	}
	else if(pos==2)
	{
		d=(c[0]*10)+c[1];
		m=0;
		y=0;	
	}
	else if(pos==1)
	{
		d=c[0];
		m=0;
		y=0;
	}
	else if(pos==0)
	{
		d=0;
		m=0;
		y=0;
	}

	if(s_flag==0)
	{
		display_int(10,10,d,2);
	}
	if(set_date_flag==1)
	{
		display_int(8,9,d,2);
		display_int(11,9,m,2);
		display_int(14,9,y,2);
	}
	if(moon_flag==1)
	{
		mun=d;
		display_int(8,6,mun,2);
	}
}

// display intensity

void display_inten(void)
{
	int j,mul;
	inten=0;
	mul=1;
	
	for(j=pos;j>0;j--)
	{
		inten=inten+(mul*c[j-1]);
		mul=mul*10;
	}
		
	display_int(10,13,inten,3);	
}

// to display time

void display_time(void)
{
	uint8_t mak=0,nik=0;
	if(pos==4)
	{
		mak=(c[0]*10)+c[1];
		nik=(c[2]*10)+c[3];
	}
	else if(pos==3)
	{
		mak=(c[0]*10)+c[1];
		nik=(c[2]*10)+(0*c[3]);
		
	}
	else if(pos==2)
	{
		mak=(c[0]*10)+c[1];
		nik=0;
			
	}
	else if(pos==1)
	{
		mak=(c[0]*10)+(0*c[1]);
		nik=0;
	}
	
	if(month_flag==1)
	{
		if(frm_flag==1)
		{	
			frm_hour=mak;
			frm_minute=nik;
			display_int(19,10,frm_hour,2);
			display_int(22,10,frm_minute,2);
		}
		else if(to_flag==1)
		{
			to_hour=mak;
			to_minute=nik;
			display_int(19,13,to_hour,2);
			display_int(22,13,to_minute,2);
		}
	}
	
	// SETTING
	else
	{
		display_int(8,12,frm_hour,2);
		display_int(11,12,frm_minute,2);
	}
}

void display_digit(void)
{
	if((set_date_flag==1)&&(pos<7))
	{
		display_date();
	}
	else if(((month_flag==1)&&(pos<5))||((t_flag==1)&&(pos<5)))
	{
		display_time();
	}
	else if(date_flag==1)
	{
		if((int_flag==1)&&(pos<4))
		{
			display_inten();
		}
		else if(pos<3)
		{
			display_date();
		}
	}
	else if((pos<3)&&(moon_flag==1))
	{
			display_date();
	}
}


// keypad

void keypad_view(void) 
{ 
	int ok=0;
	pos=0;
	for(int i=0;i<6;i++)
		c[i]=0;

	while(ok==0)
	{
		if(((pos<6)&&(set_date_flag==1))||((pos<4)&&(month_flag==1))||((int_flag==1)&&(pos<3))||((pos<2)&&(date_flag==1))||((pos<4)&&(t_flag==1))||((pos<2)&&(moon_flag==1)))
		{
			touch_value();
			if(((RH1>711)&&(RH1<755))&&((RH2>441)&&(RH2<480)))
			{
				c[pos]=0;
				pos++;
				display_digit();
			
				while(((RH1>711)&&(RH1<755))&&((RH2>441)&&(RH2<480)))
				{
					touch_value();
				}
			}

			else if(((RH1>637)&&(RH1<698))&&((RH2>622)&&(RH2<679)))
			{
				c[pos]=1;
				pos++;
				display_digit();
						
				while(((RH1>637)&&(RH1<698))&&((RH2>622)&&(RH2<679)))
				{
					touch_value();
				}
			}
			else if(((RH1>704)&&(RH1<761))&&((RH2>622)&&(RH2<679)))
			{
				c[pos]=2;
				pos++;
				display_digit();
						
				while(((RH1>704)&&(RH1<761))&&((RH2>622)&&(RH2<679)))
				{
					touch_value();
				}
			}
			else if(((RH1>774)&&(RH1<831))&&((RH2>622)&&(RH2<679)))
			{
				c[pos]=3;
				pos++;
				display_digit();
			
				while(((RH1>774)&&(RH1<831))&&((RH2>622)&&(RH2<679)))
				{
					touch_value();
				}
			}
			else if(((RH1>635)&&(RH1<696))&&((RH2>567)&&(RH2<612)))
			{
				c[pos]=4;
				pos++;
				display_digit();
			
				while(((RH1>635)&&(RH1<696))&&((RH2>567)&&(RH2<612)))
				{
					touch_value();
				}
			}
			else if(((RH1>701)&&(RH1<761))&&((RH2>567)&&(RH2<612)))
			{
				c[pos]=5;
				pos++;
				display_digit();
			
				while(((RH1>701)&&(RH1<761))&&((RH2>567)&&(RH2<612)))
				{
					touch_value();
				}
			}
			else if(((RH1>773)&&(RH1<830))&&((RH2>567)&&(RH2<612)))
			{
				c[pos]=6;
				pos++;
				display_digit();
		
				while(((RH1>773)&&(RH1<830))&&((RH2>567)&&(RH2<612)))
				{
					touch_value();
				}
			}
			else if(((RH1>638)&&(RH1<695))&&((RH2>501)&&(RH2<549)))
			{
				c[pos]=7;
				pos++;
				display_digit();
					
				while(((RH1>638)&&(RH1<695))&&((RH2>501)&&(RH2<549)))
				{
					touch_value();
				}
			}
			else if(((RH1>704)&&(RH1<763))&&((RH2>501)&&(RH2<549)))
			{
				c[pos]=8;
				pos++;
				display_digit();
			
				while(((RH1>704)&&(RH1<763))&&((RH2>501)&&(RH2<549)))
				{
					touch_value();
				}
			}
			else if(((RH1>773)&&(RH1<830))&&((RH2>501)&&(RH2<549)))
			{
				c[pos]=9;
				pos++;
				display_digit();
			
				while(((RH1>773)&&(RH1<830))&&((RH2>501)&&(RH2<549)))
				{
					touch_value();
				}
			}
		}
		touch_value();
		if(((RH1>642)&&(RH1<689))&&((RH2>442)&&(RH2<482)))
		{
			ok=1;
			
			if (s_flag==1)
			{
				if(set_date_flag==1)
				{
					GLCD_ClearText1();
					if(((d<32)&&(d>0))&&((m<13)&&(m>0)))
					{
						clear_message();
						d=((d/10)<<4)|(d%10);
						DS1307Write(0x04,d);
						m=((m/10)<<4)|(m%10);
						DS1307Write(0x05,m);
						y=((y/10)<<4)|(y%10);
						DS1307Write(0x06,y);
						GLCD_WriteString("date updated");
					}
					else
					{
						clear_message();
						GLCD_WriteString("parameters are invalid.");
					}

				}
				else if(t_flag==1)
				{
					GLCD_ClearText1();
					if(((frm_minute<60)&&(frm_hour<24))&&((frm_minute>=0)&&(frm_hour>=0)))
					{
						frm_minute=((frm_minute/10)<<4)|(frm_minute%10);
						DS1307Write(0x01,frm_minute);
						frm_hour=((frm_hour/10)<<4)|(frm_hour%10);
						DS1307Write(0x02,frm_hour);
						clear_message();
						GLCD_WriteString("time updated");

					}
					else
					{
						clear_message();
						GLCD_WriteString("parameters are invalid.");
					}
				}
				else if(moon_flag==1)
				{
					GLCD_ClearText1();
					if((mun>0)&&(mun<31))
					{
						clear_message();
						GLCD_WriteString("moon selected");
						EEPROM_write(400,mun);
					}
					else
					{
						clear_message();
						GLCD_WriteString("parameters are invalid.");
					}
				}
			}
						
			while(((RH1>642)&&(RH1<689))&&((RH2>442)&&(RH2<482)))
			{
				touch_value();
			}
		}
		else if(((RH1>776)&&(RH1<826))&&((RH2>436)&&(RH2<482)))
		{
			if(pos>0)
				pos--;
			c[pos]=0;
			display_digit();
				
			while(((RH1>776)&&(RH1<826))&&((RH2>436)&&(RH2<482)))
			{
				touch_value();
			}
		
		}
	}
	GLCD_ClearGraphic(216,56,96,130);
}
// data entry
	
void keypad1(void)
{
	touch_value();

	// FROM
	if(((RH1>501)&&(RH1<586))&&((RH2>596)&&(RH2<636)))
	{
		frm_flag=1;
		from_sign_flag=1;
		to_flag=0;
		frm_hour=0;
		frm_minute=0;
		display_int(19,10,frm_hour,2);
		display_int(22,10,frm_minute,2);
		clear_message();
		GLCD_WriteString("Enter Start Time.");
		LoadBitmap(keypad,216,56,96,104);
		keypad_view();
		
	}
	// TO
	else if(((RH1>501)&&(RH1<586))&&((RH2>545)&&(RH2<575)))
	{
		frm_flag=0;
		to_flag=1;
		to_sign_flag=1;
		to_hour=0;
		to_minute=0;
		display_int(19,13,to_hour,2);
		display_int(22,13,to_minute,2);
		clear_message();
		GLCD_WriteString("Enter Stop Time.");
		LoadBitmap(keypad,216,56,96,104);
		keypad_view();
	}
} 

void add_schedule(void)
{
	
	if(scrol==0)
	{
		//display_int(30,12,sch_flag,2);
		//display_int(30,13,scrol,2);

		if(sch_flag==1)
			add_schedule_m(Month_Starting_Address+0);
		else if(sch_flag==2)
			add_schedule_m(Month_Starting_Address+4);
		else if(sch_flag==3)
			add_schedule_m(Month_Starting_Address+8);
		else if(sch_flag==4)
			add_schedule_m(Month_Starting_Address+12);
		else if(sch_flag==5)
			add_schedule_m(Month_Starting_Address+16);
		else if(sch_flag==6)
			add_schedule_m(Month_Starting_Address+20);
	}	

	else if(scrol==1)
	{
		if(sch_flag==1)
			add_schedule_m(Month_Starting_Address+24);
		else if(sch_flag==2)
			add_schedule_m(Month_Starting_Address+28);
		else if(sch_flag==3)
			add_schedule_m(Month_Starting_Address+32);
		else if(sch_flag==4)
			add_schedule_m(Month_Starting_Address+36);
		else if(sch_flag==5)
			add_schedule_m(Month_Starting_Address+40);
		else if(sch_flag==6)
			add_schedule_m(Month_Starting_Address+44);
	}
}

// for datwise schedule

void date_wise(void)
{
	touch_value();	

	// DATE
	if(((RH1>353)&&(RH1<429))&&((RH2>611)&&(RH2<641)))
	{	
		int_flag=0;
		d=0;
		s_flag=0;
		month_flag=0;
		t_flag=0;
		date_flag=1;
		display_int(10,10,d,2);
		clear_message();
		GLCD_WriteString("Now enter Date.");
		LoadBitmap(keypad,216,56,96,104);
		keypad_view();
	}				

	// INTENSITY
	else if(((RH1>355)&&(RH1<421))&&((RH2>557)&&(RH2<592)))
	{
		int_flag=1;
		inten=0;
		display_int(10,13,inten,3);
		clear_message();
		GLCD_WriteString("Now enter intensity.");
		LoadBitmap(keypad,216,56,96,104);
		keypad_view();
	}	

	// to store data in memory

	else if(((RH1>437)&&(RH1<572))&&((RH2>365)&&(RH2<392)))  
	{
		GLCD_ClearText1();
		if((d>30)||(d==0)||(inten>100))
		{
			clear_message();
			GLCD_WriteString("Parameters are not valid.");
		}
		else
		{
			add_schedule_d(Date_Starting_Address+d);
			clear_message();
			GLCD_WriteString("Schedule successfully added.");
		}
	}	
			
	// BACK					
		
	if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
	{
		date_flag=0;
		GLCD_ClearText1();
		GLCD_ClearGraphic(8,19,304,190);
		LoadBitmap(datewise_tab,32,25,88,14);
		LoadBitmap(monthwise_tab,200,25,88,14);
										
		while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
		{
			touch_value();
		}	
			
	}
	
}


// for month wise schedule

void month_wise(void)
{

	touch_value();

	// Scrol Down

 	if(((RH1>383)&&(RH1<403))&&((RH2>470)&&(RH2<509))&&(scrol==0))
		{
			scrol=1;
			from_sign_flag=0;
			to_sign_flag=0;
			clear_text();
			GLCD_ClearGraphic(8,56,90,100);
			LoadBitmap(monthwise2,16,56,88,93);
		}	
	
	// Scrol Up
		
	if(((RH1>383)&&(RH1<403))&&((RH2>638)&&(RH2<674))&&(scrol==1))
		{
			scrol=0;
			from_sign_flag=0;
			to_sign_flag=0;
			clear_text();
			GLCD_ClearGraphic(8,56,90,100);
			LoadBitmap(monthwise1,16,56,88,93);
		}	
	
	// JAN/JUL

	if(((RH1>230)&&(RH1<368))&&((RH2>660)&&(RH2<684)))
	{
		sch_flag=1;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("JANUARY");
		else if(scrol==1)
			GLCD_WriteString("JULY");

		while(((RH1>230)&&(RH1<368))&&((RH2>660)&&(RH2<684)))
		{
			touch_value();
		}
	}

	// FEB/AUG

	else if(((RH1>230)&&(RH1<368))&&((RH2>628)&&(RH2<647)))
	{
		sch_flag=2;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("FEBRUARY");
		else if(scrol==1)
			GLCD_WriteString("AUGUST");
		
		while(((RH1>230)&&(RH1<368))&&((RH2>628)&&(RH2<647)))
		{
			touch_value();
		}

	}

	// MAR/SEP

	else if(((RH1>230)&&(RH1<368))&&((RH2>607)&&(RH2<622)))
	{
		sch_flag=3;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("MARCH");
		else if(scrol==1)
			GLCD_WriteString("SEPTEMBER");
					
		while(((RH1>230)&&(RH1<368))&&((RH2>622)&&(RH2<607)))
		{
			touch_value();
		}
	}

	// APR/OCT

	else if(((RH1>230)&&(RH1<368))&&((RH2>549)&&(RH2<572)))
	{	
		sch_flag=4;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("APRIL");
		else if(scrol==1)
			GLCD_WriteString("OCTOBER");
			
		while(((RH1>230)&&(RH1<368))&&((RH2>549)&&(RH2<572)))
		{
			touch_value();
		}

	}

	// MAY/NOV

	else if(((RH1>230)&&(RH1<368))&&((RH2>510)&&(RH2<530)))
	{
		sch_flag=5;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("MAY");
		else if(scrol==1)
			GLCD_WriteString("NOVEMBER");
			
		while(((RH1>230)&&(RH1<368))&&((RH2>510)&&(RH2<530)))
		{
			touch_value();
		}

	}


	// JUN/DEC
	
	else if(((RH1>230)&&(RH1<368))&&((RH2>476)&&(RH2<495)))
	{
		sch_flag=6;
		clear_text();
		if(scrol==0)
			GLCD_WriteString("JUNE");
		else if(scrol==1)
			GLCD_WriteString("DECEMBER");
						
		while(((RH1>230)&&(RH1<368))&&((RH2>476)&&(RH2<495)))
		{
			touch_value();
		}

	}
	// ADD SCHEDULE
	else if(((RH1>437)&&(RH1<572))&&((RH2>365)&&(RH2<392))) 
	{
		GLCD_ClearText1();
		if((frm_hour>23)||(from_sign_flag!=1)||(frm_minute>59)||(from_sign_flag!=1)||(to_hour>23)||(to_minute>59))
		{
			clear_message();
			GLCD_WriteString("Parameters are not valid.");
		}
		else
		{
			clear_message();
			GLCD_WriteString("Schedule successfully added.");
			add_schedule();
			from_sign_flag=0;
			to_sign_flag=0;
		}
		sch_flag=0;

	
		while(((RH1>437)&&(RH1<572))&&((RH2>365)&&(RH2<392)))
		{
			touch_value();
		}
	}

	// BACK					
	
	else if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
	{
		GLCD_ClearText1();		
		GLCD_ClearGraphic(8,19,304,190);
		LoadBitmap(datewise_tab,32,25,88,14);
		LoadBitmap(monthwise_tab,200,25,88,14);
		month_flag=0;
		scrol=0;					
		while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
		{
			touch_value();
		}	
	}

	keypad1();	
}
//-------------------------------------------------------------------------------------------------------
// main programme
//-------------------------------------------------------------------------------------------------------
int main ()
{
	int menu=1,menu_flag=0,schedule_flag,x,mode_flag=0;
	unsigned int i;
	init_devices();
	
	//LoadBitmap(logo,0,0,320,240);
	GLCD_ClearGraphic1();
	GLCD_ClearText1();
	//auto_run_flag=1;
	LoadBitmap(sharp,0,0,320,18);
	LoadBitmap(menu1,0,212,320,28);
	LoadBitmap(vertical_left,0,18,8,194);
	LoadBitmap(vertical_right,304,18,16,194);
	LoadBitmap(display1,144,21,168,185);
	display_moon();
	rtc_view();
	
	led_glow();
	do
	{
		rtc_view();
		touch_value();
	}
	while(((RH1<403)&&(RH1>619))&&((RH2<265)&&(RH2>302)));
	
	x=EEPROM_read(Useless_Location);

	if(x==255)
	{
		EEPROM_write(Useless_Location,0);
		display_int(0, 0, x, 4);
		_delay_ms(5000);
		for (i=100;i<=200;i++)
		{
			EEPROM_write(i,0);
		}
		auto_mode_schedule();

		auto_run_flag=0;
		manual_run_flag= 0;
		EEPROM_write(401,auto_run_flag);
		EEPROM_write(402,manual_run_flag);
	}

	else
	{
		auto_run_flag=EEPROM_read(401);
		manual_run_flag= EEPROM_read(402);
	}
		

	
	if(manual_run_flag==1||auto_run_flag==1)
	{
		led_glow();
	}
				
	while(1)
	{
		display_moon();
		rtc_view();
		led_glow();
		do
		{
			rtc_view();
			touch_value();
		}
		while(((RH1<403)&&(RH1>619))&&((RH2<265)&&(RH2>302)));
		
		if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
		{
			led_glow();
			if(menu==1)
			{
				led_glow();
				menu=0;
				menu_flag=1;
				GLCD_ClearText1();
				GLCD_ClearGraphic1();
				LoadBitmap(sharp,0,0,320,18);
				LoadBitmap(back,0,212,320,28);
				LoadBitmap(vertical_left,0,18,8,194);
				LoadBitmap(vertical_right,304,18,16,194);
				LoadBitmap(mode_tab,40,56,104,29);
				LoadBitmap(schedule_tab,168,56,104,29);
				LoadBitmap(setting_tab,104,120,104,29);
			}
			else if(menu==0)
			{
				led_glow();
				menu=1;
				menu_flag=0;
				GLCD_ClearGraphic1();
				GLCD_ClearText1();
				LoadBitmap(sharp,0,0,320,18);
				LoadBitmap(menu1,0,212,320,28);
				LoadBitmap(vertical_left,0,18,8,194);
				LoadBitmap(vertical_right,304,18,16,194);
				LoadBitmap(display1,144,21,168,185);
			}	
			
			while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
			{
				touch_value();
			}
			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);
			_delay_ms(500);

		}

		while(menu_flag==1)
		{
			led_glow();
			touch_value();
			// SCHEDULE
			
			if(((RH1>541)&&(RH1<747))&&((RH2>612)&&(RH2<681)))
			{
				GLCD_ClearGraphic1();
				GLCD_ClearText1();
				GLCD_ClearGraphic1();
				schedule_flag=1;
				LoadBitmap(sharp,0,0,320,18);
				LoadBitmap(back,0,212,320,28);
				LoadBitmap(vertical_left,0,18,8,194);
				LoadBitmap(vertical_right,304,18,16,194);
				LoadBitmap(datewise_tab,32,25,88,14);
				LoadBitmap(monthwise_tab,200,25,88,14);
					
				while(schedule_flag==1)
				{
					led_glow();
					touch_value();
					//Date wise
					if(((RH1>261)&&(RH1<432))&&((RH2>725)&&(RH2<758)))
					{
					    led_glow();
						date_flag=1;
						month_flag=0;
						s_flag=0;
						GLCD_ClearGraphic(16,25,88,14);
						LoadBitmap(datewise_black,32,25,88,14);
						LoadBitmap(monthwise_tab,200,25,88,14);
						LoadBitmap(date1,24,75,88,41);
						LoadBitmap(schedule,120,182,72,12);
						while(date_flag==1)
						{
						led_glow();
						date_wise();
						}
						while(((RH1>261)&&(RH1<432))&&((RH2>725)&&(RH2<758)))
						{
							touch_value();
						}	

					}
			
					//month	wise
			
					else if(((RH1>605)&&(RH1<775))&&((RH2>723)&&(RH2<755)))
					{
						led_glow();
						month_flag=1;
						date_flag=0;
						s_flag=0;
						GLCD_ClearGraphic(200,25,88,14);
						LoadBitmap(datewise_tab,32,25,88,14);
						LoadBitmap(monthwise_black,200,25,88,14);
						LoadBitmap(monthwise1,16,56,88,93);
						LoadBitmap(from_to,112,56,80,60);
						LoadBitmap(schedule,120,182,72,12);
						scrol=0;
						while(month_flag==1)
						{
							led_glow();
							month_wise();
						}

						while(((RH1>605)&&(RH1<775))&&((RH2>723)&&(RH2<755)))
						{
							touch_value();
						}	

					}
			
					// BACK

					else if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
					{
						led_glow();
						month_flag=0;
						date_flag=0;
						s_flag=0;
						schedule_flag=0;
						menu=0;
						GLCD_ClearGraphic1();
						LoadBitmap(sharp,0,0,320,18);
						LoadBitmap(back,0,212,320,28);
						LoadBitmap(vertical_left,0,18,8,194);
						LoadBitmap(vertical_right,304,18,16,194);
						LoadBitmap(mode_tab,40,56,104,29);
						LoadBitmap(schedule_tab,168,56,104,29);
						LoadBitmap(setting_tab,104,120,104,29);

						
						while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
						{
							touch_value();
						}
					}
				}
		
		}	
		
		// MODE TAB
		
		else  if(((RH1>282)&&(RH1<480))&&((RH2>615)&&(RH2<684)))
		{
			GLCD_ClearText1();
			mode_flag=1;
			LoadBitmap(auto_manu,88,180,152,14);
			led_glow();			
			while(mode_flag==1)
			{
				led_glow();
				touch_value();
				// AUTO
				if(((RH1>368)&&(RH1<478))&&((RH2>360)&&(RH2<398)))
				{
					month_flag=0;
					date_flag=0;
					s_flag=0;
					t_flag=0;
					schedule_flag=0;
					manual_run_flag=0;
					auto_run_flag=1;
					menu=1;
					menu_flag=0;
					GLCD_ClearGraphic1();
					GLCD_ClearText1();
					LoadBitmap(sharp,0,0,320,18);
					LoadBitmap(menu1,0,212,320,28);
					LoadBitmap(vertical_left,0,18,8,194);
					LoadBitmap(vertical_right,304,18,16,194);
					LoadBitmap(display1,144,21,168,185);
					mode_flag=0;
					EEPROM_write(401,auto_run_flag);
					EEPROM_write(402,manual_run_flag);
					led_glow();		
					while(((RH1>368)&&(RH1<478))&&((RH2>360)&&(RH2<398)))
					{
						touch_value();
					}
				}
			
				// MANUAL
				else if(((RH1>525)&&(RH1<690))&&((RH2>360)&&(RH2<398)))
				{
					month_flag=0;
					date_flag=0;
					s_flag=0;
					t_flag=0;
					schedule_flag=0;
					manual_run_flag=1;
					auto_run_flag=0;
					menu=1;
					menu_flag=0;
					GLCD_ClearGraphic1();
					GLCD_ClearText1();
					LoadBitmap(sharp,0,0,320,18);
					LoadBitmap(menu1,0,212,320,28);
					LoadBitmap(vertical_left,0,18,8,194);
					LoadBitmap(vertical_right,304,18,16,194);
					LoadBitmap(display1,144,21,168,185);
					mode_flag=0;
					EEPROM_write(401,auto_run_flag);
					EEPROM_write(402,manual_run_flag);
					led_glow();
					while(((RH1>525)&&(RH1<690))&&((RH2>360)&&(RH2<398)))
					{
						touch_value();
					}
				}
			}
			
			while(((RH1>282)&&(RH1<480))&&((RH2>615)&&(RH2<684)))
			{
				touch_value();
			}
		}


		//SETTING
		else  if(((RH1>407)&&(RH1<616))&&((RH2>470)&&(RH2<536)))
		{
			led_glow();
			GLCD_ClearGraphic1();
			s_flag=1;
			frm_minute=0;
			frm_hour=0;
			d=0;
			m=0;
			y=0;
			mun=0;
			LoadBitmap(sharp,0,0,320,18);
			LoadBitmap(back,0,212,320,28);
			LoadBitmap(vertical_left,0,18,8,194);
			LoadBitmap(vertical_right,304,18,16,194);
			LoadBitmap(moon,24,49,64,10);
			LoadBitmap(setting,24,68,112,38);
			while(s_flag==1)
			{
				led_glow();
				touch_value();
				
				// Date
				if(((RH1>318)&&(RH1<366))&&((RH2>620)&&(RH2<652)))
				{
					led_glow();
					t_flag=0;
					moon_flag=0;
					set_date_flag=1;
					d=0;
					m=0;
					y=0;
					display_int(8,9,d,2);
					display_int(11,9,m,2);
					display_int(14,9,y,2);
					clear_message();
					GLCD_WriteString("Enter Date.");
					LoadBitmap(keypad,216,56,96,104);
					keypad_view();
				}

				//Time	
				else if(((RH1>318)&&(RH1<366))&&((RH2>555)&&(RH2<593)))
				{
					led_glow();
					t_flag=1;
					moon_flag=0;
					set_date_flag=0;
					frm_hour=0;
					frm_minute=0;
					display_int(8,12,frm_hour,2);
					display_int(11,12,frm_minute,2);
					clear_message();
					GLCD_WriteString("Enter Time.");
					LoadBitmap(keypad,216,56,96,104);
					keypad_view();
				}
				
				//Moon
				else if(((RH1>319)&&(RH1<362))&&((RH2>666)&&(RH2<701)))
				{
					led_glow();
					moon_flag=1;
					t_flag=0;
					set_date_flag=0;
					mun=0;
					display_int(8,6,mun,2);
					clear_message();
					GLCD_WriteString("Enter moon no");
					LoadBitmap(keypad,216,56,96,104);
					keypad_view();
				}
				
							
				// BACK
				else if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
				{
					led_glow();
					month_flag=0;
					date_flag=0;
					s_flag=0;
					t_flag=0;
					moon_flag=0;
					set_date_flag=0;
					schedule_flag=0;
					menu=0;
					GLCD_ClearText1();
					GLCD_ClearGraphic1();
					LoadBitmap(sharp,0,0,320,18);
					LoadBitmap(back,0,212,320,28);
					LoadBitmap(vertical_left,0,18,8,194);
					LoadBitmap(vertical_right,304,18,16,194);
					LoadBitmap(mode_tab,40,56,104,29);
					LoadBitmap(schedule_tab,168,56,104,29);
					LoadBitmap(setting_tab,104,120,104,29);

				
					while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
					{
						touch_value();
					}
				}
			}
		}
		// BACK
		else if(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
		{
			led_glow();
			month_flag=0;
			date_flag=0;
			s_flag=0;
			t_flag=0;
			set_date_flag=0;
			schedule_flag=0;
			menu=1;
			menu_flag=0;
			GLCD_ClearText1();
			GLCD_ClearGraphic1();
			LoadBitmap(sharp,0,0,320,18);
			LoadBitmap(menu1,0,212,320,28);
			LoadBitmap(vertical_left,0,18,8,194);
			LoadBitmap(vertical_right,304,18,16,194);
			LoadBitmap(display1,144,21,168,185);
			
			while(((RH1>403)&&(RH1<619))&&((RH2>265)&&(RH2<302)))
			{
				touch_value();
			}
		}		
	}
}
}

