#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the full XSAL tests on a Linux host. As designed, it:
#
#   1) Is run from the MDF's tst/bsd subdirectory and executes the test code
#      in ../../out/linux_host/bin.
#
#   2) Writes the test output to the MDF's tst/bsd subdirectory in a file named
#      linux_host_test_results.txt. These test results can then be compared
#      with the "official" results in ../verification and used to replace/update
#      them at the completion of a task.
#
#   3) Writes the output of procman to the MDF's tst/bsd subdirectory in a file
#      named linux_host_procman.txt
#
#   4) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to linux_host_test_results.txt
echo procman trace information is written to linux_host_procman.txt

RESULT=0
echo LAUNCHING procman
../../out/linux_host/bin/procman -n > linux_host_procman.txt 2>&1

###############################
# Tests common with XSAL Light
###############################

echo STARTING test_alloc_free
if ! ../../out/linux_host/bin/test_alloc_free > linux_host_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool
if ! ../../out/linux_host/bin/test_buffer_pool >> linux_host_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock
if ! ../../out/linux_host/bin/test_clock >> linux_host_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue
if ! ../../out/linux_host/bin/test_message_queue >> linux_host_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex
if ! ../../out/linux_host/bin/test_mutex >> linux_host_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish
if ! ../../out/linux_host/bin/test_publish >> linux_host_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback
if ! ../../out/linux_host/bin/test_publish_callback >> linux_host_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only
if ! ../../out/linux_host/bin/test_receive_only >> linux_host_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore
if ! ../../out/linux_host/bin/test_semaphore >> linux_host_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send
if ! ../../out/linux_host/bin/test_send >> linux_host_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback
if ! ../../out/linux_host/bin/test_send_callback >> linux_host_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread
if ! ../../out/linux_host/bin/test_thread >> linux_host_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer
if ! ../../out/linux_host/bin/test_timer >> linux_host_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread
if ! ../../out/linux_host/bin/test_unknown_thread >> linux_host_test_results.txt 2>&1
then
   echo test_unknown_thread FAILED
   RESULT=1
fi

###########################
# Tests unique to full XSAL
###########################

echo STARTING test_cond
if ! ../../out/linux_host/bin/test_cond >> linux_host_test_results.txt 2>&1
then
   echo test_cond FAILED
   RESULT=1
fi

echo STARTING test_local_publish
if ! ../../out/linux_host/bin/test_local_publish >> linux_host_test_results.txt 2>&1
then
   echo test_local_publish FAILED
   RESULT=1
fi

echo STARTING test_local_publish_callback
if ! ../../out/linux_host/bin/test_local_publish_callback >> linux_host_test_results.txt 2>&1
then
   echo test_local_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_from
if ! ../../out/linux_host/bin/test_receive_from >> linux_host_test_results.txt 2>&1
then
   echo test_receive_from FAILED
   RESULT=1
fi

echo STARTING test_rpc
if ! ../../out/linux_host/bin/test_rpc >> linux_host_test_results.txt 2>&1
then
   echo test_rpc FAILED
   RESULT=1
fi

echo STARTING test_rwlock
if ! ../../out/linux_host/bin/test_rwlock >> linux_host_test_results.txt 2>&1
then
   echo test_rwlock FAILED
   RESULT=1
fi

echo STARTING test_shm
../../out/linux_host/bin/test_shm -p3 -N test_shm_3 >> linux_host_test_results.txt 2>&1 &
../../out/linux_host/bin/test_shm -p4 -N test_shm_4 >> linux_host_test_results.txt 2>&1 &
../../out/linux_host/bin/test_shm -p5 -N test_shm_5 >> linux_host_test_results.txt 2>&1 &
sleep 1
if ! ../../out/linux_host/bin/test_shm -p2 -N test_shm_2 >> linux_host_test_results.txt 2>&1
then
   echo test_shm failed
   RESULT=1
fi



echo KILLING procman
pkill procman

exit $RESULT

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 14-sep-2010 kirk bailey
# + Enabled semaphore now that race conditions are fixed.
#
# 16-jun-2010 kirk bailey
# + Created initial file.
#
#===========================================================================

