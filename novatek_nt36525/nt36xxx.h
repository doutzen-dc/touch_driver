 /*
 * Copyright (C) 2010 - 2017 Novatek, Inc.
 *
 * $Revision: 11112 $
 * $Date: 2017-04-05 11:07:39 +0800 (週三, 05 四月 2017) $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
#ifndef 	_LINUX_NVT_TOUCH_H
#define		_LINUX_NVT_TOUCH_H

#include <linux/i2c.h>
#include <linux/input.h>

//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 Begin
#include "nt36xxx_mem_map.h"
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 End

#define NVT_DEBUG 1

#define NVT_GPIO_AS_INT(pin) tpd_gpio_as_int(pin)
#define NVT_GPIO_OUTPUT(pin, level) tpd_gpio_output(pin, level)

//---INT trigger mode---
//#define EINTF_TRIGGER_RISING     0x00000001
//#define EINTF_TRIGGER_FALLING    0x00000002
#define INT_TRIGGER_TYPE IRQF_TRIGGER_RISING

//---I2C driver info.---
#define NVT_I2C_NAME "NVT-ts"
#define I2C_BLDR_Address 0x01
#define I2C_FW_Address 0x01
#define I2C_HW_Address 0x62

#if NVT_DEBUG
//Chenyee <CY_BSP_CTP> <xuel> <2018-02-02> Modify For #CSW1705A-1523 Begin
#define NVT_LOG(fmt, args...)    pr_debug("[%s] %s %d: " fmt, NVT_I2C_NAME, __func__, __LINE__, ##args)
//Chenyee <CY_BSP_CTP> <xuel> <2018-02-02> Modify For #CSW1705A-1523 End
#else
#define NVT_LOG(fmt, args...)    pr_info("[%s] %s %d: " fmt, NVT_I2C_NAME, __func__, __LINE__, ##args)
#endif
#define NVT_ERR(fmt, args...)    pr_err("[%s] %s %d: " fmt, NVT_I2C_NAME, __func__, __LINE__, ##args)

//---Input device info.---
#define NVT_TS_NAME "NVTCapacitiveTouchScreen"

//---Touch info.---
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 Begin
#define TOUCH_DEFAULT_MAX_WIDTH 720
#define TOUCH_DEFAULT_MAX_HEIGHT 1440
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 End
//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 Begin
#define TOUCH_MAX_FINGER_NUM 5  //10
//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 End
#define TOUCH_KEY_NUM 0
#if TOUCH_KEY_NUM > 0
extern const uint16_t touch_key_array[TOUCH_KEY_NUM];
#endif
#define TOUCH_FORCE_NUM 1000

//---Customerized func.---
#define NVT_TOUCH_PROC 1
#define NVT_TOUCH_EXT_PROC 1
#define NVT_TOUCH_MP 1
#define MT_PROTOCOL_B 1
#define WAKEUP_GESTURE 1
//#if WAKEUP_GESTURE
//extern const uint16_t gesture_key_array[];
//#endif
//Chenyee <CY_BSP_CTP> <xuel> <2018-04-20> Modify For #CSW1705A-2568 Begin
#define BOOT_UPDATE_FIRMWARE 1
#define BOOT_UPDATE_FIRMWARE_NAME "novatek/nt36525b/CSW1705_NT36525B_TM_0570_V13_20180420.bin"//"novatek_ts_fw.bin"
//Chenyee <CY_BSP_CTP> <xuel> <2018-04-20> Modify For #CSW1705A-2568 End

//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 Begin
//---ESD Protect.---
//Chenyee <CY_BSP_CTP> <daicheng> <2018-02-08> Modify For #CSW1705A-1600 Begin
#define NVT_TOUCH_ESD_PROTECT 1
//Chenyee <CY_BSP_CTP> <daicheng> <2018-02-08> Modify For #CSW1705A-1600 End
#define NVT_TOUCH_ESD_CHECK_PERIOD 1500	/* ms */
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 End

//--I2C DMA info.---
#define I2C_DMA_SUPPORT 1
#define DMA_MAX_TRANSACTION_LENGTH        255   // for DMA mode
#define DMA_MAX_I2C_TRANSFER_SIZE        (DMA_MAX_TRANSACTION_LENGTH - 1)
#define MAX_TRANSACTION_LENGTH            8
#define MAX_I2C_TRANSFER_SIZE            (MAX_TRANSACTION_LENGTH - 1)

struct nvt_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct work_struct nvt_work;
	struct delayed_work nvt_fwu_work;
	uint16_t addr;
	int8_t phys[32];
	uint8_t fw_ver;
	uint8_t x_num;
	uint8_t y_num;
	uint16_t abs_x_max;
	uint16_t abs_y_max;
	uint8_t max_touch_num;
	uint8_t max_button_num;
	uint32_t int_trigger_type;
	int32_t irq_gpio;
	uint32_t irq_flags;
	int32_t reset_gpio;
	uint32_t reset_flags;
	struct mutex lock;
	const struct nvt_ts_mem_map *mmap;
	uint8_t carrier_system;
	//Chenyee <CY_BSP_CTP> <xuel> <2017-07-21> Modify For 168662 Begin
	uint16_t nvt_pid;
	//Chenyee <CY_BSP_CTP> <xuel> <2017-07-21> Modify For 168662 End
};

#if NVT_TOUCH_PROC
struct nvt_flash_data{
	rwlock_t lock;
	struct i2c_client *client;
};
#endif

typedef enum {
	RESET_STATE_INIT = 0xA0,// IC reset
	RESET_STATE_REK,		// ReK baseline
	RESET_STATE_REK_FINISH,	// baseline is ready
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 Begin
	RESET_STATE_NORMAL_RUN,	// normal run
	RESET_STATE_MAX  = 0xAF
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 End
} RST_COMPLETE_STATE;

typedef enum {
    EVENT_MAP_HOST_CMD                      = 0x50,
    EVENT_MAP_HANDSHAKING_or_SUB_CMD_BYTE   = 0x51,
    EVENT_MAP_RESET_COMPLETE                = 0x60,
    EVENT_MAP_FWINFO                        = 0x78,
	//Chenyee <CY_BSP_CTP> <xuel> <2017-07-21> Modify For 168662 Begin
    EVENT_MAP_PROJECTID                     = 0x9A,
	//Chenyee <CY_BSP_CTP> <xuel> <2017-07-21> Modify For 168662 End
    //Chenyee <CY_BSP_CTP> <xuel> <2017-12-22> Modify For #CSW1705A-12 Begin
    EVENT_MAP_VENDORID                      = 0x4E,
    //Chenyee <CY_BSP_CTP> <xuel> <2017-12-22> Modify For #CSW1705A-12 End
} I2C_EVENT_MAP;

//---extern structures---
extern struct nvt_ts_data *ts;

//---extern functions---
extern int32_t CTP_I2C_READ(struct i2c_client *client, uint16_t address, uint8_t *buf, uint16_t len);
extern int32_t CTP_I2C_WRITE(struct i2c_client *client, uint16_t address, uint8_t *buf, uint16_t len);
extern void nvt_bootloader_reset(void);
extern void nvt_sw_reset_idle(void);
extern int32_t nvt_check_fw_reset_state(RST_COMPLETE_STATE check_reset_state);
extern int32_t nvt_get_fw_info(void);
extern int32_t nvt_clear_fw_status(void);
extern int32_t nvt_check_fw_status(void);
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 Begin
#if NVT_TOUCH_ESD_PROTECT
extern void nvt_esd_check_enable(uint8_t enable);
#endif /* #if NVT_TOUCH_ESD_PROTECT */
extern void nvt_stop_crc_reboot(void);
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-06> Modify For #CSW1705A-460 End

#endif /* _LINUX_NVT_TOUCH_H */
