/*
 *
 * FocalTech TouchScreen driver.
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
* File Name: focaltech_gestrue.c
*
* Author: Focaltech Driver Team
*
* Created: 2016-08-08
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
#if FTS_GESTURE_EN
/******************************************************************************
* Private constant and macro definitions using #define
*****************************************************************************/
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
#define KEY_GESTURE_2F_UP                       KEY_F21
#define KEY_GESTURE_2F_DOWN                     KEY_F22
#define KEY_GESTURE_2F_LEFT                     197 
#define KEY_GESTURE_2F_RIGHT                    198
#define KEY_GESTURE_U_UP                        KEY_F13
#define KEY_GESTURE_U_DOWN                      KEY_F14
#define KEY_GESTURE_U_LEFT                     	KEY_F15
#define KEY_GESTURE_O                           195
#define KEY_GESTURE_E                           KEY_F18
#define KEY_GESTURE_M                           KEY_F19
#define KEY_GESTURE_W                           KEY_F23
#define KEY_GESTURE_S                           196
#define KEY_GESTURE_C                           KEY_F16
#define KEY_GESTURE_Z                           KEY_F24
#define KEY_DOUBLE_WAKE                         KEY_F17

#define GESTURE_LEFT                            0x20
#define GESTURE_RIGHT                           0x21
#define GESTURE_UP                              0x22
#define GESTURE_DOWN                            0x23
#define GESTURE_DOUBLECLICK                     0x24
#define GESTURE_2F_LEFT                         0x25
#define GESTURE_2F_RIGHT                        0x26
#define GESTURE_2F_UP                           0x27
#define GESTURE_2F_DOWN                         0x28
#define GESTURE_O                               0x30
#define GESTURE_W                               0x31
#define GESTURE_M                               0x32
#define GESTURE_E                               0x33
#define GESTURE_L                               0x44
#define GESTURE_S                               0x46
#define GESTURE_V                               0x54
#define GESTURE_Z                               0x41
#define GESTURE_C                               0x34
#define GESTURE_U_UP                            0x54
#define GESTURE_U_DOWN                          0x53
#define GESTURE_U_LEFT                          0x52
#define FTS_GESTRUE_POINTS                      255
#define FTS_GESTRUE_POINTS_HEADER               8

#define TST_BIT(flag,bit)	(flag & (0x1 << bit))
#define CLR_BIT(flag,bit)	(flag &= (~(0x1 << bit)))
#define SET_BIT(flag,bit)	(flag |= (0x1 << bit))
enum {
    TYPE_E = 0,
    TYPE_C ,
    TYPE_M,
    TYPE_TRIANGLE,
    TYPE_2F_UP,
    TYPE_2F_DOWN,
    TYPE_W,
    TYPE_Z,
    TYPE_CIRCLE,
    TYPE_S,
    TYPE_V_LEFT,
    TYPE_V_UP ,
    TYPE_V_DOWN ,
    TYPE_2F_LEFT,
    TYPE_2F_RIGHT,
};
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
/*****************************************************************************
* Private enumerations, structures and unions using typedef
*****************************************************************************/
/*
* header        -   byte0:gesture id
*                   byte1:pointnum
*                   byte2~7:reserved
* coordinate_x  -   All gesture point x coordinate
* coordinate_y  -   All gesture point y coordinate
* mode          -   1:enable gesture function(default)
*               -   0:disable
* active        -   1:enter into gesture(suspend)
*                   0:gesture disable or resume
*/
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
struct ges_feature_info
{
    union {
        struct{
            u16 start_x;
            u16 start_y;
            u16 end_x;
            u16 end_y;
            u16 width;
            u16 height;
            u16 mid_x;
            u16 mid_y;
            u16 top_x;
            u16 top_y;
            u16 bottom_x;
            u16 bottom_y;
            u16 left_x;
            u16 left_y;
            u16 right_x;
            u16 right_y;
        };
        u16 data[16];
    };
};

struct fts_gesture_st {
    u8 header[FTS_GESTRUE_POINTS_HEADER];
    u16 coordinate_x[FTS_GESTRUE_POINTS];
    u16 coordinate_y[FTS_GESTRUE_POINTS];
    u8 mode;   /*host driver enable gesture flag*/
    u8 active;  /*gesture actutally work*/
    struct ges_feature_info f_point;
};

/*****************************************************************************
* Static variables
*****************************************************************************/
static struct fts_gesture_st fts_gesture_data;

/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
bool is_need_coord = false;
unsigned int gesture_cfg;
int fts_wake_switch = 0;
int fts_gesture_switch = 0;
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

/*****************************************************************************
* Static function prototypes
*****************************************************************************/
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
ssize_t fts_double_wake_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t fts_double_wake_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t fts_gesture_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t fts_gesture_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t fts_gesture_buf_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t fts_gesture_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t fts_gesture_config_show(struct device *dev,struct device_attribute *attr, char *buf);
ssize_t fts_gesture_config_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count);
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

/************************************************************************
* Name: fts_double_wake_show
*  Brief:
*  Input: device, device attribute, char buf
* Output:
* Return:
***********************************************************************/
ssize_t fts_double_wake_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", fts_wake_switch);
}

/************************************************************************
* Name: fts_double_wake_store
*  Brief:
*  Input: device, device attribute, char buf
* Output:
* Return:
***********************************************************************/
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
ssize_t fts_double_wake_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int rt;
    unsigned long val;
    struct input_dev *input_dev = fts_data->input_dev;

    mutex_lock(&input_dev->mutex);

    rt = kstrtoul(buf, 10, &val);
    if(rt != 0){
        FTS_ERROR("%s, invalid value\n", __func__);
        return rt;
    }
    fts_wake_switch = val;
    fts_gesture_data.mode = fts_wake_switch || fts_gesture_switch;

    FTS_INFO("%s, %d\n", __func__, fts_wake_switch);

    mutex_unlock(&input_dev->mutex);

    return count;
}


/* sysfs gesture node
 *   read example: cat  fts_gesture_mode        ---read gesture mode
 *   write example:echo 01 > fts_gesture_mode   ---write gesture mode to 01
 *
 */
static DEVICE_ATTR (fts_gesture_mode, S_IRUGO | S_IWUSR, fts_gesture_show, fts_gesture_store);
/*
 *   read example: cat fts_gesture_buf        ---read gesture buf
 */
static DEVICE_ATTR (fts_gesture_buf, S_IRUGO | S_IWUSR, fts_gesture_buf_show, fts_gesture_buf_store);
static struct attribute *fts_gesture_mode_attrs[] = {
    &dev_attr_fts_gesture_mode.attr,
    &dev_attr_fts_gesture_buf.attr,
    NULL,
};

static struct attribute_group fts_gesture_group = {
    .attrs = fts_gesture_mode_attrs,
};

/************************************************************************
* Name: fts_gesture_show
*  Brief:
*  Input: device, device attribute, char buf
* Output:
* Return:
***********************************************************************/
ssize_t fts_gesture_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", fts_gesture_switch);;
}

/************************************************************************
* Name: fts_gesture_store
*  Brief:
*  Input: device, device attribute, char buf, char count
* Output:
* Return:
***********************************************************************/
ssize_t fts_gesture_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int rt;
    unsigned long val;
     struct input_dev *input_dev = fts_data->input_dev;

    mutex_lock(&input_dev->mutex);

    rt = kstrtoul(buf, 10, &val);
    if(rt != 0){
        FTS_ERROR("%s, invalid value\n", __func__);
        return rt;
    }
    fts_gesture_switch = val;
    fts_gesture_data.mode = fts_gesture_switch || fts_wake_switch;

    FTS_INFO("%s, %d\n", __func__, fts_gesture_switch);

    mutex_unlock(&input_dev->mutex);

    return count;
}
/************************************************************************
* Name: fts_gesture_buf_show
*  Brief:
*  Input: device, device attribute, char buf
* Output:
* Return:
***********************************************************************/
ssize_t fts_gesture_buf_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int count;
    int i = 0;
    int len = 0;
     struct input_dev *input_dev = fts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    count = sizeof(fts_gesture_data.f_point)/sizeof(fts_gesture_data.f_point.data[0]);

    for (i = 0; i < count; i++)
    {
        if(i==count-1)
            len += sprintf(buf+len,"%d",fts_gesture_data.f_point.data[i]);
        else
            len += sprintf(buf+len,"%d,",fts_gesture_data.f_point.data[i]);
    }
    
    mutex_unlock(&input_dev->mutex);

//Chenyee <CY_BSP_CTP> <xuel> <2018-01-09> Modify For #CSW1705A-137 Begin
    return len;
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-09> Modify For #CSW1705A-137 End
}

/************************************************************************
* Name: fts_gesture_buf_store
*  Brief:
*  Input: device, device attribute, char buf, char count
* Output:
* Return:
***********************************************************************/
ssize_t fts_gesture_buf_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    /* place holder for future use */
    return -EPERM;
}

/*****************************************************************************
*   Name: fts_create_gesture_sysfs
*  Brief:
*  Input:
* Output:
* Return: 0-success or others-error
*****************************************************************************/
ssize_t fts_gesture_config_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf,"%u\n",gesture_cfg);
}
	
ssize_t fts_gesture_config_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
    int rt;
    unsigned long val;
    struct input_dev *input_dev = fts_data->input_dev;

    mutex_lock(&input_dev->mutex);
    rt = kstrtoul(buf, 10, &val);
    if(rt != 0) {
        printk(KERN_ERR "%s, invalid value \n",__func__);
    }
    gesture_cfg = val & 0xffff;
    printk(" %s val=%lu \n",__func__,val);
    mutex_unlock(&input_dev->mutex);

    return count;
}
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

/*****************************************************************************
*   Name: fts_gesture_report
*  Brief:
*  Input:
* Output:
* Return:
*****************************************************************************/
static void fts_gesture_report(struct input_dev *input_dev, int gesture_id)
{
    int gesture;

    FTS_FUNC_ENTER();
    FTS_INFO("fts gesture_id==0x%x ", gesture_id);
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    if((gesture_id == GESTURE_DOUBLECLICK) && (fts_wake_switch == 1)) {
        input_report_key(input_dev, KEY_DOUBLE_WAKE, 1);
        input_sync(input_dev);
        input_report_key(input_dev, KEY_DOUBLE_WAKE, 0);
        input_sync(input_dev);
        return;
    }

    if(fts_gesture_switch == 1) {
        switch (gesture_id) {
            case GESTURE_2F_LEFT:
                if(TST_BIT(gesture_cfg, TYPE_2F_LEFT)) {
                    gesture = KEY_GESTURE_2F_LEFT;
                    FTS_INFO("[report position]gesture 2 fingers LEFT.\n");
                }
                break;
            case GESTURE_2F_RIGHT:
                if(TST_BIT(gesture_cfg, TYPE_2F_LEFT)) {
                    gesture = KEY_GESTURE_2F_RIGHT;
                    FTS_INFO("[report position]gesture 2 fingers RIGHT.\n");
                }
                break;
            case GESTURE_2F_UP:
                if(TST_BIT(gesture_cfg, TYPE_2F_UP)) {
                    gesture = KEY_GESTURE_2F_UP;
                    FTS_INFO("[report position]gesture 2 fingers UP.\n");
                }
                break;
            case GESTURE_2F_DOWN:
                if(TST_BIT(gesture_cfg, TYPE_2F_DOWN)) {
                    gesture = KEY_GESTURE_2F_DOWN;
                    FTS_INFO("[report position]gesture 2 fingers DOWN.\n");
                }
                break;
            case GESTURE_C:
                if(TST_BIT(gesture_cfg, TYPE_C)) {
                    gesture = KEY_GESTURE_C;
                    FTS_INFO("[report position]gesture 'C'.\n");
                }
                break;
            case GESTURE_U_LEFT:
                if(TST_BIT(gesture_cfg, TYPE_V_LEFT)) {
                    gesture = KEY_GESTURE_U_LEFT;
                    FTS_INFO("[report position]gesture '>'.\n");
                }
               break;
            case GESTURE_U_UP:
                if(TST_BIT(gesture_cfg, TYPE_V_UP)) {
                    gesture = KEY_GESTURE_U_UP;
                    FTS_INFO("[report position]gesture 'v'.\n");
                }
                break;
            case GESTURE_U_DOWN:
                if(TST_BIT(gesture_cfg, TYPE_V_DOWN)) {
                    gesture = KEY_GESTURE_U_DOWN;
                    FTS_INFO("[report position]gesture '^'.\n");
                }
                break;
            case GESTURE_O:
                if(TST_BIT(gesture_cfg, TYPE_CIRCLE)) {
                    gesture = KEY_GESTURE_O;
                    FTS_INFO("[report position]gesture 'O'.\n");
                }
                break;
            case GESTURE_W:
                if(TST_BIT(gesture_cfg, TYPE_W)) {
                    gesture = KEY_GESTURE_W;
                    FTS_INFO("[report position]gesture 'W'.\n");
                }
                break;
            case GESTURE_M:
                if(TST_BIT(gesture_cfg, TYPE_M)) {
                    gesture = KEY_GESTURE_M;
                    FTS_INFO("[report position]gesture 'M'.\n");
                }
                break;
            case GESTURE_E:
                if(TST_BIT(gesture_cfg, TYPE_E)) {
                    gesture = KEY_GESTURE_E;
                    FTS_INFO("[report position]gesture 'E'.\n");
                }
	            break;
            case GESTURE_S:
                if(TST_BIT(gesture_cfg, TYPE_S)) {
                    gesture = KEY_GESTURE_S;
                    FTS_INFO("[report position]gesture 'S'.\n");
                }
                break;
            case GESTURE_Z:
                if(TST_BIT(gesture_cfg, TYPE_Z)) {
                    gesture = KEY_GESTURE_Z;
                    FTS_INFO("[report position]gesture 'Z'.\n");
                }
                break;
            break;
        default:
            gesture = -1;
            break;
        }
    }
    /* report event key */
    if (gesture != -1) {
        FTS_DEBUG("Gesture Code=%d", gesture);
        input_report_key(input_dev, gesture, 1);
        input_sync(input_dev);
        input_report_key(input_dev, gesture, 0);
        input_sync(input_dev);
        is_need_coord = true;
    }

    FTS_FUNC_EXIT();
}

/************************************************************************
*   Name: fts_gesture_read_buffer
*  Brief: read data from TP register
*  Input:
* Output:
* Return: fail <0
***********************************************************************/
static int fts_gesture_read_buffer(struct i2c_client *client, u8 *buf, int read_bytes)
{
    int remain_bytes;
    int ret;
    int i;

    if (read_bytes <= I2C_BUFFER_LENGTH_MAXINUM) {
        ret = fts_i2c_read(client, buf, 1, buf, read_bytes);
    } else {
        ret = fts_i2c_read(client, buf, 1, buf, I2C_BUFFER_LENGTH_MAXINUM);
        remain_bytes = read_bytes - I2C_BUFFER_LENGTH_MAXINUM;
        for (i = 1; remain_bytes > 0; i++) {
            if (remain_bytes <= I2C_BUFFER_LENGTH_MAXINUM)
                ret = fts_i2c_read(client, buf, 0, buf + I2C_BUFFER_LENGTH_MAXINUM * i, remain_bytes);
            else
                ret = fts_i2c_read(client, buf, 0, buf + I2C_BUFFER_LENGTH_MAXINUM * i, I2C_BUFFER_LENGTH_MAXINUM);
            remain_bytes -= I2C_BUFFER_LENGTH_MAXINUM;
        }
    }

    return ret;
}

/************************************************************************
*   Name: fts_gesture_readdata
*  Brief: read data from TP register
*  Input:
* Output:
* Return: return 0 if succuss, otherwise reture error code
***********************************************************************/
int fts_gesture_readdata(struct fts_ts_data *ts_data)
{
    u8 buf[FTS_GESTRUE_POINTS * 4] = { 0 };
    int ret = 0;
    int i = 0;
    int gestrue_id = 0;
    int read_bytes = 0;
    u8 pointnum;
    u8 state;
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    u16 x_min = 65535;
    u16 x_max = 0;
    u16 y_min = 65535;
    u16 y_max = 0;
    struct ges_feature_info ges_info;
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
    struct i2c_client *client = ts_data->client;
    struct input_dev *input_dev = ts_data->input_dev;

    if (!ts_data->suspended) {
        return -EINVAL;
    }

    ret = fts_i2c_read_reg(client, FTS_REG_GESTURE_EN, &state);
    if ((ret < 0) || (state != ENABLE)) {
        FTS_DEBUG("gesture not enable, don't process gesture");
        return -EIO;
    }

    /* init variable before read gesture point */
    memset(fts_gesture_data.header, 0, FTS_GESTRUE_POINTS_HEADER);
    memset(fts_gesture_data.coordinate_x, 0, FTS_GESTRUE_POINTS * sizeof(u16));
    memset(fts_gesture_data.coordinate_y, 0, FTS_GESTRUE_POINTS * sizeof(u16));

    buf[0] = FTS_REG_GESTURE_OUTPUT_ADDRESS;
    ret = fts_i2c_read(client, buf, 1, buf, FTS_GESTRUE_POINTS_HEADER);
    if (ret < 0) {
        FTS_ERROR("[GESTURE]Read gesture header data failed!!");
        FTS_FUNC_EXIT();
        return ret;
    }

    memcpy(fts_gesture_data.header, buf, FTS_GESTRUE_POINTS_HEADER);
    gestrue_id = buf[0];
    pointnum = buf[1];
    read_bytes = ((int)pointnum) * 4 + 2;
    buf[0] = FTS_REG_GESTURE_OUTPUT_ADDRESS;
    FTS_DEBUG("[GESTURE]PointNum=%d", pointnum);
    ret = fts_gesture_read_buffer(client, buf, read_bytes);
    if (ret < 0) {
        FTS_ERROR("[GESTURE]Read gesture touch data failed!!");
        FTS_FUNC_EXIT();
        return ret;
    }

    fts_gesture_report(input_dev, gestrue_id);
    for (i = 0; i < pointnum; i++) {
        fts_gesture_data.coordinate_x[i] = (((s16) buf[0 + (4 * i + 2)]) & 0x0F) << 8
                                           | (((s16) buf[1 + (4 * i + 2)]) & 0xFF);
        fts_gesture_data.coordinate_y[i] = (((s16) buf[2 + (4 * i + 2)]) & 0x0F) << 8
                                           | (((s16) buf[3 + (4 * i + 2)]) & 0xFF);
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
        if(fts_gesture_data.coordinate_x[i] < x_min)
            x_min = fts_gesture_data.coordinate_x[i];
        if(fts_gesture_data.coordinate_x[i] > x_max)
            x_max = fts_gesture_data.coordinate_x[i];
        if(fts_gesture_data.coordinate_y[i] < y_min)
            y_min = fts_gesture_data.coordinate_y[i];
        if(fts_gesture_data.coordinate_y[i] > y_max)
            y_max = fts_gesture_data.coordinate_y[i];
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
    }

//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    ges_info.start_x = fts_gesture_data.coordinate_x[0];
    ges_info.start_y = fts_gesture_data.coordinate_y[0];
    ges_info.end_x = fts_gesture_data.coordinate_x[pointnum-1];
    ges_info.end_y = fts_gesture_data.coordinate_y[pointnum-1];

    ges_info.width = x_max - x_min;
    ges_info.height = y_max - y_min;
    ges_info.mid_x = x_min + ges_info.width/2;
    ges_info.mid_y = y_min + ges_info.height/2;

    ges_info.left_x = ges_info.mid_x - ges_info.width/2;
    ges_info.left_y = ges_info.mid_y;

    ges_info.right_x = ges_info.mid_x + ges_info.width/2;
    ges_info.right_y = ges_info.mid_y;

    ges_info.bottom_x= ges_info.mid_x;
    ges_info.bottom_y = ges_info.mid_y + ges_info.height/2;

    ges_info.top_x = ges_info.mid_x;
    ges_info.top_y = ges_info.mid_y - ges_info.height/2;

    FTS_DEBUG("[Gesture]start_x=%d,start_y=%d,end_x=%d,end_y=%d,width=%d,height=%d,mid_x=%d,mid_y=%d,left_x=%d,left_y=%d,right_x=%d,right_y=%d,bottom_x=%d,bottom_y=%d,top_x=%d,top_y=%d\n", \
        ges_info.start_x, ges_info.start_y, ges_info.end_x, ges_info.end_y, ges_info.width, ges_info.height, \
        ges_info.mid_x, ges_info.mid_y, ges_info.left_x, ges_info.left_y, ges_info.right_x, ges_info.right_y, \
        ges_info.bottom_x, ges_info.bottom_y, ges_info.top_x, ges_info.top_y);

    fts_gesture_data.f_point = ges_info;
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End

    return 0;
}

/*****************************************************************************
*   Name: fts_gesture_recovery
*  Brief: recovery gesture state when reset or power on
*  Input:
* Output:
* Return:
*****************************************************************************/
void fts_gesture_recovery(struct i2c_client *client)
{
    if ((ENABLE == fts_gesture_data.mode) && (ENABLE == fts_gesture_data.active)) {
        FTS_INFO("enter fts_gesture_recovery");
        fts_i2c_write_reg(client, 0xD1, 0xff);
        fts_i2c_write_reg(client, 0xD2, 0xff);
        fts_i2c_write_reg(client, 0xD5, 0xff);
        fts_i2c_write_reg(client, 0xD6, 0xff);
        fts_i2c_write_reg(client, 0xD7, 0xff);
        fts_i2c_write_reg(client, 0xD8, 0xff);
        fts_i2c_write_reg(client, FTS_REG_GESTURE_EN, ENABLE);
    }
}

/*****************************************************************************
*   Name: fts_gesture_suspend
*  Brief:
*  Input:
* Output:
* Return: return 0 if succuss, otherwise return error code
*****************************************************************************/
int fts_gesture_suspend(struct i2c_client *client)
{
    int ret;
    int i;
    u8 state;

    FTS_INFO("gesture suspend...");
    /* gesture not enable, return immediately */
    if (fts_gesture_data.mode == DISABLE) {
        FTS_INFO("gesture is disabled");
        return -EINVAL;
    }

    for (i = 0; i < 5; i++) {
        fts_i2c_write_reg(client, 0xd1, 0xff);
        fts_i2c_write_reg(client, 0xd2, 0xff);
        fts_i2c_write_reg(client, 0xd5, 0xff);
        fts_i2c_write_reg(client, 0xd6, 0xff);
        fts_i2c_write_reg(client, 0xd7, 0xff);
        fts_i2c_write_reg(client, 0xd8, 0xff);
        fts_i2c_write_reg(client, FTS_REG_GESTURE_EN, ENABLE);
        msleep(1);
        fts_i2c_read_reg(client, FTS_REG_GESTURE_EN, &state);
        if (state == ENABLE)
            break;
    }

    if (i >= 5) {
        FTS_ERROR("[GESTURE]Enter into gesture(suspend) failed!\n");
        fts_gesture_data.active = DISABLE;
        return -EIO;
    }

    ret = enable_irq_wake(fts_data->irq);
    if (ret) {
        FTS_INFO("enable_irq_wake(irq:%d) failed", fts_data->irq);
    }

    fts_gesture_data.active = ENABLE;
    FTS_INFO("[GESTURE]Enter into gesture(suspend) successfully!");
    FTS_FUNC_EXIT();
    return 0;
}

/*****************************************************************************
*   Name: fts_gesture_resume
*  Brief:
*  Input:
* Output:
* Return: return 0 if succuss, otherwise return error code
*****************************************************************************/
int fts_gesture_resume(struct i2c_client *client)
{
    int ret;
    int i;
    u8 state;

    FTS_INFO("gesture resume...");
    /* gesture not enable, return immediately */
    if (fts_gesture_data.mode == DISABLE) {
        FTS_DEBUG("gesture is disabled");
        return -EINVAL;
    }

    if (fts_gesture_data.active == DISABLE) {
        FTS_DEBUG("gesture in suspend is failed, no running fts_gesture_resume");
        return -EINVAL;
    }

    fts_gesture_data.active = DISABLE;
    for (i = 0; i < 5; i++) {
        fts_i2c_write_reg(client, FTS_REG_GESTURE_EN, DISABLE);
        msleep(1);
        fts_i2c_read_reg(client, FTS_REG_GESTURE_EN, &state);
        if (state == DISABLE)
            break;
    }

    if (i >= 5) {
        FTS_ERROR("[GESTURE]Clear gesture(resume) failed!\n");
        return -EIO;
    }

    ret = disable_irq_wake(fts_data->irq);
    if (ret) {
        FTS_INFO("disable_irq_wake(irq:%d) failed", fts_data->irq);
    }

    FTS_INFO("[GESTURE]resume from gesture successfully!");
    FTS_FUNC_EXIT();
    return 0;
}

/*****************************************************************************
*   Name: fts_gesture_init
*  Brief:
*  Input:
* Output:
* Return:
*****************************************************************************/
int fts_gesture_init(struct fts_ts_data *ts_data)
{
    //struct i2c_client *client = ts_data->client;
    struct input_dev *input_dev = ts_data->input_dev;

    FTS_FUNC_ENTER();
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 Begin
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_2F_UP);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_2F_DOWN);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_2F_LEFT);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_2F_RIGHT);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U_UP);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U_DOWN);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_O);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_E);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_M);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_W);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_S);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_Z);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_C);
    input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U_LEFT);
    input_set_capability(input_dev, EV_KEY, KEY_DOUBLE_WAKE);

    __set_bit(KEY_GESTURE_2F_UP, input_dev->keybit);
    __set_bit(KEY_GESTURE_2F_DOWN, input_dev->keybit);
    __set_bit(KEY_GESTURE_2F_LEFT, input_dev->keybit);
    __set_bit(KEY_GESTURE_2F_RIGHT, input_dev->keybit);
    __set_bit(KEY_GESTURE_U_UP, input_dev->keybit);
    __set_bit(KEY_GESTURE_U_DOWN, input_dev->keybit);
    __set_bit(KEY_GESTURE_O, input_dev->keybit);
    __set_bit(KEY_GESTURE_E, input_dev->keybit);
    __set_bit(KEY_GESTURE_M, input_dev->keybit);
    __set_bit(KEY_GESTURE_W, input_dev->keybit);
    __set_bit(KEY_GESTURE_S, input_dev->keybit);
    __set_bit(KEY_GESTURE_C, input_dev->keybit);
    __set_bit(KEY_GESTURE_U_LEFT, input_dev->keybit);
    __set_bit(KEY_GESTURE_Z, input_dev->keybit);
    __set_bit(KEY_DOUBLE_WAKE, input_dev->keybit);
//Chenyee <CY_BSP_CTP> <xuel> <2017-12-09> Modify For #CSW1705A-8 End
    fts_gesture_data.mode = ENABLE;
    fts_gesture_data.active = DISABLE;

    FTS_FUNC_EXIT();
    return 0;
}

/************************************************************************
*   Name: fts_gesture_exit
*  Brief: call when driver removed
*  Input:
* Output:
* Return:
***********************************************************************/
int fts_gesture_exit(struct i2c_client *client)
{
    FTS_FUNC_ENTER();
    sysfs_remove_group(&client->dev.kobj, &fts_gesture_group);
    FTS_FUNC_EXIT();
    return 0;
}
#endif
