/*
*  This head file is used for gionee touch debug
*  Gionee log tool will Control authority of the log 
*
*	AUTHOR:		linzhanchi
*	DATE:		2017-7-19
*	COPYWRITE:	GIONEE
*/

#ifndef __GN_LOG_H
#define __GN_LOG_H

#define GN_DEBUG_INFO(fmt,arg...)				printk("<GN_TOUCH_INFO>"fmt"\n",##arg)
#define GN_DEBUG_ERROR(fmt,arg...)				printk("<GN_TOUCH_ERROR>"fmt"\n",##arg)

/*
*	param 1:		0 for closing log,others for debugging
*	other params:	the same of "printk"
*/
#define GN_DEBUG_LOG(switching,fmt,arg...)		do{\
													if(switching)\
														printk("tp_trace_"fmt"\n", ##arg);\
												}while(0)

#define GN_FUNCTION_LOG()						do{\
													printk("<GN_TOUCH_FUNCTION> Func:%s@Line:%d\n",__func__,__LINE__);\
												}while(0)

#define	TP_TRACE(fmt,arg...)					do{\
                            						printk("tp_trace_"fmt"\n", ##arg);\
                   								}while(0)
#endif
