/** @file module.h
 *
 * @brief A description of the module’s purpose. 
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2018 Electronut Labs.
 * All rights reserved. 
*/
#ifndef _LTR329ALS01_H
#define _LTR329ALS01_H
 
#ifdef __cplusplus 
extern "C" { 
#endif

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

LOG_MODULE_REGISTER(ltr329als01);
/* Light sensor address */
#define ALS_ADDR 0x29

/* Light sensor registers */
#define ALS_CONTR_REG 0x80
#define ALS_MEAS_RATE_REG 0x85
#define PART_ID_REG 0x86
#define MANUFAC_ID_REG 0x87
#define ALS_DATA_CH1_0_REG 0x88
#define ALS_DATA_CH1_1_REG 0x89
#define ALS_DATA_CH0_0_REG 0x8A
#define ALS_DATA_CH0_1_REG 0x8B
#define ALS_STATUS_REG 0x8C

#define RESERVED -1

/**
 * @brief function for write a byte
 * 
 * @return 0 for success
 */
static int write_byte(struct device *i2c_dev, u8_t addr, u8_t data);

/**
 * @brief function to read a byte
 * 
 * @return 0 for success
 */
static int read_byte(struct device *i2c_dev, u8_t addr, u8_t *data);

/**
 * @brief check for new data
 * 
 * @return true 
 * @return false 
 */
bool ALS_check_for_new_valid_data(struct device *i2c_dev);

/**
 * @brief get integration time in ms from ALS_MEAS_RATE register
 * 
 * @return int 
 */
int ALS_get_integration_time(struct device *i2c_dev);

/**
 * @brief get adc values of channel-0 and channel-1 from als data registers
 * 
 * @return uint16_t* 
 */
uint16_t *ALS_get_channels_data(struct device *i2c_dev);

/**
 * @brief get gain values from ALS_CONTR register
 * 
 * @return int 
 */
int ALS_get_gain(struct device *i2c_dev);

/**
 * @brief calculate lux values from lux buffer
 * 
 * @return float 
 */
float ALS_get_lux(struct device *i2c_dev);

/**
 * @brief fetch a sample from the sensor 
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int ltr329als01_sample_fetch(struct device *dev,
				    enum sensor_channel chan);

/**
 * @brief sensor value get
 * 
 * @return 0 for success and EAGAIN for fail
 */
static int ltr329als01_channel_get(struct device *dev, enum sensor_channel chan,
				   struct sensor_value *val);

/**
 * @brief initiasize the sensor
 * 
 * @return 0 for success 
 */
static int ltr329als01_init(struct device *dev);

#ifdef __cplusplus 
}
#endif
 
#endif /* _MODULE_H */