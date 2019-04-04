/** @file ltr329als01.c
 *
 * @brief driver for ltr329als01 light sensor. 
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2018 Electronut Labs.
 * All rights reserved. 
*/

#include "ltr329als01.h"

struct ltr329als01_data {
	struct device *i2c_dev;
	uint16_t lux_val;
};

/**
 * @brief function for write a byte
 * 
 * @return 0 for success
 */
static int write_byte(struct device *i2c_dev, u8_t addr, u8_t data)
{
	struct i2c_msg msg;

	u8_t buf[2] = { addr, data };
	/* Setup I2C messages */

	/* Send the address to write to */
	msg.buf = &buf[0];
	msg.len = 2;
	msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msg, 1, ALS_ADDR);
}

/**
 * @brief function to read a byte
 * 
 * @return 0 for success
 */
static int read_byte(struct device *i2c_dev, u8_t addr, u8_t *data)
{
	struct i2c_msg msgs[2];

	/* Send the address to read from */
	msgs[0].buf = &addr;
	msgs[0].len = 1;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = 1;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, ALS_ADDR);
}

/**
 * @brief check for new data
 * 
 * @return true 
 * @return false 
 */

bool ALS_check_for_new_valid_data(struct device *i2c_dev)
{
	uint8_t status;

	read_byte(i2c_dev, ALS_STATUS_REG, &status);

	if ((status & 0x04) && ((status & 0x80) == 0)) {
		return true;
	} else {
		return false;
	}
}

/**
 * @brief get integration time in ms from ALS_MEAS_RATE register
 * 
 * @return int 
 */
int ALS_get_integration_time(struct device *i2c_dev)
{
	int integration_time_lookup[8] = {
		100, 50, 200, 400, 150, 250, 300, 350
	};

	uint8_t meas_rate_reg_data;

	if (read_byte(i2c_dev, ALS_MEAS_RATE_REG, &meas_rate_reg_data) != 0) {
		LOG_ERR("get ch err");
	}

	uint8_t integration_time_byte = (meas_rate_reg_data & 0x38) >> 3;

	int als_integration_time_ms =
		integration_time_lookup[integration_time_byte];

	return als_integration_time_ms;
}

/**
 * @brief get adc values of channel-0 and channel-1 from als data registers
 * 
 * @return uint16_t* 
 */
uint16_t *ALS_get_channels_data(struct device *i2c_dev)
{
	uint8_t als_buffer[4];
	static uint16_t als_channels_data[2];

	for (int i = 0; i < 4; i++) {
		if (read_byte(i2c_dev, ALS_DATA_CH1_0_REG + i,
			      &als_buffer[i]) != 0) {
			LOG_ERR("get ch err");
		}
	}

	als_channels_data[0] = (als_buffer[1] << 8) | als_buffer[0];
	als_channels_data[1] = (als_buffer[3] << 8) | als_buffer[2];

	LOG_DBG("channels %d %d", als_channels_data[0], als_channels_data[1]);

	return als_channels_data;
}

/**
 * @brief get gain values from ALS_CONTR register
 * 
 * @return int 
 */
int ALS_get_gain(struct device *i2c_dev)
{
	int gain_lookup[8] = { 1, 2, 4, 8, RESERVED, RESERVED, 48, 96 };

	uint8_t contr_reg_data;
	read_byte(i2c_dev, ALS_CONTR_REG, &contr_reg_data);

	uint8_t als_gain_byte = (contr_reg_data & 0x1C) >> 2;

	int als_gain = gain_lookup[als_gain_byte];

	return als_gain;
}

/**
 * @brief calculate lux values from lux buffer
 * 
 * @return float 
 */
float ALS_get_lux(struct device *i2c_dev)
{
	uint16_t *als_channels_data;

	als_channels_data = ALS_get_channels_data(i2c_dev);

	//LOG_DBG("LTR  %x %x", als_channels_data[0], als_channels_data[1]);

	float ratio = (float)als_channels_data[1] /
		      (float)(als_channels_data[0] + als_channels_data[1]);

	int ALS_GAIN = ALS_get_gain(i2c_dev);

	float ALS_INT = (float)ALS_get_integration_time(i2c_dev) / 100;

	float als_lux;

	if (ratio < 0.45) {
		als_lux = ((1.7743 * (float)als_channels_data[0]) +
			   (1.1059 * (float)als_channels_data[1])) /
			  (ALS_GAIN * ALS_INT);
	} else if (ratio < 0.64 && ratio >= 0.45) {
		als_lux = ((4.2785 * (float)als_channels_data[0]) -
			   (1.9548 * (float)als_channels_data[1])) /
			  (ALS_GAIN * ALS_INT);
	} else if (ratio < 0.85 && ratio >= 0.64) {
		als_lux = ((0.5926 * (float)als_channels_data[0]) +
			   (0.1185 * (float)als_channels_data[1])) /
			  (ALS_GAIN * ALS_INT);
	} else {
		als_lux = 0;
	}
	return als_lux;
}

/**
 * @brief fetch a sample from the sensor 
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int ltr329als01_sample_fetch(struct device *dev,
				    enum sensor_channel chan)
{
	struct ltr329als01_data *ltr_data = dev->driver_data;

	if (chan == SENSOR_CHAN_ALL || chan == SENSOR_CHAN_LIGHT) {
		if (ALS_check_for_new_valid_data(ltr_data->i2c_dev) == true) {
			float als_val = ALS_get_lux(ltr_data->i2c_dev);
			ltr_data->lux_val = als_val;
			return 0;
		}
		else
		{
			return EAGAIN;
		}
		
	} else {
		return -ENOTSUP;
	}
}

/**
 * @brief sensor value get
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int ltr329als01_channel_get(struct device *dev, enum sensor_channel chan,
				   struct sensor_value *val)
{
	struct ltr329als01_data *ltr_data = dev->driver_data;

	if (chan == SENSOR_CHAN_ALL || chan == SENSOR_CHAN_LIGHT) {
		float als_val = ltr_data->lux_val;
		val->val1 = (s32_t)als_val;
		val->val2 = ((s32_t)(als_val * 1000000)) % 1000000;
		LOG_DBG("val1=%d, val2=%d", val->val1, val->val2);
		return 0;

	} else {
		return -ENOTSUP;
	}
}

static const struct sensor_driver_api ltr329als01_api = {
	.sample_fetch = &ltr329als01_sample_fetch,
	.channel_get = &ltr329als01_channel_get,
};

static int ltr329als01_init(struct device *dev)
{
	struct ltr329als01_data *drv_data = dev->driver_data;
	uint8_t manf_id = 0;

	k_sleep(100); // wait after power on

	LOG_INF("ltr329als01_init\r\n");

	drv_data->i2c_dev = device_get_binding("I2C_0");
	if (!drv_data->i2c_dev) {
		LOG_ERR("i2c master not found.");
		return -EINVAL;
	}

	if (read_byte(drv_data->i2c_dev, MANUFAC_ID_REG, &manf_id) == 0) {
		LOG_DBG("found! %x \r\n", manf_id);
	} else {
		return -EIO;
	}

	LOG_DBG("ltr_init is ok");

	// set measurement and intergration time
	uint8_t mea_int_time = 0x1b;
	if (write_byte(drv_data->i2c_dev, ALS_MEAS_RATE_REG, mea_int_time) !=
	    0) {
		LOG_ERR("measurement and intergration time not set!");
	}

	/*enable ltr sensor - set to active mode  For Int time = 400ms, Meas 
    rate = 500ms, Command = 0x1B*/
	uint8_t enable_ltr = 0x01;
	if (write_byte(drv_data->i2c_dev, ALS_CONTR_REG, enable_ltr) != 0) {
		LOG_ERR("not active");
	}

	k_sleep(10); // wait after active mode set

	return 0;
}

static struct ltr329als01_data ltr_data;

DEVICE_AND_API_INIT(ltr329als01, "LTR_0", ltr329als01_init, &ltr_data, NULL,
		    POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &ltr329als01_api);