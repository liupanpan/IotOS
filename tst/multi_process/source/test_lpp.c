#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xsal.h"
#include "xsal_thread_priority.h"
#include "pi_unistd.h"


void Thread1(void* param) 
{  
	SAL_Send(8, 1, SEND_TO_YL, NULL, 0);
	printf("send app_8 thread_1 event is SEND_TO_YL>>>>>>>>>>\n");
}

void Thread2(void* param) 
{
	printf("<<<<<<<<<\n");
}

int main(int argc, char* argv[])
{
   SAL_Config_T sal_config;
   SAL_Thread_Attr_T attr;    

   /* Retrieve the default configuration parameters. */
   SAL_Get_Config(&sal_config);
   
   /* Define application ID and max number of threads. */
   sal_config.app_id = 7;
   sal_config.max_number_of_threads = 10;
   
   /* Modify some other values. */
   sal_config.first_unknown_thread_id = 5;
   
   /* Initialize XSAL. */
   SAL_Init(&sal_config);

   /* Initialize the system, create threads, etc. */
   attr.name = "Thread1";
   attr.id = 1;
   attr.priority = 2;
   SAL_Create_Thread(Thread1, NULL, &attr);

   attr.name = "Thread2";
   attr.id = 2;
   attr.priority = 5; 
   SAL_Create_Thread(Thread2, NULL, &attr);

   return SAL_Run(); 
}

