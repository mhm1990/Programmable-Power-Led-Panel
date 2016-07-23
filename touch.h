
//****************************************************************************************
// ***** HEADER FILE : touch.h ******
//**************************************************
#ifndef _TOUCH_H_
#define _TOUCH_H_


//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
#define TOUCH_X_PO PF0
#define TOUCH_Y_PO PF2
#define TOUCH_X_NE PF1
#define TOUCH_Y_NE PF3

uint16_t RH1;
uint16_t RH2;

//---------------------------------------------------------------------------------------
// functions
//---------------------------------------------------------------------------------------
void InitADC();
uint16_t ReadADC(uint8_t ch);
void read_y_cord();
void read_x_cord();
void touch_value();
void initialise_touch();


#endif