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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum States{start, d1, d2, d3, wait, restart }state;
unsigned char CU;
void tick(){
	switch(state){
		case start:
			PORTB = 0x00;
			state = d1;
			break;
		
		case d1:
			if((~PINA & 0x01) == 0x01) {
				state = wait;
			}
			else{
				state = d2;
			}
			CU = 1;
			break;
		case d2:
			if((~PINA & 0x01) == 0x01) {
				state = wait;
			}
			else{
				if(CU == 0){
					state = d1;	
				}
				else if(CU == 1){
					state = d3;
				}
			}
			break;
		case d3:
			if((~PINA & 0x01) == 0x01) {
				state = wait;
			}
			else{
				state = d2; 
			}
			CU = 0;
			break;
	
		case wait:
			if((~PINA & 0x01) == 0x01){
				state = wait; 
				break;
			}
			else{
				state = restart; 
				break;
			}
		case restart:
			if((~PINA & 0x01) == 0x01){
				state = d1; 
				break;
			}
			else{
				state = restart; 
				break;
			}
			
		default:
			break;
	}
	
	switch(state){
		case start:
			break;
			
		case d1:
			PORTB = 0x01;
			break;
		case d2:
			PORTB = 0x02;
			break;
		case d3:
			PORTB = 0x04;
			break;
			
		case wait:
			break;
		case restart:
			break;
			
		default:
			break;
	}
		
}
	
int main(void){
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = start;
	TimerSet(300);
	TimerOn();
    /* Insert your solution below */
    while (1){
	tick();	
	while(!TimerFlag){TimerFlag = 0;}
    }
    return 1;
}
