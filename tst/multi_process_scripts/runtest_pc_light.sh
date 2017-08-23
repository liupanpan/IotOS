#!/bin/sh
#---------------------------------------------------------------------------
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
#
# This script runs the XSAL light tests on a PC (Win32) host. As designed, it:
#
#   1) Is run from the MDF's tst/bsd subdirectory and executes the test code
#      in ../../out/pc_light/bin.
#
#   2) Writes the test output to the MDF's tst/bsd subdirectory in a file named
#      pc_light_test_results.txt. These test results can then be
#      compared with the "official" results in ../verification and used to
#      replace/update them at the completion of a task.
#
#   3) Prints test progress information on the console along with
#      an indication of any test that FAILED.
#
#---------------------------------------------------------------------------

echo Trace information from tests is written to pc_light_test_results.txt

RESULT=0

echo STARTING test_alloc_free.exe
if ! ../../out/pc_light/bin/test_alloc_free.exe > pc_light_test_results.txt 2>&1
then
   echo test_alloc_free FAILED
   RESULT=1
fi

echo STARTING test_buffer_pool.exe
if ! ../../out/pc_light/bin/test_buffer_pool.exe >> pc_light_test_results.txt 2>&1
then
   echo test_buffer_pool FAILED
   RESULT=1
fi

echo STARTING test_clock.exe
if ! ../../out/pc_light/bin/test_clock.exe >> pc_light_test_results.txt 2>&1
then
   echo test_clock FAILED
   RESULT=1
fi

echo STARTING test_message_queue.exe
if ! ../../out/pc_light/bin/test_message_queue.exe >> pc_light_test_results.txt 2>&1
then
   echo test_message_queue FAILED
   RESULT=1
fi

echo STARTING test_mutex.exe
if ! ../../out/pc_light/bin/test_mutex.exe >> pc_light_test_results.txt 2>&1
then
   echo test_mutex FAILED
   RESULT=1
fi

echo STARTING test_publish.exe
if ! ../../out/pc_light/bin/test_publish.exe >> pc_light_test_results.txt 2>&1
then
   echo test_publish FAILED
   RESULT=1
fi

echo STARTING test_publish_callback.exe
if ! ../../out/pc_light/bin/test_publish_callback.exe >> pc_light_test_results.txt 2>&1
then
   echo test_publish_callback FAILED
   RESULT=1
fi

echo STARTING test_receive_only.exe
if ! ../../out/pc_light/bin/test_receive_only.exe >> pc_light_test_results.txt 2>&1
then
   echo test_receive_only FAILED
   RESULT=1
fi

echo STARTING test_semaphore.exe
if ! ../../out/pc_light/bin/test_semaphore.exe >> pc_light_test_results.txt 2>&1
then
   echo test_semaphore FAILED
   RESULT=1
fi

echo STARTING test_send.exe
if ! ../../out/pc_light/bin/test_send.exe >> pc_light_test_results.txt 2>&1
then
   echo test_send FAILED
   RESULT=1
fi

echo STARTING test_send_callback.exe
if ! ../../out/pc_light/bin/test_send_callback.exe >> pc_light_test_results.txt 2>&1
then
   echo test_send_callback FAILED
   RESULT=1
fi

echo STARTING test_thread.exe
if ! ../../out/pc_light/bin/test_thread.exe >> pc_light_test_results.txt 2>&1
then
   echo test_thread FAILED
   RESULT=1
fi

echo STARTING test_timer.exe
if ! ../../out/pc_light/bin/test_timer.exe >> pc_light_test_results.txt 2>&1
then
   echo test_timer FAILED
   RESULT=1
fi

echo STARTING test_unknown_thread.exe
if ! ../../out/pc_light/bin/test_unknown_thread.exe >> pc_light_test_results.txt 2>&1
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
# 15-Sep-2010 kirk bailey
# + Renamed pc_cunit_bullseye to pc_light.
#
# 16-jun-2010 kirk bailey
# + Cleaned up file and added header/revision blocks.
#
#===========================================================================
