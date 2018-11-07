/*
 * CS_AMT8.c
 *
 * Created: 29.10.2018 10:36:18
 * Author : mkac
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "Init.h"

volatile static	uint8_t _Status=0;
volatile static uint8_t _Com=0;
volatile static unsigned char _pc[3];
volatile static uint16_t adc_board;
volatile static uint16_t adc_sensor1;
volatile static uint16_t adc_sensor2;
volatile static uint16_t adc_sensor3;
volatile const uint8_t __ch_board=6;	//chanel0
volatile const uint8_t __ch_sensor1=0;	//chanel1
volatile const uint8_t __ch_sensor2=1;	//chanel2
volatile const uint8_t __ch_sensor3=2;	//chanel3
volatile const uint8_t _add_ch_board=0x36;	//chanel0
volatile const uint8_t _add_ch_sensor1=0x30;	//chanel1
volatile const uint8_t _add_ch_sensor2=0x31;	//chanel2
volatile const uint8_t _add_ch_sensor3=0x33;	//chanel3
void ReadControlbyte();
void PcTask();

int main(void)
{
	
	DDRD|=(1<<PIND7);	//EN12
	DDRB|=(1<<PINB0);	//EN34
	PORTD|=(1<<PIND7);
	PORTB|=(1<<PINB0);
	DDRD &=~((1<<PIND2)|(1<<PIND3)); //input interr
	
	ADCInit();
	PWMInit();
	UartInit();

	PWM(0);
	
	GICR=(1<<INT0);
	MCUCR|=(1<<ISC00);		//		(1<<ISC00);any logical change			//	(1<<ISC01)|(1<<ISC00);rising edge
	
	sei();
	
    while (1) 
    {		
		OnAdc();
		adc_board=ReadAdc(__ch_board);
		_delay_ms(5);
		adc_sensor1=ReadAdc(__ch_sensor1);
		_delay_ms(5);
		if (_Status)
		{	
			//Uart comunication Send data do pc
			//SendADCValue(_add_ch_board,adc_board);
			//_delay_ms(5);
			//SendADCValue(_add_ch_sensor1,adc_sensor1);
			//_delay_ms(5);
		//////////////////////////////////////////////////////////////////////////
			SendInfoStatus(0xff,0xff,0xff);
			ReadControlbyte();
			_delay_ms(1);
			PcTask();
			
			_Status=0;
		//////////////////////////////////////////////////////////////////////////
			//break;
		}
		if(_Com)
		{
			SendADCValue(_pc[1],ReadAdc(_pc[1]));
			_Com=0;
		}
		OffAdc();
		_delay_ms(1000);
	}
}

ISR(ADC_vect)
{
	if(adc_sensor1<150){
		PWM(300);
	}
	else {
		if(adc_sensor1*2>1023){
			PWM(1023);
		}
		else{
			PWM(adc_sensor1*2);
		}
	}
}
ISR(INT0_vect)
{
	if(_Status==1)
	{
		_Status=0;
	}
	else
	{
		_Status=1;
	}
}

//////////////////////////////////////////////////////////////////////////
 void ReadControlbyte()
 {	
	 for(int i=0; i<3; i++)
	 {
		 _pc[i]=GetChar();
	 }
 }
 void PcTask()
 {
	 if (_pc[0]==0x63)
	 {
		 _Com=1;
	 }
 }