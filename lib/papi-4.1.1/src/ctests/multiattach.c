/* This file performs the following test: start, stop and timer functionality for
   multiple attached processes.

   - It attempts to use the following two counters. It may use less depending on
     hardware counter resource limitations. These are counted in the default counting
     domain and default granularity, depending on the platform. Usually this is 
     the user domain (PAPI_DOM_USER) and thread context (PAPI_GRN_THR).
     + PAPI_FP_INS
     + PAPI_TOT_CYC
   - Get us.
   - Start counters
   - Do flops
   - Stop and read counters
   - Get us.
*/

#include "papi_test.h"
#include <sys/ptrace.h>
#include <inttypes.h>

#ifdef _AIX
#define _LINUX_SOURCE_COMPAT
#endif

int
wait_for_attach_and_loop( void )
{
	kill( getpid(  ), SIGSTOP );
	do_flops( NUM_FLOPS );
	kill( getpid(  ), SIGSTOP );
	return ( 0 );
}

int
main( int argc, char **argv )
{
	int status, retval, num_tests = 2, tmp;
	int EventSet1 = PAPI_NULL, EventSet2 = PAPI_NULL;
	int PAPI_event, mask1;
	int num_events1;
	long long **values;
	long long elapsed_us, elapsed_cyc, elapsed_virt_us, elapsed_virt_cyc;
	char event_name[PAPI_MAX_STR_LEN], add_event_str[PAPI_MAX_STR_LEN];
	const PAPI_hw_info_t *hw_info;
	const PAPI_component_info_t *cmpinfo;
	pid_t pid, pid2;

	pid = fork(  );
	if ( pid < 0 )
		test_fail( __FILE__, __LINE__, "fork()", PAPI_ESYS );
	if ( pid == 0 )
		exit( wait_for_attach_and_loop(  ) );
	pid2 = fork(  );
	if ( pid2 < 0 )
		test_fail( __FILE__, __LINE__, "fork()", PAPI_ESYS );
	if ( pid2 == 0 )
		exit( wait_for_attach_and_loop(  ) );

	tests_quiet( argc, argv );	/* Set TESTS_QUIET variable */

	retval = PAPI_library_init( PAPI_VER_CURRENT );
	if ( retval != PAPI_VER_CURRENT )
		test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

	if ( ( cmpinfo = PAPI_get_component_info( 0 ) ) == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_component_info", 0 );

	if ( cmpinfo->attach == 0 )
		test_skip( __FILE__, __LINE__, "Platform does not support attaching",
				   0 );

	hw_info = PAPI_get_hardware_info(  );
	if ( hw_info == NULL )
		test_fail( __FILE__, __LINE__, "PAPI_get_hardware_info", 0 );

	/* add PAPI_TOT_CYC and one of the events in PAPI_FP_INS, PAPI_FP_OPS or
	   PAPI_TOT_INS, depending on the availability of the event on the
	   platform */
	EventSet1 = add_two_events( &num_events1, &PAPI_event, hw_info, &mask1 );
	EventSet2 = add_two_events( &num_events1, &PAPI_event, hw_info, &mask1 );

	if ( cmpinfo->attach_must_ptrace ) {
		if ( ptrace( PTRACE_ATTACH, pid, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_ATTACH)" );
			return ( 1 );
		}
		if ( waitpid( pid, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 )
			test_fail( __FILE__, __LINE__,
					   "Child process didnt return true to WIFSTOPPED", 0 );

		if ( ptrace( PTRACE_ATTACH, pid2, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_ATTACH)" );
			return ( 1 );
		}
		if ( waitpid( pid2, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 )
			test_fail( __FILE__, __LINE__,
					   "Child process didnt return true to WIFSTOPPED", 0 );
	}

	retval = PAPI_attach( EventSet1, ( unsigned long ) pid );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_attach", retval );

	retval = PAPI_attach( EventSet2, ( unsigned long ) pid2 );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_attach", retval );

	retval = PAPI_event_code_to_name( PAPI_event, event_name );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_event_code_to_name", retval );
	sprintf( add_event_str, "PAPI_add_event[%s]", event_name );

	/* num_events1 is greater than num_events2 so don't worry. */

	values = allocate_test_space( num_tests, num_events1 );

	elapsed_us = PAPI_get_real_usec(  );

	elapsed_cyc = PAPI_get_real_cyc(  );

	elapsed_virt_us = PAPI_get_virt_usec(  );

	elapsed_virt_cyc = PAPI_get_virt_cyc(  );

	/* Wait for the SIGSTOP. */
	if ( cmpinfo->attach_must_ptrace ) {
		if ( ptrace( PTRACE_CONT, pid, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_CONT)" );
			return 1;
		}
		if ( waitpid( pid, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't return true to WIFSTOPPED", 0 );
		}
		if ( WSTOPSIG( status ) != SIGSTOP ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't stop on SIGSTOP", 0 );
		}

		if ( ptrace( PTRACE_CONT, pid2, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_CONT)" );
			return 1;
		}
		if ( waitpid( pid2, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't return true to WIFSTOPPED", 0 );
		}
		if ( WSTOPSIG( status ) != SIGSTOP ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't stop on SIGSTOP", 0 );
		}
	}

	retval = PAPI_start( EventSet1 );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	retval = PAPI_start( EventSet2 );
	if ( retval != PAPI_OK )
		test_fail( __FILE__, __LINE__, "PAPI_start", retval );

	/* Wait for the SIGSTOP. */
	if ( cmpinfo->attach_must_ptrace ) {
		if ( ptrace( PTRACE_CONT, pid, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_ATTACH)" );
			return ( 1 );
		}
		if ( waitpid( pid, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't return true to WIFSTOPPED", 0 );
		}
		if ( WSTOPSIG( status ) != SIGSTOP ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't stop on SIGSTOP", 0 );
		}

		if ( ptrace( PTRACE_CONT, pid2, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_ATTACH)" );
			return ( 1 );
		}
		if ( waitpid( pid2, &status, 0 ) == -1 ) {
			perror( "waitpid()" );
			exit( 1 );
		}
		if ( WIFSTOPPED( status ) == 0 ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't return true to WIFSTOPPED", 0 );
		}
		if ( WSTOPSIG( status ) != SIGSTOP ) {
			test_fail( __FILE__, __LINE__,
					   "Child process didn't stop on SIGSTOP", 0 );
		}
	}

	elapsed_virt_us = PAPI_get_virt_usec(  ) - elapsed_virt_us;

	elapsed_virt_cyc = PAPI_get_virt_cyc(  ) - elapsed_virt_cyc;

	elapsed_us = PAPI_get_real_usec(  ) - elapsed_us;

	elapsed_cyc = PAPI_get_real_cyc(  ) - elapsed_cyc;

	retval = PAPI_stop( EventSet1, values[0] );
	if ( retval != PAPI_OK )
		printf( "Warning: PAPI_stop returned error %d, probably ok.\n",
				retval );

	retval = PAPI_stop( EventSet2, values[1] );
	if ( retval != PAPI_OK )
		printf( "Warning: PAPI_stop returned error %d, probably ok.\n",
				retval );

	remove_test_events( &EventSet1, mask1 );
	remove_test_events( &EventSet2, mask1 );

	if ( cmpinfo->attach_must_ptrace ) {
		if ( ptrace( PTRACE_CONT, pid, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_CONT)" );
			return 1;
		}
		if ( ptrace( PTRACE_CONT, pid2, NULL, NULL ) == -1 ) {
			perror( "ptrace(PTRACE_CONT)" );
			return 1;
		}
	}

	if ( waitpid( pid, &status, 0 ) == -1 ) {
		perror( "waitpid()" );
		exit( 1 );
	}
	if ( WIFEXITED( status ) == 0 )
		test_fail( __FILE__, __LINE__,
				   "Child process didn't return true to WIFEXITED", 0 );

	if ( waitpid( pid2, &status, 0 ) == -1 ) {
		perror( "waitpid()" );
		exit( 1 );
	}
	if ( WIFEXITED( status ) == 0 )
		test_fail( __FILE__, __LINE__,
				   "Child process didn't return true to WIFEXITED", 0 );

	/* This code isn't necessary as we know the child has exited,
	   it *may* return an error if the substrate so chooses. You should use read() instead. */


	printf( "Test case: multiple 3rd party attach start, stop.\n" );
	printf( "-----------------------------------------------\n" );
	tmp = PAPI_get_opt( PAPI_DEFDOM, NULL );
	printf( "Default domain is: %d (%s)\n", tmp, stringify_all_domains( tmp ) );
	tmp = PAPI_get_opt( PAPI_DEFGRN, NULL );
	printf( "Default granularity is: %d (%s)\n", tmp,
			stringify_granularity( tmp ) );
	printf( "Using %d iterations of c += a*b\n", NUM_FLOPS );
	printf
		( "-------------------------------------------------------------------------\n" );

	sprintf( add_event_str, "(PID %jd) %-12s : \t", ( intmax_t ) pid,
			 event_name );
	printf( TAB1, add_event_str, ( values[0] )[0] );
	sprintf( add_event_str, "(PID %jd) PAPI_TOT_CYC : \t", ( intmax_t ) pid );
	printf( TAB1, add_event_str, ( values[0] )[1] );
	sprintf( add_event_str, "(PID %jd) %-12s : \t", ( intmax_t ) pid2,
			 event_name );
	printf( TAB1, add_event_str, ( values[1] )[0] );
	sprintf( add_event_str, "(PID %jd) PAPI_TOT_CYC : \t", ( intmax_t ) pid2 );
	printf( TAB1, add_event_str, ( values[1] )[1] );
	printf( TAB1, "Real usec    : \t", elapsed_us );
	printf( TAB1, "Real cycles  : \t", elapsed_cyc );
	printf( TAB1, "Virt usec    : \t", elapsed_virt_us );
	printf( TAB1, "Virt cycles  : \t", elapsed_virt_cyc );

	printf
		( "-------------------------------------------------------------------------\n" );

	printf( "Verification: none\n" );

	test_pass( __FILE__, values, num_tests );
	exit( 1 );
}
