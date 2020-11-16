#include "xgpio.h"

XGpio rgbled;

void setup_rgbleds(){
	XGpio_Initialize(&rgbled, XPAR_AXI_GPIO_RGBLEDS_DEVICE_ID);
//	XGpio_SetDataDirection(&rgbled, 1, 0x0);
}

void write_rgbleds(int val) {
	XGpio_DiscreteWrite(&rgbled, 1, val);
}
