/*
 * Copyright (C) 2010 - 2017 Novatek, Inc.
 *
 * $Revision: 12730 $
 * $Date: 2017-05-25 18:30:46 +0800 (週四, 25 五月 2017) $
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
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-26> Modify For #CSW1705A-1266 Begin
#if NVT_TOUCH_MP

#define IC_X_CFG_SIZE 18
#define IC_Y_CFG_SIZE 32
#define IC_KEY_CFG_SIZE 4
#define X_Channel	18
#define Y_Channel	32
#if TOUCH_KEY_NUM > 0
#define Key_Channel TOUCH_KEY_NUM
#else /* #if TOUCH_KEY_NUM > 0 */
#define Key_Channel 0
#endif /* #if TOUCH_KEY_NUM > 0 */

static uint8_t AIN_X[IC_X_CFG_SIZE] = {17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
static uint8_t AIN_Y[IC_Y_CFG_SIZE] = {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
#if TOUCH_KEY_NUM > 0
static uint8_t AIN_KEY[IC_KEY_CFG_SIZE] = {0, 1, 2, 0xFF};
#endif /* #if TOUCH_KEY_NUM > 0 */

static int32_t PS_Config_Lmt_Short_Rawdata_P = 13500;
static int32_t PS_Config_Lmt_Short_Rawdata_N = 11500;
static int32_t PS_Config_Lmt_Key_Short_Rawdata_P = 20000;
static int32_t PS_Config_Lmt_Key_Short_Rawdata_N = 11550;
static int32_t PS_Config_Lmt_Short_Diff_P = 6300;
static int32_t PS_Config_Lmt_Short_Diff_N = 0;
static int32_t PS_Config_Lmt_Key_Short_Diff_P = 6300;
static int32_t PS_Config_Lmt_Key_Short_Diff_N = 0;
static int32_t PS_Config_Lmt_Short_Base_P = 2000;
static int32_t PS_Config_Lmt_Short_Base_N = -2000;
static int32_t PS_Config_Lmt_Key_Short_Base_P = 2000;
static int32_t PS_Config_Lmt_Key_Short_Base_N = -2000;

static int32_t PS_Config_Lmt_Open_Rawdata_P[Y_Channel * X_Channel + Key_Channel] = {
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,   
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
    5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,5000,
#if TOUCH_KEY_NUM > 0
    13000,13000,13000,
#endif /* #if TOUCH_KEY_NUM > 0 */
};

static int32_t PS_Config_Lmt_Open_Rawdata_N[Y_Channel * X_Channel + Key_Channel] = {
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
    3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
#if TOUCH_KEY_NUM > 0
    6500,6500,6500,
#endif /* #if TOUCH_KEY_NUM > 0 */
};

static int32_t PS_Config_Lmt_FW_Rawdata_P[Y_Channel * X_Channel + Key_Channel] = {
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
    800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,
#if TOUCH_KEY_NUM > 0
    800,800,800,
#endif /* #if TOUCH_KEY_NUM > 0 */
};

static int32_t PS_Config_Lmt_FW_Rawdata_N[Y_Channel * X_Channel + Key_Channel] = {
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
    400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,   
#if TOUCH_KEY_NUM > 0
    400,400,400,
#endif /* #if TOUCH_KEY_NUM > 0 */
};

static int32_t PS_Config_Lmt_FW_CC_P = 130;
static int32_t PS_Config_Lmt_FW_CC_N = 80;
static int32_t PS_Config_Lmt_Key_FW_CC_P = 38;
static int32_t PS_Config_Lmt_Key_FW_CC_N = 9;
static int32_t PS_Config_Lmt_FW_CC_I_P = 25;
static int32_t PS_Config_Lmt_FW_CC_I_N = 0;
static int32_t PS_Config_Lmt_FW_CC_Q_P = 25;
static int32_t PS_Config_Lmt_FW_CC_Q_N = 0;
static int32_t PS_Config_Lmt_Key_FW_CC_I_P = 25;
static int32_t PS_Config_Lmt_Key_FW_CC_I_N = 0;
static int32_t PS_Config_Lmt_Key_FW_CC_Q_P = 25;
static int32_t PS_Config_Lmt_Key_FW_CC_Q_N = 0;

static int32_t PS_Config_Lmt_FW_Diff_P = 35;
static int32_t PS_Config_Lmt_FW_Diff_N = -35;
static int32_t PS_Config_Lmt_Key_FW_Diff_P = 35;
static int32_t PS_Config_Lmt_Key_FW_Diff_N = -35;

static int32_t PS_Config_Diff_Test_Frame = 50;

#endif /* #if NVT_TOUCH_MP */
//Chenyee <CY_BSP_CTP> <xuel> <2018-01-26> Modify For #CSW1705A-1266 End
