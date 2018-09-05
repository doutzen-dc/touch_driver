#ifndef _TPD_GESTURE_H_
#define _TPD_GESTURE_H_


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/compat.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#include <linux/delay.h>
#include <linux/input.h>

#include <linux/platform_device.h>

//#include <mach/mt_typedefs.h>


#include "synaptics_dsx.h"
#include "synaptics_dsx_i2c.h"
//#include "synaptics_dsx_core.h"
#ifdef KERNEL_ABOVE_2_6_38
#include <linux/input/mt.h>
#endif

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 begin
#include "gn_log.h"
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 end

#define DEVICE_NAME "tpd_gesture"

#define TPD_GESTURE_IOC_MAGIC 'G'
#define TPD_GESTURE_ENABLE        _IO(TPD_GESTURE_IOC_MAGIC,1)
#define TPD_GESTURE_POINT_NUMBER  _IO(TPD_GESTURE_IOC_MAGIC,2)
#define TPD_GESTURE_FEATURE_INFO  _IO(TPD_GESTURE_IOC_MAGIC,3)
#define TPD_GESTURE_COORDINATE    _IO(TPD_GESTURE_IOC_MAGIC,4)

#define MAX_COORDINATE_COUNT 64
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> add for TP external functions begin
#define GN_MTK_BSP_TP_GLOVE_SUPPORT
#define GN_MTK_BSP_TP_GESTURE_SUPPORT
//#define GN_MTK_BSP_TP_SKYLIGHT_SUPPORT
//#define GN_MTK_BSP_TP_USE_FULL_COORDINATE

#define GN_MTK_BSP_TP_SELF_TEST
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> add for TP external functions end
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> modify for get data start
#define GN_GET_TP_TEST_DATA 	1
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> modify for get data start

//Gionee <GN_BSP_CTP> <linzhanchi> <20172020> add for ID72615 start
/*
*	1	support sensitivity switch function  
* 	0	do not support sensitivity switch function
*/
#define GN_BSP_TP_SWITCH_SENSITIVITY_SUPPORT	0
#if GN_BSP_TP_SWITCH_SENSITIVITY_SUPPORT
#define GN_BSP_TP_SWITCH_SENSITIVITY			1
#else
#define GN_BSP_TP_SWITCH_SENSITIVITY			0
#endif
//Gionee <GN_BSP_CTP> <linzhanchi> <20170220> add for ID72615 end

/*
extern unsigned long tp_dbg;
#if defined(TPD_DEBUG)
#undef TPD_DEBUG
#define TPD_DEBUG(args...) \
	do { \
		if (1 == tp_dbg) { \
			printk("MTK-tpd:  "args); \
		} \
	} while (0)
#else
#define TPD_DEBUG(arg...)
#endif
*/

struct coordinate_data 
{
	u16 x;
	u16 y;
};
struct ges_feature_info
{
	union {
		struct {
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
struct tpd_ges_data 
{
	struct cdev cdev;
	u8 length;
	//struct coordinate_data coordinate[MAX_COORDINATE_COUNT];
	struct ges_feature_info f_point;
#ifdef GN_MTK_BSP_TP_USE_FULL_COORDINATE
	u16 coordinate[MAX_COORDINATE_COUNT*2];
#endif
};


//Gionee BSP1 yaoyc 20141105 add for CR01399422  begin
enum {
	GKEY_U_UP = 183,
	GKEY_U_DOWN = 184,
	GKEY_U_LEFT = 185,
	GKEY_U_RIGHT = 186,
	GKEY_DOUBLE_TAP = 187,
	GKEY_E = 188,
	GKEY_M = 189,
	GKEY_TRIANGLE = 190,
	GKEY_2F_UP = 191,
	GKEY_2F_DOWN = 192,
	GKEY_W = 193,
	GKEY_Z = 194,
	GKEY_CIRCLE = 195,
	GKEY_S = 196,
	GKEY_2F_LEFT = 197,
	GKEY_2F_RIGHT = 198,
	GKEY_1F_SWIPE = 199,
};
enum {
    GESTURE_U_DOWN= 1,
    GESTURE_U_UP = 2,
    GESTURE_U_RIGHT = 4,
    GESTURE_U_LEFT = 8,
};
enum {
	UNICODE_W = 0x77,
	UNICODE_M = 0x6d,
	UNICODE_E = 0x65,
	UNICODE_Z = 0x7a,
	UNICODE_S = 0x73,
	UNICODE_C = 0x63,
};
enum {
	TWO_FINGER_UP = 0x88,
	TWO_FINGER_DOWN = 0x84,
	TWO_FINGER_LEFT = 0x82,
	TWO_FINGER_RIGHT = 0x81,
};
enum {
	ONE_FINGER_UP = 0x48,
	ONE_FINGER_DOWN = 0x44,
	ONE_FINGER_LEFT = 0x42,
	ONE_FINGER_RIGHT = 0x41,
};
enum {
	NO_GESTUER = 0x00,
	ONE_FINGER_SINGLE_TAP = 0x01,
	ONE_FINGER_TAP_AND_HOLD = 0x02,
	ONE_FINGER_DOUBLE_TAP = 0x03,
	ONE_FINGER_EARLY_TAP = 0x04,
	ONE_FINGER_FLICK = 0x05,
	ONE_FINGER_PRESS = 0x06,
	FINGER_SWIPE = 0x07,
	ONE_FINGER_CIRCLE = 0x08,
	ONE_FINGER_TRIANGLE = 0x09,
	ONE_FINGER_VEE = 0x0A,
	ONE_FINGER_UNICODE = 0x0b,
};


 void synaptics_rmi4_gesture_func(struct synaptics_rmi4_data *rmi4_data,
						struct synaptics_rmi4_fn *fhandler, unsigned char gesture_type);

 int gesture_inputdev_key_init(struct synaptics_rmi4_data *rmi4_data);
 
 void synaptics_rmi4_gesture_confige_update(struct synaptics_rmi4_data *rmi4_data,
										 struct synaptics_rmi4_fn *fhandler);

//Gionee linzhanchi add for TP gesture start
int tpd_gesture_exit(void);
int tpd_gesture_init(void);
//Gionee linzhanchi add for TP gesture end

//Gionee BSP1 yaoyc 20141105 add for CR01399422  end
#endif
