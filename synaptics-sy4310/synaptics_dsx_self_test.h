
#define WATCHDOG_HRTIMER
#define WATCHDOG_TIMEOUT_S 2
#define FORCE_TIMEOUT_100MS 10
#define STATUS_WORK_INTERVAL 20 /* ms */

#define STATUS_IDLE 0
#define STATUS_BUSY 1

#define DATA_REPORT_INDEX_OFFSET 1
#define DATA_REPORT_DATA_OFFSET 3

#define SENSOR_RX_MAPPING_OFFSET 1
#define SENSOR_TX_MAPPING_OFFSET 2

#define COMMAND_GET_REPORT 1
#define COMMAND_FORCE_CAL 2
#define COMMAND_FORCE_UPDATE 4

#define CONTROL_42_SIZE 2
#define CONTROL_43_54_SIZE 13
#define CONTROL_55_56_SIZE 2
#define CONTROL_58_SIZE 1
#define CONTROL_59_SIZE 2
#define CONTROL_60_62_SIZE 3
#define CONTROL_63_SIZE 1
#define CONTROL_64_67_SIZE 4
#define CONTROL_68_73_SIZE 8
#define CONTROL_74_SIZE 2
#define CONTROL_76_SIZE 1
#define CONTROL_77_78_SIZE 2
#define CONTROL_79_83_SIZE 5
#define CONTROL_84_85_SIZE 2
#define CONTROL_86_SIZE 1
#define CONTROL_87_SIZE 1


#define HIGH_RESISTANCE_DATA_SIZE 6
#define FULL_RAW_CAP_MIN_MAX_DATA_SIZE 4
#define TREX_DATA_SIZE 7

struct f54_query {
	union {
		struct {
			/* query 0 */
			unsigned char num_of_rx_electrodes;

			/* query 1 */
			unsigned char num_of_tx_electrodes;

			/* query 2 */
			unsigned char f54_query2_b0__1:2;
			unsigned char has_baseline:1;
			unsigned char has_image8:1;
			unsigned char f54_query2_b4__5:2;
			unsigned char has_image16:1;
			unsigned char f54_query2_b7:1;

			/* queries 3.0 and 3.1 */
			unsigned short clock_rate;

			/* query 4 */
			unsigned char touch_controller_family;

			/* query 5 */
			unsigned char has_pixel_touch_threshold_adjustment:1;
			unsigned char f54_query5_b1__7:7;

			/* query 6 */
			unsigned char has_sensor_assignment:1;
			unsigned char has_interference_metric:1;
			unsigned char has_sense_frequency_control:1;
			unsigned char has_firmware_noise_mitigation:1;
			unsigned char has_ctrl11:1;
			unsigned char has_two_byte_report_rate:1;
			unsigned char has_one_byte_report_rate:1;
			unsigned char has_relaxation_control:1;

			/* query 7 */
			unsigned char curve_compensation_mode:2;
			unsigned char f54_query7_b2__7:6;

			/* query 8 */
			unsigned char f54_query8_b0:1;
			unsigned char has_iir_filter:1;
			unsigned char has_cmn_removal:1;
			unsigned char has_cmn_maximum:1;
			unsigned char has_touch_hysteresis:1;
			unsigned char has_edge_compensation:1;
			unsigned char has_per_frequency_noise_control:1;
			unsigned char has_enhanced_stretch:1;

			/* query 9 */
			unsigned char has_force_fast_relaxation:1;
			unsigned char has_multi_metric_state_machine:1;
			unsigned char has_signal_clarity:1;
			unsigned char has_variance_metric:1;
			unsigned char has_0d_relaxation_control:1;
			unsigned char has_0d_acquisition_control:1;
			unsigned char has_status:1;
			unsigned char has_slew_metric:1;

			/* query 10 */
			unsigned char has_h_blank:1;
			unsigned char has_v_blank:1;
			unsigned char has_long_h_blank:1;
			unsigned char has_startup_fast_relaxation:1;
			unsigned char has_esd_control:1;
			unsigned char has_noise_mitigation2:1;
			unsigned char has_noise_state:1;
			unsigned char has_energy_ratio_relaxation:1;

			/* query 11 */
			unsigned char has_excessive_noise_reporting:1;
			unsigned char has_slew_option:1;
			unsigned char has_two_overhead_bursts:1;
			unsigned char has_query13:1;
			unsigned char has_one_overhead_burst:1;
			unsigned char f54_query11_b5:1;
			unsigned char has_ctrl88:1;
			unsigned char has_query15:1;

			/* query 12 */
			unsigned char number_of_sensing_frequencies:4;
			unsigned char f54_query12_b4__7:4;

			/* query 13 */
			unsigned char has_ctrl86:1;
			unsigned char has_ctrl87:1;
			unsigned char has_ctrl87_sub0:1;
			unsigned char has_ctrl87_sub1:1;
			unsigned char has_ctrl87_sub2:1;
			unsigned char has_cidim:1;
			unsigned char has_noise_mitigation_enhancement:1;
			unsigned char has_rail_im:1;
		} __packed;
		unsigned char data[15];
	};
};

struct f54_control_0 {
	union {
		struct {
			unsigned char no_relax:1;
			unsigned char no_scan:1;
			unsigned char force_fast_relaxation:1;
			unsigned char startup_fast_relaxation:1;
			unsigned char gesture_cancels_sfr:1;
			unsigned char enable_energy_ratio_relaxation:1;
			unsigned char excessive_noise_attn_enable:1;
			unsigned char f54_control0_b7:1;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_1 {
	union {
		struct {
			unsigned char bursts_per_cluster:4;
			unsigned char f54_ctrl1_b4__7:4;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_2 {
	union {
		struct {
			unsigned short saturation_cap;
		} __packed;
		struct {
			unsigned char data[2];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_3 {
	union {
		struct {
			unsigned char pixel_touch_threshold;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_4__6 {
	union {
		struct {
			/* control 4 */
			unsigned char rx_feedback_cap:2;
			unsigned char bias_current:2;
			unsigned char f54_ctrl4_b4__7:4;

			/* control 5 */
			unsigned char low_ref_cap:2;
			unsigned char low_ref_feedback_cap:2;
			unsigned char low_ref_polarity:1;
			unsigned char f54_ctrl5_b5__7:3;

			/* control 6 */
			unsigned char high_ref_cap:2;
			unsigned char high_ref_feedback_cap:2;
			unsigned char high_ref_polarity:1;
			unsigned char f54_ctrl6_b5__7:3;
		} __packed;
		struct {
			unsigned char data[3];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_7 {
	union {
		struct {
			unsigned char cbc_cap:3;
			unsigned char cbc_polarity:1;
			unsigned char cbc_tx_carrier_selection:1;
			unsigned char f54_ctrl7_b5__7:3;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_8__9 {
	union {
		struct {
			/* control 8 */
			unsigned short integration_duration:10;
			unsigned short f54_ctrl8_b10__15:6;

			/* control 9 */
			unsigned char reset_duration;
		} __packed;
		struct {
			unsigned char data[3];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_10 {
	union {
		struct {
			unsigned char noise_sensing_bursts_per_image:4;
			unsigned char f54_ctrl10_b4__7:4;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_11 {
	union {
		struct {
			unsigned short f54_ctrl11;
		} __packed;
		struct {
			unsigned char data[2];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_12__13 {
	union {
		struct {
			/* control 12 */
			unsigned char slow_relaxation_rate;

			/* control 13 */
			unsigned char fast_relaxation_rate;
		} __packed;
		struct {
			unsigned char data[2];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_14 {
	union {
		struct {
			unsigned char rxs_on_xaxis:1;
			unsigned char curve_comp_on_txs:1;
			unsigned char f54_ctrl14_b2__7:6;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_15n {
	unsigned char sensor_rx_assignment;
};

struct f54_control_15 {
	struct f54_control_15n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_16n {
	unsigned char sensor_tx_assignment;
};

struct f54_control_16 {
	struct f54_control_16n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_17n {
	unsigned char burst_count_b8__10:3;
	unsigned char disable:1;
	unsigned char f54_ctrl17_b4:1;
	unsigned char filter_bandwidth:3;
};

struct f54_control_17 {
	struct f54_control_17n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_18n {
	unsigned char burst_count_b0__7;
};

struct f54_control_18 {
	struct f54_control_18n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_19n {
	unsigned char stretch_duration;
};

struct f54_control_19 {
	struct f54_control_19n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_20 {
	union {
		struct {
			unsigned char disable_noise_mitigation:1;
			unsigned char f54_ctrl20_b1__7:7;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_21 {
	union {
		struct {
			unsigned short freq_shift_noise_threshold;
		} __packed;
		struct {
			unsigned char data[2];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_22__26 {
	union {
		struct {
			/* control 22 */
			unsigned char f54_ctrl22;

			/* control 23 */
			unsigned short medium_noise_threshold;

			/* control 24 */
			unsigned short high_noise_threshold;

			/* control 25 */
			unsigned char noise_density;

			/* control 26 */
			unsigned char frame_count;
		} __packed;
		struct {
			unsigned char data[7];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_27 {
	union {
		struct {
			unsigned char iir_filter_coef;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_28 {
	union {
		struct {
			unsigned short quiet_threshold;
		} __packed;
		struct {
			unsigned char data[2];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_29 {
	union {
		struct {
			/* control 29 */
			unsigned char f54_ctrl29_b0__6:7;
			unsigned char cmn_filter_disable:1;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_30 {
	union {
		struct {
			unsigned char cmn_filter_max;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_31 {
	union {
		struct {
			unsigned char touch_hysteresis;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_32__35 {
	union {
		struct {
			/* control 32 */
			unsigned short rx_low_edge_comp;

			/* control 33 */
			unsigned short rx_high_edge_comp;

			/* control 34 */
			unsigned short tx_low_edge_comp;

			/* control 35 */
			unsigned short tx_high_edge_comp;
		} __packed;
		struct {
			unsigned char data[8];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_36n {
	unsigned char axis1_comp;
};

struct f54_control_36 {
	struct f54_control_36n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_37n {
	unsigned char axis2_comp;
};

struct f54_control_37 {
	struct f54_control_37n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_38n {
	unsigned char noise_control_1;
};

struct f54_control_38 {
	struct f54_control_38n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_39n {
	unsigned char noise_control_2;
};

struct f54_control_39 {
	struct f54_control_39n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_40n {
	unsigned char noise_control_3;
};

struct f54_control_40 {
	struct f54_control_40n *data;
	unsigned short address;
	unsigned char length;
};

struct f54_control_41 {
	union {
		struct {
			unsigned char no_signal_clarity:1;
			unsigned char f54_ctrl41_b1__7:7;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_57 {
	union {
		struct {
			unsigned char cbc_cap_0d:3;
			unsigned char cbc_polarity_0d:1;
			unsigned char cbc_tx_carrier_selection_0d:1;
			unsigned char f54_ctrl57_b5__7:3;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control_88 {
	union {
		struct {
			unsigned char tx_low_reference_polarity:1;
			unsigned char tx_high_reference_polarity:1;
			unsigned char abs_low_reference_polarity:1;
			unsigned char abs_polarity:1;
			unsigned char cbc_polarity:1;
			unsigned char cbc_tx_carrier_selection:1;
			unsigned char charge_pump_enable:1;
			unsigned char cbc_abs_auto_servo:1;
		} __packed;
		struct {
			unsigned char data[1];
			unsigned short address;
		} __packed;
	};
};

struct f54_control {
	struct f54_control_0 *reg_0;
	struct f54_control_1 *reg_1;
	struct f54_control_2 *reg_2;
	struct f54_control_3 *reg_3;
	struct f54_control_4__6 *reg_4__6;
	struct f54_control_7 *reg_7;
	struct f54_control_8__9 *reg_8__9;
	struct f54_control_10 *reg_10;
	struct f54_control_11 *reg_11;
	struct f54_control_12__13 *reg_12__13;
	struct f54_control_14 *reg_14;
	struct f54_control_15 *reg_15;
	struct f54_control_16 *reg_16;
	struct f54_control_17 *reg_17;
	struct f54_control_18 *reg_18;
	struct f54_control_19 *reg_19;
	struct f54_control_20 *reg_20;
	struct f54_control_21 *reg_21;
	struct f54_control_22__26 *reg_22__26;
	struct f54_control_27 *reg_27;
	struct f54_control_28 *reg_28;
	struct f54_control_29 *reg_29;
	struct f54_control_30 *reg_30;
	struct f54_control_31 *reg_31;
	struct f54_control_32__35 *reg_32__35;
	struct f54_control_36 *reg_36;
	struct f54_control_37 *reg_37;
	struct f54_control_38 *reg_38;
	struct f54_control_39 *reg_39;
	struct f54_control_40 *reg_40;
	struct f54_control_41 *reg_41;
	struct f54_control_57 *reg_57;
	struct f54_control_88 *reg_88;
};


enum func_command_types{
	F54_SELF_TEST = 1,
	F54_GET_REPORT_TYPE = 2,
	F54_SET_REPORT_TYPE = 3,
	F54_GET_REPORT_DATA = 4,
};


enum f54_report_types {
	F54_8BIT_IMAGE = 1,
	F54_16BIT_IMAGE = 2,
	F54_RAW_16BIT_IMAGE = 3,
	F54_HIGH_RESISTANCE = 4,
	F54_TX_TO_TX_SHORT = 5,
	F54_RX_TO_RX1 = 7,
	F54_TRUE_BASELINE = 9,
	F54_FULL_RAW_CAP_MIN_MAX = 13,
	F54_RX_OPENS1 = 14,
	F54_TX_OPEN = 15,
	F54_TX_TO_GROUND = 16,
	F54_RX_TO_RX2 = 17,
	F54_RX_OPENS2 = 18,
	F54_FULL_RAW_CAP = 19,
	F54_FULL_RAW_CAP_RX_COUPLING_COMP = 20,
	F54_SENSOR_SPEED = 22,
	F54_ADC_RANGE = 23,
	F54_TREX_OPENS = 24,
	F54_TREX_TO_GND = 25,
	F54_TREX_SHORTS = 26,
	F54_FULL_RAW_TD4300 = 92,
	INVALID_REPORT_TYPE = -1,
	
};


struct rim4_f54_data {
	unsigned char rx_assigned;
	unsigned char tx_assigned;
	unsigned char *report_data;
	unsigned short query_base_addr;
	unsigned short control_base_addr;
	unsigned short data_base_addr;
	unsigned short command_base_addr;
	unsigned int report_size;
	unsigned int data_buffer_size;
	enum f54_report_types report_type;
	struct mutex data_mutex;
	struct mutex control_mutex;
	struct f54_query query;
	struct f54_control control;
	struct synaptics_rmi4_data *rmi4_data;
	
};

