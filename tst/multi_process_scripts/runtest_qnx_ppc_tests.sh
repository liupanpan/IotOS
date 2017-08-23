#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the full XSAL tests on a QNX target. As designed, it:
#
#   1) Must be in a directory on the target (usually /usr/bin) that is in
#      $PATH and must have execute permissions.
#
#   2) Writes the test output to the /tmp/qnx_ppc_test_results.txt.
#      These test results can then be compared with the "official" results in
#      the MDF's tst/verification subdirectory and used to replace/update them
#      at the completion of a task.
#
#   3) Writes the output of procman to /tmp/qnx_ppc_procman.txt.
#
#   4) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to /tmp/qnx_ppc_test_results.txt
echo procman trace information is written to /tmp/qnx_ppc_procman.txt.txt

RESULT=0
echo LAUNCHING procman
procman -n > /tmp/qnx_ppc_procman.txt 2>&1 &
sleep 1

###############################
# Tests common with XSAL Light
###############################

echo STARTING test_alloc_free
if ! test_alloc_free > /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool
if ! test_buffer_pool >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock
if ! test_clock >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue
if ! test_message_queue >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex
if ! test_mutex >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish
if ! test_publish >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback
if ! test_publish_callback >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only
if ! test_receive_only >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore
if ! test_semaphore >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send
if ! test_send >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback
if ! test_send_callback >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread
if ! test_thread >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer
if ! test_timer >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread
if ! test_unknown_thread >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_unknown_thread FAILED
   RESULT=1
fi

###########################
# Tests unique to full XSAL
###########################

echo STARTING test_cond
if ! test_cond >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_cond FAILED
   RESULT=1
fi

echo STARTING test_local_publish
if ! test_local_publish >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_local_publish FAILED
   RESULT=1
fi

echo STARTING test_local_publish_callback
if ! test_local_publish_callback >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_local_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_from
if ! test_receive_from >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_receive_from FAILED
   RESULT=1
fi

echo STARTING test_rpc
if ! test_rpc >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_rpc FAILED
   RESULT=1
fi

echo STARTING test_rwlock
if ! test_rwlock >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_rwlock FAILED
   RESULT=1
fi

echo STARTING test_shm
test_shm -p3 -N test_shm_3 >> /tmp/qnx_ppc_test_results.txt 2>&1 &
test_shm -p4 -N test_shm_4 >> /tmp/qnx_ppc_test_results.txt 2>&1 &
test_shm -p5 -N test_shm_5 >> /tmp/qnx_ppc_test_results.txt 2>&1 &
sleep 1
if ! test_shm -p2 -N test_shm_2 >> /tmp/qnx_ppc_test_results.txt 2>&1
then
   echo test_shm failed
   RESULT=1
fi

echo KILLING procman
slay procman

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

