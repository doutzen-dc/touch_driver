#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 begin
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 end

#define COVER_FILE 		1
#define ADD_FILE_TAIL 	0
#define TEST_DATA_LEN   1024*80
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 begin
#define CFG_F54_TXCOUNT 18
#define CFG_F54_RXCOUNT 32
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  modify for ID186117 end
#define SYNAPTICS_SAVE_FILEPATH "/mnt/sdcard/tpdata/" 

extern char *synaptics_data_buf;
extern int 	synaptics_data_len;
extern int rawdata_tx;
extern int rawdata_rx;

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 begin
static int num;
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 end

static void synaptics_save_data(char *file_name, char *data_buf, int iLen, int flag)
{
	struct file *pfile = NULL;
	
	char filepath[128];
	loff_t pos;
	mm_segment_t old_fs;
	int current_len = 0;

	memset(filepath, 0, sizeof(filepath));
	sprintf(filepath, "%s%s", SYNAPTICS_SAVE_FILEPATH, file_name);
	if (NULL == pfile)
		pfile = filp_open(filepath, O_CREAT|O_RDWR, 0);
	if (IS_ERR(pfile)) {
		pr_err("error occured while opening file %s.\n", filepath);
		return;
	}

	if (ADD_FILE_TAIL == flag)
		current_len = pfile->f_op->llseek(pfile, 0, SEEK_END);

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	
	if (ADD_FILE_TAIL == flag)
		pos = current_len;
	else
		pos = 0;
	
	vfs_write(pfile, data_buf, iLen, &pos);
	filp_close(pfile, NULL);
	set_fs(old_fs);
	
}

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 begin
static int Synaptics_GetDataTime(char *data_time, int iLen)
{
	struct timex  txc;
	struct rtc_time tm;

    memset(data_time,0,iLen);

	do_gettimeofday(&(txc.time));
	rtc_time_to_tm(txc.time.tv_sec+28800, &tm);

    sprintf(data_time,"\n\nnum=%d %d-%02d-%02d %02d-%02d-%02d\n", num, (tm.tm_year+1900), 
        (tm.tm_mon+1), tm.tm_mday, (tm.tm_hour), tm.tm_min, tm.tm_sec);

	num++;

	return 0;
}
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 end

void gn_synaptics_get_data(int result)
{
	char *testdata = NULL;
	int result_len = 0;

	testdata = (char *)vmalloc(TEST_DATA_LEN);
	if(NULL == testdata)
	{
		printk("vmalloc failed in function:%s\n", __func__);
		vfree(synaptics_data_buf);
		return;
	}
	memset(testdata, 0, TEST_DATA_LEN);
	if (1 == result)
	{	
		sprintf(testdata, "Sucess\nchannelnum_x=%d\nchannelnum_y=%d\n", rawdata_tx, rawdata_rx);
	} else {
		sprintf(testdata, "Failed\nchannelnum_x=%d\nchannelnum_y=%d\n", rawdata_tx, rawdata_rx);
	}
	result_len = strlen(testdata);
	if (NULL != synaptics_data_buf)
	{
		memcpy(testdata+result_len, synaptics_data_buf, synaptics_data_len);		
		synaptics_save_data("raw_data.csv", testdata, synaptics_data_len+result_len, COVER_FILE);
		vfree(synaptics_data_buf);
		vfree(testdata);
	}

}

// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 begin
void synaptics_save_diff_data(char *diff_data, int len)
{
	char data_time[50] = {0};
	char new_line[] = "\n\n";
	
	Synaptics_GetDataTime(data_time,sizeof(data_time));
	synaptics_save_data("diff_data.csv", data_time, strlen(data_time), ADD_FILE_TAIL);
	synaptics_save_data("diff_data.csv", diff_data, len, ADD_FILE_TAIL);
	synaptics_save_data("diff_data.csv", new_line, strlen(new_line), ADD_FILE_TAIL);
}
// Gionee <GN_BSP_CTP> <linzhanchi> <20170818>  add for ID186117 end


