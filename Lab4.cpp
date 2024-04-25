#include "fsl_device_registers.h"
#include <stdint.h>

unsigned int nr_overflows = 0;

void FTM3_IRQHANDLER(void) {
    nr_overflows++;
    uint32_t SC_VAL = FTM3_SC;
    FTM3_SC &= 0x7F; //clear TOF
}

void main(void) {
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; //Port C clock enable
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; //Port D clock enable

    SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK; //FTM3 clock enable

    PORTC_PCR10 = 0x300; //Port C Pin 10 as FTM3_CH6 (ALT3)
    PORTD_PCR10 = 0x300; //Port D Pin 10 as FTM3_CH6 (ALT3)

    FTM3_MODE = 0x5; //Enable FTM3
    FTM3_MOD = 0xFFFF;
    FTM3_SC = 0x0D; //system clock / 32

    NVIC_EnableIRQ(FTM3_IRQn); //Enable FTM3 interrupts
    FTM3_SC |= 0x40; //Enable TOF

    unsigned int t1, t2, t3, pulse_width, duty_cycle, period;
    while (1) {
        FTM3_CNT = 0; nr_overflows = 0; // initialize counters
        FTM3_C6SC = 0x4; // rising edge
        while(!(FTM3_C6SC & 0x80)); // wait for CHF
        FTM3_C6SC &= ~(1 << 7);
        t1 = FTM3_C6V; // first edge

        FTM3_C6SC = 0x8; // falling edge
        while(!(FTM3_C6SC & 0x80)); // wait for CHF
        FTM3_C6SC &= ~(1 << 7); //c6 continues to capture
        t2 = FTM3_C6V; // second edge

        FTM3_CNT = 0x4; //rising edge
        while(!(FTM3_C6SC & 0x80)); //wait for CHF
        FTM3_C6SC &= ~(1 << 7); //c6 continues to capture
        t3 = FTM3_C6V; //third edge

        if (t2 >= t1)
            pulse_width = (nr_overflows << 16) + (t2 - t1);
        else
        	pulse_width = ((nr_overflows-1) << 16) + (t2 - t1);

        if (t3 >= t1)
        	period = (nr_overflows << 16) + (t3 - t1);
        else
        	period = ((nr_overflows-1) << 16) + (t3-t1);

        duty_cycle = (pulse_width * 100)/period;
        
        //for (int i = 0; i < 100000; i++); // breakpoint here for debugging
    }
}
