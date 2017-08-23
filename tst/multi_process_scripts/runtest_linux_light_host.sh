#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the XSAL Light tests on a Linux host. As designed, it:
#
#   1) Is run from the MDF's tst/bsd subdirectory and executes the test code
#      in ../../out/linux_light_host/bin.
#
#   2) Writes the test output to the MDF's tst/bsd subdirectory in a file named
#      linux_light_host_test_results.txt. These test results can then be compared
#      with the "official" results in ../verification and used to replace/update
#      them at the completion of a task.
#
#   3) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to linux_light_host_test_results.txt

RESULT=0

###############################
# Tests common with XSAL Light
###############################

echo STARTING test_alloc_free
if ! ../../out/linux_light_host/bin/test_alloc_free > linux_light_host_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool
if ! ../../out/linux_light_host/bin/test_buffer_pool >> linux_light_host_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock
if ! ../../out/linux_light_host/bin/test_clock >> linux_light_host_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue
if ! ../../out/linux_light_host/bin/test_message_queue >> linux_light_host_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex
if ! ../../out/linux_light_host/bin/test_mutex >> linux_light_host_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish
if ! ../../out/linux_light_host/bin/test_publish >> linux_light_host_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback
if ! ../../out/linux_light_host/bin/test_publish_callback >> linux_light_host_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only
if ! ../../out/linux_light_host/bin/test_receive_only >> linux_light_host_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore
if ! ../../out/linux_light_host/bin/test_semaphore >> linux_light_host_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send
if ! ../../out/linux_light_host/bin/test_send >> linux_light_host_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback
if ! ../../out/linux_light_host/bin/test_send_callback >> linux_light_host_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread
if ! ../../out/linux_light_host/bin/test_thread >> linux_light_host_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer
if ! ../../out/linux_light_host/bin/test_timer >> linux_light_host_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread
if ! ../../out/linux_light_host/bin/test_unknown_thread >> linux_light_host_test_results.txt 2>&1
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
# 27-jun-2010 kirk bailey
# + Created initial file.
#
#===========================================================================


