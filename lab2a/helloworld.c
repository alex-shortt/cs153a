#include <stdio.h>
#include <math.h>
#include "xparameters.h"	// Contains hardware addresses and bit masks
#include "xil_cache.h"		// Cache Drivers
#include "xintc.h"		// Interrupt Drivers
#include "xtmrctr.h"		// Timer Drivers
#include "xtmrctr_l.h" 		// Low-level timer drivers
#include "xil_printf.h" 	// Used for xil_printf()
#include "xgpio.h"

#include "rotary.h"
#include "rgbleds.h"

#define TIMER_INTERRUPT_HZ 100000 // 100000 for seconds

XGpio led;

int main()
{
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();

	setup_rgbleds();
	setup_encoder_intr();

	XGpio_Initialize(&led, XPAR_AXI_GPIO_LED_DEVICE_ID);

	u32 time = 0;
	u32 sec = 0;

	int show = 1;
	int index = 0;

	int button_time = 0;
	int BUTTON_TIMEOUT = TIMER_INTERRUPT_HZ / 2;

	while (1) {
		time++;
		sec = (time / TIMER_INTERRUPT_HZ) % 10;

		if(sec % 2 == 0) write_rgbleds(0b010000);
		else write_rgbleds(0b000010);

		if(button_pushed == 1){
			button_pushed = 2;
			button_time = time;
			show = !show;
			if(!show){
				XGpio_DiscreteWrite(&led, 1, 0);
			}
		}

		if(button_pushed == 2 && (time - button_time) > BUTTON_TIMEOUT){
			button_pushed = 0;
		}

		if(!show){
			turn_state_output = 0;
		}

		if(turn_state_output == 1){
			index--;
			if(index < 0) index = 15;
			turn_state_output = 0;
		} else if (turn_state_output == 2){
			index ++;
			if(index > 15) index = 0;
			turn_state_output = 0;
		}


		if (show){
			XGpio_DiscreteWrite(&led, 1, 1 << index);
		}
	}

	return 0;
}
