/*
 * Init.c
 *
 * Created: 30.10.2018 06:09:59
 *  Author: mkac
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdlib.h>

volatile static unsigned char h,l;

void SendChar(char data)
{
	while(!(UCSRA&(1<<UDRE)));
	{
		UDR=data;
	}
}
void SendInfoStatus(unsigned char info,unsigned char abb,unsigned char spec)
{
	SendChar(info);
	SendChar(abb);
	SendChar(spec);
}

void UartInit(void)
{
	DDRD|=(1<<PIND1);
	DDRD&=~(1<<PIND0);
	PORTD|=1<<(PIND0);
	
	#define BAUD 9600
	#include <util/setbaud.h>
	
	UBRRH=UBRRH_VALUE;
	UBRRL=UBRRL_VALUE;
	
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
	UCSRB = (1<<TXEN) | (1<<RXEN);
	_delay_ms(10);
	SendInfoStatus(0x00,0x00,0x00);
}

void SendADCValue(unsigned char _chaneladdress,uint16_t data)
{
	h=(unsigned char)(data>>8);
	l=(unsigned char)(data);
	SendChar(_chaneladdress);
	SendChar(h);
	SendChar(l);
}



void PWMInit(void)
{
	DDRB|=(1<<PINB1)|(1<<PINB2);
	//TCCR1A |= (1<<WGM12)|(1<<WGM10);		//8bit Fast PWM
	//TCCR1B |= (1<<WGM12)|(1<<WGM10);
	TCCR1A |= (1<<WGM12)|(1<<WGM11)|(1<<WGM10);	//10bit Fast PWM
	TCCR1B |= (1<<WGM12)|(1<<WGM11)|(1<<WGM10);
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1);	//
	TCCR1B |= (1<<CS10);	//Prescaler 1 |(1<<CS11)
	TCCR1A |= (1<<CS10);
}

void PWM(uint16_t _pwm)
{
	OCR1A=_pwm;
	OCR1B=_pwm;
}

void ADCInit()
{
	ADMUX|=(1<<REFS0);
	ADCSRA|=((1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
	_delay_ms(500);

}

uint16_t ReadAdc(uint8_t chanel)
{
	chanel&=0x07;
	ADMUX=(ADMUX&0xf8)|chanel;
	ADCSRA|=(1<<ADSC);
	while(ADCSRA&(1<<ADSC));
	return ADC;
}

void OnAdc()
{
	ADCSRA|=(1<<ADEN);
}

void OffAdc()
{
	ADCSRA&=~(1<ADEN);
}