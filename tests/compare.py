#!/usr/bin/python

from sys      import argv
from operator import itemgetter
from re       import match
from datetime import datetime

# PAPI events
events = [
#    name         ,  description,                                                   true iff higher values are better
	('PAPI_L1_ICM',  'Level 1 instruction cache misses',                                                        False),
	('PAPI_L2_DCM',  'Level 2 data cache misses',                                                               False),
	('PAPI_L2_ICM',  'Level 2 instruction cache misses',                                                        False),
	('PAPI_L3_DCM',  'Level 3 data cache misses',                                                               False),
	('PAPI_L3_ICM',  'Level 3 instruction cache misses',                                                        False),
	('PAPI_L1_TCM',  'Level 1 cache misses',                                                                    False),
	('PAPI_L2_TCM',  'Level 2 cache misses',                                                                    False),
	('PAPI_L3_TCM',  'Level 3 cache misses',                                                                    False),
	('PAPI_CA_SNP',  'Requests for a snoop',                                                                    False),
	('PAPI_CA_SHR',  'Requests for exclusive access to shared cache line',                                      False),
	('PAPI_CA_CLN',  'Requests for exclusive access to clean cache line',                                       False),
	('PAPI_CA_INV',  'Requests for cache line invalidation',                                                    False),
	('PAPI_CA_ITV',  'Requests for cache line intervention',                                                    False),
	('PAPI_L3_LDM',  'Level 3 load misses',                                                                     False),
	('PAPI_L3_STM',  'Level 3 store misses',                                                                    False),
	('PAPI_BRU_IDL', 'Cycles branch units are idle',                                                            False),
	('PAPI_FXU_IDL', 'Cycles integer units are idle',                                                           False),
	('PAPI_FPU_IDL', 'Cycles floating point units are idle',                                                    False),
	('PAPI_LSU_IDL', 'Cycles load/store units are idle',                                                        False),
	('PAPI_TLB_DM',  'Data translation lookaside buffer misses',                                                False),
	('PAPI_TLB_IM',  'Instruction translation lookaside buffer misses',                                         False),
	('PAPI_TLB_TL',  'Total translation lookaside buffer misses',                                               False),
	('PAPI_L1_LDM',  'Level 1 load misses',                                                                     False),
	('PAPI_L1_STM',  'Level 1 store misses',                                                                    False),
	('PAPI_L2_LDM',  'Level 2 load misses',                                                                     False),
	('PAPI_L2_STM',  'Level 2 store misses',                                                                    False),
	('PAPI_BTAC_M',  'Branch target address cache misses',                                                      False),
	('PAPI_PRF_DM',  'Data prefetch cache misses',                                                              False),
	('PAPI_L3_DCH',  'Level 3 data cache hits',                                                                 False),
	('PAPI_TLB_SD',  'Translation lookaside buffer shootdowns',                                                 False),
	('PAPI_CSR_FAL', 'Failed store conditional instructions',                                                   False),
	('PAPI_CSR_SUC', 'Successful store conditional instructions',                                               False),
	('PAPI_CSR_TOT', 'Total store conditional instructions',                                                    False),
	('PAPI_MEM_SCY', 'Cycles Stalled Waiting for memory accesses',                                              False),
	('PAPI_MEM_RCY', 'Cycles Stalled Waiting for memory Reads',                                                 False),
	('PAPI_MEM_WCY', 'Cycles Stalled Waiting for memory writes',                                                False),
	('PAPI_STL_ICY', 'Cycles with no instruction issue',                                                        False),
	('PAPI_FUL_ICY', 'Cycles with maximum instruction issue',                                                   False),
	('PAPI_STL_CCY', 'Cycles with no instructions completed',                                                   False),
	('PAPI_FUL_CCY', 'Cycles with maximum instructions completed',                                              False),
	('PAPI_HW_INT',  'Hardware interrupts',                                                                     False),
	('PAPI_BR_UCN',  'Unconditional branch instructions',                                                       False),
	('PAPI_BR_CN',   'Conditional branch instructions',                                                         False),
	('PAPI_BR_TKN',  'Conditional branch instructions taken',                                                   False),
	('PAPI_BR_NTK',  'Conditional branch instructions not taken',                                               False),
	('PAPI_BR_MSP',  'Conditional branch instructions mispredicted',                                            False),
	('PAPI_BR_PRC',  'Conditional branch instructions correctly predicted',                                     False),
	('PAPI_FMA_INS', 'FMA instructions completed',                                                              False),
	('PAPI_TOT_IIS', 'Instructions issued',                                                                     False),
	('PAPI_TOT_INS', 'Instructions completed',                                                                  False),
	('PAPI_INT_INS', 'Integer instructions',                                                                    False),
	('PAPI_FP_INS',  'Floating point instructions',                                                             False),
	('PAPI_LD_INS',  'Load instructions',                                                                       False),
	('PAPI_SR_INS',  'Store instructions',                                                                      False),
	('PAPI_BR_INS',  'Branch instructions',                                                                     False),
	('PAPI_VEC_INS', 'Vector/SIMD instructions (could include integer)',                                        False),
	('PAPI_RES_STL', 'Cycles stalled on any resource',                                                          False),
	('PAPI_FP_STAL', 'Cycles the FP unit(s) are stalled',                                                       False),
	('PAPI_TOT_CYC', 'Total cycles',                                                                            False),
	('PAPI_LST_INS', 'Load/store instructions completed',                                                       False),
	('PAPI_SYC_INS', 'Synchronization instructions completed',                                                  False),
	('PAPI_L1_DCH',  'Level 1 data cache hits',                                                                 True),
	('PAPI_L2_DCH',  'Level 2 data cache hits',                                                                 True),
	('PAPI_L1_DCA',  'Level 1 data cache accesses',                                                             False),
	('PAPI_L2_DCA',  'Level 2 data cache accesses',                                                             False),
	('PAPI_L3_DCA',  'Level 3 data cache accesses',                                                             False),
	('PAPI_L1_DCR',  'Level 1 data cache reads',                                                                False),
	('PAPI_L2_DCR',  'Level 2 data cache reads',                                                                False),
	('PAPI_L3_DCR',  'Level 3 data cache reads',                                                                False),
	('PAPI_L1_DCW',  'Level 1 data cache writes',                                                               False),
	('PAPI_L2_DCW',  'Level 2 data cache writes',                                                               False),
	('PAPI_L3_DCW',  'Level 3 data cache writes',                                                               False),
	('PAPI_L1_ICH',  'Level 1 instruction cache hits',                                                          True),
	('PAPI_L2_ICH',  'Level 2 instruction cache hits',                                                          True),
	('PAPI_L3_ICH',  'Level 3 instruction cache hits',                                                          True),
	('PAPI_L1_ICA',  'Level 1 instruction cache accesses',                                                      False),
	('PAPI_L2_ICA',  'Level 2 instruction cache accesses',                                                      False),
	('PAPI_L3_ICA',  'Level 3 instruction cache accesses',                                                      False),
	('PAPI_L1_ICR',  'Level 1 instruction cache reads',                                                         False),
	('PAPI_L2_ICR',  'Level 2 instruction cache reads',                                                         False),
	('PAPI_L3_ICR',  'Level 3 instruction cache reads',                                                         False),
	('PAPI_L1_ICW',  'Level 1 instruction cache writes',                                                        False),
	('PAPI_L2_ICW',  'Level 2 instruction cache writes',                                                        False),
	('PAPI_L3_ICW',  'Level 3 instruction cache writes',                                                        False),
	('PAPI_L1_TCH',  'Level 1 total cache hits',                                                                True),
	('PAPI_L2_TCH',  'Level 2 total cache hits',                                                                True),
	('PAPI_L3_TCH',  'Level 3 total cache hits',                                                                True),
	('PAPI_L1_TCA',  'Level 1 total cache accesses',                                                            False),
	('PAPI_L2_TCA',  'Level 2 total cache accesses',                                                            False),
	('PAPI_L3_TCA',  'Level 3 total cache accesses',                                                            False),
	('PAPI_L1_TCR',  'Level 1 total cache reads',                                                               False),
	('PAPI_L2_TCR',  'Level 2 total cache reads',                                                               False),
	('PAPI_L3_TCR',  'Level 3 total cache reads',                                                               False),
	('PAPI_L1_TCW',  'Level 1 total cache writes',                                                              False),
	('PAPI_L2_TCW',  'Level 2 total cache writes',                                                              False),
	('PAPI_L3_TCW',  'Level 3 total cache writes',                                                              False),
	('PAPI_FML_INS', 'Floating point multiply instructions',                                                    False),
	('PAPI_FAD_INS', 'Floating point add instructions',                                                         False),
	('PAPI_FDV_INS', 'Floating point divide instructions',                                                      False),
	('PAPI_FSQ_INS', 'Floating point square root instructions',                                                 False),
	('PAPI_FNV_INS', 'Floating point inverse instructions',                                                     False),
	('PAPI_FP_OPS',  'Floating point operations',                                                               False),
	('PAPI_SP_OPS',  'Floating point operations; optimized to count scaled single precision vector operations', False),
	('PAPI_DP_OPS',  'Floating point operations; optimized to count scaled double precision vector operations', False),
	('PAPI_VEC_SP',  'Single precision vector/SIMD instructions',                                               False),
	('PAPI_VEC_DP',  'Double precision vector/SIMD instructions',                                               False)
]

tests = {}

# lookup descriptor tupel for event.
def lookup( name ):
	for e in events:
		if e[0] == name:
			return e

# add data to statistics
def add( test, event, date, count ):
	# get the performance counts for test
	perfCounts = tests.setdefault( test, {} )
	# get the data for the event
	eventData  = perfCounts.setdefault( event, [] )

	# append data from run
	eventData.append( (count,date) )

## MAIN LOOP
# gather statistics
for log in argv[1:]:
	date, test = match( ".*perf_log-(.{19})-(.*)", log ).groups()

	for line in open( log, "r" ):
		count, event, descr = match( "\s*(\d+)\s+(\w+)\s+(\w.*)", line.strip("\n") ).groups()

		add( test, event, date, count )

# print statistics
for test, perfCounts in tests.items():
	print test + ":"

	for event, runs in perfCounts.items():
		eventName, eventDescr, moreIsBetter = lookup( event )

		print "\t", eventName.ljust(16), eventDescr

		runs.sort( reverse=moreIsBetter )

		for count, date in runs:
			print "\t\t", count.rjust(20), "  ", date

