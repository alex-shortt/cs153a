#include "rotary.h"
#include <stdlib.h>
#include "xgpio.h"
#include "fsm.h"

XIntc sys_intc;
XTmrCtr sys_tmrctr;
static u16 GlobalIntrMask = 1;

XGpio encoder;
u32 encoder_result;
u32 prev_encoder_result = 7;

// turn fsm
int cur_turn_state_index = 0;
int turn_state_output = 0;
int button_pushed = 0;

void encoder_handler(void *CallbackRef) {
	XGpio *GpioPtr = (XGpio *)CallbackRef;

	encoder_result = XGpio_DiscreteRead(&encoder, 1);
	int encoder_result_pins = encoder_result & 3;
	int encoder_result_push = (encoder_result & 4) >> 2;

	int turn_result = move_turn_fsm(cur_turn_state_index, encoder_result_pins);
	//	xil_printf("move from %d to %d\n", cur_turn_state_index, turn_result >> 4);
	cur_turn_state_index = turn_result >> 4;
	int output_val = (turn_result & 15);

	if(output_val){
		turn_state_output = output_val;
	}

	if (encoder_result_push && button_pushed == 0) {
		button_pushed = 1;
	}

	prev_encoder_result = encoder_result;

	XGpio_InterruptClear(GpioPtr, GlobalIntrMask);
}

int setup_encoder_intr()
{
	XStatus Status = XST_SUCCESS;
	XGpio_Initialize(&encoder, XPAR_ENCODER_DEVICE_ID);

	Status = XIntc_Initialize(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	if ( Status != XST_SUCCESS ) return XST_FAILURE;

	Xil_ExceptionHandler handler = (Xil_ExceptionHandler)encoder_handler;
	Status = XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_2_VEC_ID, handler, &encoder);
	if ( Status != XST_SUCCESS ) return XST_FAILURE;

	Status = XIntc_Start(&sys_intc, XIN_REAL_MODE);
	if ( Status != XST_SUCCESS ) return XST_FAILURE;

	XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_2_VEC_ID);

	Status = XTmrCtr_Initialize(&sys_tmrctr, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	if ( Status != XST_SUCCESS ) return XST_FAILURE;

	XGpio_InterruptEnable(&encoder, GlobalIntrMask);
	XGpio_InterruptGlobalEnable(&encoder);

	microblaze_enable_interrupts();

	return XST_SUCCESS;
}
