/*
 *
 * FocalTech ftxxxx TouchScreen driver.
 *
 * Copyright (c) 2012-2018, Focaltech Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*****************************************************************************
*
* File Name: focaltech_ex_mode.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-31
*
* Abstract:
*
* Reference:
*
*****************************************************************************/

/*****************************************************************************
* 1.Included header files
*****************************************************************************/
#include "focaltech_core.h"

//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
#ifdef CONFIG_CY_BSP_MTK_DEVICE_CHECK
#undef CONFIG_CY_BSP_MTK_DEVICE_CHECK
#endif
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
/*****************************************************************************
* 2.Private constant and macro definitions using #define
*****************************************************************************/

/*****************************************************************************
* 3.Private enumerations, structures and unions using typedef
*****************************************************************************/
struct fts_mode_flag {
    int  fts_glove_mode_flag;
    int  fts_cover_mode_flag;
    int  fts_charger_mode_flag;
};

struct fts_mode_flag g_fts_mode_flag;

/*****************************************************************************
* 4.Static variables
*****************************************************************************/

/*****************************************************************************
* 5.Global variable or extern global variabls/functions
*****************************************************************************/
int fts_enter_glove_mode(struct i2c_client *client, int mode );
int fts_enter_cover_mode(struct i2c_client *client, int mode );
int fts_enter_charger_mode(struct i2c_client *client, int mode );

//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
extern ssize_t fts_double_wake_show(struct device *dev, struct device_attribute *attr, char *buf);
extern ssize_t fts_double_wake_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t fts_gesture_show(struct device *dev, struct device_attribute *attr, char *buf);
extern ssize_t fts_gesture_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t fts_gesture_buf_show(struct device *dev, struct device_attribute *attr, char *buf);
extern ssize_t fts_gesture_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t fts_test_show(struct device *dev, struct device_attribute *attr, char *buf);
extern ssize_t fts_test_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t fts_gesture_config_show(struct device *dev,struct device_attribute *attr, char *buf);
extern ssize_t fts_gesture_config_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count);
#ifdef CONFIG_CY_BSP_MTK_DEVICE_CHECK
extern int fts_register_hardware_info(struct i2c_client *client);
#endif
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

/*****************************************************************************
* 6.Static function prototypes
*******************************************************************************/

#if FTS_GLOVE_EN
static ssize_t fts_touch_glove_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    u8 val;
    struct input_dev *input_dev = fts_data->input_dev;
    struct i2c_client *client = container_of(dev, struct i2c_client, dev);

    mutex_lock(&input_dev->mutex);
    fts_i2c_read_reg(client, FTS_REG_GLOVE_MODE_EN, &val);
    count = snprintf(buf, PAGE_SIZE, "Glove Mode: %s\n", g_fts_mode_flag.fts_glove_mode_flag ? "On" : "Off");
    count += snprintf(buf + count, PAGE_SIZE, "Glove Reg(0xC0) = %d\n", val);
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_touch_glove_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    struct fts_ts_data *ts_data = fts_data;
    struct i2c_client *client;


    client = ts_data->client;
    if (FTS_SYSFS_ECHO_ON(buf)) {
        if (!g_fts_mode_flag.fts_glove_mode_flag) {
            FTS_INFO("[Mode]enter glove mode");
            ret = fts_enter_glove_mode(client, true);
            if (ret >= 0) {
                g_fts_mode_flag.fts_glove_mode_flag = true;
            }
        }
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        if (g_fts_mode_flag.fts_glove_mode_flag) {
            FTS_INFO("[Mode]exit glove mode");
            ret = fts_enter_glove_mode(client, false);
            if (ret >= 0) {
                g_fts_mode_flag.fts_glove_mode_flag = false;
            }
        }
    }
    FTS_INFO("[Mode]glove mode status:  %d", g_fts_mode_flag.fts_glove_mode_flag);
    return count;
}

/************************************************************************
* Name: fts_enter_glove_mode
* Brief:  change glove mode
* Input:  glove mode
* Output: no
* Return: success >=0, otherwise failed
***********************************************************************/
int fts_enter_glove_mode( struct i2c_client *client, int mode)
{
    int ret = 0;
    static u8 buf_addr[2] = { 0 };
    static u8 buf_value[2] = { 0 };
    buf_addr[0] = FTS_REG_GLOVE_MODE_EN; /* glove control */

    if (mode)
        buf_value[0] = 0x01;
    else
        buf_value[0] = 0x00;

    ret = fts_i2c_write_reg( client, buf_addr[0], buf_value[0]);
    if (ret < 0) {
        FTS_ERROR("[Mode]fts_enter_glove_mode write value fail");
    }

    return ret ;

}

/* read and write glove mode
*   read example: cat  fts_touch_glove_mode---read  glove mode
*   write example:echo 01 > fts_touch_glove_mode ---write glove mode to 01
*
*/
static DEVICE_ATTR (fts_glove_mode,  S_IRUGO | S_IWUSR, fts_touch_glove_show, fts_touch_glove_store);

#endif

#if CY_CTP_LEFT_HAND_MODE
int fts_hand_switch=0; //0:default    1:change

static ssize_t hand_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
	printk("enter");

	len = scnprintf(buf, PAGE_SIZE,"%d\n", fts_hand_switch);
	return len;
}

static ssize_t hand_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int value;
	int ret;
	printk("enter %s",buf);
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		printk("ret=0x%x", ret);
		return ret;
	}
	fts_hand_switch = value;
	return count;
}
static DEVICE_ATTR(left_hand_mode, 0664, hand_mode_show, hand_mode_store);
#endif

#if FTS_COVER_EN
static ssize_t fts_touch_cover_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    u8 val;
    struct input_dev *input_dev = fts_data->input_dev;
    struct i2c_client *client = container_of(dev, struct i2c_client, dev);

    mutex_lock(&input_dev->mutex);
    fts_i2c_read_reg(client, FTS_REG_COVER_MODE_EN, &val);
    count = snprintf(buf, PAGE_SIZE, "Cover Mode: %s\n", g_fts_mode_flag.fts_cover_mode_flag ? "On" : "Off");
    count += snprintf(buf + count, PAGE_SIZE, "Cover Reg(0xC1) = %d\n", val);
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_touch_cover_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    struct fts_ts_data *ts_data = fts_data;
    struct i2c_client *client;

    client = ts_data->client;
    if (FTS_SYSFS_ECHO_ON(buf)) {
        if (!g_fts_mode_flag.fts_cover_mode_flag) {
            FTS_INFO("[Mode]enter cover mode");
            ret = fts_enter_cover_mode(client, true);
            if (ret >= 0) {
                g_fts_mode_flag.fts_cover_mode_flag = true;
            }
        }
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        if (g_fts_mode_flag.fts_cover_mode_flag) {
            FTS_INFO("[Mode]exit cover mode");
            ret = fts_enter_cover_mode(client, false);
            if (ret >= 0) {
                g_fts_mode_flag.fts_cover_mode_flag = false;
            }
        }
    }
    FTS_INFO("[Mode]cover mode status:  %d", g_fts_mode_flag.fts_cover_mode_flag);
    return count;
}

/************************************************************************
* Name: fts_enter_cover_mode
* Brief:  change cover mode
* Input:  cover mode
* Output: no
* Return: success >=0, otherwise failed
***********************************************************************/
int  fts_enter_cover_mode( struct i2c_client *client, int mode)
{
    int ret = 0;
    static u8 buf_addr[2] = { 0 };
    static u8 buf_value[2] = { 0 };
    buf_addr[0] = FTS_REG_COVER_MODE_EN; /* cover control */

    if (mode)
        buf_value[0] = 0x01;
    else
        buf_value[0] = 0x00;

    ret = fts_i2c_write_reg( client, buf_addr[0], buf_value[0]);
    if (ret < 0) {
        FTS_ERROR("[Mode] fts_enter_cover_mode write value fail \n");
    }

    return ret ;

}

/* read and write cover mode
*   read example: cat  fts_touch_cover_mode---read  cover mode
*   write example:echo 01 > fts_touch_cover_mode ---write cover mode to 01
*
*/
static DEVICE_ATTR (fts_cover_mode,  S_IRUGO | S_IWUSR, fts_touch_cover_show, fts_touch_cover_store);

#endif

#if FTS_CHARGER_EN
static ssize_t fts_touch_charger_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    u8 val;
    struct input_dev *input_dev = fts_data->input_dev;
    struct i2c_client *client = container_of(dev, struct i2c_client, dev);

    mutex_lock(&input_dev->mutex);
    fts_i2c_read_reg(client, FTS_REG_CHARGER_MODE_EN, &val);
    count = snprintf(buf, PAGE_SIZE, "Charge Mode: %s\n", g_fts_mode_flag.fts_charger_mode_flag ? "On" : "Off");
    count += snprintf(buf + count, PAGE_SIZE, "Charge Reg(0x8B) = %d\n", val);
    mutex_unlock(&input_dev->mutex);

    return count;
}

static ssize_t fts_touch_charger_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    struct fts_ts_data *ts_data = fts_data;
    struct i2c_client *client;

    client = ts_data->client;

    if (FTS_SYSFS_ECHO_ON(buf)) {
        if (!g_fts_mode_flag.fts_charger_mode_flag) {
            FTS_INFO("[Mode]enter charger mode");
            ret = fts_enter_charger_mode(client, true);
            if (ret >= 0) {
                g_fts_mode_flag.fts_charger_mode_flag = true;
            }
        }
    } else if (FTS_SYSFS_ECHO_OFF(buf)) {
        if (g_fts_mode_flag.fts_charger_mode_flag) {
            FTS_INFO("[Mode]exit charger mode");
            ret = fts_enter_charger_mode(client, false);
            if (ret >= 0) {
                g_fts_mode_flag.fts_charger_mode_flag = false;
            }
        }
    }
    FTS_INFO("[Mode]charger mode status: %d", g_fts_mode_flag.fts_charger_mode_flag);
    return count;
}

/************************************************************************
* Name: fts_enter_charger_mode
* Brief:  change charger mode
* Input:  charger mode
* Output: no
* Return: success >=0, otherwise failed
***********************************************************************/
int  fts_enter_charger_mode(struct i2c_client *client, int mode)
{
    int ret = 0;
    static u8 buf_addr[2] = { 0 };
    static u8 buf_value[2] = { 0 };
    buf_addr[0] = FTS_REG_CHARGER_MODE_EN; /* charger control */

    if (mode)
        buf_value[0] = 0x01;
    else
        buf_value[0] = 0x00;

    ret = fts_i2c_write_reg( client, buf_addr[0], buf_value[0]);
    if (ret < 0) {
        FTS_DEBUG("[Mode]fts_enter_charger_mode write value fail");
    }

    return ret ;

}

/* read and write charger mode
*   read example: cat  fts_touch_charger_mode---read  charger mode
*   write example:echo 01 > fts_touch_charger_mode ---write charger mode to 01
*
*/
static DEVICE_ATTR (fts_charger_mode,  S_IRUGO | S_IWUSR, fts_touch_charger_show, fts_touch_charger_store);

#endif

#if FTS_GESTURE_EN
static DEVICE_ATTR (double_wake, S_IRUGO|S_IWUSR, fts_double_wake_show, fts_double_wake_store);
/* sysfs gesture node
 *   read example: cat  fts_gesture_mode        ---read gesture mode
 *   write example:echo 01 > fts_gesture_mode   ---write gesture mode to 01
 *
 */
static DEVICE_ATTR (gesture_wake, S_IRUGO|S_IWUSR, fts_gesture_show, fts_gesture_store);
/*
 *   read example: cat fts_gesture_buf        ---read gesture buf
 */
static DEVICE_ATTR (gesture_coordition, S_IRUGO|S_IWUSR, fts_gesture_buf_show, fts_gesture_buf_store);

static DEVICE_ATTR(gesture_config, S_IRUGO|S_IWUSR, fts_gesture_config_show, fts_gesture_config_store);
#endif

#if FTS_TEST_EN
/*  upgrade from app.bin
*    example:echo "***.ini" > fts_test
*/
static DEVICE_ATTR(factory_check, S_IRUGO, fts_test_show, NULL);
#endif
static struct device_attribute *tp_feature_attr_list[] = {
#if FTS_GESTURE_EN
    &dev_attr_double_wake,
    &dev_attr_gesture_wake,
    &dev_attr_gesture_coordition,
    &dev_attr_gesture_config,
#endif
#if FTS_GLOVE_EN
    &dev_attr_fts_glove_mode.attr,
#endif

#if FTS_COVER_EN
    &dev_attr_fts_cover_mode.attr,
#endif

#if FTS_CHARGER_EN
    &dev_attr_fts_charger_mode.attr,
#endif

#if FTS_TEST_EN
    &dev_attr_factory_check,
#endif

#if CY_CTP_LEFT_HAND_MODE
    &dev_attr_left_hand_mode,
#endif

};
#if 0
static struct attribute_group fts_touch_mode_group = {
    .attrs = fts_touch_mode_attrs,
};
#endif
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
static struct platform_device gn_tp_wake_device = {
    .name   = "tp_wake_switch",
    .id     = -1,
};
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
static int tpd_create_attr(struct device *dev)
{
    int idx, err = 0;
    int num = (int)(sizeof(tp_feature_attr_list)/sizeof(tp_feature_attr_list[0]));

    if (dev == NULL)
    {
        return -EINVAL;
    }
    
    for(idx = 0; idx < num; idx++)
    {
        if(err != device_create_file(dev, tp_feature_attr_list[idx]))
        {
            FTS_ERROR("[GESTURE]driver_create_file failed.\n");
            break;
        }
    }

    return err;
}

static int tpd_delete_attr(struct device *dev)
{
    int idx ,err = 0;
    int num = (int)(sizeof(tp_feature_attr_list)/sizeof(tp_feature_attr_list[0]));

    if (dev == NULL)
    {
        return -EINVAL;
    }

    for(idx = 0; idx < num; idx++)
    {
        device_remove_file(dev,tp_feature_attr_list[idx]);
    }
    return err;
}

/*****************************************************************************
*   Name: fts_create_gesture_sysfs
*  Brief:
*  Input:
* Output:
* Return: 0-success or others-error
*****************************************************************************/
int fts_create_gesture_sysfs(struct i2c_client *client)
{
    int ret = 0;

    ret = platform_device_register(&gn_tp_wake_device);
    if (ret)
    {
        FTS_ERROR("[GESTURE]create gn_tp_wake_device failed\n");
    }
    ret = tpd_create_attr(&(gn_tp_wake_device.dev));
    if(ret)
    {
        FTS_ERROR("[GESTURE]create gn_tp_feature attr failed \n");
    }

    return 0;
}
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

int fts_ex_mode_init(struct i2c_client *client)
{
    int err = 0;

    g_fts_mode_flag.fts_glove_mode_flag = false;
    g_fts_mode_flag.fts_cover_mode_flag = false;
    g_fts_mode_flag.fts_charger_mode_flag = false;

//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    err = fts_create_gesture_sysfs(client);
    if (0 != err) {
        FTS_ERROR("[Mode]create gn_tp_wake_device failed.");
        tpd_delete_attr(&(gn_tp_wake_device.dev));
        platform_device_unregister(&gn_tp_wake_device);
        return -EIO;
    } else {
        FTS_DEBUG("[Mode]create gn_tp_wake_device succeeded");
    }

#ifdef CONFIG_CY_BSP_MTK_DEVICE_CHECK
    err = fts_register_hardware_info(client);
    if (err < 0) {
        FTS_ERROR("Read Hardware Info Failed.\n");
    }
#endif
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
    return err;

}

int fts_ex_mode_exit(struct i2c_client *client)
{
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    tpd_delete_attr(&(gn_tp_wake_device.dev));
    platform_device_unregister(&gn_tp_wake_device);
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
    return 0;
}

int fts_ex_mode_recovery(struct i2c_client *client)
{
    int ret = 0;
#if FTS_GLOVE_EN
    if (g_fts_mode_flag.fts_glove_mode_flag)
        ret = fts_enter_glove_mode(client, true);
#endif

#if FTS_COVER_EN
    if (g_fts_mode_flag.fts_cover_mode_flag)
        ret = fts_enter_cover_mode(client, true);
#endif

#if FTS_CHARGER_EN
    if (g_fts_mode_flag.fts_charger_mode_flag)
        ret = fts_enter_charger_mode(client, true);
#endif

    return ret;
}

