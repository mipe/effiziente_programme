#ifndef PERF_H
#define PERF_H 1

void startPerformanceCounters( int argc, char** argv );
void stopPerformanceCounters();

void printAvailableEvents();

#endif /* PERF_H */
