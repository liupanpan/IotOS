/*===========================================================================*/
/**
 * @file process_observer.c
 *
 * Process Observation Library (POL) implementation. This implementation
 * supports the process obseravtion only for QNX RTOS/targets.
 *
 * %full_filespec:process_observer.c~3:csrc:kok_basa#1 %
 * @version %version:3 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Fri Jan  8 13:07:16 2016 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 * The Process (crash) Observation implementation was originally in IRC module
 * and later seperated and moved to here and built as a lib for Process manager.
 *
 *  The Process Observer thread gets notified whenever a process
 *  dies due to any of the following signals:
 *
 *  SIGABRT
 *  SIGBUS
 *  SIGEMT
 *  SIGFPE
 *  SIGILL
 *  SIGQUIT
 *  SIGSEGV
 *  SIGSYS
 *  SIGTRAP
 *  SIGXCPU
 *  SIGXFSZ
 *
 *  To do so, it registers the path, /proc/dumper with type
 *  _FTYPE_DUMPER. When a process dies due to one of the above
 *  signals, the process manager will open /proc/dumper, and
 *  write the pid, name, tid, signal of the process that died -
 *  it will wait until for reply to the write with success, and
 *  then it will finish terminating the process.
 *
 *  Note that while it is possible for more than one process to
 *  have /proc/dumper registered at the same time, the process
 *  manager will notify only the one that is at the beginning of
 *  its list for that name.
 *
 *  But we want both us and dumper to handle this termination.
 *  To do this, we make sure that we get notified instead of
 *  dumper by asking the process manager to put us at the
 *  beginning of its list for /proc/dumper (done by passing
 *  _RESMGR_FLAG_BEFORE to  resmgr_attach()).  We also open
 *  /proc/dumper so that we can communicate with dumper if it is
 *  running.  Whenever our io_write handler is called, we write
 *  the pid to dumper and do our own handling.  Of course, this
 *  works only if dumper is run before we are, or else our open
 *  will not work.
 *
 * ABBREVIATIONS:
 *   - None
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifdef QNX_NTO
#include <unistd.h>
#include <libgen.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/procfs.h>
#include <sys/param.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include "xsal.h"
#include "pm_events.h"
#include "pm_process.h"
#include "process_observer.h"
#include "pbc_trace.h"

EM_FILENUM(PROCMAN_MODULE_ID, 6);

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

extern PM_Process_T* Find_Process_By_Pid(int pid);
/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/
#ifdef QNX_NTO

static int dumper_fd;
resmgr_connect_funcs_t connect_funcs;
resmgr_io_funcs_t io_funcs;
dispatch_t *dpp;
resmgr_attr_t rattr;
dispatch_context_t *ctp;
iofunc_attr_t ioattr;

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

static void process_observer_thread();

static const SAL_Thread_Attr_T* init_thread_attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T priority,
   SAL_Thread_Attr_T * attr);

static void start_process_observer_thread();

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/
void POL_Start_Process_Observer(void)
{
   start_process_observer_thread();
}

static int display_process_info(pid_t pid)
{
   char buf[PATH_MAX + 1];
   char dinfo_buf[PATH_MAX + 1];
   int fd, status;
   struct _procfs_debug_info *dinfo = (struct _procfs_debug_info *) dinfo_buf;
   debug_process_t proc;
   unsigned int i;
   procfs_status pinfo;
   PM_Process_T *process;
   POL_Proc_Termination_Msg proc_term_msg;

   (void)sprintf(buf, "/proc/%d/as", pid);
   if ((fd = open(buf, O_RDONLY | O_NONBLOCK)) == -1)
   {
      Tr_Fault_1("Process Observer: open(%s) FAILED!!\n", buf);
      return errno;
   }

   status = devctl(fd, DCMD_PROC_STATUS, &pinfo, sizeof(pinfo), NULL);
   if (status != EOK)
   {
      Tr_Fault("Process Observer: devctl(fd, DCMD_PROC_STATUS,..) FAILED!!\n");
      close(fd);
      return status;
   }

   /* Just fill the debug info buff with NULL, before passig to devctl */
   memset(dinfo_buf, '\0', sizeof(dinfo_buf));

   status = devctl(fd, DCMD_PROC_MAPDEBUG_BASE, dinfo, sizeof(dinfo_buf), NULL);
   if (status != EOK)
   {
      Tr_Fault("Process Observer: devctl(fd, DCMD_PROC_MAPDEBUG_BASE,..) FAILED!!\n");
      close(fd);
      return status;
   }

   status = devctl(fd, DCMD_PROC_INFO, &proc, sizeof(proc), NULL);
   if (status != EOK)
   {
      close(fd);
      return status;
   }

   /* Make the SAL message and Publish the App Termination event.
    * Sys Mgr is the current user for it.
    */
   strncpy(proc_term_msg.pathbuffer, basename(dinfo->path), PATH_MAX);
   proc_term_msg.pathbuffer[PATH_MAX] = '\0';
   proc_term_msg.pid = pid;
   proc_term_msg.tid = pinfo.tid;
   proc_term_msg.signo = pinfo.info.si_signo;
   proc_term_msg.fltip = (uint32_t)(pinfo.info.__data.__fault.__fltip);

   process = Find_Process_By_Pid(pid);
   if (NULL != process)
   {
      proc_term_msg.appid = process->app_id;
   }
   else
   {
      proc_term_msg.appid = 0;
   }

   for (i = 1; i <= proc.num_threads; i++)
   {
      pinfo.tid = i;
      status = devctl(fd, DCMD_PROC_TIDSTATUS, &pinfo, sizeof(pinfo), NULL);
      if ((pinfo.state == STATE_DEAD) || (pinfo.state == STATE_STOPPED))
      {
         Tr_Fault_4("process %s ((%d) has %d threads and problem in thread %d ", basename(dinfo->path), pid, proc.num_threads, pinfo.tid);

         //Update ThreadID-Parameter in Termination-Message-Structure
         proc_term_msg.tid = pinfo.tid;

         switch (pinfo.why)
         {
            case _DEBUG_WHY_SIGNALLED:
            {
               Tr_Fault_1("died on SIGNAL %d \n", pinfo.what);
               break;
            }
            case _DEBUG_WHY_FAULTED:
            {
               Tr_Fault_1("faulted on  %d \n", pinfo.what);
               break;
            }
            case _DEBUG_WHY_TERMINATED:
            {
               Tr_Fault_1("terminated with exit code  %d \n", pinfo.what);
               break;
            }
            case _DEBUG_WHY_REQUESTED:
            {
               Tr_Fault("terminated with  _DEBUG_WHY_REQUESTED\n");
               break;
            }
            case _DEBUG_WHY_JOBCONTROL:
            {
               Tr_Fault("terminated with _DEBUG_WHY_JOBCONTROL\n");
               break;
            }
            default:
            {
               Tr_Fault_1("died with %d \n", pinfo.why);
               break;
            }
         }
      }
   }

   SAL_Publish(EV_APP_TERMINATED, &proc_term_msg, sizeof(proc_term_msg));
   /*
    Wrn("================================================");
    Wrn("Process Name: %s \n", dinfo.pathbuffer);
    Wrn("Process ID: %d \n", pid);
    Wrn("Signal Num: %d \n", pinfo.info.si_signo);
    Wrn("================================================\n");
    */
   close(fd);
   return EOK;
}

static int io_write(resmgr_context_t *pctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
   char *pstr;
   int status;

   /*  printf("\nProcess Observer Thread: io_write()\n"); */

   if ((status = iofunc_write_verify(pctp, msg, ocb, NULL)) != EOK)
   return status;

   if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
   return ENOSYS;

   if (pctp->msg_max_size < (unsigned int) msg->i.nbytes + 1)
   return ENOSPC; /* not all the message could fit in the
    message buffer */

   pstr = (char *) (&msg->i) + sizeof(msg->i);
   pstr[msg->i.nbytes] = '\0';

   if (dumper_fd != -1)
   {
      /* pass it on to dumper so it can handle it too */
      if (write(dumper_fd, pstr, strlen(pstr)) == -1)
      {
         close(dumper_fd);
         dumper_fd = -1; /* something wrong, no sense in
          doing it again later */
      }
      else
      {
         Tr_Fault("Core dump written\n");
      }
   }
   else
   {
      Tr_Warn("Termination info received by Process Observer. No other dumpers");
   }

   if ((status = display_process_info(atoi(pstr))) == -1)
   return status;

   _IO_SET_WRITE_NBYTES(pctp, msg->i.nbytes);

   return EOK;
}

static void process_observer_thread()
{
   static int wait_list[] =
   {  PM_MAIN_THREAD};

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(int));
   Tr_Warn("Process Observer Thread Started!!\n");

   /* find dumper so that we can pass any pids on to it */
   dumper_fd = open("/proc/dumper", O_WRONLY);

   dpp = dispatch_create();

   memset(&rattr, 0, sizeof(rattr));
   rattr.msg_max_size = 2048;

   iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
   io_funcs.write = io_write;

   iofunc_attr_init(&ioattr, S_IFNAM | 0600, NULL, NULL);

   if (-1 == resmgr_attach(dpp, &rattr, "/proc/dumper", _FTYPE_DUMPER, _RESMGR_FLAG_BEFORE, &connect_funcs, &io_funcs, &ioattr))
   {
      Tr_Fault("Process Observer: Resource Manager attach FAILED!!\n");
   }

   ctp = dispatch_context_alloc(dpp);

   while (1)
   {
      if ((ctp = dispatch_block(ctp)) == NULL)
      {
         Tr_Fault_1("Process Observer: Prodispatch_block FAILED: %s", strerror(errno));
      }
      dispatch_handler(ctp);
   }
}

static const SAL_Thread_Attr_T* init_thread_attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T priority,
   SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
//   attr->priority = priority; Just disabled priority init. Use the default.

   return attr;
}

static void start_process_observer_thread()
{
   SAL_Thread_Attr_T attr;
   SAL_Create_Thread(process_observer_thread, NULL, init_thread_attr("POL", PM_POL_THREAD, 1, &attr));
}

#endif
/*===========================================================================*/
/*!
 * @file process_observer.c
 *
 * REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 08-Oct-2009 Prince-TCI
 *   - Created initial file.
 *
 * - 27-May-2015 Kirk Bailey
 *   - Filled in unresolved comments.
 *
 * - 08-Jan-2016 Kirk Bailey
 *   - Fixed CHECKED_RETURN Coverity issue.
 */
/*===========================================================================*/
