#ifndef PERF_H
#define PERF_H 1

void startPerformanceCounters( char* name );
void stopPerformanceCounters();

void printAllEvents();
void printAvailableEvents();

#endif /* PERF_H */
