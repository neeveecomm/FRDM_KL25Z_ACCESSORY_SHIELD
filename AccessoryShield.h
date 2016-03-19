/*
 * AccessoryShield.h
 *
 *  Created on: Mar 12, 2016
 *      Author: neeveecomm
 */

#ifndef SOURCES_BLUNOSHIELD_ACCESSORYSHIELD_H_
#define SOURCES_BLUNOSHIELD_ACCESSORYSHIELD_H_

#include "mbed.h"
#include "U8glib.h"

typedef enum {
	JS_KEY_NONE	= 0,
	JS_KEY_RIGHT,
	JS_KEY_LEFT,
	JS_KEY_UP,
	JS_KEY_DOWN,
	JS_KEY_PUSH,
	JS_KEY_MAX,
}JoyStick_t;

typedef struct {
	int humidity;
	int temperature;

	int ledRed;
	int ledGreen;
	int ledBlue;

	int knob;
	JoyStick_t joyStick;
}BlunoSheildInfo_t;

typedef union {
	unsigned char buffer[6];
}DhtTemp_t;

class AccessoryShield {
	PwmOut 		red;
	PwmOut 		green;
	PwmOut 		blue;
	DigitalOut 	shldBuzz;
	DigitalOut 	shldRelay;
	AnalogIn   	jsIn;
	AnalogIn	knobIn;
	JoyStick_t	lastKey;
	DigitalInOut dhtTemp;
	DhtTemp_t	dht11;
	DigitalOut	oledDC;
	DigitalOut	oledRes;
	I2C			mI2C;
	U8GLIB_SSD1306_128X64 dispOled;
	int			mAddr;
	virtual bool DhtRead();

public:
	AccessoryShield();
	virtual ~AccessoryShield();

	virtual void SetBuzzer(bool on);
	virtual void SetRelay(bool on);
	virtual void SetLedColour(float r, float g, float b);
	virtual JoyStick_t GetJoyStick(void);
	virtual int GetKnob(void);
	virtual float readHumidity();
	virtual float readTemperature();
	virtual void updateOledDisp (BlunoSheildInfo_t *bsInfo);

	virtual int GetRed();
	virtual int GetGreen();
	virtual int GetBlue();
};

#endif /* SOURCES_BLUNOSHIELD_ACCESSORYSHIELD_H_ */
