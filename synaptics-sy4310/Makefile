# Linux driver folder

# Chenyee <CY_BSP_CTP> <hezheng> <2017-10-18> modify For ID241394 Begin 
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/synaptics_dsx/
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
#ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/synaptics_v61/include
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
#ccflags-y += -I$(srctree)/drivers/watchdog/mediatek/
#ccflags-y += -I$(srctree)/drivers/watchdog/mediatek/wdt/
#ccflags-y += -I$(srctree)/drivers/watchdog/mediatek/wdt/$(MTK_PLATFORM)/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/i2c/$(MTK_PLATFORM)/

ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/gn_td4310/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
# Chenyee <CY_BSP_CTP> <hezheng> <2017-10-18> modify For ID241394 end 

obj-y +=  synaptics_dsx_i2c.o
obj-y +=  synaptics_dsx_rmi_dev.o
obj-y +=  synaptics_dsx_fw_update.o
obj-y +=  synaptics_dsx_test_reporting.o
obj-y +=  gn_tpd_feature.o
obj-y +=  synaptics_dsx_self_test.o
obj-y += gn_tp_auto_test.o


