#!/usr/bin/python

from os.path  import exists, basename
from sys      import argv
from decimal  import Decimal
from re       import match
from datetime import datetime

DATE_FORMAT = "%d.%m.%Y-%H:%M:%S"

PAPI_EVENTS = {
	'PAPI_L2_TCA':  'Level 2 total cache accesses',
	'PAPI_INT_INS': 'Integer instructions',
	'PAPI_L2_TCH':  'Level 2 total cache hits',
	'PAPI_L2_TCM':  'Level 2 cache misses',
	'PAPI_L2_TCR':  'Level 2 total cache reads',
	'PAPI_L2_TCW':  'Level 2 total cache writes',
	'PAPI_BR_MSP':  'Conditional branch instructions mispredicted',
	'PAPI_BR_PRC':  'Conditional branch instructions correctly predicted',
	'PAPI_VEC_SP':  'Single precision vector/SIMD instructions',
	'PAPI_CSR_TOT': 'Total store conditional instructions',
	'PAPI_CSR_SUC': 'Successful store conditional instructions',
	'PAPI_BR_UCN':  'Unconditional branch instructions',
	'PAPI_CA_CLN':  'Requests for exclusive access to clean cache line',
	'PAPI_FML_INS': 'Floating point multiply instructions',
	'PAPI_FMA_INS': 'FMA instructions completed',
	'PAPI_MEM_SCY': 'Cycles Stalled Waiting for memory accesses',
	'PAPI_FUL_ICY': 'Cycles with maximum instruction issue',
	'PAPI_PRF_DM':  'Data prefetch cache misses',
	'PAPI_CA_SHR':  'Requests for exclusive access to shared cache line',
	'PAPI_LST_INS': 'Load/store instructions completed',
	'PAPI_FNV_INS': 'Floating point inverse instructions',
	'PAPI_MEM_WCY': 'Cycles Stalled Waiting for memory writes',
	'PAPI_L2_DCW':  'Level 2 data cache writes',
	'PAPI_L1_ICM':  'Level 1 instruction cache misses',
	'PAPI_L1_ICH':  'Level 1 instruction cache hits',
	'PAPI_L2_DCA':  'Level 2 data cache accesses',
	'PAPI_L1_DCW':  'Level 1 data cache writes',
	'PAPI_FPU_IDL': 'Cycles floating point units are idle',
	'PAPI_L2_DCM':  'Level 2 data cache misses',
	'PAPI_L2_LDM':  'Level 2 load misses',
	'PAPI_LD_INS':  'Load instructions',
	'PAPI_BTAC_M':  'Branch target address cache misses',
	'PAPI_STL_ICY': 'Cycles with no instruction issue',
	'PAPI_L3_TCA':  'Level 3 total cache accesses',
	'PAPI_TLB_IM':  'Instruction translation lookaside buffer misses',
	'PAPI_L1_DCR':  'Level 1 data cache reads',
	'PAPI_L2_STM':  'Level 2 store misses',
	'PAPI_STL_CCY': 'Cycles with no instructions completed',
	'PAPI_L3_TCH':  'Level 3 total cache hits',
	'PAPI_L3_TCM':  'Level 3 cache misses',
	'PAPI_L3_TCR':  'Level 3 total cache reads',
	'PAPI_FXU_IDL': 'Cycles integer units are idle',
	'PAPI_L3_TCW':  'Level 3 total cache writes',
	'PAPI_L2_ICM':  'Level 2 instruction cache misses',
	'PAPI_L3_LDM':  'Level 3 load misses',
	'PAPI_TOT_IIS': 'Instructions issued',
	'PAPI_L1_STM':  'Level 1 store misses',
	'PAPI_CA_SNP':  'Requests for a snoop',
	'PAPI_L3_STM':  'Level 3 store misses',
	'PAPI_MEM_RCY': 'Cycles Stalled Waiting for memory Reads',
	'PAPI_L1_DCA':  'Level 1 data cache accesses',
	'PAPI_SR_INS':  'Store instructions',
	'PAPI_TLB_DM':  'Data translation lookaside buffer misses',
	'PAPI_BR_NTK':  'Conditional branch instructions not taken',
	'PAPI_LSU_IDL': 'Cycles load/store units are idle',
	'PAPI_L1_ICA':  'Level 1 instruction cache accesses',
	'PAPI_L3_ICW':  'Level 3 instruction cache writes',
	'PAPI_L3_ICR':  'Level 3 instruction cache reads',
	'PAPI_L1_TCW':  'Level 1 total cache writes',
	'PAPI_CA_INV':  'Requests for cache line invalidation',
	'PAPI_L1_DCH':  'Level 1 data cache hits',
	'PAPI_BR_CN':   'Conditional branch instructions',
	'PAPI_L3_ICH':  'Level 3 instruction cache hits',
	'PAPI_VEC_INS': 'Vector/SIMD instructions (could include integer)',
	'PAPI_L3_ICA':  'Level 3 instruction cache accesses',
	'PAPI_TLB_TL':  'Total translation lookaside buffer misses',
	'PAPI_FUL_CCY': 'Cycles with maximum instructions completed',
	'PAPI_L2_ICW':  'Level 2 instruction cache writes',
	'PAPI_L3_ICM':  'Level 3 instruction cache misses',
	'PAPI_L2_ICR':  'Level 2 instruction cache reads',
	'PAPI_CA_ITV':  'Requests for cache line intervention',
	'PAPI_HW_INT':  'Hardware interrupts',
	'PAPI_L1_ICW':  'Level 1 instruction cache writes',
	'PAPI_L1_LDM':  'Level 1 load misses',
	'PAPI_L2_ICA':  'Level 2 instruction cache accesses',
	'PAPI_L2_DCR':  'Level 2 data cache reads',
	'PAPI_L2_ICH':  'Level 2 instruction cache hits',
	'PAPI_BRU_IDL': 'Cycles branch units are idle',
	'PAPI_L1_TCR':  'Level 1 total cache reads',
	'PAPI_L1_ICR':  'Level 1 instruction cache reads',
	'PAPI_FP_STAL': 'Cycles the FP unit(s) are stalled',
	'PAPI_VEC_DP':  'Double precision vector/SIMD instructions',
	'PAPI_FSQ_INS': 'Floating point square root instructions',
	'PAPI_L1_TCA':  'Level 1 total cache accesses',
	'PAPI_L1_TCH':  'Level 1 total cache hits',
	'PAPI_FP_OPS':  'Floating point operations',
	'PAPI_L1_TCM':  'Level 1 cache misses',
	'PAPI_L2_DCH':  'Level 2 data cache hits',
	'PAPI_DP_OPS':  'Floating point operations; optimized to count scaled double precision vector operations',
	'PAPI_TLB_SD':  'Translation lookaside buffer shootdowns',
	'PAPI_RES_STL': 'Cycles stalled on any resource',
	'PAPI_TOT_INS': 'Instructions completed',
	'PAPI_FDV_INS': 'Floating point divide instructions',
	'PAPI_SYC_INS': 'Synchronization instructions completed',
	'PAPI_L3_DCR':  'Level 3 data cache reads',
	'PAPI_L3_DCW':  'Level 3 data cache writes',
	'PAPI_L3_DCA':  'Level 3 data cache accesses',
	'PAPI_FP_INS':  'Floating point instructions',
	'PAPI_SP_OPS':  'Floating point operations; optimized to count scaled single precision vector operations',
	'PAPI_L3_DCH':  'Level 3 data cache hits',
	'PAPI_CSR_FAL': 'Failed store conditional instructions',
	'PAPI_L3_DCM':  'Level 3 data cache misses',
	'PAPI_BR_INS':  'Branch instructions',
	'PAPI_BR_TKN':  'Conditional branch instructions taken',
	'PAPI_FAD_INS': 'Floating point add instructions',
	'PAPI_TOT_CYC': 'Total cycles'
}

tests = {}

# insert record into statistics
def addRun( test, user, commit, date, oLevel, counts ):
	events = tests.setdefault( test, {} )
	
	for event, count in counts.items():
		oLevels = events.setdefault( event, {} )
		runs    = oLevels.setdefault( oLevel, [] )	

		runs.append( (date, count, user, commit) )

##### MAIN PROGRAM

# read log files
for log in argv[1:]:
	# ignore non-existent files
	if not exists( log ):
		continue
	
	# extract id from file name
	m = match( "(?:.*/)?log_([^_]*)_([^_]*)_(\w*)_([^_]*)_(-O\d)", log )

	if m:
		test, user, commit, date, oLevel = m.groups()
	else:
		print "Name of log file %s is not formatted properly!" % log
		exit(1)

	# this will hold performance counter data
	counts = {}

	# read file
	for line in open(log):
		# extract data from line, skipping malformed lines
		m = match( "\s*(\d*)\s*(PAPI_\w*).*", line )

		if m:
			count, event = m.groups()
		else:
			continue

		counts[event] = Decimal(count)

	# save statistics
	addRun( test, user, commit, date, oLevel, counts )

# print results
print '\033[1;31m', ">> A '-' IN A IMPROVEMENT COLUMN IS GOOD, A '+' IS BAD <<", '\033[0m'

for test, events in tests.items():
	print "%s:" % test

	for event, oLevels in events.items():
		print "\t%10s: %s" % (event, PAPI_EVENTS.setdefault( event, "" ))

		# print header
		print "\t\t%10s %22s %22s %21s %15s %10s %10s" % ("COUNT", "IMPROVEMENT TO NEXT", "IMPROVEMENT TO OLDEST", "TIMESTAMP", "USER", "COMMIT", "-O LEVEL")

		for oLevel, runs in oLevels.items():
			# sort runs from newest to oldest
			runs.sort( key=lambda x: datetime.strptime(x[0], DATE_FORMAT), reverse=True )

			# append dummy to test run list
			runs += [runs[-1]]

			statistics = []
	
			## calculate improvement
			for i in range(0, len(runs)-1):
				date, count,  user, commit = runs[i]
				_,    next,   _,    _      = runs[i+1]
				_,    oldest, _,    _      = runs[-1]

				improvementToNext   = (count / next   - 1) * 100
				improvementToOldest = (count / oldest - 1) * 100

				statistics += [(count, improvementToNext, improvementToOldest, date, user, commit)]


			## print final test results
			for count, improvementToNext, improvementToOldest, date, user, commit in statistics:
				# color improvement output, values smaller than 0 are green, others red.
				def color( val ):
					# round of after 2 places
					val = val.quantize( Decimal(10) ** -2 )

					if val >= 0:
						return "\033[1;31m%+.02f%%\033[0m" % val
					else:
						return "\033[1;32m%+.02f%%\033[0m" % val

				improvementToNext   = color( improvementToNext )
				improvementToOldest = color( improvementToOldest )
	
				print "\t\t%10d %33s %33s %21s %15s %10s %10s" % (count, improvementToNext, improvementToOldest, date, user, commit, oLevel)
			print
# print footer
print '\033[1;31m', ">> A '-' IN A IMPROVEMENT COLUMN IS GOOD, A '+' IS BAD <<", '\033[0m'



