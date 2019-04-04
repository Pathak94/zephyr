/** @file Si7006.h
 *
 * @brief Driver for Si7006 humidity and temperature sensor 
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2018 Electronut Labs.
 * All rights reserved. 
*/
 
#ifndef _SI7006_H
#define _SI7006_H

#include <sensor.h>
#include <kernel.h>
#include <device.h>
#include <init.h>
#include <string.h>
#include <misc/byteorder.h>
#include <misc/__assert.h>
#include <logging/log.h>
#include <i2c.h>
#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL
#include <logging/log.h>
#include <stdio.h>

LOG_MODULE_REGISTER(si7006);

/* Si7006 sensor address */
#define SI7006_ADDR 0x40

// Si7006 register addresses
#define Si7006_MEAS_REL_HUMIDITY_MASTER_MODE    0xE5
#define Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE 0xF5
#define Si7006_MEAS_TEMP_MASTER_MODE            0xE3
#define Si7006_MEAS_TEMP_NO_MASTER_MODE         0xF3
#define Si7006_READ_OLD_TEMP                    0xE0
#define Si7006_RESET 							0xFE
#define Si7006_WRITE_HUMIDITY_TEMP_CONTR		0xE6
#define Si7006_READ_HUMIDITY_TEMP_CONTR 		0xE7
#define Si7006_WRITE_HEATER_CONTR				0x51
#define Si7006_READ_HEATER_CONTR				0x11
#define Si7006_READ_ID_LOW_0					0xFA
#define Si7006_READ_ID_LOW_1					0x0F
#define Si7006_READ_ID_HIGH_0					0xFC
#define Si7006_READ_ID_HIGH_1					0xC9
#define Si7006_FIRMWARE_0						0x84
#define Si7006_FIRMWARE_1                       0xB8

/**
 * @brief function to read a byte
 * 
 * @return 0 for success
 */

static int read_bytes(struct device *i2c_dev, u8_t addr, u8_t *data,
		      u32_t num_bytes);
/**
 * @brief function to get relative humidity
 * 
 * @return float 
 */

float si7006_get_humidity(struct device *i2c_dev);

/**
 * @brief function to get temperature
 * 
 * @return float 
 */

float si7006_get_temperature(struct device *i2c_dev);

/**
 * @brief fetch a sample from the sensor 
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int si7006_sample_fetch(struct device *dev, enum sensor_channel chan);

/**
 * @brief sensor value get
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int si7006_channel_get(struct device *dev, enum sensor_channel chan,
			      struct sensor_value *val);

/**
 * @brief initiasize the sensor
 * 
 * @return 0 for success 
 */
static int si7006_init(struct device *dev);


#ifdef __cplusplus 
extern "C" { 
#endif


#ifdef __cplusplus 
}
#endif
 
#endif /* _SI7006_H */