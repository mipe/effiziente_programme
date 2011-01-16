#include "perf.h"

#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

// MACROS
#define BUF_SIZE 512

#define _ASSERT( TEST, MSG, ... ) ({                                     \
	if ( !(TEST) ) {                                                     \
		fprintf( stderr, #TEST " failed: " MSG " \n", ## __VA_ARGS__ );  \
		abort();                                                         \
	}                                                                    \
})

#define _CALL_PAPI( CALL ) ({                                            \
	int err = (CALL);                                                    \
	if ( err < PAPI_OK ) {                                               \
		fprintf( stderr, #CALL " failed: %s\n", PAPI_strerror( err ) );  \
		abort();                                                         \
	}                                                                    \
	err;                                                                 \
})

#define _NUM_EVENTS sizeof( _EVENTS ) / sizeof( int )

// GLOBALS
static int       _EVENTS[]   = { PAPI_TOT_INS,
                                 PAPI_TOT_CYC,	
                                 PAPI_L2_TCM,
                                 PAPI_BR_MSP };
static long long _COUNTERS[_NUM_EVENTS];
static FILE*     _LOG_FILE   = NULL;

// FUNCTIONS
void startPerformanceCounters( char* name ) {
	// if no name was specified, do nothing
	if ( !name ) return;	

	//***** OPEN LOG FILE ***********//

 	_LOG_FILE = fopen( name, "w" );
	_ASSERT( _LOG_FILE != NULL, "Could not create log file: %s", name );	

	//***** INIT PAPI ***************//

	_ASSERT( PAPI_library_init( PAPI_VER_CURRENT ) == PAPI_VER_CURRENT, "Could not init papi." );	 

	// check if all requested events are supported on this machine
	for ( int i = 0; i < _NUM_EVENTS; i++ ) {
		char name[PAPI_MAX_STR_LEN];
		(void) _CALL_PAPI( PAPI_event_code_to_name( _EVENTS[i], name ) );

		_ASSERT( PAPI_query_event( _EVENTS[i] ) == PAPI_OK, "Event %s cannot be counted on this machine.", name );
	}

	// check if this machine can count all requested events at once
	int numCounters = _CALL_PAPI( PAPI_num_counters() );
	_ASSERT( _NUM_EVENTS <= numCounters, "Hardware can't count %ld events.", _NUM_EVENTS );

	//***** START COUNTERS **********//

	(void) _CALL_PAPI( PAPI_start_counters( _EVENTS, _NUM_EVENTS ) );
}
void stopPerformanceCounters() {
	// if log file wasn't opened, do nothing
	if ( !_LOG_FILE ) return;

	//***** STOP COUNTERS ***********//

	(void) _CALL_PAPI( PAPI_stop_counters( _COUNTERS, _NUM_EVENTS ) );	

	//***** PRINT RESULTS ***********//

	// print all counted events
	for ( int i = 0; i < _NUM_EVENTS; i++ ) {
		// get event info
		PAPI_event_info_t info;
		(void) _CALL_PAPI( PAPI_get_event_info( _EVENTS[i], &info ) );

		fprintf( _LOG_FILE, "%20lld %-15s %s\n", _COUNTERS[i], info.symbol, info.long_descr );
	}

	// close log file
	fclose( _LOG_FILE );
}

void printAllEvents() {
	// init PAPI
	_ASSERT( PAPI_library_init( PAPI_VER_CURRENT ) == PAPI_VER_CURRENT, "Could not init papi." );

	// print header
	printf( "--- Available events ---\n" );
	printf( "%-15s %s\n", "Name", "Description" );

	// loop through all PAPI events supported on this plattform
	int event = 0 | PAPI_PRESET_MASK; // the first event
	do {
		// get event info
		PAPI_event_info_t info;
		(void) _CALL_PAPI( PAPI_get_event_info( event, &info ) );

		// print
		printf( "%-15s %s\n", info.symbol, info.long_descr );
	} while ( PAPI_enum_event( &event, PAPI_ENUM_ALL ) == PAPI_OK );
}

void printAvailableEvents() {
	// init PAPI
	_ASSERT( PAPI_library_init( PAPI_VER_CURRENT ) == PAPI_VER_CURRENT, "Could not init papi." );

	// print header
	printf( "--- Available events ---\n" );
	printf( "%-15s %s\n", "Name", "Description" );

	// loop through all PAPI events supported on this plattform
	int event = 0 | PAPI_PRESET_MASK; // the first event
	do {
		// ignore unsupported events
		if ( PAPI_query_event( event ) != PAPI_OK ) continue; 

		// get event info
		PAPI_event_info_t info;
		(void) _CALL_PAPI( PAPI_get_event_info( event, &info ) );

		// print
		printf( "%-15s %s\n", info.symbol, info.long_descr );
	} while ( PAPI_enum_event( &event, PAPI_ENUM_ALL ) == PAPI_OK );
}

