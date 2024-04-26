#include "fsl_device_registers.h"
#include <stdint.h>

unsigned char decoder[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B}; //display values 0-9 on 7 segment
unsigned int count = 0;
float value = 0.0f;
unsigned int potVal = 0;

unsigned short ADC_Read16B(void){
	ADC0_SC1A = 0x00; //Write to SC1A to start conversion
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK); //conversion in process
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK)); //Until conversion is complete
	return ADC0_RA; //ADC conversion result for ADC0
}

void PORTA_IRQHandler(void){
	//Toggle ones place decimal point
	NVIC_ClearPendingIRQ(PORTA_IRQn); //clear pending interrupts
	unsigned int input = GPIOB_PDIR;//Read port B
	unsigned int MODE_SW = input & 0x04; //PB2
	unsigned int CNT_DIR = input & 0x08; //PB3

	if(!MODE_SW){ //ADC mode (SW 2 down)
		//read from ADC and convert to decimal value
		potVal = ADC_Read16B();
	}
	else if(MODE_SW){ //Count Mode
		if(!CNT_DIR){ //(SW 1 down)
			//count up to 99 and roll over to 0
			if(count < 99){
				count++; //count up
			}
			else{
				count = 0;
			}
		}
		else{
			//count down to 0 and roll over to 99
			if(count > 0){
				count--; //count down
			}
			else{
				count = 99;
			}
		}
	}

	//Display value based on MODE_SW
	if(MODE_SW){ //count mode
		value = count;
	}
	else{ //ADC mode
		 //scale down to 0-3.3V
		value = (potVal*3.3)/6400;
		//value = (result * (Vrh - Vrl))/(2^16-1)
	}
	PORTA_ISFR = (1 << 1); //Clear ISFR for PORTA, Pin 1
	GPIOD_PCOR = 0xFF; //clears output on PortD[0:7]
	GPIOC_PCOR = 0xBF; //clears output on PortC[0:7]
	GPIOD_PSOR = (unsigned int)decoder[(int)value/10]; //sets output to converted value PortD
	GPIOC_PSOR = ((((unsigned int)decoder[(int)value%10]) & 0x40) << 1) | ((unsigned int)decoder[(int)value%10] & 0x3F);
	//GPIOC_PSOR = ((((unsigned int)decoder[value%10]) & 0x40) << 1) | ((unsigned int)decoder[value%10] & 0x3F); //sets output to converted value PortC
}

int main(void)
{
	//clock gating
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable Port A Clock Gate Control */
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK; /*Enable Port ADC Clock Gate Control*/
	ADC0_CFG1 = 0x0C; //16 bits ADC; bus CLOCK
	ADC0_SC1A = 0x1F; //Disable the module, ADCH = 11111

	//configuration & initialization
	PORTD_GPCLR = 0xFF0100; //Configure pins 0-7 on PORT D to be GPIO
	PORTC_GPCLR = 0x1BF0100; //Configure pins 0-5, 7-8 on PORT C to be GPIO (skip PTC 6 because that is reserved by the K64F for interrupt)
	PORTB_GPCLR = 0x40C0100; //configures pins 2,3, and 10 on port B to be GPIO
	PORTA_PCR1 = 0xA0100; /* Configure Port A pin 1 for GPIO and interrupt on falling edge */

	//Direction registers
	GPIOD_PDDR = 0xFF; //Configures pins 0-7 of Port D as output
	GPIOC_PDDR = 0xFF; //configure Port C pins 0-5 7-8 for output
	GPIOA_PDDR = 0x00; //Configure PortA1 to be input
	GPIOB_PDDR = 0x400; //Configure Port B pins 2-3 to for Input reading from the switch and pin 10 for output

	//enable interrupt service routine
	PORTA_ISFR = (1 << 1); /* Clear ISFR for Port A, Pin 1*/
	NVIC_EnableIRQ(PORTA_IRQn); /* Enable interrupts from Port A*/

	//Artificial Clock generation
	for(;;){
		GPIOB_PTOR |= (1<<10);
		for(int i = 0; i < 0x10000; i++);//delay for loop
	}


	return 0;
}
