/*****************************************************************************
* lab2a.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#define AO_LAB2A

#include "qpn_port.h"
#include "bsp.h"
#include "lab2a.h"
#include "lcd.h"
#define DISPLAY_TIMEOUT 1000 * 2



typedef struct Lab2ATag  {               //Lab2A State machine
	QActive super;

	int show;
	int counter;
	int volume;
	int mute;
	int label;
}  Lab2A;

/* Setup state machines */
/**********************************************************************/
static QState Lab2A_initial (Lab2A *me);
static QState Lab2A_on      (Lab2A *me);
static QState Lab2A_stateA  (Lab2A *me);
static QState Lab2A_stateB  (Lab2A *me);
static QState Lab2A_stateC  (Lab2A *me);
static QState Lab2A_stateD  (Lab2A *me);


/**********************************************************************/


Lab2A AO_Lab2A;

float MAX_VOLUME = 36.0;

void volumeUp();
void volumeDown();
void toggleMute();
void setLabel(int num);
void drawBackground();
void updateDisplay(int showVolume, int showLabel);

void Lab2A_ctor(void)  {
	Lab2A *me = &AO_Lab2A;
	QActive_ctor(&me->super, (QStateHandler)&Lab2A_initial);
	AO_Lab2A.show = 0;
	AO_Lab2A.counter = 0;
	AO_Lab2A.volume = 0;
	AO_Lab2A.mute = 0;
	AO_Lab2A.label = 0;
}


QState Lab2A_initial(Lab2A *me) {
	xil_printf("\n\rInitialization");

    return Q_TRAN(&Lab2A_on);
}

QState Lab2A_on(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("\n\rOn");
		}
			
		case Q_INIT_SIG: {
			return Q_TRAN(&Lab2A_stateA);
		}
	}
	
	return Q_SUPER(&QHsm_top);
}


/* Create Lab2A_on state and do any initialization code if needed */
/******************************************************************/

QState Lab2A_stateA(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			updateDisplay(0, 0);
			drawBackground();
			return Q_HANDLED();
		}
		
		case TICK: {
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			volumeUp();
			return Q_TRAN(&Lab2A_stateC);
		}

		case ENCODER_DOWN: {
			volumeDown();
			return Q_TRAN(&Lab2A_stateC);
		}

		case ENCODER_CLICK:  {
			toggleMute();
			return Q_TRAN(&Lab2A_stateC);
		}

		case BUTTON_LEFT:  {
			setLabel(2);
			return Q_TRAN(&Lab2A_stateB);
		}

		case BUTTON_TOP:  {
			setLabel(1);
			return Q_TRAN(&Lab2A_stateB);
		}

		case BUTTON_RIGHT:  {
			setLabel(4);
			return Q_TRAN(&Lab2A_stateB);
		}

		case BUTTON_BOTTOM:  {
			setLabel(5);
			return Q_TRAN(&Lab2A_stateB);
		}

		case BUTTON_CENTER:  {
			setLabel(3);
			return Q_TRAN(&Lab2A_stateB);
		}

	}

	return Q_SUPER(&Lab2A_on);

}

QState Lab2A_stateB(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			AO_Lab2A.counter = current_time;
			updateDisplay(0, 1);
			return Q_HANDLED();
		}
		

		case TICK: {
			if (current_time - AO_Lab2A.counter > DISPLAY_TIMEOUT){
				return Q_TRAN(&Lab2A_stateA);
			}
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			volumeUp();
			return Q_TRAN(&Lab2A_stateD);
		}

		case ENCODER_DOWN: {
			volumeDown();
			return Q_TRAN(&Lab2A_stateD);
		}

		case ENCODER_CLICK:  {
			toggleMute();
			return Q_TRAN(&Lab2A_stateD);
		}

		case BUTTON_LEFT:  {
			setLabel(2);
			updateDisplay(0, 1);
			return Q_HANDLED();
		}

		case BUTTON_TOP:  {
			setLabel(1);
			updateDisplay(0, 1);
			return Q_HANDLED();
		}

		case BUTTON_RIGHT:  {
			setLabel(4);
			updateDisplay(0, 1);
			return Q_HANDLED();
		}

		case BUTTON_BOTTOM:  {
			setLabel(5);
			updateDisplay(0, 1);
			return Q_HANDLED();
		}

		case BUTTON_CENTER:  {
			setLabel(3);
			updateDisplay(0, 1);
			return Q_HANDLED();
		}

	}

	return Q_SUPER(&Lab2A_on);

}

QState Lab2A_stateC(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			AO_Lab2A.counter = current_time;
			updateDisplay(1, 0);
			return Q_HANDLED();
		}


		case TICK: {
			if (current_time - AO_Lab2A.counter > DISPLAY_TIMEOUT){
				return Q_TRAN(&Lab2A_stateA);
			}
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			volumeUp();
			updateDisplay(1, 0);
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			volumeDown();
			updateDisplay(1, 0);
			return Q_HANDLED();
		}

		case ENCODER_CLICK:  {
			toggleMute();
			updateDisplay(1, 0);
			return Q_HANDLED();
		}

		case BUTTON_LEFT:  {
			setLabel(2);
			return Q_TRAN(&Lab2A_stateD);
		}

		case BUTTON_TOP:  {
			setLabel(1);
			return Q_TRAN(&Lab2A_stateD);
		}

		case BUTTON_RIGHT:  {
			setLabel(4);
			return Q_TRAN(&Lab2A_stateD);
		}

		case BUTTON_BOTTOM:  {
			setLabel(5);
			return Q_TRAN(&Lab2A_stateD);
		}

		case BUTTON_CENTER:  {
			setLabel(3);
			return Q_TRAN(&Lab2A_stateD);
		}

	}

	return Q_SUPER(&Lab2A_on);

}

QState Lab2A_stateD(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			updateDisplay(1, 1);
			return Q_HANDLED();
		}


		case TICK: {
			if (current_time - AO_Lab2A.counter > DISPLAY_TIMEOUT){
				return Q_TRAN(&Lab2A_stateA);
			}
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			volumeUp();
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			volumeDown();
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case ENCODER_CLICK:  {
			toggleMute();
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case BUTTON_LEFT:  {
			setLabel(2);
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case BUTTON_TOP:  {
			setLabel(1);
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case BUTTON_RIGHT:  {
			setLabel(4);
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case BUTTON_BOTTOM:  {
			setLabel(5);
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

		case BUTTON_CENTER:  {
			setLabel(3);
			updateDisplay(1, 1);
			return Q_HANDLED();
		}

	}

	return Q_SUPER(&Lab2A_on);

}

void volumeUp(){
	AO_Lab2A.counter = current_time;
	if(AO_Lab2A.volume < MAX_VOLUME && !AO_Lab2A.mute){
		AO_Lab2A.volume++;
	}
}

void volumeDown(){
	AO_Lab2A.counter = current_time;
	if(AO_Lab2A.volume > 0 && !AO_Lab2A.mute){
		AO_Lab2A.volume--;
	}
}

void toggleMute(){
	AO_Lab2A.counter = current_time;
	AO_Lab2A.mute =  AO_Lab2A.mute ? 0 : 1;
}

void setLabel(int num){
	AO_Lab2A.counter = current_time;
	AO_Lab2A.label = num;
}

void updateDisplay(int showVolume, int showLabel) {
	// cover old
	setColor(255, 255, 255);
	fillRect(20, 280, 220, 300);

	if (showVolume) {
		float perc = (float)(~AO_Lab2A.mute & 0x1) * (float) AO_Lab2A.volume / MAX_VOLUME;
		int width = (int) (198.0 * perc);
		setColor(0, 255, 0);
		fillRect(20, 280, width + 22, 300);
	}

	// cover old
	setColor(255, 255, 255);
	fillRect(80, 50, 160, 80);

	if(showLabel) {
//		setColorBg(255, 0, 00);
		switch(AO_Lab2A.label){
			case 1: {
				lcdPrint("Yooo", 90, 60);
				break;
			}
			case 2: {
				lcdPrint("Hello", 90, 60);
				break;
			}
			case 3: {
				lcdPrint("World", 90, 60);
				break;
			}
			case 4: {
				lcdPrint("What's", 90, 60);
				break;
			}
			case 5: {
				lcdPrint("Good", 90, 60);
				break;
			}
		}
	}
}

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

void drawBackground() {
	setColor(255, 255, 255);
	fillRect(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
	int xSize = 1;
	int ySize = 1;
	for(int xi = 0; (xi + 1) * xSize < DISP_X_SIZE; xi++){
		if (xi % 2 == 0) continue;
		for (int yi = 0; (yi + 1) * ySize < DISP_Y_SIZE; yi++){
			if (yi % 2 == 0) continue;

			float progX = (float) (xi * xSize) / 239.0;
			float progY = (float) (yi * ySize)  / 329.0;

			hsv old_color;
			old_color.h = progX;
			old_color.s = 0.8;
			old_color.v = 1.0 - progY;

			rgb color = hsv2rgb(old_color);
			setColor((int) (color.r * 255), (int) (color.g * 255), (int) (color.b * 255));
			fillRect(xi * xSize, yi * ySize, (xi + 1) * xSize, (yi + 1) * ySize);
			ySize++;
		}
		xSize++;
	}

}
