
#include "gn_tpd_feature.h"
#include <linux/miscdevice.h>
#include "tpd.h"
//Gionee <GN_BSP_CTP> <linzhanchi> <20170826> add for ID197141 start
#include "gn_log.h"
//Gionee <GN_BSP_CTP> <linzhanchi> <20170826> add for ID197141 end

MODULE_LICENSE("GPL V2");
/*
extern int synaptics_rmi4_reg_read(struct synaptics_rmi4_data *rmi4_data,
		unsigned short addr, unsigned char *data,
		unsigned short length);
extern int synaptics_rmi4_reg_write(struct synaptics_rmi4_data *rmi4_data,
		unsigned short addr, unsigned char *data,
		unsigned short length);
*/
struct tpd_ges_data *tpd_synaptics_devp = NULL;
static struct synaptics_rmi4_data *g_rmi4_data = NULL;

int synaptics_wake_switch = 0;
int synaptics_gesture_switch = 0;

//Gionee <GN_BSP_CTP> <mogongfu> <20161210> add for get data start
#include <linux/vmalloc.h>
char *synaptics_data_buf = NULL;
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> add for get data end

// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 begin
extern int synaptics_log_switch;
// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 end

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
extern void synaptics_save_diff_data(char *diff_data, int len);

#if defined(GN_MTK_BSP_TP_SELF_TEST)
extern void synaptics_rmi4_f54_get_TxRx_count(int *rx_tx);
extern void synaptics_rmi4_f54_get_raw_data(short *rawdata);
extern void synaptics_rmi4_f54_get_rawdata_range(int *range);

extern int synaptics_rmi4_f54_data_init(struct synaptics_rmi4_data *rmi4_data);
#endif
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

#if defined(GN_MTK_BSP_TP_GESTURE_SUPPORT)
static	unsigned int gesture_cfg;
#define TST_BIT(flag,bit)	(flag & (0x1 << bit))
#define CLR_BIT(flag,bit)	(flag &= (~(0x1 << bit)))
#define SET_BIT(flag,bit)	(flag |= (0x1 << bit))
enum {
	TYPE_E = 0,
	TYPE_V_RIGHT ,
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
#endif
static void gesture_unicode_get_coordination(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	int i;
	unsigned char coordinate[24];
	u16 point[12];
	u16 x_min = 65535;
	u16 x_max = 0;
	u16 y_min = 65535;
	u16 y_max = 0;
	struct ges_feature_info ges_info;
	retval = synaptics_rmi4_reg_read(rmi4_data,0x0400,
						(unsigned char*)&coordinate,sizeof(coordinate));
//Gionee <GN_BSP_CTP> <linzhanchi> <20170826> add for ID197141 start
	if(retval < 0) {
		GN_DEBUG_ERROR("%s; fail to read coordination; error=%d",__func__,retval);
	}
//Gionee <GN_BSP_CTP> <linzhanchi> <20170826> add for ID197141 end
	for(i=0;i<24;i+=2){
		point[i/2]=coordinate[i] | (coordinate[i+1] << 8);
		TPD_DEBUG("%s;point[%d]=%d;\n",__func__,i/2,point[i/2]);
	}
	ges_info.start_x = point[0];
	ges_info.start_y = point[1];
	ges_info.end_x = point[2];
	ges_info.end_y = point[3];

	for(i=0;i<12;i+=2) {
		if(point[i] == 0)
			continue;
		if(point[i] >= x_max) {
			x_max = point[i];
			ges_info.right_x = point[i];
			ges_info.right_y = point[i+1];
		}
		if(point[i] <= x_min) {
			x_min = point[i];
			ges_info.left_x = point[i];
			ges_info.left_y = point[i+1];
		}
		if(point[i+1] >= y_max) {
			y_max = point[i+1];
			ges_info.bottom_y = point[i+1];
			ges_info.bottom_x = point[i];
		}
		if(point[i+1] <= y_min) {
			y_min = point[i+1];
			ges_info.top_y = point[i+1];
			ges_info.top_x = point[i];
		}
	}
	ges_info.width = ges_info.right_x - ges_info.left_x;
	ges_info.height = ges_info.bottom_y - ges_info.top_y;
	ges_info.mid_x = (ges_info.right_x + ges_info.left_x)/2;
	ges_info.mid_y = (ges_info.bottom_y + ges_info.top_y)/2;

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	for(i=0;i<16;i++)
		GN_DEBUG_INFO("%s [%d]=%d", __func__, i, ges_info.data[i]);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

	tpd_synaptics_devp->f_point = ges_info;

	return;
}
static void gesture_get_coordination(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	int i;
	unsigned char coordinate[25];
	u16 swipe_id = 0;
	struct ges_feature_info ges_info;
	retval = synaptics_rmi4_reg_read(rmi4_data,0x0400,
						(unsigned char*)&coordinate,sizeof(coordinate));
	
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to read coordination; error=%d \n",__func__,retval);
	}
	ges_info.start_x = (coordinate[0]) | (coordinate[1] << 8);
	ges_info.start_y = (coordinate[2]) | (coordinate[3] << 8);
	ges_info.end_x = (coordinate[4]) | (coordinate[5] << 8);
	ges_info.end_y = (coordinate[6]) | (coordinate[7] << 8);
	ges_info.top_x = (coordinate[8]) | (coordinate[9] << 8);
	ges_info.top_y = (coordinate[10]) | (coordinate[11] << 8);
	ges_info.left_x = (coordinate[12]) | (coordinate[13] << 8);
	ges_info.left_y = (coordinate[14]) | (coordinate[15] << 8);
	ges_info.bottom_x = (coordinate[16]) | (coordinate[17] << 8);
	ges_info.bottom_y = (coordinate[18]) | (coordinate[19] << 8);
	ges_info.right_x = (coordinate[20]) | (coordinate[21] << 8);
	ges_info.right_y = (coordinate[22]) | (coordinate[23] << 8);
	swipe_id = coordinate[24];
	ges_info.width = ges_info.right_x - ges_info.left_x;
	ges_info.height = ges_info.bottom_y - ges_info.top_y;
	ges_info.mid_x = (ges_info.right_x + ges_info.left_x)/2;
	ges_info.mid_y = (ges_info.top_y + ges_info.bottom_y)/2;

	//printk(KERN_ERR "yaoyc swipe_id=%d \n",swipe_id);
	for(i=0;i<16;i++)
		TPD_DEBUG("%s [%d]=%d \n", __func__, i, ges_info.data[i]);

	tpd_synaptics_devp->f_point = ges_info;
	return;
}

static void gesture_double_tap(struct synaptics_rmi4_data *rmi4_data)
{
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s; double click",__func__);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end
	
	input_report_key(tpd->dev, GKEY_DOUBLE_TAP, 1);
	input_sync(tpd->dev);
	input_report_key(tpd->dev, GKEY_DOUBLE_TAP, 0);
	input_sync(tpd->dev);
	
//	gesture_get_coordination(rmi4_data);
//	rmi4_data->suspend = false;

}

static void gesture_Vee(struct synaptics_rmi4_data *rmi4_data,
			struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned short data_addr;
	unsigned char detected_gestures[3];
	bool is_valid = false;
	struct synaptics_rmi4_f12_extra_data *extra_data;
	//struct synaptics_rmi4_f12_finger_data *data;
	//struct synaptics_rmi4_f12_finger_data *finger_data;

	data_addr = fhandler->full_addr.data_base;
	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;


	retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data4_offset,
				(unsigned char*)&detected_gestures,
				sizeof(detected_gestures));
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to read Vee direction ,error = %d\n", __func__,retval);
	}
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s; detected_gesture[2]=%d",__func__,detected_gestures[2]);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end
		
    switch (detected_gestures[2]) {
		case GESTURE_U_UP:
			if(TST_BIT(gesture_cfg,TYPE_V_UP)) {
				input_report_key(tpd->dev, GKEY_U_UP, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_U_UP, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case GESTURE_U_DOWN:
			if(TST_BIT(gesture_cfg, TYPE_V_DOWN)) {
				input_report_key(tpd->dev, GKEY_U_DOWN, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_U_DOWN, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case GESTURE_U_LEFT:
			if(TST_BIT(gesture_cfg, TYPE_V_LEFT)){
				input_report_key(tpd->dev, GKEY_U_LEFT, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_U_LEFT, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case GESTURE_U_RIGHT:
			if(TST_BIT(gesture_cfg, TYPE_V_RIGHT)) {
				input_report_key(tpd->dev, GKEY_U_RIGHT, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_U_RIGHT, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;

    	default:
              break;
      }
	if(is_valid)
		gesture_unicode_get_coordination(rmi4_data);
	  return;
}
/*
static void gesture_one_finger_swipe(struct synaptics_rmi4_data *rmi4_data)
{

	input_report_key(tpd->dev, GKEY_1F_SWIPE, 1);
	input_sync(tpd->dev);
	input_report_key(tpd->dev, GKEY_1F_SWIPE, 0);
	input_sync(tpd->dev);

	//gesture_get_coordination(rmi4_data);
	return;
}
*/

static void gesture_finger_swipe(struct synaptics_rmi4_data *rmi4_data,
									struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned short data_addr;
	//unsigned char detected_gestures[3];
	bool is_valid = false;
	struct synaptics_rmi4_f12_extra_data *extra_data;
	//struct synaptics_rmi4_f12_finger_data *data;
	//struct synaptics_rmi4_f12_finger_data *finger_data;
	unsigned char swipe;
	data_addr = fhandler->full_addr.data_base;
	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;

/*
	retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data4_offset,
				&detected_gestures,
				sizeof(detected_gestures));
*/
	retval = synaptics_rmi4_reg_read(rmi4_data,0x0418,&swipe,sizeof(swipe));
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to read Vee direction ,error = %d\n", __func__,retval);
	}
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s; swipe:0x%x;", __func__,swipe);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

	switch (swipe) {
		case TWO_FINGER_UP:
			if(TST_BIT(gesture_cfg, TYPE_2F_UP)){
				input_report_key(tpd->dev, GKEY_2F_UP, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_2F_UP, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case TWO_FINGER_DOWN:
			if(TST_BIT(gesture_cfg, TYPE_2F_DOWN)) {
				input_report_key(tpd->dev, GKEY_2F_DOWN, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_2F_DOWN, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case TWO_FINGER_LEFT:
			if(TST_BIT(gesture_cfg, TYPE_2F_LEFT)) {
				input_report_key(tpd->dev, GKEY_2F_LEFT, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_2F_LEFT, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case TWO_FINGER_RIGHT:
			if(TST_BIT(gesture_cfg, TYPE_2F_RIGHT)) {
				input_report_key(tpd->dev, GKEY_2F_RIGHT, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_2F_RIGHT, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case ONE_FINGER_UP:
		case ONE_FINGER_DOWN:
		case ONE_FINGER_LEFT:
		case ONE_FINGER_RIGHT:
			break;
    	default:
              break;
      }
	if(is_valid)
		gesture_unicode_get_coordination(rmi4_data);

	return;
}
static void gesture_one_finger_circle(struct synaptics_rmi4_data *rmi4_data)
{
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s; gesture O",__func__);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end
	
	if(TST_BIT(gesture_cfg,TYPE_CIRCLE)) {
		input_report_key(tpd->dev, GKEY_CIRCLE, 1);
		input_sync(tpd->dev);
		input_report_key(tpd->dev, GKEY_CIRCLE, 0);
		input_sync(tpd->dev);

		gesture_get_coordination(rmi4_data);
	}
	return;
}
static void gesture_one_finger_triangle(struct synaptics_rmi4_data * rmi4_data)
{
	if(TST_BIT(gesture_cfg,TYPE_TRIANGLE)) {
		input_report_key(tpd->dev, GKEY_TRIANGLE, 1);
		input_sync(tpd->dev);
		input_report_key(tpd->dev, GKEY_TRIANGLE, 0);
		input_sync(tpd->dev);

		gesture_get_coordination(rmi4_data);
	}
	return;
}

static void gesture_unicode(struct synaptics_rmi4_data *rmi4_data,
			struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned short data_addr;
	unsigned char detected_gestures[3];
	bool is_valid = false;
	struct synaptics_rmi4_f12_extra_data *extra_data;
	//struct synaptics_rmi4_f12_finger_data *data;
	//struct synaptics_rmi4_f12_finger_data *finger_data;

	data_addr = fhandler->full_addr.data_base;
	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;


	retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data4_offset,
				(unsigned char*)&detected_gestures,
				sizeof(detected_gestures));
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to read Vee direction ,error = %d\n", __func__,retval);
	}

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s detected_gestures=0x%02x",__func__, detected_gestures[2]);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

	switch (detected_gestures[2]) {
		case UNICODE_W:
			if(TST_BIT(gesture_cfg,TYPE_W)) {
				input_report_key(tpd->dev, GKEY_W, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_W, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case UNICODE_M:
			if(TST_BIT(gesture_cfg,TYPE_M)) {
				input_report_key(tpd->dev, GKEY_M, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_M, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case UNICODE_E:
			if(TST_BIT(gesture_cfg,TYPE_E)) {
				input_report_key(tpd->dev, GKEY_E, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_E, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case UNICODE_Z:
			if(TST_BIT(gesture_cfg,TYPE_Z)) {
				input_report_key(tpd->dev, GKEY_Z, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_Z, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
		case UNICODE_S:
			if(TST_BIT(gesture_cfg,TYPE_S)) {
				input_report_key(tpd->dev, GKEY_S, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_S, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
			// 'C' and V left is the same
		case UNICODE_C:
			if(TST_BIT(gesture_cfg,TYPE_V_RIGHT)) {
				input_report_key(tpd->dev, GKEY_U_RIGHT, 1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev, GKEY_U_RIGHT, 0);
				input_sync(tpd->dev);
				is_valid = true;
			}
			break;
    	default:
              break;
      }
	if(is_valid)
		gesture_unicode_get_coordination(rmi4_data);
	return;
}
void synaptics_rmi4_gesture_func(struct synaptics_rmi4_data *rmi4_data,
						struct synaptics_rmi4_fn *fhandler, unsigned char gesture_type)
{
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
	GN_DEBUG_INFO("%s gesture_type=0x%02x",__func__, gesture_type);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end
	switch (gesture_type) {
		case NO_GESTUER:
		case ONE_FINGER_SINGLE_TAP:
		case ONE_FINGER_TAP_AND_HOLD:
			break;
		case ONE_FINGER_DOUBLE_TAP:
			if(synaptics_wake_switch)
				gesture_double_tap(rmi4_data);
			break;
		case ONE_FINGER_EARLY_TAP:
		case ONE_FINGER_FLICK:
		case ONE_FINGER_PRESS:
			break;
		case FINGER_SWIPE:
			if(synaptics_gesture_switch)
				gesture_finger_swipe(rmi4_data,fhandler);
			break;
		case ONE_FINGER_CIRCLE:
			if(synaptics_gesture_switch)
				gesture_one_finger_circle(rmi4_data);
			break;
		case ONE_FINGER_TRIANGLE:
			if(synaptics_gesture_switch)
				gesture_one_finger_triangle(rmi4_data);
			break;
		case ONE_FINGER_VEE:
			if(synaptics_gesture_switch)
				gesture_Vee(rmi4_data,fhandler);
			break;
		case ONE_FINGER_UNICODE:
			if(synaptics_gesture_switch)
				gesture_unicode(rmi4_data,fhandler);
			break;
		default:
			break;
				
	}
	return ;
}

//Gionee BSP1 yaoyc 20141105 add for CR01399422  begin
int gesture_inputdev_key_init(struct synaptics_rmi4_data *rmi4_data)
{
	if(rmi4_data == NULL){
		printk(KERN_ERR "%s , rmi4_data is null \n",__func__);
		return -1;
	}
    set_bit(GKEY_U_UP, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_U_UP);
    set_bit(GKEY_U_DOWN, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_U_DOWN);
    set_bit(GKEY_U_LEFT, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_U_LEFT);
    set_bit(GKEY_U_RIGHT, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_U_RIGHT);
    set_bit(GKEY_DOUBLE_TAP, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_DOUBLE_TAP);
    set_bit(GKEY_E, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_E);
    set_bit(GKEY_M, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_M);
	set_bit(GKEY_TRIANGLE, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_TRIANGLE);
    set_bit(GKEY_2F_UP, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_2F_UP);
    set_bit(GKEY_2F_DOWN, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_2F_DOWN);
    set_bit(GKEY_W, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_W);
	set_bit(GKEY_Z, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_Z);
    set_bit(GKEY_CIRCLE, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_CIRCLE);
    set_bit(GKEY_S, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_S);
    set_bit(GKEY_2F_LEFT, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_2F_LEFT);
    set_bit(GKEY_2F_RIGHT, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_2F_RIGHT);
    set_bit(GKEY_1F_SWIPE, tpd->dev->keybit);
	input_set_capability(tpd->dev, EV_KEY, GKEY_1F_SWIPE);
	g_rmi4_data = rmi4_data;
#if defined(GN_MTK_BSP_TP_SELF_TEST)
	synaptics_rmi4_f54_data_init(rmi4_data);
#endif
	return 0;
}
//Gionee BSP1 yaoyc 20141105 add for CR01399422 end
static inline bool is_unicode_enable(void)
{
	return (TST_BIT(gesture_cfg,TYPE_W) | TST_BIT(gesture_cfg,TYPE_M) | TST_BIT(gesture_cfg,TYPE_E) | TST_BIT(gesture_cfg,TYPE_Z) | TST_BIT(gesture_cfg,TYPE_S) | TST_BIT(gesture_cfg,TYPE_V_RIGHT));
}

static inline bool is_Vee_enable(void)
{
	return (TST_BIT(gesture_cfg,TYPE_V_UP) | TST_BIT(gesture_cfg,TYPE_V_DOWN) | TST_BIT(gesture_cfg,TYPE_V_LEFT) | TST_BIT(gesture_cfg,TYPE_V_RIGHT)); 
}
static inline bool is_triangle_enable(void)
{
	return TST_BIT(gesture_cfg,TYPE_TRIANGLE);
}
static inline bool is_circle_enable(void)
{
	return TST_BIT(gesture_cfg,TYPE_CIRCLE);
}
static inline bool is_double_tap_enable(void)
{
	return synaptics_wake_switch > 0 ? true:false;
}
static inline bool is_two_finger_swipe_enable(void)
{
	return (TST_BIT(gesture_cfg,TYPE_2F_UP) | TST_BIT(gesture_cfg,TYPE_2F_DOWN) | TST_BIT(gesture_cfg,TYPE_2F_LEFT) | TST_BIT(gesture_cfg,TYPE_2F_RIGHT)); 
}
void synaptics_rmi4_gesture_confige_update(struct synaptics_rmi4_data *rmi4_data,
										struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned char offset;
	unsigned char ges_cfg;
	struct synaptics_rmi4_f12_extra_data *extra_data;
	//struct synaptics_rmi4_f12_finger_data *data;
	//struct synaptics_rmi4_f12_finger_data *finger_data;
	TPD_DEBUG("%s synaptics_wake_switch enter 4\n",__func__);

	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;
	offset = extra_data->ctrl27_offset;

	retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.ctrl_base + offset,
				&ges_cfg,
				sizeof(ges_cfg));
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to read gesture config ,error = %d\n", __func__,retval);
	}
	printk(KERN_ERR "%s, addr:0x%04x; ges_cfg = 0x%x \n",__func__,fhandler->full_addr.ctrl_base + offset,ges_cfg);
	if(is_unicode_enable())
		SET_BIT(ges_cfg,6);
	else
		CLR_BIT(ges_cfg,6);

	if(is_Vee_enable())
		SET_BIT(ges_cfg,5);
	else
		CLR_BIT(ges_cfg,5);

	if(is_triangle_enable())
		SET_BIT(ges_cfg,4);
	else
		CLR_BIT(ges_cfg,4);
	
	if(is_circle_enable())
		SET_BIT(ges_cfg,3);
	else
		CLR_BIT(ges_cfg,3);

	if(is_double_tap_enable())
		SET_BIT(ges_cfg,0);
	else
		CLR_BIT(ges_cfg,0);

	if(is_two_finger_swipe_enable())
		SET_BIT(ges_cfg,1);
	else
		CLR_BIT(ges_cfg,1);

	CLR_BIT(ges_cfg,2);
	TPD_DEBUG("%s, ges_cfg=0x%x \n",__func__,ges_cfg);

	retval = synaptics_rmi4_reg_write(rmi4_data,fhandler->full_addr.ctrl_base + offset,
					&ges_cfg,sizeof(ges_cfg));
	if(retval < 0) {
		printk(KERN_ERR "%s; fail to write gesture config ,error = %d\n", __func__,retval);
	}
	
	return;
}

//gionee songll 20131128 modify for printing dynamic TP log begin


static ssize_t tp_double_tap_switch_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", synaptics_wake_switch);
}

static ssize_t tp_double_tap_switch_write(struct device *dev,
				struct device_attribute *attr, const char *buf,size_t count)
{
	int rt;
	unsigned long val;
       //unsigned char test_wake_flag = 0, device_ctrl = 0;
	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}
	synaptics_wake_switch = val;
	printk("%s, %d\n", __func__, synaptics_wake_switch);

    
	return 1;
}

static DEVICE_ATTR(double_wake, 0664, tp_double_tap_switch_show, tp_double_tap_switch_write);

static ssize_t tp_gesture_switch_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", synaptics_gesture_switch);
}

static ssize_t tp_gesture_switch_write(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val);

	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}
	synaptics_gesture_switch = val;
	printk("%s, %d\n", __func__, synaptics_gesture_switch);
	return 1;
}

static DEVICE_ATTR(gesture_wake, 0664, tp_gesture_switch_show, tp_gesture_switch_write);




static unsigned long tp_dbg = 0;

static ssize_t tp_debug_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", tp_dbg);
}


static ssize_t tp_debug_write(struct device *dev,
				struct device_attribute *attr, const char *buf,size_t count)
{
	int rt;
	unsigned long val;

	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0){
		printk("%s, invalid value\n", __func__);
		return rt;
	}
	tp_dbg = val;
        
	return 1;
}

static DEVICE_ATTR(debug, 0664, tp_debug_show, tp_debug_write);




//gionee songll 20131128 modify for printing dynamic TP log end
// Gionee BSP 20150429 add for tp skylight mode begin
#if defined(GN_MTK_BSP_TP_SKYLIGHT_SUPPORT)
unsigned int skylight_switch = 0;
static ssize_t tp_skylight_switch_show(struct device *dev, 
					struct device_attribute *attr, const char *buf)
{
	return sprintf(buf, "%u\n", skylight_switch);
}
extern void synaptics_rmi4_skylight_control(int state);
static ssize_t tp_skylight_switch_write(struct device *dev,
				struct device_attribute *attr,const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	int enable;
	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}

	//glove_support = (val>0)?0x01:0x00;
	enable = val>0?1:0;
	synaptics_rmi4_skylight_control(enable);
	skylight_switch = val;
	TPD_DEBUG("%s, %u\n", __func__, skylight_switch);
	return count;
}
static DEVICE_ATTR(skylight_enable, 0664, tp_skylight_switch_show, tp_skylight_switch_write);

#endif
// Gionee BSP 20150429 add for tp skylight mode end

//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for glove mode begin

#if defined(GN_MTK_BSP_TP_GLOVE_SUPPORT)
unsigned int synaptics_glove_switch = 0;
//extern void gf_glove_mode(int state);
//unsigned int glove_support = 0;
static ssize_t tp_synaptics_glove_switch_show(struct device *dev, 
					struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", synaptics_glove_switch);
}

extern void synaptics_rmi4_glove_control(bool enable);
static ssize_t tp_synaptics_glove_switch_write(struct device *dev,
				struct device_attribute *attr,const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	bool enable;
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}

	//glove_support = (val>0)?0x01:0x00;
	enable = val>0?true:false;
	synaptics_rmi4_glove_control(enable);
	synaptics_glove_switch = val;
	TPD_DEBUG("%s, %u\n", __func__, synaptics_glove_switch);
	return count;
}
static DEVICE_ATTR(glove_enable, 0664, tp_synaptics_glove_switch_show, tp_synaptics_glove_switch_write);


//Gionee <GN_BSP_CTP> <mogongfu> <20161026> add for app/back key begin
#if 1
int hand_switch = 0;// 0:default 1:change
#else
extern int hand_switch;
#endif
static ssize_t hand_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
	printk("enter");

	len = scnprintf(buf, PAGE_SIZE,"%d\n", hand_switch);
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
	
	hand_switch = value;
	return count;
}
static DEVICE_ATTR(left_hand_mode, 0664, hand_mode_show, hand_mode_store);
#if 0
#if 1
int nav_mode_value = 1;
#else
extern int nav_mode_value;
#endif
static ssize_t nav_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
	int mod = 1;
	len = sprintf(buf, "%d\n", mod);
	return len;
}

/*
static ssize_t nav_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int value;
	int ret;
	printk("enter %s",buf);
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		printk("ret=0x%x", ret);
		return ret;
	}
	nav_mode_value = value;
	return count;
}
*/
static DEVICE_ATTR(nav_mode, 0444, nav_mode_show, NULL);

static ssize_t nav_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
        
	len = sprintf(buf,"%d\n", nav_mode_value);
	return len;
}

static ssize_t nav_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int value;
	int ret;
	printk("enter %s",buf);
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		printk("ret=0x%x", ret);
		return ret;
	}
	nav_mode_value = value;
	return count;
}
static DEVICE_ATTR(nav_enable, 0664, nav_enable_show, nav_enable_store);
#endif 
//Gionee <GN_BSP_CTP> <mogongfu> <20161026> add for app/back key end

#endif
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for glove mode end

//Gionee <GN_BSP_CTP> <linzhanchi> <20170825> modify for ID195979 start
//#if SYNAPTICS_ESD_CHECK_EN
#if 0
extern int touch_reset_sig_for_lcd;
static ssize_t tp_esd_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf,"%u\n",touch_reset_sig_for_lcd);
}
static ssize_t tp_esd_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	rt = kstrtoul(buf, 10,&val);
	if(rt != 0) {
		printk(KERN_ERR "%s, invalid value \n",__func__);
	}
	touch_reset_sig_for_lcd = val & 0xffff;
	TPD_DEBUG(" %s val=%lu \n",__func__,val);
	return count;
}
static DEVICE_ATTR(esd_debug, 0664, tp_esd_show, tp_esd_store);
//Gionee <GN_BSP_CTP> <linzhanchi> <20170825> modify for ID195979 end
#endif
#if defined(GN_MTK_BSP_TP_GESTURE_SUPPORT)
static ssize_t tp_gesture_config_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf,"%u\n",gesture_cfg);
}
static ssize_t tp_gesture_config_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	//rt = strict_strtoul(buf, 10,&val);
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0) {
		printk(KERN_ERR "%s, invalid value \n",__func__);
	}
	gesture_cfg = val & 0xffff;
	TPD_DEBUG(" %s val=%lu \n",__func__,val);
	return count;
}
static DEVICE_ATTR(gesture_config, 0664, tp_gesture_config_show, tp_gesture_config_store);

static ssize_t tp_gesture_coordition_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i,count;
	int len = 0;

	if(tpd_synaptics_devp == NULL)
		return 0;
	count = sizeof(tpd_synaptics_devp->f_point)/sizeof(tpd_synaptics_devp->f_point.data[0]);
	TPD_DEBUG(" %s count=%d \n",__func__,count);
	for(i=0;i<count;i++) {
		if(i==count-1)
			len += sprintf(buf+len,"%d",tpd_synaptics_devp->f_point.data[i]);
		else
			len += sprintf(buf+len,"%d,",tpd_synaptics_devp->f_point.data[i]);
	}
	
	return len;


}
static DEVICE_ATTR(gesture_coordition, 0444, tp_gesture_coordition_show, NULL);
#endif

//Gionee <GN_BSP_CTP> <mogongfu> <20161026> add for get TP data begin
#if defined(GN_MTK_BSP_TP_SELF_TEST)
extern int synaptics_rmi4_f54_command_func(int command, int value);
#if GN_GET_TP_TEST_DATA
extern void gn_synaptics_get_data(int result);
static int gn_synaptics_factory_check(void)
{
	int val=0x00;
	int count = 0;
	char *rawdata_tmp = NULL;

	synaptics_data_buf = (char *)vmalloc(128*16);
	if (NULL == synaptics_data_buf) 
		printk("[synaptics]can not malloc synaptics_data_buf\n");

	rawdata_tmp = synaptics_data_buf;

	do {
		synaptics_data_buf = rawdata_tmp;
		val = synaptics_rmi4_f54_command_func(1,0);
		printk("%s, val = %d count = %d\n",__func__, val, count);
		if(1 == val){
			count = 0;
			break;
		}
		msleep(50);
		count++;
	}while(count < 5);

	synaptics_data_buf = rawdata_tmp;

	if (1 == val)
		gn_synaptics_get_data(1);
	else
		gn_synaptics_get_data(0);
	return val;

}

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
static ssize_t get_tp_autotest_data_show(struct device *dev,struct device_attribute *attr,char *buf)
{

	int val = 0;
	
#if defined(GN_MTK_BSP_TP_SELF_TEST)
	short *rawdata;
	int rx_tx[2] = {0};
	int i,j;
	int size = 0;
	int len = 0;
	
	val = synaptics_rmi4_f54_command_func(3,2);
	val = synaptics_rmi4_f54_command_func(4,0);

	synaptics_rmi4_f54_get_TxRx_count((int* )&rx_tx);
	if(rx_tx[0] == 0 || rx_tx[1] == 0)
		return sprintf(buf,"%d",0);
	
	size = sizeof(short)*rx_tx[0]*rx_tx[1];
	rawdata = kzalloc(size,GFP_KERNEL);
	
	synaptics_rmi4_f54_get_raw_data(rawdata);

	for(i=0;i<rx_tx[0];i++) {
		for(j=0;j<rx_tx[1];j++) {
			if ((0 == ((i*rx_tx[1]+j)%(rx_tx[1]))) && (0 != (i*rx_tx[1])))
				len += sprintf(buf+len,"\n");
			
			len += sprintf(buf+len,"%d, ",rawdata[i*rx_tx[1]+j]);
		}
	}
	len += sprintf(buf+len,"\n");

	synaptics_save_diff_data(buf, len);

	kfree(rawdata);
#endif


	if (0 == val)
		val = 1;
	
	return sprintf(buf, "%d\n",val);

}
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

static DEVICE_ATTR(get_tp_autotest_data, 0444, get_tp_autotest_data_show, NULL);
#endif
//Gionee <GN_BSP_CTP> <mogongfu> <20161026> add for get TP data end



//extern int synaptics_f54_set_report_type(unsigned long val);
//extern int synaptics_f54_get_report_type();

static ssize_t synaptics_chip_self_test_function(struct device *dev, 
				struct device_attribute *attr, char *buf)
{
	int val=0x00;
	val = gn_synaptics_factory_check();
	return sprintf(buf, "%d\n",val);
}

static DEVICE_ATTR(factory_check, 0444, synaptics_chip_self_test_function, NULL);
static ssize_t synaptics_f54_report_type_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int val =0;
	val = synaptics_rmi4_f54_command_func(2,0);
	return sprintf(buf, "%d\n",val);
}

static ssize_t synaptics_f54_report_type_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	//rt = strict_strtoul(buf, 10,&val);
	rt = kstrtoul(buf, 10, &val);
	if(rt != 0) {
		printk(KERN_ERR "%s, invalid value \n",__func__);
	}
	synaptics_rmi4_f54_command_func(3,val);
	return count;
}
static DEVICE_ATTR(f54_report_type, 0664, synaptics_f54_report_type_show,synaptics_f54_report_type_store);

static ssize_t synaptics_f54_get_report_data_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int val = 0;
	val = synaptics_rmi4_f54_command_func(4,0);
	return sprintf(buf,"%d\n",val);
}
static DEVICE_ATTR(f54_get_report_data, 0444,synaptics_f54_get_report_data_show,NULL );

static ssize_t tpd_get_TxRx_count(struct device *dev,struct device_attribute *attr,
									char *buf)
{
	int rx_tx[2] = {0};
	synaptics_rmi4_f54_get_TxRx_count((int* )&rx_tx);
	//printk(KERN_ERR "yaoyc %s, tx=%d;rx=%d\n",__func__,rx_tx[0],rx_tx[1]);
	return sprintf(buf, "%d,%d\n",rx_tx[0],rx_tx[1]);
}
static DEVICE_ATTR(TxRx_count,0444,tpd_get_TxRx_count,NULL);

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
static ssize_t tpd_get_rawdata_value(struct device *dev,
				struct device_attribute *attr,char *buf)
{
	short *rawdata;
	int rx_tx[2] = {0};
	int i,j;
	int size = 0;
	int len = 0;
	synaptics_rmi4_f54_get_TxRx_count((int* )&rx_tx);
	if(rx_tx[0] == 0 || rx_tx[1] == 0)
		return sprintf(buf,"%d",0);
	
	size = sizeof(short)*rx_tx[0]*rx_tx[1];
	rawdata = kzalloc(size,GFP_KERNEL);
	
	synaptics_rmi4_f54_get_raw_data(rawdata);
	
	for(i=0;i<rx_tx[0];i++) {
		for(j=0;j<rx_tx[1];j++) {
			if ((0 == ((i*rx_tx[1]+j)%(rx_tx[1]))) && (0 != (i*rx_tx[1])))
				len += sprintf(buf+len,"\n");
			
			len += sprintf(buf+len,"%4d",rawdata[i*rx_tx[1]+j]);
		}
	}

	len += sprintf(buf+len,"\n");
		
	kfree(rawdata);
	
	return len;
}
static DEVICE_ATTR(get_rawdata,0444,tpd_get_rawdata_value,NULL);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end

static ssize_t tpd_get_rawdata_range(struct device *dev,
				struct device_attribute *attr,char *buf)
{
	int range[4];
	synaptics_rmi4_f54_get_rawdata_range((int* )&range);
	//printk(KERN_ERR "yaoyc %s, max=%d,min=%d key_max=%d,key_min=%d\n",__func__,range[0],range[1],range[2],range[3]);
	return sprintf(buf,"%d,%d,%d,%d\n",range[0],range[1],range[2],range[3]);
}

static DEVICE_ATTR(rawdata_range,0444,tpd_get_rawdata_range,NULL);

static ssize_t tpd_delta_data_show(struct device *dev,
				struct device_attribute *attr,char *buf)
{
	int val = 0;
	val = synaptics_rmi4_f54_command_func(3,2);
	val = synaptics_rmi4_f54_command_func(4,0);
	return sprintf(buf,"%d\n",val);
}

static DEVICE_ATTR(delta_data,0444,tpd_delta_data_show,NULL);

#endif

// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 begin
static ssize_t switching_log_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
        
	len = sprintf(buf,"%d\n", synaptics_log_switch);
	return len;
}

static ssize_t switching_log_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int value;
	int ret;
	
	printk("enter %s",buf);
	
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		printk("ret=0x%x", ret);
		return ret;
	}
	
	synaptics_log_switch = value;
	return count;
}
static DEVICE_ATTR(switching_log, 0664, switching_log_show, switching_log_store);
// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 end

//Gionee <GN_BSP_CTP> <linzhanchi> <20172020> add for ID72615 start
static ssize_t sensitivity_switch_support_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	return sprintf(buf, "%d\n", GN_BSP_TP_SWITCH_SENSITIVITY_SUPPORT);
}
static DEVICE_ATTR(sensitivity_switch_support, 0444, sensitivity_switch_support_show, NULL);

#if GN_BSP_TP_SWITCH_SENSITIVITY
int synap_sensitive_switch = 0;

static ssize_t tp_sensitivity_switch_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", synap_sensitive_switch);
}
static ssize_t tp_sensitivity_switch_write(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	int rt;
	int send_cfg_err = 0;
	
	rt = kstrtoint(buf, 10, &synap_sensitive_switch); 
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}	
	
	return size;
}
DEVICE_ATTR(tp_sensitivity_switch, 0664, tp_sensitivity_switch_show, tp_sensitivity_switch_write);
#endif
//Gionee <GN_BSP_CTP> <linzhanchi> <20172020> add for ID72615 end


static struct device_attribute *tp_feature_attr_list[] = {
    &dev_attr_debug, 
	&dev_attr_double_wake,	
	&dev_attr_gesture_wake,
#if defined(GN_MTK_BSP_TP_SKYLIGHT_SUPPORT)
	&dev_attr_skylight_enable,
#endif
#if defined(GN_MTK_BSP_TP_GLOVE_SUPPORT)
	&dev_attr_glove_enable,
	&dev_attr_left_hand_mode,
#if 	0
    &dev_attr_nav_mode,
	&dev_attr_nav_enable,
#endif
#endif
//#if SYNAPTICS_ESD_CHECK_EN
#if 0
//Gionee xuel 2016-4-16 Add For CR01660494 Begin
	&dev_attr_esd_debug,
#endif
//Gionee xuel 2016-4-16 Add For CR01660494 End
#if GN_GET_TP_TEST_DATA
	&dev_attr_get_tp_autotest_data,
//	&dev_attr_set_tp_autotest_data,
#endif
#if defined(GN_MTK_BSP_TP_GESTURE_SUPPORT)
	&dev_attr_gesture_config,
	&dev_attr_gesture_coordition,
#endif	
#if defined(GN_MTK_BSP_TP_SELF_TEST)	
	&dev_attr_f54_report_type,
	&dev_attr_factory_check,
	&dev_attr_f54_get_report_data,
	&dev_attr_TxRx_count,
	&dev_attr_get_rawdata,
	&dev_attr_rawdata_range,
	&dev_attr_delta_data,
#endif	

// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 begin
	&dev_attr_switching_log,
// Gionee <GN_BSP_CTP> <linzhanchi> <20170719>  add for ID171319 end

//Gionee <GN_BSP_CTP> <linzhanchi> <20172020> add for ID72615 start
	&dev_attr_sensitivity_switch_support,
#if GN_BSP_TP_SWITCH_SENSITIVITY
	&dev_attr_tp_sensitivity_switch,
#endif
//Gionee <GN_BSP_CTP> <linzhanchi> <20172020> add for ID72615 end

};

static int tpd_create_attr(struct device *dev) 
{
    int idx, err = 0;
    int num = (int)(sizeof(tp_feature_attr_list)/sizeof(tp_feature_attr_list[0]));
    if (dev == NULL)
    {
        return -EINVAL;
    }
    TPD_DMESG("tpd_create_attr ----0 \n");
    for(idx = 0; idx < num; idx++)
    {
		err = device_create_file(dev, tp_feature_attr_list[idx]);
        if(err != 0)
        {            
            TPD_DMESG("TPD  driver_create_file failed");
            break;
        }
    }    
    TPD_DMESG("TPD  driver_create_file success\n");
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

static long tpd_ges_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int temp;
	struct tpd_ges_data *dev_data = filp->private_data;
	TPD_DEBUG("%s; cmd : %u \n",__func__, cmd);
	switch (cmd)
	{
		case TPD_GESTURE_ENABLE:
			ret = __get_user(temp, (int __user *)arg);
			if(ret == 0) {
				//enable tpd_gesture or disable tpd_gesture;
			}
			break;
		case TPD_GESTURE_POINT_NUMBER:
			ret = __put_user(dev_data->length, (u8 __user *)arg);
			break;
		case TPD_GESTURE_FEATURE_INFO:
			if (copy_to_user((u16 __user*)arg, &dev_data->f_point, sizeof(dev_data->f_point))) {
                ret = -EFAULT;
                break;
            }
			break;
		case TPD_GESTURE_COORDINATE:
#ifdef GN_MTK_BSP_TP_USE_FULL_COORDINATE
			if(dev_data->length > 0) {
				ret = copy_to_user((u16 __user *)arg, dev_data->coordinate, dev_data->length*4);
			}
#endif
			break;
		default:
			return -EINVAL;
	}
	return ret;
}


#ifdef CONFIG_COMPAT
static long tpd_ges_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return tpd_ges_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
}
#else
#define tpd_ges_compat_ioctl NULL
#endif
static int tpd_ges_open(struct inode *inode, struct file *filp)
{
	filp->private_data = tpd_synaptics_devp;
	return 0;
}

static int tpd_ges_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	
	return ret;
}


struct file_operations tpd_ges_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = tpd_ges_ioctl,
	.compat_ioctl = tpd_ges_compat_ioctl,
	.open = tpd_ges_open,
	.release = tpd_ges_release,
};
/*
static struct miscdevice tpd_gesture_misc_device = {    
	.minor = MISC_DYNAMIC_MINOR,    
	.name = DEVICE_NAME,    
	.fops = &tpd_ges_fops,
};
*/
//#ifdef GN_MTK_BSP_TP_GLOVE_SUPPORT
#if 1
//extern struct platform_driver tpd_driver;

static struct platform_device gn_tp_wake_device = {
       .name   = "tp_wake_switch",
       .id     = -1,
};
#endif

int tpd_gesture_init(void)
{
	int ret;
#if 0
	ret = misc_register(&tpd_gesture_misc_device);  
	if (ret < 0){        
		printk(KERN_ERR "%s fail to register misc device.\n", __func__);
	}
#endif
	tpd_synaptics_devp = kzalloc(sizeof(struct tpd_ges_data),GFP_KERNEL);
	if(!tpd_synaptics_devp) {
		ret = -ENOMEM;
		goto fail_malloc;
	}


	ret = platform_device_register(&gn_tp_wake_device);
	if (ret)
	{
		printk("tpd: create gn_tp_wake_device failed\n");
	}
	ret = tpd_create_attr(&(gn_tp_wake_device.dev));
	if(ret)
	{
		printk("tpd: create gn_tp_feature attr failed \n");
	}
	/*
#if defined(GN_MTK_BSP_TP_GLOVE_SUPPORT)
	ret = device_create_file(&(gn_tp_wake_device.dev), &dev_attr_glove_enable);
	if (ret) 
	{
		printk("tpd: wake_tp_create_attr glove_enable failed.\n");
	}
#endif

#if defined(GN_MTK_BSP_TP_GESTURE_SUPPORT)
	ret = device_create_file(&(gn_tp_wake_device.dev), &dev_attr_gesture_config);
	if (ret) 
	{
		printk("tpd: wake_tp_create_attr gesture_config failed.\n");
	}
#endif

#if defined(GN_MTK_BSP_TP_SELF_TEST)
	ret = device_create_file(&(gn_tp_wake_device.dev), &dev_attr_factory_check);
    if(ret) {
		printk("tpd: wake_tp_create_attr factory_check failed.\n");
	}
	ret = device_create_file(&(gn_tp_wake_device.dev), &dev_attr_f54_report_type);
	if(ret) {
		printk("tpd: wake_tp_create_attr f54_report_type failed\n");
	}
	ret = device_create_file(&(gn_tp_wake_device.dev), &dev_attr_f54_get_report_data);
	if(ret) {
		printk("tpd: wake_tp_create_attr f54_get_report_data failed \n");
	}
#endif
*/
	return ret;
fail_malloc:
#if 0
	misc_deregister(&tpd_gesture_misc_device);
#endif
	return ret;
}

int tpd_gesture_exit(void)
{
	
	printk(KERN_INFO "tpd_gesture: char dev clean up \n");
	tpd_delete_attr(&(gn_tp_wake_device.dev));
	return 0;
}

