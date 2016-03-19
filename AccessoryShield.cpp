/*
 * AccessoryShield.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: neeveecomm
 */

#include "AccessoryShield.h"

/* Defintions */
#define PIN_HIGH	true
#define PIN_LOW		false

/* RGB LED Pin Definitions */
#define SHLD_LED_R 	D9
#define SHLD_LED_G 	D10
#define SHLD_LED_B 	D3

/* Buzzer Pin Definitions */
#define  BUZZER  	D8

/* Relay Pin Definitions */
#define  RELAY  	D11

/* JOY Stick Pin Definitions */
#define JOY_STICK	A0

/* POT KNOW Pin Definitions */
#define POT_KNOB	A1

/* Temperature Pin Definitions */
#define DHT11_PIN	D2
#define MAXTIMINGS 	85
#define DHTCOUNT 	6

/* OLED Pin Definitons */
#define OLED_DC		D7
#define OLED_RES	D6
#define OLED_SCL	D15
#define OLED_SDA	D14
#define OLED_ADDR

AccessoryShield::AccessoryShield() : red(SHLD_LED_R), green(SHLD_LED_G), blue(SHLD_LED_B),
									shldBuzz(BUZZER), shldRelay(RELAY), jsIn(JOY_STICK),
									knobIn(POT_KNOB), dhtTemp(DHT11_PIN), oledDC(OLED_DC),
									oledRes(OLED_RES), mI2C(OLED_SDA, OLED_SCL), mAddr(OLED_ADDR),
									dispOled(U8G_I2C_OPT_NONE)
{
	/* Initialize RGB LED */
	red.period(0.001f);
	green.period(0.001f);
	blue.period(0.001f);

	/* Initialize Joy Stick Key */
	lastKey = JS_KEY_NONE;

	/* Initialize DHT11 */
	dhtTemp.output();
	dhtTemp.write(PIN_HIGH);

	/* Initialize OLED */
	oledDC = 0;
	oledRes = 1; wait(0.100f);
	oledRes = 0; wait(0.100f);
	oledRes = 1; wait(0.100f);
}

AccessoryShield::~AccessoryShield()
{
}

void AccessoryShield::SetBuzzer(bool on)
{
	shldRelay = on;
}

void AccessoryShield::SetRelay(bool on)
{
	shldBuzz = on;
}

void AccessoryShield::SetLedColour(float r, float g, float b)
{
	red = r;
	green = g;
	blue = b;
}

int AccessoryShield::GetRed()
{
	float r = red;

	return (((1000 - r) * 100)/1000);
}

int AccessoryShield::GetGreen()
{
	float r = green;

	return (((1000 - r) * 100)/1000);
}

int AccessoryShield::GetBlue()
{
	float r = blue;

	return (((1000 - r) * 100)/1000);
}

JoyStick_t AccessoryShield::GetJoyStick(void)
{
	JoyStick_t joyStick = JS_KEY_NONE;
	unsigned int jsVal;

	jsVal = jsIn.read_u16();

	if(jsVal < 10000)
		joyStick = JS_KEY_RIGHT;
	else if((10000 <= jsVal) && (jsVal < 20000))
		joyStick = JS_KEY_PUSH;
	else if((20000 <= jsVal) && (jsVal < 32000))
		joyStick = JS_KEY_UP;
	else if((32000 <= jsVal) && (jsVal < 60000))
		joyStick = JS_KEY_LEFT;
	//else if((60000 <= jsVal) && (jsVal < 65536))
	//	joyStick = JS_KEY_DOWN;

	if(joyStick != lastKey) {
		lastKey = joyStick;
	}

	return joyStick;
}

int AccessoryShield::GetKnob(void)
{
	unsigned int knobVal;
	knobVal = knobIn.read_u16();
	return (knobVal * 100)/65535;
}

float AccessoryShield::readHumidity()
{
	float f = 0;

	if(DhtRead()) {
		f = dht11.buffer[0];
	}

	return f;
}

float AccessoryShield::readTemperature()
{
	float f = 0;

	if(DhtRead()) {
		f = dht11.buffer[2];
	}

	return f;
}

bool AccessoryShield::DhtRead()
{
	unsigned char i =0, j=0, counter = 0;
	bool lastState = PIN_HIGH;
	bool state;

	dht11 = {0};

	dhtTemp.output();
	dhtTemp.write(PIN_HIGH);
	wait(0.002f);

	dhtTemp.write(PIN_LOW);
	wait(0.020f);

	dhtTemp.write(PIN_HIGH);
	wait(0.000040f);

	dhtTemp.input();

	// read in timings
	for ( i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while ((state = dhtTemp.read()) == lastState) {
			counter++;
			wait(0.000001f);
			if (counter == 255) {
				break;
			}
		}
		lastState = dhtTemp.read();
		if (counter == 255) break;
		// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			dht11.buffer[j/8] <<= 1;
			if (counter > DHTCOUNT)
				dht11.buffer[j/8] |= 1;
			j++;
		}
	}

	if ((j >= 40) &&
			(dht11.buffer[4] == ((dht11.buffer[0] + dht11.buffer[1] + dht11.buffer[2] + dht11.buffer[3]) & 0xFF)) ) {
		return true;
	}

	return false;
}

void AccessoryShield::updateOledDisp (BlunoSheildInfo_t *bsInfo)
{
	dispOled.firstPage();
	do{

		dispOled.setFont(u8g_font_unifont_0_8 /*u8g_font_unifont*/);

		dispOled.setPrintPos(10,16);      //set the print position
		dispOled.print("H:");
		dispOled.print(bsInfo->humidity);         //show the humidity on oled
		dispOled.print("%");

		dispOled.setPrintPos(10,32);
		dispOled.print("T:");             //show the temperature on oled
		dispOled.print(bsInfo->temperature);
		dispOled.print("C");

		dispOled.setPrintPos(88,16);
		dispOled.print("R:");             //show RGBLED red value
		dispOled.print(bsInfo->ledRed);
		dispOled.setPrintPos(88,32);
		dispOled.print("G:");             //show RGBLED green value
		dispOled.print(bsInfo->ledGreen);
		dispOled.setPrintPos(88,48);
		dispOled.print("B:");             //show RGBLED blue value
		dispOled.print(bsInfo->ledBlue);

		dispOled.setPrintPos(10,48);
		dispOled.print("Knob:");
		dispOled.print(bsInfo->knob);             //show knob(potentiometer) value read from analog pin
		dispOled.print("%");

		dispOled.setPrintPos(10,60);
		dispOled.print("Joystick:");  //if string is null, show the state of joystick
        switch (bsInfo->joyStick){
            case JS_KEY_NONE:
            	dispOled.print("Normal");
                break;
            case JS_KEY_RIGHT:
            	dispOled.print("Right");
                break;
            case JS_KEY_UP:
            	dispOled.print("Up");
                break;
            case JS_KEY_LEFT:
            	dispOled.print("Left");
                break;
            case JS_KEY_DOWN:
            	dispOled.print("Down");
                break;
            case JS_KEY_PUSH:
            	dispOled.print("Push");
                break;
            default:
                break;
        }

	}
	while(dispOled.nextPage());
}
