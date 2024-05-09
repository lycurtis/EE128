#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdio.h>

int main() {
	unsigned long i;

	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; //Port D clock gate control enable
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; //Port B clock gate control enable

    PORTB_GPCLR = 0x0C0100; //configures pins 2 and 3 on port B to be GPIO
    PORTD_GPCLR = 0xFF0100; //Configure pins on PORT D to be GPIO

    //Direction registers
    GPIOD_PDDR = 0xFF; //configure Port C pins 0-7 for output
    GPIOB_PDDR = 0x00; // Configures pins 2 and 3 of port B as Input

    //variable declarations for switch and delay
    unsigned int direction; //PTB2
    unsigned int speed; //PTB3
    unsigned long delayL = 50000; //delay
    unsigned short delayS = 10000;


    for(;;){
    	unsigned int input = GPIOB_PDIR;  //check direction
    	direction = input & 0x04; //PTB2
    	speed = input & 0x08; //PTB3

    	if(!direction && !speed){
    		//long delay
    		/*
    		 * 0x36
    		 * 0x35
    		 * 0x39
    		 * 0x3A
    		 */
    		GPIOD_PDOR = 0x36;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x35;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x39;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x3A;
    		for(i = 0; i < delayL; i++);
    	}
    	else if(!direction && speed){
    		//short delay
    		/*
    		 * 0x36
    		 * 0x35
    		 * 0x39
    		 * 0x3A
    		 */
    		GPIOD_PDOR = 0x36;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x35;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x39;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x3A;
    		for(i = 0; i < delayS; i++);

    	}
    	else if(direction && !speed){
    		//long delay
    		/*
    		 * 0x36
    		 * 0x3A
    		 * 0x39
    		 * 0x35
    		 */
    		GPIOD_PDOR = 0x36;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x3A;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x39;
    		for(i = 0; i < delayL; i++);
    		GPIOD_PDOR = 0x35;
    		for(i = 0; i < delayL; i++);
    	}
    	else if(direction && speed){
    		//short delay
    		/*
    		 * 0x36
    		 * 0x3A
    		 * 0x39
    		 * 0x35
    		 */
    		GPIOD_PDOR = 0x36;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x3A;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x39;
    		for(i = 0; i < delayS; i++);
    		GPIOD_PDOR = 0x35;
    		for(i = 0; i < delayS; i++);
    	}
    }
    return 0;
}
