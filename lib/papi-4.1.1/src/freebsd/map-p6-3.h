/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/* 
* File:    map-p6-3.h
* CVS:     $Id: map-p6-3.h,v 1.5 2010-04-15 18:32:41 bsheely Exp $
* Author:  Harald Servat
*          redcrash@gmail.com
*/

#ifndef FreeBSD_MAP_P6_3
#define FreeBSD_MAP_P6_3

enum NativeEvent_Value_P6_3_Processor {
	/* P6 common events */
	PNE_P6_3_BACLEARS = PAPI_NATIVE_MASK,
	PNE_P6_3_BR_BOGUS,
	PNE_P6_3_BR_INST_DECODED,
	PNE_P6_3_BR_INST_RETIRED,
	PNE_P6_3_BR_MISS_PRED_RETIRED,
	PNE_P6_3_BR_MISS_PRED_TAKEN_RET,
	PNE_P6_3_BR_TAKEN_RETIRED,
	PNE_P6_3_BTB_MISSES,
	PNE_P6_3_BUS_BNR_DRV,
	PNE_P6_3_BUS_DATA_RCV,
	PNE_P6_3_BUS_DRDY_CLOCKS,
	PNE_P6_3_BUS_HIT_DRV,
	PNE_P6_3_BUS_HITM_DRV,
	PNE_P6_3_BUS_LOCK_CLOCKS,
	PNE_P6_3_BUS_REQ_OUTSTANDING,
	PNE_P6_3_BUS_SNOOP_STALL,
	PNE_P6_3_BUS_TRAN_ANY,
	PNE_P6_3_BUS_TRAN_BRD,
	PNE_P6_3_BUS_TRAN_BURST,
	PNE_P6_3_BUS_TRAN_DEF,
	PNE_P6_3_BUS_TRAN_IFETCH,
	PNE_P6_3_BUS_TRAN_INVAL,
	PNE_P6_3_BUS_TRAN_MEM,
	PNE_P6_3_BUS_TRAN_POWER,
	PNE_P6_3_BUS_TRAN_RFO,
	PNE_P6_3_BUS_TRANS_IO,
	PNE_P6_3_BUS_TRANS_P,
	PNE_P6_3_BUS_TRANS_WB,
	PNE_P6_3_CPU_CLK_UNHALTED,
	PNE_P6_3_CYCLES_DIV_BUSY,
	PNE_P6_3_CYCLES_IN_PENDING_AND_MASKED,
	PNE_P6_3_CYCLES_INT_MASKED,
	PNE_P6_3_DATA_MEM_REFS,
	PNE_P6_3_DCU_LINES_IN,
	PNE_P6_3_DCU_M_LINES_IN,
	PNE_P6_3_DCU_M_LINES_OUT,
	PNE_P6_3_DCU_MISS_OUTSTANDING,
	PNE_P6_3_DIV,
	PNE_P6_3_FLOPS,
	PNE_P6_3_FP_ASSIST,
	PNE_P6_3_FTP_COMPS_OPS_EXE,
	PNE_P6_3_HW_INT_RX,
	PNE_P6_3_IFU_FETCH,
	PNE_P6_3_IFU_FETCH_MISS,
	PNE_P6_3_IFU_MEM_STALL,
	PNE_P6_3_ILD_STALL,
	PNE_P6_3_INST_DECODED,
	PNE_P6_3_INST_RETIRED,
	PNE_P6_3_ITLB_MISS,
	PNE_P6_3_L2_ADS,
	PNE_P6_3_L2_DBUS_BUSY,
	PNE_P6_3_L2_DBUS_BUSY_RD,
	PNE_P6_3_L2_IFETCH,
	PNE_P6_3_L2_LD,
	PNE_P6_3_L2_LINES_IN,
	PNE_P6_3_L2_LINES_OUT,
	PNE_P6_3_L2M_LINES_INM,
	PNE_P6_3_L2M_LINES_OUTM,
	PNE_P6_3_L2_RQSTS,
	PNE_P6_3_L2_ST,
	PNE_P6_3_LD_BLOCKS,
	PNE_P6_3_MISALIGN_MEM_REF,
	PNE_P6_3_MUL,
	PNE_P6_3_PARTIAL_RAT_STALLS,
	PNE_P6_3_RESOURCE_STALL,
	PNE_P6_3_SB_DRAINS,
	PNE_P6_3_SEGMENT_REG_LOADS,
	PNE_P6_3_UOPS_RETIRED,
	/* Pentium 3 specific events */
	PNE_P6_3_FP_MMX_TRANS,
	PNE_P6_3_MMX_ASSIST,
	PNE_P6_3_MMX_INSTR_EXEC,
	PNE_P6_3_MMX_INSTR_RET,
	PNE_P6_3_MMX_SAT_INSTR_EXEC,
	PNE_P6_3_MMX_UOPS_EXEC,
	PNE_P6_3_RET_SEG_RENAMES,
	PNE_P6_3_SEG_RENAME_STALLS,
	PNE_P6_3_EMON_KNI_COMP_INST_RET,
	PNE_P6_3_EMON_KNI_INST_RETIRED,
	PNE_P6_3_EMON_KNI_PREF_DISPATCHED,
	PNE_P6_3_EMON_KNI_PREF_MISS,
	PNE_P6_3_NATNAME_GUARD
};

extern Native_Event_LabelDescription_t P6_3_Processor_info[];
extern hwi_search_t P6_3_Processor_map[];

#endif
