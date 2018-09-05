/*
 * Synaptics DSX touchscreen driver
 *
 * Copyright (C) 2012 Synaptics Incorporated
 *
 * Copyright (C) 2012 Alexandra Chin <alexandra.chin@tw.synaptics.com>
 * Copyright (C) 2012 Scott Lin <scott.lin@tw.synaptics.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/ctype.h>
#include <linux/hrtimer.h>
#include "synaptics_dsx.h"
#include "synaptics_dsx_i2c.h"
//#include "synaptics_dsx_core.h"
#include "synaptics_dsx_self_test.h"

/*
#define RAW_HEX
#define HUMAN_READABLE
*/


// Chenyee <YC_BSP_CTP> <hezheng> <20171024>  modify for ID241394 begin
#define CFG_F54_TXCOUNT 18
#define CFG_F54_RXCOUNT 30	//32
// Chenyee <YC_BSP_CTP> <hezheng> <20171024>  modify for ID241394 end

#define F54_HR_RX_OFFSET 400
#define F54_HR_TX_OFFSET 400
#define F54_HR_MIN       -400

//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test begin
#define RAW_DATA_MAX 4000
#define RAW_DATA_MIN 1000
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test end
#define RAW_KEY_DATA_MAX 3000
#define RAW_KEY_DATA_MIN 1300

//Gionee <GN_BSP_CTP> <mogongfu> <20161210> add for get data start
extern char *synaptics_data_buf;
int synaptics_data_len = 0;
int rawdata_tx = 0;
int rawdata_rx = 0;
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> add for get data end

//Chenyee <YC_BSP_CTP> <hezheng> <20171024> modify for ID241394 tp full screen test begin
#if 0
static short LowerLimit[CFG_F54_TXCOUNT][CFG_F54_RXCOUNT] ={	
	{1336, 1257, 1235, 1220, 1183, 1172, 1225, 1198, 1119, 1199, 1161, 1272, 1189, 1209, 1188, 1126, 1151, 1193, 1093, 1108, 1220, 1156, 1163, 1114, 1029, 1062},
	{1277, 1198, 1183, 1167, 1135, 1130, 1188, 1161, 1082, 1167, 1125, 1235, 1151, 1188, 1161, 1098, 1125, 1167, 1066, 1082, 1193, 1130, 1135, 1082, 998, 1024},
	{1283, 1198, 1177, 1163, 1135, 1125, 1189, 1161, 1082, 1161, 1119, 1230, 1140, 1183, 1151, 1093, 1125, 1167, 1071, 1088, 1193, 1130, 1130, 1082, 987, 1014},
	{1288, 1198, 1183, 1168, 1140, 1135, 1204, 1167, 1088, 1161, 1125, 1230, 1145, 1188, 1156, 1099, 1135, 1177, 1077, 1093, 1198, 1130, 1135, 1082, 987, 1014},
	{1283, 1193, 1177, 1167, 1131, 1125, 1193, 1161, 1082, 1161, 1125, 1235, 1145, 1188, 1152, 1098, 1130, 1172, 1077, 1088, 1194, 1131, 1130, 1077, 982, 1008},
	{1299, 1209, 1193, 1193, 1156, 1151, 1214, 1188, 1114, 1188, 1161, 1267, 1183, 1220, 1188, 1135, 1167, 1204, 1108, 1119, 1230, 1161, 1161, 1103, 1008, 1034},
	{1314, 1251, 1235, 1225, 1204, 1204, 1246, 1240, 1161, 1214, 1209, 1320, 1230, 1277, 1257, 1193, 1225, 1258, 1167, 1184, 1279, 1230, 1230, 1161, 1077, 1105},
	{1257, 1184, 1144, 1158, 1159, 1115, 1134, 1181, 1095, 1222, 1172, 1148, 1121, 1188, 1140, 1167, 1118, 1062, 1094, 1069, 1170, 991, 1095, 1112, 1030, 1057},
	{1235, 1172, 1135, 1147, 1147, 1103, 1125, 1172, 1082, 1214, 1156, 1130, 1093, 1167, 1115, 1140, 1098, 1045, 1071, 1051, 1151, 973, 1077, 1103, 1019, 1051},
	{1295, 1235, 1198, 1216, 1214, 1177, 1204, 1251, 1167, 1309, 1246, 1230, 1189, 1267, 1225, 1253, 1204, 1151, 1183, 1161, 1262, 1082, 1183, 1198, 1110, 1135},
	{1299, 1231, 1198, 1214, 1214, 1177, 1204, 1251, 1161, 1299, 1240, 1220, 1177, 1262, 1214, 1246, 1188, 1135, 1168, 1151, 1246, 1071, 1172, 1188, 1103, 1130},
	{1235, 1161, 1119, 1135, 1126, 1082, 1114, 1147, 1061, 1188, 1130, 1098, 1056, 1140, 1082, 1110, 1045, 987, 1019, 998, 1103, 934, 1036, 1062, 987, 1020},
	{1226, 1156, 1114, 1135, 1125, 1077, 1110, 1142, 1056, 1183, 1130, 1103, 1052, 1140, 1079, 1108, 1047, 987, 1019, 999, 1103, 929, 1034, 1061, 987, 1008},
	{1283, 1220, 1183, 1198, 1189, 1151, 1177, 1220, 1140, 1262, 1220, 1198, 1156, 1240, 1189, 1225, 1172, 1114, 1151, 1130, 1235, 1056, 1156, 1177, 1098, 1119},
	{1299, 1240, 1204, 1220, 1204, 1168, 1188, 1235, 1145, 1272, 1235, 1214, 1167, 1257, 1204, 1240, 1177, 1114, 1151, 1130, 1240, 1057, 1167, 1183, 1105, 1140},
	{6434, 8395, 10502, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500},
};

static short UpperLimit[CFG_F54_TXCOUNT][CFG_F54_RXCOUNT] ={	
	{2775, 2610, 2566, 2533, 2457, 2435, 2544, 2489, 2324, 2492, 2412, 2643, 2470, 2512, 2467, 2339, 2390, 2478, 2270, 2303, 2533, 2401, 2416, 2313, 2138, 2207},
	{2654, 2489, 2457, 2424, 2358, 2347, 2467, 2412, 2249, 2424, 2336, 2566, 2390, 2467, 2412, 2281, 2336, 2424, 2215, 2249, 2478, 2347, 2358, 2249, 2073, 2127},
	{2664, 2489, 2446, 2416, 2358, 2336, 2470, 2412, 2249, 2412, 2324, 2555, 2369, 2457, 2390, 2270, 2336, 2424, 2226, 2259, 2478, 2347, 2347, 2249, 2050, 2106},
	{2677, 2489, 2457, 2427, 2369, 2358, 2501, 2424, 2259, 2412, 2336, 2555, 2380, 2467, 2401, 2284, 2358, 2446, 2236, 2270, 2489, 2347, 2358, 2249, 2050, 2106},
	{2664, 2478, 2446, 2424, 2350, 2336, 2478, 2412, 2249, 2412, 2336, 2566, 2380, 2467, 2393, 2281, 2347, 2435, 2236, 2259, 2481, 2350, 2347, 2236, 2039, 2095},
	{2698, 2512, 2478, 2478, 2401, 2390, 2523, 2467, 2313, 2467, 2412, 2632, 2457, 2533, 2467, 2358, 2424, 2501, 2303, 2324, 2555, 2412, 2412, 2292, 2095, 2149},
	{2731, 2600, 2566, 2544, 2501, 2501, 2589, 2577, 2412, 2523, 2512, 2741, 2555, 2654, 2610, 2478, 2544, 2613, 2424, 2459, 2656, 2555, 2555, 2412, 2236, 2295},
	{2610, 2459, 2377, 2407, 2408, 2316, 2355, 2454, 2276, 2539, 2435, 2385, 2328, 2467, 2369, 2424, 2322, 2207, 2273, 2220, 2430, 2058, 2276, 2311, 2141, 2196},
	{2566, 2435, 2358, 2382, 2382, 2292, 2336, 2435, 2249, 2523, 2401, 2347, 2270, 2424, 2316, 2369, 2281, 2172, 2226, 2182, 2390, 2020, 2236, 2292, 2116, 2182},
	{2690, 2566, 2489, 2525, 2523, 2446, 2501, 2600, 2424, 2720, 2589, 2555, 2470, 2632, 2544, 2602, 2501, 2390, 2457, 2412, 2621, 2249, 2457, 2489, 2305, 2358},
	{2698, 2558, 2489, 2523, 2523, 2446, 2501, 2600, 2412, 2698, 2577, 2533, 2446, 2621, 2523, 2589, 2467, 2358, 2427, 2390, 2589, 2226, 2435, 2467, 2292, 2347},
	{2566, 2412, 2324, 2358, 2339, 2249, 2313, 2382, 2204, 2467, 2347, 2281, 2193, 2369, 2249, 2305, 2172, 2050, 2116, 2073, 2292, 1941, 2151, 2207, 2050, 2119},
	{2547, 2401, 2313, 2358, 2336, 2236, 2305, 2371, 2193, 2457, 2347, 2292, 2185, 2369, 2241, 2303, 2174, 2050, 2116, 2076, 2292, 1930, 2149, 2204, 2050, 2095},
	{2664, 2533, 2457, 2489, 2470, 2390, 2446, 2533, 2369, 2621, 2533, 2489, 2401, 2577, 2470, 2544, 2435, 2313, 2390, 2347, 2566, 2193, 2401, 2446, 2281, 2324},
	{2698, 2577, 2501, 2533, 2501, 2427, 2467, 2566, 2380, 2643, 2566, 2523, 2424, 2610, 2501, 2577, 2446, 2313, 2390, 2347, 2577, 2196, 2424, 2457, 2295, 2369},
	{11949, 15592, 19505, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000}, 
};
#else 
static short LowerLimit[CFG_F54_TXCOUNT][CFG_F54_RXCOUNT] ={	
	{1713, 1674, 1698, 1674, 1665, 1694, 1678, 1687, 1716, 1669, 1673, 1708, 1672, 1695, 1733, 1671, 1701, 1720, 1705, 1712, 1750, 1710, 1692, 1738, 1708, 1690, 1738, 1715, 1706, 1812},
	{1638, 1596, 1624, 1601, 1593, 1624, 1610, 1617, 1647, 1601, 1605, 1656, 1611, 1627, 1667, 1604, 1630, 1650, 1634, 1640, 1671, 1639, 1618, 1669, 1631, 1612, 1658, 1636, 1627, 1710},
	{1639, 1594, 1625, 1602, 1594, 1622, 1613, 1619, 1652, 1602, 1607, 1642, 1605, 1629, 1669, 1607, 1630, 1651, 1635, 1640, 1670, 1636, 1615, 1664, 1626, 1606, 1652, 1631, 1620, 1703},
	{1641, 1594, 1625, 1601, 1595, 1623, 1616, 1621, 1654, 1607, 1609, 1651, 1609, 1631, 1674, 1609, 1628, 1654, 1632, 1635, 1670, 1633, 1609, 1660, 1625, 1600, 1653, 1629, 1614, 1702},
	{1657, 1600, 1640, 1614, 1602, 1634, 1630, 1628, 1660, 1620, 1617, 1657, 1620, 1638, 1685, 1620, 1630, 1656, 1638, 1636, 1672, 1639, 1610, 1659, 1628, 1600, 1652, 1632, 1614, 1691},
	{1664, 1605, 1642, 1619, 1607, 1645, 1636, 1633, 1669, 1625, 1622, 1662, 1623, 1639, 1684, 1619, 1627, 1652, 1635, 1632, 1668, 1635, 1606, 1654, 1622, 1595, 1647, 1629, 1609, 1680},
	{1675, 1614, 1652, 1628, 1616, 1655, 1644, 1640, 1676, 1629, 1623, 1663, 1625, 1640, 1686, 1620, 1628, 1653, 1636, 1632, 1668, 1636, 1606, 1654, 1623, 1595, 1647, 1631, 1608, 1679},
	{1682, 1619, 1658, 1633, 1618, 1655, 1643, 1638, 1674, 1628, 1622, 1661, 1624, 1639, 1685, 1619, 1626, 1650, 1633, 1629, 1664, 1633, 1603, 1654, 1619, 1591, 1649, 1630, 1605, 1685},
	{1670, 1622, 1630, 1621, 1619, 1649, 1636, 1640, 1676, 1621, 1623, 1663, 1618, 1641, 1681, 1613, 1628, 1652, 1628, 1631, 1668, 1628, 1605, 1652, 1617, 1595, 1645, 1619, 1614, 1684},
	{1731, 1641, 1613, 1659, 1628, 1620, 1662, 1639, 1641, 1669, 1652, 1634, 1692, 1638, 1633, 1680, 1615, 1617, 1666, 1628, 1653, 1653, 1659, 1607, 1640, 1627, 1613, 1649, 1598, 1617},
	{1701, 1641, 1606, 1676, 1637, 1617, 1665, 1643, 1638, 1669, 1654, 1630, 1687, 1641, 1629, 1678, 1618, 1615, 1668, 1633, 1650, 1652, 1663, 1605, 1638, 1632, 1612, 1654, 1612, 1612},
	{1726, 1648, 1603, 1664, 1635, 1618, 1661, 1643, 1638, 1668, 1655, 1631, 1689, 1642, 1630, 1679, 1619, 1616, 1669, 1634, 1651, 1653, 1663, 1606, 1640, 1632, 1613, 1655, 1613, 1611},
	{1717, 1639, 1595, 1655, 1626, 1611, 1656, 1639, 1634, 1666, 1653, 1629, 1687, 1641, 1628, 1677, 1617, 1614, 1667, 1632, 1650, 1652, 1660, 1605, 1639, 1630, 1611, 1653, 1613, 1611},
	{1711, 1634, 1591, 1650, 1620, 1606, 1650, 1633, 1630, 1661, 1648, 1627, 1685, 1638, 1628, 1677, 1617, 1614, 1668, 1633, 1650, 1650, 1663, 1606, 1637, 1632, 1613, 1655, 1615, 1620},
	{1705, 1627, 1582, 1639, 1610, 1598, 1642, 1622, 1622, 1652, 1635, 1619, 1677, 1625, 1621, 1670, 1611, 1613, 1664, 1627, 1650, 1652, 1658, 1607, 1638, 1628, 1615, 1653, 1611, 1635},
	{1696, 1608, 1579, 1624, 1602, 1594, 1632, 1613, 1618, 1643, 1629, 1615, 1669, 1620, 1619, 1665, 1610, 1615, 1661, 1629, 1656, 1654, 1661, 1614, 1647, 1633, 1623, 1661, 1620, 1653},
	{1692, 1607, 1579, 1623, 1600, 1591, 1628, 1611, 1616, 1639, 1627, 1614, 1665, 1617, 1617, 1663, 1609, 1614, 1662, 1637, 1657, 1672, 1667, 1620, 1651, 1641, 1633, 1672, 1629, 1666},
};

static short UpperLimit[CFG_F54_TXCOUNT][CFG_F54_RXCOUNT] ={	
	{3181, 3109, 3153, 3109, 3093, 3145, 3116, 3133, 3187, 3100, 3107, 3172, 3105, 3148, 3219, 3104, 3158, 3194, 3167, 3180, 3250, 3176, 3143, 3227, 3172, 3139, 3228, 3185, 3169, 3365},
	{3043, 2964, 3016, 2974, 2959, 3016, 2989, 3004, 3058, 2974, 2981, 3075, 2991, 3022, 3096, 2979, 3027, 3064, 3035, 3045, 3103, 3044, 3006, 3099, 3030, 2994, 3080, 3038, 3022, 3175},
	{3043, 2961, 3017, 2975, 2961, 3012, 2995, 3007, 3069, 2975, 2984, 3050, 2981, 3025, 3099, 2984, 3028, 3066, 3036, 3045, 3102, 3039, 3000, 3090, 3020, 2983, 3069, 3030, 3009, 3164},
	{3048, 2960, 3019, 2974, 2962, 3015, 3002, 3010, 3072, 2984, 2988, 3066, 2987, 3029, 3108, 2988, 3024, 3072, 3030, 3036, 3101, 3032, 2989, 3083, 3017, 2971, 3070, 3026, 2998, 3160},
	{3077, 2972, 3047, 2998, 2975, 3035, 3027, 3023, 3084, 3009, 3004, 3078, 3009, 3042, 3129, 3008, 3028, 3075, 3042, 3038, 3105, 3043, 2991, 3081, 3023, 2971, 3067, 3031, 2998, 3140},
	{3090, 2980, 3050, 3007, 2985, 3056, 3038, 3032, 3100, 3018, 3012, 3086, 3013, 3044, 3128, 3007, 3021, 3068, 3036, 3031, 3097, 3036, 2982, 3071, 3013, 2962, 3059, 3025, 2988, 3119},
	{3110, 2997, 3067, 3023, 3001, 3073, 3054, 3046, 3113, 3025, 3015, 3088, 3018, 3047, 3131, 3009, 3024, 3070, 3037, 3031, 3098, 3038, 2983, 3072, 3014, 2962, 3059, 3028, 2987, 3118},
	{3123, 3007, 3079, 3033, 3005, 3073, 3052, 3043, 3110, 3024, 3012, 3086, 3016, 3044, 3129, 3007, 3020, 3064, 3033, 3025, 3091, 3032, 2976, 3071, 3007, 2955, 3062, 3027, 2980, 3129},
	{3102, 3012, 3028, 3010, 3007, 3063, 3038, 3046, 3112, 3010, 3014, 3089, 3005, 3047, 3121, 2995, 3024, 3067, 3024, 3029, 3098, 3023, 2981, 3069, 3003, 2962, 3055, 3008, 2998, 3128},
	{3215, 3048, 2995, 3081, 3023, 3009, 3086, 3044, 3048, 3099, 3068, 3034, 3141, 3043, 3032, 3119, 2999, 3004, 3094, 3023, 3070, 3069, 3081, 2984, 3046, 3021, 2996, 3063, 2967, 3003},
	{3160, 3047, 2982, 3112, 3040, 3003, 3092, 3051, 3042, 3099, 3073, 3028, 3134, 3048, 3026, 3117, 3005, 3000, 3098, 3033, 3065, 3068, 3089, 2981, 3042, 3030, 2994, 3071, 2993, 2993},
	{3205, 3060, 2977, 3089, 3036, 3004, 3084, 3052, 3042, 3098, 3074, 3029, 3137, 3049, 3027, 3118, 3007, 3001, 3099, 3035, 3067, 3070, 3089, 2983, 3045, 3030, 2995, 3073, 2995, 2991},
	{3189, 3043, 2962, 3073, 3020, 2993, 3075, 3043, 3035, 3095, 3070, 3026, 3134, 3047, 3023, 3115, 3003, 2998, 3096, 3031, 3064, 3067, 3084, 2981, 3043, 3027, 2992, 3071, 2995, 2992},
	{3178, 3034, 2954, 3064, 3009, 2983, 3064, 3033, 3027, 3086, 3061, 3021, 3129, 3041, 3023, 3115, 3002, 2998, 3097, 3032, 3065, 3065, 3089, 2983, 3039, 3031, 2996, 3074, 2999, 3009},
	{3167, 3021, 2938, 3044, 2990, 2968, 3050, 3013, 3012, 3068, 3037, 3007, 3114, 3017, 3010, 3102, 2991, 2995, 3090, 3022, 3065, 3068, 3078, 2985, 3042, 3023, 3000, 3069, 2992, 3037},
	{3149, 2986, 2933, 3016, 2975, 2959, 3030, 2996, 3005, 3051, 3026, 3000, 3099, 3008, 3007, 3092, 2991, 2999, 3084, 3026, 3075, 3072, 3085, 2997, 3059, 3034, 3015, 3084, 3009, 3069},
	{3143, 2985, 2932, 3014, 2971, 2955, 3024, 2992, 3001, 3045, 3021, 2997, 3093, 3004, 3004, 3088, 2988, 2998, 3087, 3040, 3078, 3105, 3096, 3008, 3066, 3047, 3032, 3106, 3025, 3094},
};

#endif
//Chenyee <YC_BSP_CTP> <hezheng> <20171024> modify for ID241394 tp full screen test end

/*
extern int synaptics_rmi4_reg_read(struct synaptics_rmi4_data *rmi4_data,
		unsigned short addr, unsigned char *data,
		unsigned short length);
extern int synaptics_rmi4_reg_write(struct synaptics_rmi4_data *rmi4_data,
		unsigned short addr, unsigned char *data,
		unsigned short length);
*/

static struct rim4_f54_data *f54_data = NULL;
static short ImageArray[CFG_F54_TXCOUNT][CFG_F54_RXCOUNT];
static short HighResistance[HIGH_RESISTANCE_DATA_SIZE/2];
static unsigned g_display_ratio = 0;

static void free_control_mem(void)
{
	struct f54_control control = f54_data->control;

	kfree(control.reg_0);
	kfree(control.reg_1);
	kfree(control.reg_2);
	kfree(control.reg_3);
	kfree(control.reg_4__6);
	kfree(control.reg_7);
	kfree(control.reg_8__9);
	kfree(control.reg_10);
	kfree(control.reg_11);
	kfree(control.reg_12__13);
	kfree(control.reg_14);
	kfree(control.reg_15);
	kfree(control.reg_16);
	kfree(control.reg_17);
	kfree(control.reg_18);
	kfree(control.reg_19);
	kfree(control.reg_20);
	kfree(control.reg_21);
	kfree(control.reg_22__26);
	kfree(control.reg_27);
	kfree(control.reg_28);
	kfree(control.reg_29);
	kfree(control.reg_30);
	kfree(control.reg_31);
	kfree(control.reg_32__35);
	kfree(control.reg_36);
	kfree(control.reg_37);
	kfree(control.reg_38);
	kfree(control.reg_39);
	kfree(control.reg_40);
	kfree(control.reg_41);
	kfree(control.reg_57);

	return;
}

static int synaptics_rmi4_f54_set_ctrl(void)
{
	unsigned char length;
	unsigned char reg_num = 0;
	unsigned char num_of_sensing_freqs;
	unsigned short reg_addr = f54_data->control_base_addr;
	struct f54_control *control = &f54_data->control;
	//struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;

	num_of_sensing_freqs = f54_data->query.number_of_sensing_frequencies;

	/* control 0 */
	//attrs_ctrl_regs_exist[reg_num] = true;
	control->reg_0 = kzalloc(sizeof(*(control->reg_0)),
			GFP_KERNEL);
	if (!control->reg_0)
		goto exit_no_mem;
	control->reg_0->address = reg_addr;
	reg_addr += sizeof(control->reg_0->data);
	reg_num++;

	/* control 1 */
	if ((f54_data->query.touch_controller_family == 0) ||
			(f54_data->query.touch_controller_family == 1)) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_1 = kzalloc(sizeof(*(control->reg_1)),
				GFP_KERNEL);
		if (!control->reg_1)
			goto exit_no_mem;
		control->reg_1->address = reg_addr;
		reg_addr += sizeof(control->reg_1->data);
	}
	reg_num++;

	/* control 2 */
	//attrs_ctrl_regs_exist[reg_num] = true;
	control->reg_2 = kzalloc(sizeof(*(control->reg_2)),
			GFP_KERNEL);
	if (!control->reg_2)
		goto exit_no_mem;
	control->reg_2->address = reg_addr;
	reg_addr += sizeof(control->reg_2->data);
	reg_num++;

	/* control 3 */
	if (f54_data->query.has_pixel_touch_threshold_adjustment == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_3 = kzalloc(sizeof(*(control->reg_3)),
				GFP_KERNEL);
		if (!control->reg_3)
			goto exit_no_mem;
		control->reg_3->address = reg_addr;
		reg_addr += sizeof(control->reg_3->data);
	}
	reg_num++;

	/* controls 4 5 6 */
	if ((f54_data->query.touch_controller_family == 0) ||
			(f54_data->query.touch_controller_family == 1)) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_4__6 = kzalloc(sizeof(*(control->reg_4__6)),
				GFP_KERNEL);
		if (!control->reg_4__6)
			goto exit_no_mem;
		control->reg_4__6->address = reg_addr;
		reg_addr += sizeof(control->reg_4__6->data);
	}
	reg_num++;

	/* control 7 */
	if (f54_data->query.touch_controller_family == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_7 = kzalloc(sizeof(*(control->reg_7)),
				GFP_KERNEL);
		if (!control->reg_7)
			goto exit_no_mem;
		control->reg_7->address = reg_addr;
		reg_addr += sizeof(control->reg_7->data);
	}
	reg_num++;

	/* controls 8 9 */
	if ((f54_data->query.touch_controller_family == 0) ||
			(f54_data->query.touch_controller_family == 1)) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_8__9 = kzalloc(sizeof(*(control->reg_8__9)),
				GFP_KERNEL);
		if (!control->reg_8__9)
			goto exit_no_mem;
		control->reg_8__9->address = reg_addr;
		reg_addr += sizeof(control->reg_8__9->data);
	}
	reg_num++;

	/* control 10 */
	if (f54_data->query.has_interference_metric == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_10 = kzalloc(sizeof(*(control->reg_10)),
				GFP_KERNEL);
		if (!control->reg_10)
			goto exit_no_mem;
		control->reg_10->address = reg_addr;
		reg_addr += sizeof(control->reg_10->data);
	}
	reg_num++;

	/* control 11 */
	if (f54_data->query.has_ctrl11 == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_11 = kzalloc(sizeof(*(control->reg_11)),
				GFP_KERNEL);
		if (!control->reg_11)
			goto exit_no_mem;
		control->reg_11->address = reg_addr;
		reg_addr += sizeof(control->reg_11->data);
	}
	reg_num++;

	/* controls 12 13 */
	if (f54_data->query.has_relaxation_control == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_12__13 = kzalloc(sizeof(*(control->reg_12__13)),
				GFP_KERNEL);
		if (!control->reg_12__13)
			goto exit_no_mem;
		control->reg_12__13->address = reg_addr;
		reg_addr += sizeof(control->reg_12__13->data);
	}
	reg_num++;

	/* controls 14 15 16 */
	if (f54_data->query.has_sensor_assignment == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;

		control->reg_14 = kzalloc(sizeof(*(control->reg_14)),
				GFP_KERNEL);
		if (!control->reg_14)
			goto exit_no_mem;
		control->reg_14->address = reg_addr;
		reg_addr += sizeof(control->reg_14->data);

		control->reg_15 = kzalloc(sizeof(*(control->reg_15)),
				GFP_KERNEL);
		if (!control->reg_15)
			goto exit_no_mem;
		control->reg_15->length = f54_data->query.num_of_rx_electrodes;
		control->reg_15->data = kzalloc(control->reg_15->length *
				sizeof(*(control->reg_15->data)), GFP_KERNEL);
		if (!control->reg_15->data)
			goto exit_no_mem;
		control->reg_15->address = reg_addr;
		reg_addr += control->reg_15->length;

		control->reg_16 = kzalloc(sizeof(*(control->reg_16)),
				GFP_KERNEL);
		if (!control->reg_16)
			goto exit_no_mem;
		control->reg_16->length = f54_data->query.num_of_tx_electrodes;
		control->reg_16->data = kzalloc(control->reg_16->length *
				sizeof(*(control->reg_16->data)), GFP_KERNEL);
		if (!control->reg_16->data)
			goto exit_no_mem;
		control->reg_16->address = reg_addr;
		reg_addr += control->reg_16->length;
	}
	reg_num++;

	/* controls 17 18 19 */
	if (f54_data->query.has_sense_frequency_control == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;

		length = num_of_sensing_freqs;

		control->reg_17 = kzalloc(sizeof(*(control->reg_17)),
				GFP_KERNEL);
		if (!control->reg_17)
			goto exit_no_mem;
		control->reg_17->length = length;
		control->reg_17->data = kzalloc(length *
				sizeof(*(control->reg_17->data)), GFP_KERNEL);
		if (!control->reg_17->data)
			goto exit_no_mem;
		control->reg_17->address = reg_addr;
		reg_addr += length;

		control->reg_18 = kzalloc(sizeof(*(control->reg_18)),
				GFP_KERNEL);
		if (!control->reg_18)
			goto exit_no_mem;
		control->reg_18->length = length;
		control->reg_18->data = kzalloc(length *
				sizeof(*(control->reg_18->data)), GFP_KERNEL);
		if (!control->reg_18->data)
			goto exit_no_mem;
		control->reg_18->address = reg_addr;
		reg_addr += length;

		control->reg_19 = kzalloc(sizeof(*(control->reg_19)),
				GFP_KERNEL);
		if (!control->reg_19)
			goto exit_no_mem;
		control->reg_19->length = length;
		control->reg_19->data = kzalloc(length *
				sizeof(*(control->reg_19->data)), GFP_KERNEL);
		if (!control->reg_19->data)
			goto exit_no_mem;
		control->reg_19->address = reg_addr;
		reg_addr += length;
	}
	reg_num++;

	/* control 20 */
	//attrs_ctrl_regs_exist[reg_num] = true;
	control->reg_20 = kzalloc(sizeof(*(control->reg_20)),
			GFP_KERNEL);
	if (!control->reg_20)
		goto exit_no_mem;
	control->reg_20->address = reg_addr;
	reg_addr += sizeof(control->reg_20->data);
	reg_num++;

	/* control 21 */
	if (f54_data->query.has_sense_frequency_control == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_21 = kzalloc(sizeof(*(control->reg_21)),
				GFP_KERNEL);
		if (!control->reg_21)
			goto exit_no_mem;
		control->reg_21->address = reg_addr;
		reg_addr += sizeof(control->reg_21->data);
	}
	reg_num++;

	/* controls 22 23 24 25 26 */
	if (f54_data->query.has_firmware_noise_mitigation == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_22__26 = kzalloc(sizeof(*(control->reg_22__26)),
				GFP_KERNEL);
		if (!control->reg_22__26)
			goto exit_no_mem;
		control->reg_22__26->address = reg_addr;
		reg_addr += sizeof(control->reg_22__26->data);
	}
	reg_num++;

	/* control 27 */
	if (f54_data->query.has_iir_filter == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_27 = kzalloc(sizeof(*(control->reg_27)),
				GFP_KERNEL);
		if (!control->reg_27)
			goto exit_no_mem;
		control->reg_27->address = reg_addr;
		reg_addr += sizeof(control->reg_27->data);
	}
	reg_num++;

	/* control 28 */
	if (f54_data->query.has_firmware_noise_mitigation == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_28 = kzalloc(sizeof(*(control->reg_28)),
				GFP_KERNEL);
		if (!control->reg_28)
			goto exit_no_mem;
		control->reg_28->address = reg_addr;
		reg_addr += sizeof(control->reg_28->data);
	}
	reg_num++;

	/* control 29 */
	if (f54_data->query.has_cmn_removal == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_29 = kzalloc(sizeof(*(control->reg_29)),
				GFP_KERNEL);
		if (!control->reg_29)
			goto exit_no_mem;
		control->reg_29->address = reg_addr;
		reg_addr += sizeof(control->reg_29->data);
	}
	reg_num++;

	/* control 30 */
	if (f54_data->query.has_cmn_maximum == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_30 = kzalloc(sizeof(*(control->reg_30)),
				GFP_KERNEL);
		if (!control->reg_30)
			goto exit_no_mem;
		control->reg_30->address = reg_addr;
		reg_addr += sizeof(control->reg_30->data);
	}
	reg_num++;

	/* control 31 */
	if (f54_data->query.has_touch_hysteresis == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_31 = kzalloc(sizeof(*(control->reg_31)),
				GFP_KERNEL);
		if (!control->reg_31)
			goto exit_no_mem;
		control->reg_31->address = reg_addr;
		reg_addr += sizeof(control->reg_31->data);
	}
	reg_num++;

	/* controls 32 33 34 35 */
	if (f54_data->query.has_edge_compensation == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_32__35 = kzalloc(sizeof(*(control->reg_32__35)),
				GFP_KERNEL);
		if (!control->reg_32__35)
			goto exit_no_mem;
		control->reg_32__35->address = reg_addr;
		reg_addr += sizeof(control->reg_32__35->data);
	}
	reg_num++;

	/* control 36 */
	if ((f54_data->query.curve_compensation_mode == 1) ||
			(f54_data->query.curve_compensation_mode == 2)) {
		//attrs_ctrl_regs_exist[reg_num] = true;

		if (f54_data->query.curve_compensation_mode == 1) {
			length = max(f54_data->query.num_of_rx_electrodes,
					f54_data->query.num_of_tx_electrodes);
		} else if (f54_data->query.curve_compensation_mode == 2) {
			length = f54_data->query.num_of_rx_electrodes;
		}

		control->reg_36 = kzalloc(sizeof(*(control->reg_36)),
				GFP_KERNEL);
		if (!control->reg_36)
			goto exit_no_mem;
		control->reg_36->length = length;
		control->reg_36->data = kzalloc(length *
				sizeof(*(control->reg_36->data)), GFP_KERNEL);
		if (!control->reg_36->data)
			goto exit_no_mem;
		control->reg_36->address = reg_addr;
		reg_addr += length;
	}
	reg_num++;

	/* control 37 */
	if (f54_data->query.curve_compensation_mode == 2) {
		//attrs_ctrl_regs_exist[reg_num] = true;

		control->reg_37 = kzalloc(sizeof(*(control->reg_37)),
				GFP_KERNEL);
		if (!control->reg_37)
			goto exit_no_mem;
		control->reg_37->length = f54_data->query.num_of_tx_electrodes;
		control->reg_37->data = kzalloc(control->reg_37->length *
				sizeof(*(control->reg_37->data)), GFP_KERNEL);
		if (!control->reg_37->data)
			goto exit_no_mem;

		control->reg_37->address = reg_addr;
		reg_addr += control->reg_37->length;
	}
	reg_num++;

	/* controls 38 39 40 */
	if (f54_data->query.has_per_frequency_noise_control == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;

		control->reg_38 = kzalloc(sizeof(*(control->reg_38)),
				GFP_KERNEL);
		if (!control->reg_38)
			goto exit_no_mem;
		control->reg_38->length = num_of_sensing_freqs;
		control->reg_38->data = kzalloc(control->reg_38->length *
				sizeof(*(control->reg_38->data)), GFP_KERNEL);
		if (!control->reg_38->data)
			goto exit_no_mem;
		control->reg_38->address = reg_addr;
		reg_addr += control->reg_38->length;

		control->reg_39 = kzalloc(sizeof(*(control->reg_39)),
				GFP_KERNEL);
		if (!control->reg_39)
			goto exit_no_mem;
		control->reg_39->length = num_of_sensing_freqs;
		control->reg_39->data = kzalloc(control->reg_39->length *
				sizeof(*(control->reg_39->data)), GFP_KERNEL);
		if (!control->reg_39->data)
			goto exit_no_mem;
		control->reg_39->address = reg_addr;
		reg_addr += control->reg_39->length;

		control->reg_40 = kzalloc(sizeof(*(control->reg_40)),
				GFP_KERNEL);
		if (!control->reg_40)
			goto exit_no_mem;
		control->reg_40->length = num_of_sensing_freqs;
		control->reg_40->data = kzalloc(control->reg_40->length *
				sizeof(*(control->reg_40->data)), GFP_KERNEL);
		if (!control->reg_40->data)
			goto exit_no_mem;
		control->reg_40->address = reg_addr;
		reg_addr += control->reg_40->length;
	}
	reg_num++;

	/* control 41 */
	if (f54_data->query.has_signal_clarity == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_41 = kzalloc(sizeof(*(control->reg_41)),
				GFP_KERNEL);
		if (!control->reg_41)
			goto exit_no_mem;
		control->reg_41->address = reg_addr;
		reg_addr += sizeof(control->reg_41->data);
	}
	reg_num++;

	/* control 42 */
	if (f54_data->query.has_variance_metric == 1)
		reg_addr += CONTROL_42_SIZE;

	/* controls 43 44 45 46 47 48 49 50 51 52 53 54 */
	if (f54_data->query.has_multi_metric_state_machine == 1)
		reg_addr += CONTROL_43_54_SIZE;

	/* controls 55 56 */
	if (f54_data->query.has_0d_relaxation_control == 1)
		reg_addr += CONTROL_55_56_SIZE;

	/* control 57 */
	if (f54_data->query.has_0d_acquisition_control == 1) {
		//attrs_ctrl_regs_exist[reg_num] = true;
		control->reg_57 = kzalloc(sizeof(*(control->reg_57)),
				GFP_KERNEL);
		if (!control->reg_57)
			goto exit_no_mem;
		control->reg_57->address = reg_addr;
		reg_addr += sizeof(control->reg_57->data);
	}
	reg_num++;

	/* control 58 */
	if (f54_data->query.has_0d_acquisition_control == 1)
		reg_addr += CONTROL_58_SIZE;

	/* control 59 */
	if (f54_data->query.has_h_blank == 1)
		reg_addr += CONTROL_59_SIZE;

	/* controls 60 61 62 */
	if ((f54_data->query.has_h_blank == 1) ||
			(f54_data->query.has_v_blank == 1) ||
			(f54_data->query.has_long_h_blank == 1))
		reg_addr += CONTROL_60_62_SIZE;

	/* control 63 */
	if ((f54_data->query.has_h_blank == 1) ||
			(f54_data->query.has_v_blank == 1) ||
			(f54_data->query.has_long_h_blank == 1) ||
			(f54_data->query.has_slew_metric == 1) ||
			(f54_data->query.has_slew_option == 1) ||
			(f54_data->query.has_noise_mitigation2 == 1))
		reg_addr += CONTROL_63_SIZE;

	/* controls 64 65 66 67 */
	if (f54_data->query.has_h_blank == 1)
		reg_addr += CONTROL_64_67_SIZE * 7;
	else if ((f54_data->query.has_v_blank == 1) ||
			(f54_data->query.has_long_h_blank == 1))
		reg_addr += CONTROL_64_67_SIZE;

	/* controls 68 69 70 71 72 73 */
	if ((f54_data->query.has_h_blank == 1) ||
			(f54_data->query.has_v_blank == 1) ||
			(f54_data->query.has_long_h_blank == 1))
		reg_addr += CONTROL_68_73_SIZE;

	/* control 74 */
	if (f54_data->query.has_slew_metric == 1)
		reg_addr += CONTROL_74_SIZE;

	/* control 75 */
	if (f54_data->query.has_enhanced_stretch == 1)
		reg_addr += num_of_sensing_freqs;

	/* control 76 */
	if (f54_data->query.has_startup_fast_relaxation == 1)
		reg_addr += CONTROL_76_SIZE;

	/* controls 77 78 */
	if (f54_data->query.has_esd_control == 1)
		reg_addr += CONTROL_77_78_SIZE;

	/* controls 79 80 81 82 83 */
	if (f54_data->query.has_noise_mitigation2 == 1)
		reg_addr += CONTROL_79_83_SIZE;

	/* controls 84 85 */
	if (f54_data->query.has_energy_ratio_relaxation == 1)
		reg_addr += CONTROL_84_85_SIZE;

	/* control 86 */
	if ((f54_data->query.has_query13 == 1) && (f54_data->query.has_ctrl86 == 1))
		reg_addr += CONTROL_86_SIZE;

	/* control 87 */
	if ((f54_data->query.has_query13 == 1) && (f54_data->query.has_ctrl87 == 1))
		reg_addr += CONTROL_87_SIZE;

	/* control 88 */
	if (f54_data->query.has_ctrl88 == 1) {
		control->reg_88 = kzalloc(sizeof(*(control->reg_88)),
				GFP_KERNEL);
		if (!control->reg_88)
			goto exit_no_mem;
		control->reg_88->address = reg_addr;
		reg_addr += sizeof(control->reg_88->data);
	}

	return 0;

exit_no_mem:
	printk("%s: Failed to alloc mem for control registers\n",__func__);
	return -ENOMEM;
}
/*
static int do_preparation(void)
{
	int retval;
	unsigned char value;
	unsigned char command;
	unsigned char timeout_count;
	struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;

	mutex_lock(&f54_data->control_mutex);
	
	value = 0;
	synaptics_rmi4_reg_write(rmi4_data, (rmi4_data->f01_ctrl_base_addr + 1), &value , sizeof(value));

	if (f54_data->query.touch_controller_family == 1) {
		
		value = 0;
		retval = synaptics_rmi4_reg_write(rmi4_data,
				f54_data->control.reg_7->address,
				&value,
				sizeof(f54_data->control.reg_7->data));
		if (retval < 0) {
			printk("%s: Failed to disable CBC\n",__func__);
			mutex_unlock(&f54_data->control_mutex);
			return retval;
		}
		
	} else if (f54_data->query.has_ctrl88 == 1) {

		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->control.reg_88->address,
				f54_data->control.reg_88->data,
				sizeof(f54_data->control.reg_88->data));
		if (retval < 0) {
			printk("%s: Failed to disable CBC (read ctrl88)\n",__func__);
			mutex_unlock(&f54_data->control_mutex);
			return retval;
		}
		
		f54_data->control.reg_88->cbc_polarity = 0;
		f54_data->control.reg_88->cbc_tx_carrier_selection = 0;
		retval = synaptics_rmi4_reg_write(rmi4_data,
				f54_data->control.reg_88->address,
				f54_data->control.reg_88->data,
				sizeof(f54_data->control.reg_88->data));
		if (retval < 0) {
			printk("%s: Failed to disable CBC (write ctrl88)\n",__func__);
			mutex_unlock(&f54_data->control_mutex);
			return retval;
		}

	}

	if (f54_data->query.has_0d_acquisition_control) {

		value = 0;
		retval = synaptics_rmi4_reg_write(rmi4_data,
				f54_data->control.reg_57->address,
				&value,
				sizeof(f54_data->control.reg_57->data));
		if (retval < 0) {
			printk("%s: Failed to disable 0D CBC\n",__func__);
			mutex_unlock(&f54_data->control_mutex);
			return retval;
		}

	}

	if (f54_data->query.has_signal_clarity) {

		value = 1;
		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->control.reg_41->address,
				&value,
				sizeof(f54_data->control.reg_41->data));
		if (retval < 0) {
			value = 1;
		}else{
			printk("%s read signal clarity success value = %d\n",__func__, value);
			value = value | 0x01;
		}
		
		retval = synaptics_rmi4_reg_write(rmi4_data,
				f54_data->control.reg_41->address,
				&value,
				sizeof(f54_data->control.reg_41->data));
		if (retval < 0) {
			printk("%s: Failed to disable signal clarity\n",__func__);
			mutex_unlock(&f54_data->control_mutex);
			return retval;
		}

		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->control.reg_41->address,
				&value,
				sizeof(f54_data->control.reg_41->data));
		
		//printk("%s read signal clarity  value = %d retval = %d\n",__func__, value, retval);
	}

	mutex_unlock(&f54_data->control_mutex);

	command = (unsigned char)COMMAND_FORCE_UPDATE;
	
	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->command_base_addr,
			&command,
			sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write force update command\n",__func__);
		return retval;
	}

	timeout_count = 0;
	do {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->command_base_addr,
				&value,
				sizeof(value));
		if (retval < 0) {
			printk("%s: Failed to read command register\n",__func__);
			return retval;
		}

		if (value == 0x00)
			break;

		msleep(100);
		timeout_count++;
	} while (timeout_count < FORCE_TIMEOUT_100MS);

	if (timeout_count == FORCE_TIMEOUT_100MS) {
		printk("%s: Timed out waiting for force update\n",__func__);
		return -ETIMEDOUT;
	}

	command = (unsigned char)COMMAND_FORCE_CAL;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->command_base_addr,
			&command,
			sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write force cal command\n",__func__);
		return retval;
	}

	timeout_count = 0;
	do {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->command_base_addr,
				&value,
				sizeof(value));
		if (retval < 0) {
			printk("%s: Failed to read command register\n",__func__);
			return retval;
		}

		if (value == 0x00)
			break;

		msleep(100);
		timeout_count++;
	} while (timeout_count < FORCE_TIMEOUT_100MS);

	if (timeout_count == FORCE_TIMEOUT_100MS) {
		printk("%s: Timed out waiting for force cal\n",__func__);
		return -ETIMEDOUT;
	}

	return 0;
}
*/

static int synaptics_rmi4_f54_sw_reset(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	unsigned char command = 0x01;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f01_cmd_base_addr,
			&command,
			sizeof(command));
	if (retval < 0) {
		dev_err(&rmi4_data->i2c_client->dev,
				"%s: Failed to issue reset command, error = %d\n",
				__func__, retval);
		mutex_unlock(&(rmi4_data->rmi4_reset_mutex));
		return retval;
	}

	//msleep(90);
	msleep(200);

#if 0
	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f01_cmd_base_addr,
			&command,
			sizeof(command));
	if (retval < 0)
		return retval;

	msleep(rmi4_data->hw_if->board_data->reset_delay_ms);

	if (rmi4_data->hw_if->ui_hw_init) {
		retval = rmi4_data->hw_if->ui_hw_init(rmi4_data);
		if (retval < 0)
			return retval;
	}
#endif
	return 0;
}


static void set_report_size(void)
{
	//int retval;
	unsigned char rx = f54_data->rx_assigned;
	unsigned char tx = f54_data->tx_assigned;
	//struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;

	switch (f54_data->report_type) {
	case F54_8BIT_IMAGE:
		f54_data->report_size = rx * tx;
		break;
	case F54_16BIT_IMAGE:
	case F54_RAW_16BIT_IMAGE:
	case F54_TRUE_BASELINE:
	case F54_FULL_RAW_CAP:
	case F54_FULL_RAW_CAP_RX_COUPLING_COMP:
	case F54_FULL_RAW_TD4300:
	case F54_SENSOR_SPEED:
		f54_data->report_size = 2 * rx * tx;
		break;
	case F54_HIGH_RESISTANCE:
		f54_data->report_size = HIGH_RESISTANCE_DATA_SIZE;
		break;
	case F54_TX_TO_TX_SHORT:
	case F54_TX_OPEN:
	case F54_TX_TO_GROUND:
		f54_data->report_size = (tx + 7) / 8;
		break;
	case F54_RX_TO_RX1:
	case F54_RX_OPENS1:
		if (rx < tx)
			f54_data->report_size = 2 * rx * rx;
		else
			f54_data->report_size = 2 * rx * tx;
		break;
	case F54_FULL_RAW_CAP_MIN_MAX:
		f54_data->report_size = FULL_RAW_CAP_MIN_MAX_DATA_SIZE;
		break;
	case F54_RX_TO_RX2:
	case F54_RX_OPENS2:
		if (rx <= tx)
			f54_data->report_size = 0;
		else
			f54_data->report_size = 2 * rx * (rx - tx);
		break;
	case F54_ADC_RANGE:
		//not support 
		f54_data->report_size = 2 * rx * tx;
		break;
	case F54_TREX_OPENS:
	case F54_TREX_TO_GND:
	case F54_TREX_SHORTS:
		f54_data->report_size = TREX_DATA_SIZE;
		break;
	default:
		f54_data->report_size = 0;
	}

	return;
}

static bool is_report_type_valid(enum f54_report_types report_type)
{
	switch (report_type) {
	case F54_8BIT_IMAGE:
	case F54_16BIT_IMAGE:
	case F54_RAW_16BIT_IMAGE:
	case F54_HIGH_RESISTANCE:
	case F54_TX_TO_TX_SHORT:
	case F54_RX_TO_RX1:
	case F54_TRUE_BASELINE:
	case F54_FULL_RAW_CAP_MIN_MAX:
	case F54_RX_OPENS1:
	case F54_TX_OPEN:
	case F54_TX_TO_GROUND:
	case F54_RX_TO_RX2:
	case F54_RX_OPENS2:
	case F54_FULL_RAW_CAP:
	case F54_FULL_RAW_CAP_RX_COUPLING_COMP:
	case F54_FULL_RAW_TD4300:
	case F54_SENSOR_SPEED:
	case F54_ADC_RANGE:
	case F54_TREX_OPENS:
	case F54_TREX_TO_GND:
	case F54_TREX_SHORTS:
		return true;
		break;
	default:
		f54_data->report_type = INVALID_REPORT_TYPE;
		f54_data->report_size = 0;
		return false;
	}
}

static int synaptics_f54_set_report_type(unsigned long val)
{
	enum f54_report_types type;
	type = (enum f54_report_types)val;
	if(f54_data == NULL)
		return -1;
	if(is_report_type_valid(type)) {
		f54_data->report_type = type;
	}else
		return -1;
	printk("%s,report_type = %d \n",__func__, f54_data->report_type);
	return 0;
}

//EXPORT_SYMBOL(synaptics_f54_set_report_type);

static int synaptics_f54_get_report_type(void)
{
	if(f54_data == NULL)
		return -1;
	return f54_data->report_type;
}
//EXPORT_SYMBOL(synaptics_f54_get_report_type);

#if 0//s3718
static int synaptics_rmi4_f54_check_full_raw_data()
{
	int i,j;
	int max=0;
	int min=65535;
	
	for (i = 0; i < ((f54_data->tx_assigned) - 3); i++) {   
	   for (j = 0; j < ((f54_data->rx_assigned) - 1); j++)
	   {
		    if(ImageArray[i][j] < RAW_DATA_MIN || ImageArray[i][j] > RAW_DATA_MAX) {
				printk("%s, CTP ITO maybe broken. AmageArray[%d][%d]=%d\n",__func__,i,j,ImageArray[i][j]);
				return 0;

			}
	   }
   	}

	for (i = ((f54_data->tx_assigned) - 3); i < f54_data->tx_assigned; i++) {
		if(ImageArray[i][32] < RAW_KEY_DATA_MIN || ImageArray[i][32] > RAW_KEY_DATA_MAX) {
			printk("%s, CTP key ITO maybe broken. AmageArray[%d][32]=%d\n",__func__,i,ImageArray[i][32]);
			return 0;			
		}
	}
	return 1;
}
#else //td4300
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> modify for get data start
static int synaptics_rmi4_f54_check_full_raw_data(void)
{
	int i,j;
	//int max=0;
	//int min=65535;	
	int iLen = 0;
	int rawdata_ret = 1;
	char rawdata_cp[8];

	if (NULL != synaptics_data_buf) 
		memset(synaptics_data_buf, 0, sizeof(128*16));	
	rawdata_tx = f54_data->tx_assigned-1;
	rawdata_rx = f54_data->rx_assigned;
	synaptics_data_len = 0;

	for (i = 0; i < ((f54_data->tx_assigned) -1); i++) {   
	   for (j = 0; j < (f54_data->rx_assigned); j++)
	   {
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test begin
			// Chenyee <YC_BSP_CTP> <hezheng> <20171024>  modify for ID241394 begin
		   if(ImageArray[i][j] < LowerLimit[i][j] || ImageArray[i][j] > UpperLimit[i][j]) {
		   
		   //if(ImageArray[i][j] < RAW_DATA_MIN || ImageArray[i][j] > RAW_DATA_MAX) {
		   // Chenyee <YC_BSP_CTP> <hezheng> <20171024>  modify for ID241394 end
				printk("%s, CTP VA ITO maybe broken. AmageArray[%d][%d]=%d\n",__func__,i,j,ImageArray[i][j]);
				rawdata_ret =  -EINVAL;
			}

			if (NULL != synaptics_data_buf){
				if(j == (f54_data->rx_assigned-1)) 
				{
					iLen= sprintf(rawdata_cp,"%d, \n", (signed short)ImageArray[i][j]);	
				} else {
					iLen= sprintf(rawdata_cp,"%d, ", (signed short)ImageArray[i][j]);
				}
				
				memcpy(synaptics_data_buf+synaptics_data_len, rawdata_cp, iLen);
				synaptics_data_len += iLen;
				}
	   }
   	}
#if 0
	if (i == ((f54_data->tx_assigned) -1) ){   
	   for (j = 0; j < 3; j++)
	   {
		    if(ImageArray[i][j] < LowerLimit[i][j] || ImageArray[i][j] > UpperLimit[i][j]) {
				printk("%s, CTP KEY ITO maybe broken. AmageArray[%d][%d]=%d\n",__func__,i,j,ImageArray[i][j]);
				return -EINVAL;
			}
	   }
   	}
#endif
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test end
	return rawdata_ret;
}
//Gionee <GN_BSP_CTP> <mogongfu> <20161210> modify for get data end
#endif
/*
static int synaptics_rmi4_f54_check_high_resistance_data(void)
{
	if(HighResistance[0] > F54_HR_RX_OFFSET)
		return 0;
	if(HighResistance[1] > F54_HR_TX_OFFSET)
		return 0;
	if(HighResistance[2] < F54_HR_MIN)
		return 0;
	return 1;
}
*/

void synaptics_rmi4_f54_get_TxRx_count(int *TxRx)
{
	TxRx[0]=CFG_F54_TXCOUNT;
	TxRx[1]=CFG_F54_RXCOUNT;
}
EXPORT_SYMBOL(synaptics_rmi4_f54_get_TxRx_count);

void synaptics_rmi4_f54_get_raw_data(short *rawdata)
{
	memcpy(rawdata, &ImageArray[0][0], sizeof(ImageArray));
	//memcpy(rawdata,f54_data->report_data,f54_data->report_size);
}
EXPORT_SYMBOL(synaptics_rmi4_f54_get_raw_data);

void synaptics_rmi4_f54_get_rawdata_range(int *range)
{
	range[0] = RAW_DATA_MAX;
	range[1] = RAW_DATA_MIN;
	range[2] = RAW_KEY_DATA_MAX;
	range[3] = RAW_KEY_DATA_MIN;
}
EXPORT_SYMBOL(synaptics_rmi4_f54_get_rawdata_range);
int synaptics_rmi4_f54_get_report_data(void)
{
	int retval;
	unsigned char report_index[2];
	unsigned short command;
	//unsigned short length;
	unsigned int patience = 250;
	int i, j, k =0;
	//short *data;
	
	struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;

	if(f54_data == NULL){
		printk("%s: f54_data is null \n",__func__);
		return EINVAL;
	}
	 // Set report mode to to run the AutoScan
	if(!is_report_type_valid(f54_data->report_type)) {
		printk("%s: report_type is invalid \n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}
	command = f54_data->report_type;
	retval = synaptics_rmi4_reg_write(rmi4_data,
	 		f54_data->data_base_addr,
	 		(unsigned char*)&command, sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write command=0x%x\n",__func__, command);
		retval = -EINVAL;
		goto error_exit;
	}

	// Set the GetReport bit to run the AutoScan
	command = 0x01;
	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->command_base_addr,
			(unsigned char*)&command, sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write command 0x01\n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}
	do {	
		retval = synaptics_rmi4_reg_read(rmi4_data,
			f54_data->command_base_addr, (unsigned char*)&command, sizeof(command));

		if(retval < 0) {
			printk("%s: Failed to read command 0x01 \n",__func__);
			retval = -EINVAL;
			goto error_exit;
		} 
		if(command & COMMAND_GET_REPORT)
			msleep(20);
		else
			break;
	} while (--patience > 0);
	//mdelay(20);
	if(command & COMMAND_GET_REPORT) {
		printk("%s: Time out waiting for report ready \n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}

	set_report_size();
	if (f54_data->report_size == 0) {
		printk("%s: Report data size = 0\n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}

	if (f54_data->data_buffer_size < f54_data->report_size) {
		mutex_lock(&f54_data->data_mutex);
		if (f54_data->data_buffer_size)
			kfree(f54_data->report_data);
		f54_data->report_data = kzalloc(f54_data->report_size, GFP_KERNEL);
		if (!f54_data->report_data) {
			printk("%s: Failed to alloc mem for data buffer\n",__func__);
			f54_data->data_buffer_size = 0;
			mutex_unlock(&f54_data->data_mutex);
			retval = -ENOMEM;
			goto error_exit;
		}
		f54_data->data_buffer_size = f54_data->report_size;
		mutex_unlock(&f54_data->data_mutex);
	}

	report_index[0] = 0;
	report_index[1] = 0;
	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->data_base_addr + DATA_REPORT_INDEX_OFFSET,
			report_index,
			sizeof(report_index));
	if (retval < 0) {
		printk("%s: Failed to write index\n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}
	
	
	//to read data
	retval = synaptics_rmi4_reg_read(rmi4_data,
			f54_data->data_base_addr + DATA_REPORT_DATA_OFFSET,
			f54_data->report_data,f54_data->report_size);
	if (retval < 0) {
		printk("%s: Failed to read image data\n",__func__);
		retval = -EINVAL;
		goto error_exit;
	}

	if(f54_data->report_type == F54_HIGH_RESISTANCE) {
		for(i = 0; i < HIGH_RESISTANCE_DATA_SIZE; i+=2) {
			HighResistance[i/2] = (short) (f54_data->report_data[i]|(f54_data->report_data[i+1] << 8));
			printk("%d ,",HighResistance[i/2]);
		}
		return 0;
	}
	
	for (i = 0; i < f54_data->tx_assigned; i++)
   {
       for (j = 0; j < f54_data->rx_assigned; j++)
	   {
		 ImageArray[i][j] =(short) (f54_data->report_data[k]|(f54_data->report_data[k+1] << 8));
		 k = k + 2;
	   }  	  
   }

	printk("%s: query_base_addr:0x%04x; data_base_addr:0x%04x; tx:%d; rx:%d \n",__func__,f54_data->query_base_addr,
			f54_data->data_base_addr,f54_data->tx_assigned,f54_data->rx_assigned);
   // Check against test limits
   	if(f54_data->report_type == F54_16BIT_IMAGE) {
		for (j = 0; j < f54_data->rx_assigned; j++)
		{   
		   for (i = 0; i < f54_data->tx_assigned; i++)
		   {
				printk("(%d),",ImageArray[i][j]);
		   }
		}
   	}else {
		for (i = 0; i < f54_data->tx_assigned; i++)
		{   
		   for (j = 0; j < f54_data->rx_assigned; j++)
		   {
				printk("(%d),",ImageArray[i][j]);
		   }
		}
	}
   
	return 0;
error_exit:

	return retval;
}

#define F54_CTRL_86_ADDR 0x0133

int ForceDisplayRateTo60Hz(void)
{
	struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;
	unsigned char tmp = 0x00;
	unsigned char data = 0x00;
	unsigned char mask_dynamic_sensing = 0x06; // Dynamic Sensing Select bit 2:1
	//unsigned char mask_force_update = 0x04;
	unsigned char command;
	unsigned char timeout_count;	
	int retval = 0;
		
	retval = synaptics_rmi4_reg_read(rmi4_data,
			F54_CTRL_86_ADDR,
			&tmp,
			sizeof(tmp));
	if (retval < 0) {
		printk("%s: Failed to store display ratio\n",__func__);
		return retval;
	}
	
	g_display_ratio = tmp & mask_dynamic_sensing;
	data = tmp & (~mask_dynamic_sensing);
	data |= 0x02; //  bit 2:1 sets to '01' --> 1:1 Fixed
  
	retval = synaptics_rmi4_reg_write(rmi4_data,
			F54_CTRL_86_ADDR,
			&data,
			sizeof(data));
	if (retval < 0) {
		printk("%s: Failed to set display ratio\n",__func__);
		return retval;

	}
	
	// force update
	command = (unsigned char)COMMAND_FORCE_UPDATE;	
	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->command_base_addr,
			&command,
			sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write force update command\n",__func__);
		return retval;
	}

	timeout_count = 0;
	do {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->command_base_addr,
				&command,
				sizeof(command));
		if (retval < 0) {
			printk("%s: Failed to read command register\n",__func__);
			return retval;
		}
		
		if (command == 0x00)
			break;

		msleep(100);
		timeout_count++;
	} while (timeout_count < FORCE_TIMEOUT_100MS);


	if (timeout_count == FORCE_TIMEOUT_100MS) {
		printk("%s: Timed out waiting for force update\n",__func__);
		return -ETIMEDOUT;
	}
	
	return retval;
}

int RestoreDisplayRate(void)
{
	struct synaptics_rmi4_data *rmi4_data = f54_data->rmi4_data;
	unsigned char tmp = 0x00;
	//unsigned char mask_dynamic_sensing = 0x06; // Dynamic Sensing Select bit 2:1
	//unsigned char mask_force_update = 0x04;
	unsigned char command;
	unsigned char timeout_count;
	int retval = 0;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			F54_CTRL_86_ADDR,
			&tmp,
			sizeof(tmp));
	if (retval < 0) {
		printk("%s: Failed to read display ratio\n",__func__);
		return retval;
	}
	
	tmp |= g_display_ratio;
	retval = synaptics_rmi4_reg_write(rmi4_data,
			F54_CTRL_86_ADDR,
			&tmp,
			sizeof(tmp));
	if (retval < 0) {
		printk("%s: Failed to set display ratio\n",__func__);
		return retval;;

	}

	command = (unsigned char)COMMAND_FORCE_UPDATE;	
	retval = synaptics_rmi4_reg_write(rmi4_data,
			f54_data->command_base_addr,
			&command,
			sizeof(command));
	if (retval < 0) {
		printk("%s: Failed to write force update command\n",__func__);
		return retval;
	}

	timeout_count = 0;
	do {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				f54_data->command_base_addr,
				&command,
				sizeof(command));
		if (retval < 0) {
			printk("%s: Failed to read command register\n",__func__);
			return retval;
		}
		
		if (command == 0x00)
			break;

		msleep(100);
		timeout_count++;
	} while (timeout_count < FORCE_TIMEOUT_100MS);


	if (timeout_count == FORCE_TIMEOUT_100MS) {
		printk("%s: Timed out waiting for force update\n",__func__);
		return -ETIMEDOUT;
	}
	return retval;
}


int synaptics_rmi4_f54_command_func(int command, int value)
{
	int retval = 0;
	switch (command) {
		case F54_SELF_TEST:

			printk("%s: 0 retval = %d\n",__func__, retval);			
			retval = ForceDisplayRateTo60Hz();
			if (retval < 0) {
				printk("%s: Failed to store display ratio\n",__func__);
				return retval;
			}
			
			synaptics_f54_set_report_type(F54_FULL_RAW_TD4300);
			if (retval < 0) {
				printk("%s: Failed to set report type\n",__func__);
				return retval;
			}
			
			retval = synaptics_rmi4_f54_get_report_data();

			if(retval < 0) {
				synaptics_rmi4_f54_sw_reset(f54_data->rmi4_data);
				return retval;
			}
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test begin
#if 0
			retval = RestoreDisplayRate();
			if(retval < 0) {
				printk("%s: Failed to resotre display ratio\n",__func__);
				synaptics_rmi4_f54_sw_reset(f54_data->rmi4_data);
				return retval;
			}			
#endif
//Gionee <GN_BSP_CTP> <mogongfu> <20161116> modify for tp full screen test end
#if 0
			printk("%s: 3 retval = %d\n",__func__, retval);
			synaptics_f54_set_report_type(F54_HIGH_RESISTANCE);
			retval = synaptics_rmi4_f54_get_report_data();
			if(retval < 0) {
				synaptics_rmi4_f54_sw_reset(f54_data->rmi4_data);
				return retval;
			}
#endif
			
			synaptics_rmi4_f54_sw_reset(f54_data->rmi4_data);

#if 1
			retval = synaptics_rmi4_f54_check_full_raw_data() ;
#else
			retval = (synaptics_rmi4_f54_check_full_raw_data() & synaptics_rmi4_f54_check_high_resistance_data());
#endif
			break;
		case F54_GET_REPORT_TYPE:
			retval = synaptics_f54_get_report_type();
			break;
		case F54_SET_REPORT_TYPE:
			retval = synaptics_f54_set_report_type(value);
			break;
		case F54_GET_REPORT_DATA:
			retval = synaptics_rmi4_f54_get_report_data();
			//synaptics_rmi4_f54_sw_reset(f54_data->rmi4_data);
			break;
		default:
			retval = -1;
			break;
	}
	return retval;
}
EXPORT_SYMBOL(synaptics_rmi4_f54_command_func);



static void synaptics_rmi4_f54_data_set_regs(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn_desc *fd,	unsigned char page)
{
	f54_data->query_base_addr = fd->query_base_addr | (page << 8);
	f54_data->control_base_addr = fd->ctrl_base_addr | (page << 8);
	f54_data->data_base_addr = fd->data_base_addr | (page << 8);
	f54_data->command_base_addr = fd->cmd_base_addr | (page << 8);
	return;
}
int synaptics_rmi4_f54_data_init(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	unsigned short pdt_entry_addr;
	unsigned char page_number;
	//unsigned char rx_tx[2];

	bool f54found = false;
	
	struct synaptics_rmi4_fn_desc rmi_fd;
	if(f54_data == NULL) {
		f54_data = kzalloc(sizeof(*f54_data), GFP_KERNEL);
		if (!f54_data) {
			printk("%s: Failed to alloc mem for f54\n",__func__);
			retval = -ENOMEM;
			goto exit;
		}
	}
	f54_data->rmi4_data = rmi4_data;
	/* Scan the page description tables of the pages to service */
	for (page_number = 0; page_number < PAGES_TO_SERVICE; page_number++) {
		for (pdt_entry_addr = PDT_START; pdt_entry_addr > PDT_END;
				pdt_entry_addr -= PDT_ENTRY_SIZE) {
			pdt_entry_addr |= (page_number << 8);

			retval = synaptics_rmi4_reg_read(rmi4_data,
					pdt_entry_addr,
					(unsigned char *)&rmi_fd,
					sizeof(rmi_fd));
			if (retval < 0)
				return retval;

			pdt_entry_addr &= ~(MASK_8BIT << 8);

			if (rmi_fd.fn_number == 0) {
				printk("%s: Reached end of PDT\n",__func__);
				break;
			}

			printk("%s: F%02x found (page %d)\n",__func__, rmi_fd.fn_number,
					page_number);

			switch (rmi_fd.fn_number) {
			case SYNAPTICS_RMI4_F54:
				synaptics_rmi4_f54_data_set_regs(rmi4_data,
						&rmi_fd, page_number);
				f54found = true;
				break;
			default:
				break;
			}

			if (f54found)
				goto pdt_done;

		}
	}

	if (!f54found) {
		retval = -ENODEV;
		goto exit_free_f54;
	}

pdt_done:
	retval = synaptics_rmi4_reg_read(rmi4_data,
			f54_data->query_base_addr,
			f54_data->query.data,
			sizeof(f54_data->query.data));
	if (retval < 0) {
		printk("%s: Failed to read f54 query registers\n",__func__);
		goto exit;
	}

	retval = synaptics_rmi4_f54_set_ctrl();
	if (retval < 0) {
		printk("%s: Failed to set up f54 control registers\n",__func__);
		goto exit_free_control;
	}
	
	mutex_init(&f54_data->data_mutex);
	mutex_init(&f54_data->control_mutex);

	f54_data->rx_assigned =CFG_F54_RXCOUNT ;// rx_tx[0];
	f54_data->tx_assigned =CFG_F54_TXCOUNT ;//rx_tx[1];
	f54_data->report_size = f54_data->rx_assigned * f54_data->tx_assigned *2;
	f54_data->report_type = 2;
	return 0;

exit_free_control:
	free_control_mem();	
exit_free_f54:
	kfree(f54_data);
	f54_data = NULL;

exit:
	return retval;
}
EXPORT_SYMBOL(synaptics_rmi4_f54_data_init);

MODULE_AUTHOR("Synaptics, Inc.");
MODULE_DESCRIPTION("Synaptics DSX Test Reporting Module");
MODULE_LICENSE("GPL v2");
