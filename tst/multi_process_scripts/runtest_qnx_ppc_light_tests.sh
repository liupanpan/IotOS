#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the XSAL light tests on a QNX target. As designed, it:
#
#   1) Must be in a directory on the target (usually /usr/bin) that is in
#      $PATH and must have execute permissions.
#
#   2) Writes the test output to the /tmp/qnx_ppc_light_test_results.txt.
#      These test results can then be compared with the "official" results in
#      the MDF's tst/verification subdirectory and used to replace/update them
#      at the completion of a task.
#
#   3) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to /tmp/qnx_ppc_light_test_results.txt

RESULT=0

###############################
# Tests common with XSAL Light
###############################

echo STARTING test_alloc_free
if ! test_alloc_free > /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool
if ! test_buffer_pool >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock
if ! test_clock >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue
if ! test_message_queue >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex
if ! test_mutex >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish
if ! test_publish >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback
if ! test_publish_callback >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only
if ! test_receive_only >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore
if ! test_semaphore >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send
if ! test_send >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback
if ! test_send_callback >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread
if ! test_thread >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer
if ! test_timer >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread
if ! test_unknown_thread >> /tmp/qnx_ppc_light_test_results.txt 2>&1
then
   echo test_unknown_thread FAILED
   RESULT=1
fi

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
