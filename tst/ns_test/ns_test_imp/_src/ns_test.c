/*===========================================================================*/
/**
 * @file ns_test.c
 *
 * @todo Add a one line description of the implementation.
 *
 * %full_filespec:ns_test.c~kok_inf#3:csrc:kok_basa#1 %
 * @version %version:kok_inf#3 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Sun Aug 31 13:10:26 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @todo Add full description here
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <unistd.h>

#if defined(WIN32)
#   include <windows.h>
#   include <limits.h>
#endif /* WIN32 */


/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#if defined(WIN32)
#   define IPC_SHM_PATH "XSAL_Shm_0_0"
#else
#   define IPC_SHM_PATH "/XSAL_Shm_0_0"
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

typedef struct IPC_Channel_Tag
{
#if defined(WIN32)
   int    tcp_port;
   SOCKET socket_fd;
#else
   pid_t app_pid;
   int socket_d;
   uid_t user_id;
#endif
}
IPC_Channel_T;

typedef struct IPC_NS_Data_Tag
{
   IPC_Channel_T name_server_channel;
   size_t max_number_of_channels;
   size_t app_number;
   size_t event_number;
}
IPC_NS_Data_T;

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/
#if !defined(WIN32)
  static int shm_fd;
#else
  static HANDLE handle;
#endif

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

static IPC_NS_Data_T *Open_IPC_Shmem(void)
{
   IPC_NS_Data_T *ipc_data;
#if !defined(WIN32)
   shm_fd = shm_open(IPC_SHM_PATH, O_RDONLY, 0777);

   if (shm_fd < 0)
   {
      perror("shm_open of "IPC_SHM_PATH);
      exit(-1);
   }

   ipc_data = mmap(NULL, sizeof(*ipc_data), PROT_READ, MAP_SHARED, shm_fd, 0);

   if (MAP_FAILED == ipc_data)
   {
      close(shm_fd);
      perror("mmap");
      exit(-1);
   }
#else
   handle = 
      CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0,
                        (DWORD)sizeof(*ipc_data), IPC_SHM_PATH);
 
   if (handle != NULL)
   {
      ipc_data = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, 0);
    
      if (NULL == ipc_data)
      { 
         printf("MapViewOfFile failed. name=%s, %d\n", IPC_SHM_PATH, (int)GetLastError());
         CloseHandle(handle);
         exit(-1);
      }
   }
   else
   { 
      printf("CreateFileMapping failed. name=%s, %d\n", IPC_SHM_PATH, (int)GetLastError());
      exit(-1);
   }
#endif

   return ipc_data;
}

int main(void)
{
   IPC_NS_Data_T *ipc_data;

   ipc_data = Open_IPC_Shmem();

   printf("Max channels: %d; Max App Id: %d; Max Event Id: %d\n",
          (int)ipc_data->max_number_of_channels,
          (int)ipc_data->app_number,
          (int)ipc_data->event_number);
#if defined(WIN32)
   printf("TCP Port: %d; SOCK_FD: %d\n",
          (int)ipc_data->name_server_channel.tcp_port,
          (int)ipc_data->name_server_channel.socket_fd); 
   CloseHandle(handle);
#else
   printf("PID: %d; SOCK_FD: %d; USER: %d\n",
          (int)ipc_data->name_server_channel.app_pid,
          (int)ipc_data->name_server_channel.socket_d,
          (int)ipc_data->name_server_channel.user_id);
   close(shm_fd);
#endif

   return 0;
}


/*===========================================================================*/
/*!
 * @file ns_test.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 26-jan-2011 Kirk Bailey
 *   - Created initial file.
 *
 * - 31-Aug-2014 Kirk Bailey Rev 3
 *   - Fixed Klocwork defect.
 */
/*===========================================================================*/
