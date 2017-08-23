#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the full XSAL tests on a PC (Win32) host. As designed, it:
#
#   1) Is run from the MDF's tst/bsd subdirectory and executes the test code
#      in ../../out/pc/bin.
#
#   2) Writes the test output to the MDF's tst/bsd subdirectory in a file named
#      pc_test_results.txt. These test results can then be compared with the
#      "official" results in ../verification and used to replace/update them at
#      the completion of a task.
#
#   3) Writes the output of procman to the MDF's tst/bsd subdirectory in a file
#      named pc_procman.txt.
#
#   4) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to pc_test_results.txt
echo procman trace information is written to pc_procman.txt

RESULT=0
echo LAUNCHING procman.exe
../../out/pc/bin/procman.exe -n > pc_procman.txt 2>&1 &
bgpid=$!
sleep 1

###############################
# Tests common with XSAL Light
###############################

echo STARTING test_alloc_free.exe
if ! ../../out/pc/bin/test_alloc_free.exe > pc_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool.exe
if ! ../../out/pc/bin/test_buffer_pool.exe >> pc_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock.exe
if ! ../../out/pc/bin/test_clock.exe >> pc_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue.exe
if ! ../../out/pc/bin/test_message_queue.exe >> pc_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex.exe
if ! ../../out/pc/bin/test_mutex.exe >> pc_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish.exe
if ! ../../out/pc/bin/test_publish.exe >> pc_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback.exe
if ! ../../out/pc/bin/test_publish_callback.exe >> pc_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only.exe
if ! ../../out/pc/bin/test_receive_only.exe >> pc_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore.exe
if ! ../../out/pc/bin/test_semaphore.exe >> pc_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send.exe
if ! ../../out/pc/bin/test_send.exe >> pc_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback.exe
if ! ../../out/pc/bin/test_send_callback.exe >> pc_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread.exe
if ! ../../out/pc/bin/test_thread.exe >> pc_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer.exe
if ! ../../out/pc/bin/test_timer.exe >> pc_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread.exe
if ! ../../out/pc/bin/test_unknown_thread.exe >> pc_test_results.txt 2>&1
then
   echo test_unknown_thread FAILED
   RESULT=1
fi

###########################
# Tests unique to full XSAL
###########################

echo STARTING test_cond.exe
if ! ../../out/pc/bin/test_cond.exe >> pc_test_results.txt 2>&1
then
   echo test_cond FAILED
   RESULT=1
fi

echo STARTING test_local_publish.exe
if ! ../../out/pc/bin/test_local_publish.exe >> pc_test_results.txt 2>&1
then
   echo test_local_publish FAILED
   RESULT=1
fi

echo STARTING test_local_publish_callback.exe
if ! ../../out/pc/bin/test_local_publish_callback.exe >> pc_test_results.txt 2>&1
then
   echo test_local_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_from.exe
if ! ../../out/pc/bin/test_receive_from.exe >> pc_test_results.txt 2>&1
then
   echo test_receive_from FAILED
   RESULT=1
fi

echo STARTING test_rpc.exe
if ! ../../out/pc/bin/test_rpc.exe >> pc_test_results.txt 2>&1
then
   echo test_rpc FAILED
   RESULT=1
fi

echo STARTING test_rwlock.exe
if ! ../../out/pc/bin/test_rwlock.exe >> pc_test_results.txt 2>&1
then
   echo test_rwlock FAILED
   RESULT=1
fi

echo STARTING test_shm
../../out/pc/bin/test_shm -p3 -N test_shm_3 >> pc_test_results.txt 2>&1 &
../../out/pc/bin/test_shm -p4 -N test_shm_4 >> pc_test_results.txt 2>&1 &
../../out/pc/bin/test_shm -p5 -N test_shm_5 >> pc_test_results.txt 2>&1 &
sleep 1
if ! ../../out/pc/bin/test_shm -p2 -N test_shm_2 >> pc_test_results.txt 2>&1
then
   echo test_shm failed
   RESULT=1
fi



echo KILLING procman.exe
kill -9 $bgpid

exit $RESULT

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 16-jun-2010 kirk bailey
# + Cleaned up file and added header/revision blocks.
#
#===========================================================================

