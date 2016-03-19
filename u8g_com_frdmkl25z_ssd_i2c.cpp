/*  
  Special pin usage:
    U8G_PI_I2C_OPTION	additional options
    U8G_PI_A0_STATE	used to store the last value of the command/data register selection
    U8G_PI_SET_A0		1: Signal request to update I2C device with new A0_STATE, 0: Do nothing, A0_STATE matches I2C device
    U8G_PI_SCL		clock line (NOT USED)
    U8G_PI_SDA		data line (NOT USED)

    U8G_PI_RESET		reset line (currently disabled, see below)

  Protocol:
    SLA, Cmd/Data Selection, Arguments
    The command/data register is selected by a special instruction byte, which is sent after SLA

    The continue bit is always 0 so that a (re)start is equired for the change from cmd to/data mode
 */

#include "u8g.h"

#if defined(U8G_FRDMKL25Z_PI)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "mbed.h"

#define I2C_SLA			(0x3c << 1)
#define I2C_CMD_MODE    0x000
#define I2C_DATA_MODE   0x040

#define OLED_SCL	D15
#define OLED_SDA	D14

#if defined(U8G_WITH_PINLIST)

I2C m_i2c(D14, D15);
static unsigned char mode;

uint8_t u8g_com_frdmkl25z_ssd_start_sequence(u8g_t *u8g)
{

	int len;

	/* are we requested to set the a0 state? */
	if ( u8g->pin_list[U8G_PI_SET_A0] == 0 )
		return 1;
	//m_i2c.stop();
	//wait(0.100);

    m_i2c.start();

    if (m_i2c.write(I2C_SLA)) {
        return 0;
    }

	if ( u8g->pin_list[U8G_PI_A0_STATE] == 0 )
	{
		mode = I2C_CMD_MODE;
	}
	else
	{
		mode = I2C_DATA_MODE;
	}

    if (m_i2c.write(mode)) {
        return 0;
    }

	u8g->pin_list[U8G_PI_SET_A0] = 0;
	return 1;
}

uint8_t u8g_com_frdmkl25z_ssd_i2c_fn (u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	int len;
	unsigned char mode;
	int i;
	register uint8_t buf[256];
	unsigned char *aptr;

	switch(msg)
	{
	case U8G_COM_MSG_INIT:
		//mI2c = I2C(D14, D15);
		u8g_i2c_init(u8g->pin_list[U8G_PI_I2C_OPTION]);
		break;

	case U8G_COM_MSG_STOP:
		break;

	case U8G_COM_MSG_RESET:
		break;

	case U8G_COM_MSG_CHIP_SELECT:
		u8g->pin_list[U8G_PI_A0_STATE] = 0;
		u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again, also forces start condition */
		if(arg_val == 0) {
			m_i2c.stop();
		}
		break;

	case U8G_COM_MSG_WRITE_BYTE:
	{
		if ( u8g_com_frdmkl25z_ssd_start_sequence(u8g) == 0 ) {
			m_i2c.stop();
			return 0;
		}
#if 0
		buf[0] = mode;
		buf[1] = arg_val;

		len = m_i2c.write(I2C_SLA, (char *)&buf[0], 2);
#else
	    if (m_i2c.write(arg_val)) {
	    	m_i2c.stop();
	        return 0;
	    }
#endif
		break;
	}
	case U8G_COM_MSG_WRITE_SEQ:
	{
		aptr = (unsigned char *)arg_ptr;

		if ( u8g_com_frdmkl25z_ssd_start_sequence(u8g) == 0 ) {
			m_i2c.stop();
			return 0;
		}
		buf[0] = mode;
		for(i=0; i < arg_val; i++) {
//			buf[i+1] = aptr[i];
		    if (m_i2c.write(aptr[i])) {
		    	m_i2c.stop();
		        return 0;
		    }
		}
#if 0
		len = m_i2c.write(I2C_SLA, (char *)&buf[0], (arg_val+1));
#endif
		break;
	}
	case U8G_COM_MSG_WRITE_SEQ_P:
	{
		aptr = (unsigned char *)arg_ptr;

		if ( u8g_com_frdmkl25z_ssd_start_sequence(u8g) == 0 ) {
			m_i2c.stop();
			return 0;
		}

		buf[0] = mode;
		for(i=0; i < arg_val; i++) {
#if 0
			buf[i+1] = u8g_pgm_read(aptr);
#else
		    if (m_i2c.write(u8g_pgm_read(aptr))) {
		    	m_i2c.stop();
		        return 0;
		    }
#endif
			aptr++;
		}
#if 0
		len = m_i2c.write(I2C_SLA, (char *)&buf[0], (arg_val+1));
#endif
	}
		break;

	case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
		u8g->pin_list[U8G_PI_A0_STATE] = arg_val;
		u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again */
#if 0
		if(arg_val == 0) {
			mode = I2C_CMD_MODE;
		}else{
			mode = I2C_DATA_MODE;
		}
		len = m_i2c.write(I2C_SLA, (char *)&mode, 1);
#endif
		break;
	}
	return 1;
}

#else	/* defined(U8G_WITH_PINLIST) */

uint8_t u8g_com_frdmkl25z_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
	return 1;
}

#endif	/* defined(U8G_WITH_PINLIST) */

void u8g_Delay(uint16_t val)
{
	float mSec = val;
	wait(mSec / 1000);
}

#endif
