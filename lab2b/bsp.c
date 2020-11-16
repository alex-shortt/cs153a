/*****************************************************************************
* bsp.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 27,2019
*****************************************************************************/

/**/
#include "qpn_port.h"
#include "bsp.h"
#include "lab2a.h"
#include "xintc.h"
#include "xgpio.h"
#include "xil_exception.h"
#include "fsm.h"
#include "xspi.h"
#include "xspi_l.h"
#include "lcd.h"
#include "xintc.h"		// Interrupt Drivers
#include "xtmrctr.h"		// Timer Drivers
#include "xtmrctr_l.h" 		// Low-level timer drivers

/*****************************/

/* Define all variables and Gpio objects here  */

#define GPIO_CHANNEL1 1
static u16 GlobalIntrMask = 1;

void debounceInterrupt(); // Write This function

// Create ONE interrupt controllers XIntc
XIntc sys_intc;
// Create two static XGpio variables
static XGpio encoder;
int last_enc_press_time = 0;
static XGpio buttons;
int last_btn_press_time = 0;
// Suggest Creating two int's to use for determining the direction of twist
int cur_turn_state_index = 0;

// display
static XGpio dc;
static XSpi spi;
u32 controlReg;
XSpi_Config *spiConfig;

// timer
static XTmrCtr axiTimer;
unsigned int current_time = 0;
int BUTTON_DEBOUNCE_TIME = 100;

void timer_counter_handler(void *CallBackRef, u8 TmrCtrNumber);


/*..........................................................................*/

void BSP_init(void) {
/* Setup LED's, etc */
/* Setup interrupts and reference to interrupt handler function(s)  */

	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	 *
	 * Initialize GPIO and connect the interrupt controller to the GPIO.
	 *
	 */

	XIntc_Initialize(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	XIntc_Start(&sys_intc, XIN_REAL_MODE);

	// setup encoder
	XGpio_Initialize(&encoder, XPAR_ENCODER_DEVICE_ID);
	Xil_ExceptionHandler encoder_handled = (Xil_ExceptionHandler)TwistHandler;
	XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_2_VEC_ID, encoder_handled, &encoder);
	XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_2_VEC_ID);
	XGpio_InterruptEnable(&encoder, GlobalIntrMask);
	XGpio_InterruptGlobalEnable(&encoder);

	// setup buttons
	XGpio_Initialize(&buttons, XPAR_AXI_GPIO_BTN_DEVICE_ID);
	Xil_ExceptionHandler buttons_handled = (Xil_ExceptionHandler)GpioHandler;
	XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_0_VEC_ID, buttons_handled, &buttons);
	XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_0_VEC_ID);
	XGpio_InterruptEnable(&buttons, GlobalIntrMask);
	XGpio_InterruptGlobalEnable(&buttons);

	// setup spi
	XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);
	XGpio_SetDataDirection(&dc, 1, 0x0);
	spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
	XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress);
	XSpi_Reset(&spi);
	controlReg = XSpi_GetControlReg(&spi);
	XSpi_SetControlReg(&spi,
				(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
				(~XSP_CR_TRANS_INHIBIT_MASK));
	XSpi_SetSlaveSelectReg(&spi, ~0x01);

	//setup timer
	XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
	XIntc_Connect(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
					(XInterruptHandler)XTmrCtr_InterruptHandler,
					(void *)&axiTimer);
	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	XTmrCtr_SetHandler(&axiTimer, timer_counter_handler, &axiTimer);
	XTmrCtr_SetOptions(&axiTimer, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&axiTimer, 0, 0xFFFF0000);
	XTmrCtr_Start(&axiTimer, 0);

	// init lcd
	initLCD();
	clrScr();

	microblaze_enable_interrupts();
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

/* Enable interrupts */
	xil_printf("\n\rQF_onStartup\n"); // Comment out once you are in your complete program

	// Press Knob
	// Enable interrupt controller
	// Start interupt controller
	// register handler with Microblaze
	// Global enable of interrupt
	// Enable interrupt on the GPIO

	// Twist Knob

	// General
	// Initialize Exceptions
	// Press Knob
	// Register Exception
	// Twist Knob
	// Register Exception
	// General
	// Enable Exception

	// Variables for reading Microblaze registers to debug your interrupts.
//	{
//		u32 axi_ISR =  Xil_In32(intcPress.BaseAddress + XIN_ISR_OFFSET);
//		u32 axi_IPR =  Xil_In32(intcPress.BaseAddress + XIN_IPR_OFFSET);
//		u32 axi_IER =  Xil_In32(intcPress.BaseAddress + XIN_IER_OFFSET);
//		u32 axi_IAR =  Xil_In32(intcPress.BaseAddress + XIN_IAR_OFFSET);
//		u32 axi_SIE =  Xil_In32(intcPress.BaseAddress + XIN_SIE_OFFSET);
//		u32 axi_CIE =  Xil_In32(intcPress.BaseAddress + XIN_CIE_OFFSET);
//		u32 axi_IVR =  Xil_In32(intcPress.BaseAddress + XIN_IVR_OFFSET);
//		u32 axi_MER =  Xil_In32(intcPress.BaseAddress + XIN_MER_OFFSET);
//		u32 axi_IMR =  Xil_In32(intcPress.BaseAddress + XIN_IMR_OFFSET);
//		u32 axi_ILR =  Xil_In32(intcPress.BaseAddress + XIN_ILR_OFFSET) ;
//		u32 axi_IVAR = Xil_In32(intcPress.BaseAddress + XIN_IVAR_OFFSET);
//		u32 gpioTestIER  = Xil_In32(sw_Gpio.BaseAddress + XGPIO_IER_OFFSET);
//		u32 gpioTestISR  = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_ISR_OFFSET ) & 0x00000003; // & 0xMASK
//		u32 gpioTestGIER = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_GIE_OFFSET ) & 0x80000000; // & 0xMASK
//	}
}


void QF_onIdle(void) {        /* entered with interrupts locked */

    QF_INT_UNLOCK();                       /* unlock interrupts */

    {
    	// Write code to increment your interrupt counter here.
    	 QActive_postISR((QActive *)&AO_Lab2A, TICK); //is used to post an event to your FSM



// 			Useful for Debugging, and understanding your Microblaze registers.
//    		u32 axi_ISR =  Xil_In32(intcPress.BaseAddress + XIN_ISR_OFFSET);
//    	    u32 axi_IPR =  Xil_In32(intcPress.BaseAddress + XIN_IPR_OFFSET);
//    	    u32 axi_IER =  Xil_In32(intcPress.BaseAddress + XIN_IER_OFFSET);
//
//    	    u32 axi_IAR =  Xil_In32(intcPress.BaseAddress + XIN_IAR_OFFSET);
//    	    u32 axi_SIE =  Xil_In32(intcPress.BaseAddress + XIN_SIE_OFFSET);
//    	    u32 axi_CIE =  Xil_In32(intcPress.BaseAddress + XIN_CIE_OFFSET);
//    	    u32 axi_IVR =  Xil_In32(intcPress.BaseAddress + XIN_IVR_OFFSET);
//    	    u32 axi_MER =  Xil_In32(intcPress.BaseAddress + XIN_MER_OFFSET);
//    	    u32 axi_IMR =  Xil_In32(intcPress.BaseAddress + XIN_IMR_OFFSET);
//    	    u32 axi_ILR =  Xil_In32(intcPress.BaseAddress + XIN_ILR_OFFSET) ;
//    	    u32 axi_IVAR = Xil_In32(intcPress.BaseAddress + XIN_IVAR_OFFSET);
//
//    	    // Expect to see 0x00000001
//    	    u32 gpioTestIER  = Xil_In32(sw_Gpio.BaseAddress + XGPIO_IER_OFFSET);
//    	    // Expect to see 0x00000001
//    	    u32 gpioTestISR  = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_ISR_OFFSET ) & 0x00000003;
//
//    	    // Expect to see 0x80000000 in GIER
//    		u32 gpioTestGIER = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_GIE_OFFSET ) & 0x80000000;


    }
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}

/* Interrupt handler functions here.  Do not forget to include them in lab2a.h!
To post an event from an ISR, use this template:
QActive_postISR((QActive *)&AO_Lab2A, SIGNALHERE);
Where the Signals are defined in lab2a.h  */

/******************************************************************************
*
* This is the interrupt handler routine for the GPIO for this example.
*
******************************************************************************/
void timer_counter_handler(void *CallBackRef, u8 TmrCtrNumber)
{
	Xuint32 ControlStatusReg;
	ControlStatusReg = XTimerCtr_ReadReg(axiTimer.BaseAddress, 0, XTC_TCSR_OFFSET); // Read the new Control/Status Register content.

	current_time++;
	XTmrCtr_WriteReg(axiTimer.BaseAddress, 0, XTC_TCSR_OFFSET, ControlStatusReg |XTC_CSR_INT_OCCURED_MASK);
}

void GpioHandler(void *CallbackRef) {
	XGpio *GpioPtr = (XGpio *)CallbackRef;

	if(current_time - last_btn_press_time < BUTTON_DEBOUNCE_TIME){
		return;
	}

	int btn_result = XGpio_DiscreteRead(&buttons, 1);

	switch(btn_result){
		case 2:
			QActive_postISR((QActive *)&AO_Lab2A, BUTTON_LEFT);
			break;
		case 16:
			QActive_postISR((QActive *)&AO_Lab2A, BUTTON_CENTER);
			break;
		case 4:
			QActive_postISR((QActive *)&AO_Lab2A, BUTTON_RIGHT);
			break;
		case 32:
			QActive_postISR((QActive *)&AO_Lab2A, BUTTON_TOP);
			break;
		case 8:
			QActive_postISR((QActive *)&AO_Lab2A, BUTTON_BOTTOM);
			break;
	}

	last_btn_press_time = current_time;

	XGpio_InterruptClear(GpioPtr, GlobalIntrMask);
}

void debounceTwistInterrupt(u32 encoder_result){
	int encoder_result_pins = encoder_result & 3;

	int turn_result = move_turn_fsm(cur_turn_state_index, encoder_result_pins);
	cur_turn_state_index = turn_result >> 4;
	int output_val = (turn_result & 15);

	if (output_val) {
		QActive_postISR((QActive *)&AO_Lab2A, output_val == 1 ? ENCODER_UP : ENCODER_DOWN);
	}
}

void debounceInterrupt(u32 encoder_result) {
	if(current_time - last_enc_press_time < BUTTON_DEBOUNCE_TIME){
		return;
	}

	int encoder_result_push = (encoder_result & 4) >> 2;
	if (encoder_result_push) {
		QActive_postISR((QActive *)&AO_Lab2A, ENCODER_CLICK);

		last_enc_press_time = current_time;
	}
}

void TwistHandler(void *CallbackRef) {
	XGpio *GpioPtr = (XGpio *)CallbackRef;

	u32 encoder_result = XGpio_DiscreteRead(&encoder, 1);

	debounceTwistInterrupt(encoder_result);
	debounceInterrupt(encoder_result);

	XGpio_InterruptClear(GpioPtr, GlobalIntrMask);
}
