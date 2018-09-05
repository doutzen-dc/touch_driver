
/**File Description: This is a gionee TP feature driver head file for Focaltech TouchPanel
Author: hupiing Create Date:20151109
Change List:  Time:        Author:        Description:

*/

#include <linux/miscdevice.h>
#include "tpd.h"
#include "gn_tpd_feature.h"
#include <linux/gpio.h>
#include "nt36xxx.h"
#if GN_GET_TP_AUTOTEST_DATA
#include <linux/timer.h>
#endif

MODULE_LICENSE("GPL V2");


struct tpd_ges_data *nvt_tpd_ges_devp = NULL;

#ifdef GN_BSP_TP_GESTURE_SUPPORT
unsigned int nvt_gesture_cfg;
#endif
int nvt_wake_switch = 0;
int nvt_gesture_switch = 0;

#if GN_GET_TP_AUTOTEST_DATA
#define TIMER_WORK_DELAY_TIME 100
static void tp_data_heartbeat_func(unsigned long arg);
struct timer_list heartbeat_timer;
static struct workqueue_struct *timer_workqueue = NULL;
static struct delayed_work nvt_timer_work;
DEFINE_MUTEX(nvt_timer_lock);
atomic_t get_tp_data_open = ATOMIC_INIT(0);
atomic_t get_tp_data_heartbeat_detect = ATOMIC_INIT(1);
atomic_t get_tp_data_heartbeat_check = ATOMIC_INIT(0);
#endif

extern int fts_read_reg(struct i2c_client *client, u8 regaddr, u8 *regvalue);


#ifdef CONFIG_CY_BSP_MTK_DEVICE_CHECK
extern int cy_set_device_info(struct cy_device_info cy_dev_info);

 int nvt_register_hardware_info(void)
{
	u8 fw_ver = 0;
	u8 vendor_id = 0;
	int ret = 0;
	u8 buf[3] = {0};

	struct cy_device_info cy_tp_hw_info;

	buf[0] = EVENT_MAP_FWINFO;
	buf[1] = 0x00;
	buf[2] = 0x00;
	ret = CTP_I2C_READ(ts->client, I2C_BLDR_Address, buf, 3);
	if (ret < 0) {
		NVT_ERR("i2c read error!(%d)\n", ret);
		return ret;
	}
	fw_ver = buf[1];
    //Chenyee <CY_BSP_CTP> <xuel> <2017-12-22> Modify For #CSW1705A-12 Begin
    //read vendor id
    buf[0] = EVENT_MAP_VENDORID;
    buf[1] = 0x00;
    buf[2] = 0x00;
    ret = CTP_I2C_READ(ts->client, I2C_BLDR_Address, buf, 3);
    if (ret < 0) {
        NVT_ERR("i2c read vendor id error!(%d)\n", ret);
        return ret;
    }
    vendor_id = buf[1];
    //Chenyee <CY_BSP_CTP> <xuel> <2017-12-22> Modify For #CSW1705A-12 End
	cy_tp_hw_info.cy_dev_type = CY_DEVICE_TYPE_TP;

    //Chenyee <CY_BSP_CTP> <xuel> <2018-01-29> Modify For #CSW1705A-1390 Begin
	snprintf(cy_tp_hw_info.name, CY_DEVICE_NAME_LEN, "tm-nt36525b_id[0x%x]_fw[0x%x]_[720, 1440]\n",
		vendor_id,fw_ver);
    //Chenyee <CY_BSP_CTP> <xuel> <2018-01-29> Modify For #CSW1705A-1390 End
	snprintf(cy_tp_hw_info.vendor, CY_DEVICE_VENDOR_LEN, "GIONEE");
	snprintf(cy_tp_hw_info.version, CY_DEVICE_VERSION_LEN, "00");

	cy_set_device_info(cy_tp_hw_info);

	return ret;
}
#endif

#ifdef	GN_BSP_TP_GESTURE_SUPPORT
static ssize_t tp_double_wake_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", nvt_wake_switch);
}
static ssize_t tp_double_wake_write(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	int rt;
	unsigned long val;
	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val); 
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}
	nvt_wake_switch = val;
	NVT_LOG("%s, %d\n", __func__, nvt_wake_switch);
	return size;
}
static DEVICE_ATTR(double_wake, 0664, tp_double_wake_show, tp_double_wake_write);

static ssize_t tp_gesture_switch_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", nvt_gesture_switch);
}
static ssize_t tp_gesture_switch_write(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	int rt;
	unsigned long val;
	//rt = strict_strtoul(buf, 10, &val); 
	rt = kstrtoul(buf, 10, &val); 
	if(rt != 0){
		pr_err("%s, invalid value\n", __func__);
		return rt;
	}
	nvt_gesture_switch = val;
	NVT_LOG("%s, %d\n", __func__, nvt_gesture_switch);
	return size;
}
static DEVICE_ATTR(gesture_wake, 0664, tp_gesture_switch_show, tp_gesture_switch_write);

#endif

//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 Begin
#ifdef CY_CTP_LANDSCAPE_MODE
//Chenyee <CY_CTP_BSP> <xuel> <2018-01-02> Modify For #CSW1705A-74 Begin
#define NVT_SCREEN_VERTICAL    0x77
#define NVT_SCREEN_HORIZONTAL  0x78
//Chenyee <CY_CTP_BSP> <xuel> <2018-01-02> Modify For #CSW1705A-74 End

int cy_nvt_screen_switch = 0;

void nvt_horizontal_vertical_setting(uint8_t is_horizontal)
{
    uint8_t buf[10] = {0};
    uint32_t retry = 0;
    uint32_t ret = 0;

    mutex_lock(&ts->lock);

    if (is_horizontal == 1) {
        NVT_LOG("Enter horizontal mode\n");
        buf[0] = EVENT_MAP_HOST_CMD;
        buf[1] = NVT_SCREEN_HORIZONTAL;
    } else if (is_horizontal == 0) {
        NVT_LOG("Enter vertical mode, is_horizontal = %d\n", is_horizontal);
        buf[0] = EVENT_MAP_HOST_CMD;
        buf[1] = NVT_SCREEN_VERTICAL;
    } else {
        NVT_ERR("set horizontal_vertical (%d) failed, invalid parameter\n", is_horizontal);
        return;
    }

    ret = CTP_I2C_WRITE(ts->client, I2C_FW_Address, buf, 2);
    if (ret < 0)
        NVT_ERR("set horizontal_vertical (%d) failed:%d\n", is_horizontal, ret);

    /* read back fw EVENT_MAP_HOST_CMD */
    while (1) {
        msleep(18); //wait for 1 frame (~16.66ms)

        buf[0] = EVENT_MAP_HOST_CMD;
        buf[1] = 0xFF;
        CTP_I2C_READ(ts->client, I2C_FW_Address, buf, 2);

        if (buf[1] == 0x00) {
            ret = 0;
            break;
        }

        retry++;
        if(unlikely(retry > 5)) {
            NVT_ERR("error, retry=%d, buf[1]=0x%02X\n", retry, buf[1]);
            ret = -1;
            break;
        }
    }

    mutex_unlock(&ts->lock);

    return;
}

static ssize_t nvt_landscape_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", cy_nvt_screen_switch);
}

static ssize_t nvt_landscape_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret = -1;
    long val = -1;

    ret = kstrtoul(buf, 10, &val); 
    if(ret != 0) {
        printk(KERN_ERR "%s, invalid value \n",__func__);
    }

    cy_nvt_screen_switch = (val > 0)?0x01:0x00;

    if(cy_nvt_screen_switch == 0x01)
        nvt_horizontal_vertical_setting(1);
    else
        nvt_horizontal_vertical_setting(0);

    return count;
}

static DEVICE_ATTR(landscape_mode, 0664, nvt_landscape_mode_show, nvt_landscape_mode_store);
#endif
//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 End

static ssize_t tp_manufacturer_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	return sprintf(buf, "Novatek\n");
}
static DEVICE_ATTR(manufacturer, 0444, tp_manufacturer_show, NULL);

extern int nvt_test_result;
static ssize_t nt36xxx_factory_check(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	
	struct file *pfile = NULL;
		if (NULL == pfile)
		pfile = filp_open("/proc/nvt_selftest", O_RDONLY, 0);
	if (IS_ERR(pfile)) {
		pr_err("error occured while opening file /proc/nvt_selftest.\n");
		return -EIO;
	}
	filp_close(pfile, NULL);
	return sprintf(buf, "%d\n", nvt_test_result);
}

static DEVICE_ATTR(factory_check, 0444, nt36xxx_factory_check, NULL);

#if GN_GET_TP_AUTOTEST_DATA
static ssize_t get_tp_autotest_data_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	int result = 0;  
	//result = ftxxxx_ftslibtest_sample(1);
	return sprintf(buf, "%d\n", result);

}

static DEVICE_ATTR(get_tp_autotest_data, 0444, get_tp_autotest_data_show, NULL);

static ssize_t set_tp_autotest_data_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	return sprintf(buf, "%d\n", atomic_read(&get_tp_data_open));
}


static ssize_t set_tp_autotest_data_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int rt;
	unsigned int tp_tmp;
	
	rt = kstrtoint(buf, 10, &tp_tmp);
	if(rt != 0) {
		printk(KERN_ERR "%s, invalid value \n",__func__);
	}

	switch (tp_tmp)
	{
		case 0:
			NVT_LOG("APK close get data function normally\n");
			atomic_set(&get_tp_data_open, tp_tmp);
			mutex_lock(&gt_timer_lock);
			del_timer_sync(&heartbeat_timer);
			mutex_unlock(&gt_timer_lock);
			break;
		case 1:
			NVT_LOG("APK open get data function normally\n");
			atomic_set(&get_tp_data_open, tp_tmp);
			mutex_lock(&gt_timer_lock);
			//add_timer(&heartbeat_timer);
			mod_timer(&heartbeat_timer, jiffies + 4*HZ);
			mutex_unlock(&gt_timer_lock);
			break;
		case 2:
			atomic_inc(&get_tp_data_heartbeat_detect);
			//NVT_LOG("beat beat beat!!! already beat %d times\n", atomic_read(&get_tp_data_heartbeat_detect));
			break;
		default:
			break;
	}

	//NVT_LOG(" %s val=%lu \n",__func__, atomic_read(&get_tp_data_open));
	return count;
}


static DEVICE_ATTR(set_tp_autotest_data, 0664, set_tp_autotest_data_show, set_tp_autotest_data_store);
#endif

#ifdef	GN_BSP_TP_GESTURE_SUPPORT
static ssize_t tp_gesture_coordition_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i,count;
	int len = 0;

	if(nvt_tpd_ges_devp == NULL)
		return 0;
	count = sizeof(nvt_tpd_ges_devp->f_point)/sizeof(nvt_tpd_ges_devp->f_point.data[0]);
	NVT_LOG(" %s count=%d \n",__func__,count);
	for(i=0;i<count;i++) {
		if(i==count-1)
			len += sprintf(buf+len,"%d",nvt_tpd_ges_devp->f_point.data[i]);
		else
			len += sprintf(buf+len,"%d,",nvt_tpd_ges_devp->f_point.data[i]);
	}
	
	return len;

}
static DEVICE_ATTR(gesture_coordition, 0444, tp_gesture_coordition_show, NULL);

static ssize_t tp_gesture_config_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf,"%u\n",nvt_gesture_cfg);
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
	nvt_gesture_cfg = val & 0xffff;

	NVT_LOG(" %s gesture_cfg val=0x%04lX \n",__func__,val);

	return count;
}
static DEVICE_ATTR(gesture_config, 0664, tp_gesture_config_show, tp_gesture_config_store);
#endif

int nvt_glove_switch = 0;

#ifdef CONFIG_GN_BSP_FPR_GLOVE_SUPPORT
extern void gf_glove_mode(int state);
#endif

static void nvt_set_glove_mode(void){
    u8 buf[4] = {0};
    int ret = 0;

    if (1 == nvt_glove_switch) {
            //---enable glove mode---
            buf[0] = EVENT_MAP_HOST_CMD;
            buf[1] = 0x71;
            ret = CTP_I2C_WRITE(ts->client, I2C_FW_Address, buf, 2);
            if (ret != 1)
                NVT_ERR("set glove switch:%d, failed:%d\n", nvt_glove_switch, ret);
	}else {
            //---disable glove mode---
            buf[0] = EVENT_MAP_HOST_CMD;
            buf[1] = 0x72;
            ret = CTP_I2C_WRITE(ts->client, I2C_FW_Address, buf, 2);
            if (ret != 1)
                NVT_ERR("set glove switch:%d, failed:%d\n", nvt_glove_switch, ret);
	}
}

static ssize_t smart_cover_show(struct device *dev, 
                struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", nvt_glove_switch);

}
static ssize_t smart_cover_store(struct device *dev,
				struct device_attribute *attr, const char *buf, size_t count)
{
	int rt;
	unsigned long val;
	rt = kstrtoul(buf, 10, &val); 
	if(rt != 0) {
		printk(KERN_ERR "%s, invalid value \n",__func__);
	}

	nvt_glove_switch = (val > 0)?0x01:0x00;
	NVT_LOG(" nvt_glove_switch =%d, val=%lu \n", nvt_glove_switch, val);
	nvt_set_glove_mode();

	#ifdef CONFIG_GN_BSP_FPR_GLOVE_SUPPORT
	//gf_glove_mode(nvt_glove_switch);
	#endif

	return count;
}


static DEVICE_ATTR(glove_enable, 0664, smart_cover_show, smart_cover_store);

//Chenyee <CY_CTP_BSP> <xuel> <2018-01-16> Added For #CSW1705A-207 Begin
#ifdef CONFIG_CY_BSP_KEY_SWITCH
#if 1
//Chenyee <CY_CTP_BSP> <xuel> <2018-01-16> Added For #CSW1705A-207 End
int hand_switch = 0;// 0:default 1:change
#else
extern int hand_switch;
#endif
static ssize_t hand_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t len;
	NVT_LOG("enter");

	len = scnprintf(buf, PAGE_SIZE,"%d\n", hand_switch);
	return len;
}

static ssize_t hand_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int value;
	int ret;
	NVT_LOG("enter %s",buf);
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		printk("ret=0x%x", ret);
		return ret;
	}
	hand_switch = value;
	return count;
}
static DEVICE_ATTR(left_hand_mode, 0664, hand_mode_show, hand_mode_store);
#endif

#ifdef CONFIG_GN_FP_HAVE_NAV_FUNC
#if 0
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
	NVT_LOG("enter %s",buf);
	if((ret = kstrtoint(buf, 10, &value)) < 0)
	{
		NVT_LOG("ret=0x%x", ret);
		return ret;
	}
	nav_mode_value = value;
	return count;
}
*/
static DEVICE_ATTR(nav_mode, 0664, nav_mode_show, NULL);

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
	NVT_LOG("enter %s",buf);
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

static struct device_attribute *tp_feature_attr_list[] = {
#ifdef	GN_BSP_TP_GESTURE_SUPPORT
	&dev_attr_double_wake,
	&dev_attr_gesture_wake,
	&dev_attr_gesture_coordition,
	&dev_attr_gesture_config,
#endif
	&dev_attr_glove_enable,
	&dev_attr_manufacturer,

//Chenyee <CY_CTP_BSP> <xuel> <2018-01-16> Added For #CSW1705A-207 Begin
#ifdef CONFIG_CY_BSP_KEY_SWITCH
//Chenyee <CY_CTP_BSP> <xuel> <2018-01-16> Added For #CSW1705A-207 End
    &dev_attr_left_hand_mode,
#endif

#ifdef CONFIG_GN_FP_HAVE_NAV_FUNC
    &dev_attr_nav_mode,
    &dev_attr_nav_enable,
#endif
    &dev_attr_factory_check,

#if GN_GET_TP_AUTOTEST_DATA
	&dev_attr_get_tp_autotest_data,
	&dev_attr_set_tp_autotest_data,
#endif

//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 Begin
#ifdef CY_CTP_LANDSCAPE_MODE
    &dev_attr_landscape_mode,
#endif
//Chenyee <CY_CTP_BSP> <xuel> <2017-12-14> Added For #CSW1705A-12 End

};

#if GN_GET_TP_AUTOTEST_DATA
static void tp_data_heartbeat_func(unsigned long arg)
{
	queue_delayed_work(timer_workqueue, &nvt_timer_work, msecs_to_jiffies(TIMER_WORK_DELAY_TIME));
}
static void tp_timer_work_func(struct work_struct * work)
	{
		if (atomic_read(&get_tp_data_heartbeat_detect) > atomic_read(&get_tp_data_heartbeat_check))
		{
			mutex_lock(&gt_timer_lock);
			mod_timer(&heartbeat_timer, jiffies + 4*HZ);
			mutex_unlock(&gt_timer_lock);
			atomic_set(&get_tp_data_heartbeat_check, get_tp_data_heartbeat_detect.counter);
			NVT_LOG("%s::get_tp_data_heartbeat_detect = %d\n", __func__, atomic_read(&get_tp_data_heartbeat_detect));
		} else {		
			NVT_LOG("APK exit unormally,close get tp data function\n");
			atomic_set(&get_tp_data_open, 0);
			mutex_lock(&gt_timer_lock);
			del_timer_sync(&heartbeat_timer);
			mutex_unlock(&gt_timer_lock);
		}
	}

#endif

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
        if(err)
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
static struct platform_device gn_tp_wake_device = {
       .name   = "tp_wake_switch",
       .id     = -1,
};

int gn_nvt_tpd_feature_init_data(struct i2c_client *client)
{
	int ret;

	nvt_tpd_ges_devp = kzalloc(sizeof(struct tpd_ges_data),GFP_KERNEL);
	if(!nvt_tpd_ges_devp) {
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
#ifdef CONFIG_CY_BSP_MTK_DEVICE_CHECK
	ret = nvt_register_hardware_info();
	if (ret < 0) {
		return -1;
	}
#endif

#if GN_GET_TP_AUTOTEST_DATA
	init_timer(&heartbeat_timer);
	heartbeat_timer.function= &tp_data_heartbeat_func;
	heartbeat_timer.expires = jiffies + 4*HZ;

	timer_workqueue = create_singlethread_workqueue("tp_timer_workthread");
	if (timer_workqueue == NULL) {
		printk("Create workqueue failed!");
	}
	
	INIT_DELAYED_WORK(&nvt_timer_work, tp_timer_work_func);
#endif

	return ret;
fail_malloc:
	return ret;
}

int gn_nvt_tpd_feature_reinit(void)
{
	NVT_LOG(KERN_INFO "tpd_gesture: char dev clean up \n");
	tpd_delete_attr(&(gn_tp_wake_device.dev));
	platform_device_unregister(&gn_tp_wake_device);

#if GN_GET_TP_AUTOTEST_DATA
		if (timer_workqueue) {
			destroy_workqueue(timer_workqueue);
		}
#endif

    return 0;
}
