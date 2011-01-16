#!/bin/bash

### CHECK IF FILE HAS BEEN COMPILED
if [ ! -e ../shortest-path ]
then
	echo -e '\E[1;31m'"\033[1m>>>>>> You do know that the program hasn't been compiled yet, do you? <<<<<<\033[0m"
	echo -e '\E[1;31m'"\033[1m>>>>>> Please run either 'make all', 'make debug' or 'make gprof'     <<<<<<\033[0m"
	exit 1 
fi

# id of git commit
sha=`git rev-parse HEAD`
sha=${sha:0:7}

# current time, for timestamp
now=`date +"%d.%m.%Y-%H:%M:%S"`

# name of user who ran test
user=`whoami`

# little helper function
function runTest {
	test=$1
	oLevel=$2
	id="$test"_"$user"_"$sha"_"$now"_"$oLevel"

	# print status
	printf "> %-15s with %-30s" $test $oLevel >&2

	# run test
	../shortest-path$oLevel "logs/log_""$id" <"$test"
	# store return code
	retCode=$?

	# rename gmon.out file if present
	if [ -f gmon.out ]; then mv gmon.out `date +"logs/gmon.out_$id"`; fi

	# print if test passed or failed
	if [ $retCode -eq 0 ]
	then
		echo -e '\033[0;32m' "PASSED" '\033[0m' >&2
	else
		echo -e '\033[1;31m' "FAILED" '\033[0m' >&2
	fi

	return $retCode
}

## RUN ACTUAL TESTS
# -O0
runTest "input-ref"   "-O0" $user $sha $now | diff -u - output-ref   >diff-ref-O0
runTest "input-bench" "-O0" $user $sha $now | diff -u - output-bench >diff-bench-O0
# -O3
runTest "input-ref"   "-O3" $user $sha $now | diff -u - output-ref   >diff-ref-O3
runTest "input-bench" "-O3" $user $sha $now | diff -u - output-bench >diff-bench-O3

# diffs should be empty
# -O0
if [ -s diff-ref-O0   ]; then echo -e '\E[1;31m' "\033[1m OUTPUT FROM input-ref   with -O0 differs from ertls!" '\033[0m'; exit 1; fi
if [ -s diff-bench-O0 ]; then echo -e '\E[1;31m' "\033[1m OUTPUT FROM input-bench with -O0 differs from ertls!" '\033[0m'; exit 1; fi
# -O3
if [ -s diff-ref-O3   ]; then echo -e '\E[1;31m' "\033[1m OUTPUT FROM input-ref   with -O3 differs from ertls!" '\033[0m'; exit 1; fi
if [ -s diff-bench-O3 ]; then echo -e '\E[1;31m' "\033[1m OUTPUT FROM input-bench with -O3 differs from ertls!" '\033[0m'; exit 1; fi

# cleanup
rm -f diff-ref* diff-bench*

