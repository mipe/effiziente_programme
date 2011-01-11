#!/bin/sh

# File:    papi.c
# CVS:     $Id: run_tests.sh,v 1.37 2009-11-23 16:23:22 bsheely Exp $
# Author:  Philip Mucci
#          mucci@cs.utk.edu
# Mods:    Kevin London
#          london@cs.utk.edu

AIXTHREAD_SCOPE=S
export AIXTHREAD_SCOPE
if [ "X$1" = "X-v" ]; then
  shift ; TESTS_QUIET=""
else
# This should never have been an argument, but an environment variable!
  TESTS_QUIET="TESTS_QUIET"
  export TESTS_QUIET
fi

chmod -x ctests/*.[ch]
chmod -x ftests/*.[Fch]

# Uncomment the following line to run tests using modified version of valgrind 
# Requires this patch: http://people.redhat.com/wcohen/papi/valgrind-3.5.0-3.papi.src.rpm
#VALGRIND="valgrind --leak-check=full";
VALGRIND="";

CTESTS=`find ctests -maxdepth 1 -perm -u+x -type f`;
FTESTS=`find ftests -perm -u+x -type f`;
EXCLUDE=`grep --regexp=^# --invert-match run_tests_exclude.txt`
ALLTESTS="$CTESTS $FTESTS";
x=0;
CWD=`pwd`

echo "Platform:"
uname -a

echo ""
if ["$VALGRIND" = ""]; then
  echo "The following test cases will be run:";
else
  echo "The following test cases will be run using valgrind:";
fi
echo ""

MATCH=0
for i in $ALLTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    echo -n "$i "
  fi;
  MATCH=0
done
echo ""

echo ""
echo "The following test cases will NOT be run:";
echo $EXCLUDE;

echo "";
echo "Running C Tests";
echo ""

if [ "$LD_LIBRARY_PATH" = "" ]; then
  LD_LIBRARY_PATH=.:./libpfm-3.y/lib:./libpfm-2.x/libpfm
else
  LD_LIBRARY_PATH=.:./libpfm-3.y/lib:./libpfm-2.x/libpfm:"$LD_LIBRARY_PATH"
fi
export LD_LIBRARY_PATH
if [ "$LIBPATH" = "" ]; then
  LIBPATH=.:./libpfm-3.y/lib:./libpfm-2.x/libpfm
else
  LIBPATH=.:./libpfm-3.y/lib:./libpfm-2.x/libpfm:"$LIBPATH"
fi
export LIBPATH

for i in $CTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    if [ -x $i ]; then
      if [ "$i" = "ctests/timer_overflow" ]; then
        echo Skipping test $i, it takes too long...
      else
      echo -n "Running $i: ";
      $VALGRIND ./$i $TESTS_QUIET
      fi;
    fi;
  fi;
  MATCH=0
done

echo ""
echo "Running Fortran Tests";
echo ""

for i in $FTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    if [ -x $i ]; then
    echo -n "Running $i: ";
    $VALGRIND ./$i $TESTS_QUIET
    fi;
  fi;
  MATCH=0
done
