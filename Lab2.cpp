#include "fsl_device_registers.h"
#include <stdint.h>

void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/

	PORTD_GPCLR = 0xFF0100; /* Configured Pins 0-7 on Port D*/
	PORTC_GPCLR = 0x1BF0100; /* Configured Pins 0-5, 7-8 on Port C*/
	PORTB_GPCLR = 0x000C0100; //configures pins 2 and 3 on port B to be GPIO

	GPIOB_PDDR = 0x00000000; // Configures pins 2 and 3 of port B as Input
	GPIOD_PDDR = 0x000000FF; //Configures pins 0-7 of Port D as output
	GPIOC_PDDR = 0x000001BF; //configure port c pins 0-5 7-8 for output

	GPIOC_PDOR = 0x0; //Initialize port C to 0;
	GPIOD_PDOR = 0x1;
	//GPIOD_PDOR = 0x0; //Initialize Port D such that only 1 bit is On

	unsigned int CNT_DIR = 0;
	unsigned int ROT_DIR = 1;
	unsigned int input = 0;

	/*Intialize Port C to Counter*/
	/*Intialize Port D to Shifter*/


	//unsigned long delay = 0x100000; /*Delay Value*/

	for(;;) {
		software_delay(100000);

		input = GPIOB_PDIR;
		//counter logic
		/*
		if(input & 0x8){
			if(CNT_DIR < 0xFF){
				CNT_DIR++;
			}
			else{
				CNT_DIR = 0;
			}
		}
		else{
			if(CNT_DIR > 0){
				CNT_DIR--;
			}
			else{
				CNT_DIR = 0xFF;
			}
		}

		//shifter logic
		if(input & 0x4){
			ROT_DIR & 0x80 ? ROT_DIR = 0x01 : ROT_DIR << 1;
		}
		else{
			if(ROT_DIR & 0x01){
				ROT_DIR = 0x80;
			}
			else{
				ROT_DIR >> 1;
			}
		}
		GPIOC_PDOR = CNT_DIR;
		GPIOD_PDOR = ROT_DIR;
		*/

		// Counter logic

		if(input & 0x8){
			CNT_DIR = (CNT_DIR < 0x1FF) ? CNT_DIR + 1 : 0;
		}
		else{
			CNT_DIR = (CNT_DIR > 0) ? CNT_DIR - 1 : 0x1FF;
		}


		// Shifter logic
		if(input & 0x8){
			ROT_DIR = (ROT_DIR & 0x80) ? 0x01 : ROT_DIR << 1;
		}
		else{
			ROT_DIR = (ROT_DIR & 0x01) ? 0x80 : ROT_DIR >> 1;
		}
		GPIOC_PDOR = CNT_DIR;
		GPIOD_PDOR = ROT_DIR;
	}
	return 0;
}
