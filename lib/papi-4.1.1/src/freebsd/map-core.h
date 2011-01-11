/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/* 
* File:    map-core.h
* CVS:     $Id: map-core.h,v 1.5 2010-04-15 18:32:41 bsheely Exp $
* Author:  Harald Servat
*          redcrash@gmail.com
*/

#ifndef FreeBSD_MAP_CORE
#define FreeBSD_MAP_CORE

enum NativeEvent_Value_CoreProcessor {
	PNE_CORE_BACLEARS = PAPI_NATIVE_MASK,
	PNE_CORE_BTB_MISSES,
	PNE_CORE_BR_BAC_MISSP_EXEC,
	PNE_CORE_BR_BOGUS,
	PNE_CORE_BR_CALL_EXEC,
	PNE_CORE_BR_CALL_MISSP_EXEC,
	PNE_CORE_BR_CND_EXEC,
	PNE_CORE_BR_CND_MISSP_EXEC,
	PNE_CORE_BR_IND_CALL_EXEC,
	PNE_CORE_BR_IND_EXEC,
	PNE_CORE_BR_IND_MISSP_EXEC,
	PNE_CORE_BR_INST_EXEC,
	PNE_CORE_BR_INSTR_DECODED,
	PNE_CORE_BR_INSTR_RET,
	PNE_CORE_BR_MISPRED_RET,
	PNE_CORE_BR_MISPRED_TAKEN_RET,
	PNE_CORE_BR_MISSP_EXEC,
	PNE_CORE_BR_RET_BAC_MISSP_EXEC,
	PNE_CORE_BR_RET_EXEC,
	PNE_CORE_BR_RET_MISSP_EXEC,
	PNE_CORE_BR_TAKEN_RET,
	PNE_CORE_BUS_BNR_CLOCKS,
	PNE_CORE_BUS_DRDY_CLOCKS,
	PNE_CORE_BUS_DATA_RCV,
	PNE_CORE_BUS_LOCKS_CLOCKS,
	PNE_CORE_BUS_NOT_IN_USE,
	PNE_CORE_BUS_REQ_OUTSTANDING,
	PNE_CORE_BUS_SNOOP_STALL,
	PNE_CORE_BUS_SNOOPS,
	PNE_CORE_BUS_TRANS_ANY,
	PNE_CORE_BUS_TRANS_BRD,
	PNE_CORE_BUS_TRANS_BURST,
	PNE_CORE_BUS_TRANS_DEF,
	PNE_CORE_BUS_TRANS_IO,
	PNE_CORE_BUS_TRANS_IFETCH,
	PNE_CORE_BUS_TRANS_INVAL,
	PNE_CORE_BUS_TRANS_MEM,
	PNE_CORE_BUS_TRANS_P,
	PNE_CORE_BUS_TRANS_PWR,
	PNE_CORE_BUS_TRANS_RFO,
	PNE_CORE_BUS_TRANS_WB,
	PNE_CORE_CYCLES_DIV_BUSY,
	PNE_CORE_CYCLES_INT_MASKED,
	PNE_CORE_CYCLES_INT_PENDING_MASKED,
	PNE_CORE_DCU_SNOOP_TO_SHARE,
	PNE_CORE_DCACHE_CACHE_LOCK,
	PNE_CORE_DCACHE_CACHE_LD,
	PNE_CORE_DCACHE_CACHE_ST,
	PNE_CORE_DCACHE_M_EVICT,
	PNE_CORE_DCACHE_M_REPL,
	PNE_CORE_DCACHE_PEND_MISS,
	PNE_CORE_DCACHE_REPL,
	PNE_CORE_DATA_MEM_CACHE_REF,
	PNE_CORE_DATA_MEM_REF,
	PNE_CORE_DBUS_BUSY,
	PNE_CORE_DBUS_BUSY_RD,
	PNE_CORE_DIV,
	PNE_CORE_DTLB_MISS,
	PNE_CORE_ESP_UOPS,
	PNE_CORE_EST_TRANS,
	PNE_CORE_FP_ASSIST,
	PNE_CORE_FP_COMP_INSTR_RET,
	PNE_CORE_FP_COMPS_OP_EXE,
	PNE_CORE_FP_MMX_TRANS,
	PNE_CORE_FUSED_LD_UOPS_RET,
	PNE_CORE_FUSED_ST_UOPS_RET,
	PNE_CORE_FUSED_UOPS_RET,
	PNE_CORE_HW_INT_RX,
	PNE_CORE_ICACHE_MISSES,
	PNE_CORE_ICACHE_READS,
	PNE_CORE_IFU_MEM_STALL,
	PNE_CORE_ILD_STALL,
	PNE_CORE_ITLB_MISSES,
	PNE_CORE_INSTR_DECODED,
	PNE_CORE_INSTR_RET,
	PNE_CORE_L1_PREF_REQ,
	PNE_CORE_L2_ADS,
	PNE_CORE_L2_IFETCH,
	PNE_CORE_L2_LD,
	PNE_CORE_L2_LINES_IN,
	PNE_CORE_L2_LINES_OUT,
	PNE_CORE_L2_M_LINES_IN,
	PNE_CORE_L2_M_LINES_OUT,
	PNE_CORE_L2_NO_REQUEST_CYCLES,
	PNE_CORE_L2_REJECT_CYCLES,
	PNE_CORE_L2_RQSTS,
	PNE_CORE_L2_ST,
	PNE_CORE_LD_BLOCKS,
	PNE_CORE_LLC_MISSES,
	PNE_CORE_LLC_REFERENCE,
	PNE_CORE_MMX_ASSIST,
	PNE_CORE_MMX_FP_TRANS,
	PNE_CORE_MMX_INSTR_EXEC,
	PNE_CORE_MMX_INSTR_RET,
	PNE_CORE_MISALIGN_MEM_REF,
	PNE_CORE_MUL,
	PNE_CORE_NONHLT_REF_CYCLES,
	PNE_CORE_PREF_RQSTS_DN,
	PNE_CORE_PREF_RQSTS_UP,
	PNE_CORE_RESOURCE_STALL,
	PNE_CORE_SD_DRAINS,
	PNE_CORE_SIMD_FP_DP_P_RET,
	PNE_CORE_SIMD_FP_DP_P_COMP_RET,
	PNE_CORE_SIMD_FP_DP_S_RET,
	PNE_CORE_SIMD_FP_DP_S_COMP_RET,
	PNE_CORE_SIMD_FP_SP_P_COMP_RET,
	PNE_CORE_SIMD_FP_SP_RET,
	PNE_CORE_SIMD_FP_SP_S_RET,
	PNE_CORE_SIMD_FP_SP_S_COMP_RET,
	PNE_CORE_SIMD_INT_128_RET,
	PNE_CORE_SIMD_INT_PARI_EXEC,
	PNE_CORE_SIMD_INT_PCK_EXEC,
	PNE_CORE_SIMD_INT_PLOG_EXEC,
	PNE_CORE_SIMD_INT_PMUL_EXEC,
	PNE_CORE_SIMD_INT_PSFT_EXEC,
	PNE_CORE_SIMD_INT_SAT_EXEC,
	PNE_CORE_SIMD_INT_UPCK_EXEC,
	PNE_CORE_SMC_DETECTED,
	PNE_CORE_SSE_NTSTORES_MISS,
	PNE_CORE_SSE_NTSTORES_RET,
	PNE_CORE_SSE_PREFNTA_MISS,
	PNE_CORE_SSE_PREFNTA_RET,
	PNE_CORE_SSE_PREFT1_MISS,
	PNE_CORE_SSE_PREFT1_RET,
	PNE_CORE_SSE_PREFT2_MISS,
	PNE_CORE_SSE_PREFT2_RET,
	PNE_CORE_SEG_REG_LOADS,
	PNE_CORE_SERIAL_EXECUTION_CYCLES,
	PNE_CORE_THERMAL_TRIP,
	PNE_CORE_UNFUSION,
	PNE_CORE_UNHALTED_CORE_CYCLES,
	PNE_CORE_UOPS_RET,
	PNE_CORE_NATNAME_GUARD
};

extern Native_Event_LabelDescription_t CoreProcessor_info[];
extern hwi_search_t CoreProcessor_map[];

#endif

