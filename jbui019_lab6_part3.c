/*	Author: jbui019
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{init, A0press, A0remain, A0release, A1press, A1remain, A1release, reset}state;
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;	
}

void TimerISR(){
	TimerFlag = 1;	
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char counter = 0;

void tick(){
	unsigned char PB = PORTB;
	unsigned char A1 = ~PINA & 0x02;
	unsigned char A0 = ~PINA & 0x01;
	
	if (A0 && A1){ 
		state = reset;
	}
	else {
		switch(state){
		 
		case init:
			if(A0){
				state = A0press;
			}
			else if(A1){
				state = A1press;
			}
			else{
				state = init;
			}
			break;
				
		case A0press:
			counter = 0;
			if(A0){
				state = A0remain;
			}
			else if(!A0){
				state = A0release;
			}
			else{
				state = A0remain;
			}
			break;
				
		case A1press:
			counter = 0;
			if(A1) state = A1remain;
			else if(!A1) state = A1release;
			else state = A1remain;
			break;
				
		case A0remain:
			if(A0){
				state = A0remain;
			}
			else{
				state = A0release;
			}
			break;
				
		case A1remain:
			if(A1){
				state = A1remain;
			}
			else{
				state = A1release;
			}
			break;
				
		case A0release:
			counter = 0;
			if(A0){
				state = A0press;
			}
			else if(A1){
				state = A1press;
			}
			else{
				state = A0release;
			}
			break;
		case A1release:
			counter = 0;
			if(A0){
				state = A0press;
			}
			else if(A1){
				state = A1press;
			}
			else{
				state = A1release;
			}
			break;
		case reset:
			if(A0){
				state = A0press;
			}
			else if(A1){
				state = A1press;
			}
			else{
				state = reset;
			}
			break;
				
		default:
			state = init;
			break; 
		}
	}
	
	switch(state){
	case init:
		PORTB = 7;
		break;
	case A0press:
		if(PB < 9){
			PORTB = PB + 1;
		}
		else if(PB == 9){
			PORTB = 0x09;
		}
		break;
	case A0remain:
		counter++;
		if(counter == 100 && PB < 9){
			PORTB = PB+1;
			counter = 0;
		}
		else if(counter == 100 && PB == 9){
			PORTB = 0x09;
			counter = 0;
		}
		break;
	case A1press:
		if(PB > 0){
			PORTB = PB -1;
		}
		else if(PB == 0){
			PORTB = 0x00;	
		}
		break;
	case A1remain:
		counter++;
		if(counter == 100 && PB > 0)
		{
			PORTB = PB-1;
			counter = 0;
		}
		else if(counter == 100 && PB == 0){
			PORTB = 0x00;
			counter = 0;
		}
		break;
	case reset:
		PORTB = 0;
		break;
	default:
		break;
	}

}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = init;
	TimerSet(1);
	TimerOn();
    /* Insert your solution below */
    while (1) {
	tick();	
	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
}
