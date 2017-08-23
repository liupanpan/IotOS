/* -------------------------------------------------------------------------
 *
 * File:        test_buffer_pools.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of buffer pools.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>

#include "pbc_trace.h"
#include "xsal.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(TEST_MODULE_ID, 0);

#define APP_ID 2
#define MAX_NUMB_OF_BUFFERS 100

typedef enum Thread_Id_Tag
{
   BUFFER_POOL = PM_LAST_THREAD_ID,
   CREATE_BUFFER_POOL,
   READ_BUFFER_POOL,
   MAX_NUMB_OF_THREADS
} Thread_Id_T;

typedef struct Test_Case_Tag
{
   size_t numb_of_buffers;
   size_t buffer_size;
} Test_Case_T;

void* Buffer_Id[MAX_NUMB_OF_BUFFERS + 1];

Test_Case_T Test[] = 
{
   { 10, 1 },
   { 11, 2 },
   { 12, 3 },
   { 13, 4 },
   { 14, 5 },
   { 15, 6 },
   { 16, 7 },
   { 17, 8 },
   { 18, 9 },

   { 30, 1*1024 },
   { 50, 2*1024 },
   { 70, 4*1024 },
   { 90, 8*1024 }
};

SAL_Buffer_Pool_Id_T Buffer_Pool_Id_1;
SAL_Buffer_Pool_Id_T Buffer_Pool_Id_2;

SAL_Int_T Timeout = 3000;

/**  \file
 *
 *   Test of creation, allocation, writing and reading buffer pools.
 *
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref TL in one thread. </LI>
 *   <LI> \ref TITT. </LI>
 *   </OL>
 *
 *   \section TP1 Test procedure:
 *
 *   <OL>
 *   <LI> Create buffer pool of numb_of_buffers buffers, each buffer 
 *        has buffer_size size. </LI>
 *   <LI> Allocate all buffers. </LI>
 *   <LI> Try to allocate buffer when all buffers are allocated. </LI>
 *   <LI> Write data to the buffers. </LI>
 *   <LI> Read data from buffers. </LI>
 *   <LI> Free buffers. </LI>
 *   <LI> Allocate buffers for the second time. </LI>
 *   <LI> Try to allocate buffer when all buffers are allocated. </LI>
 *   <LI> Free buffers. </LI>
 *   <LI> Destroy buffers (<B><I>only in XSAL</I></B>). </LI>
 *   </OL>
 *
 *   \section TL Tested Locally
 *
 *   There is one thread BUFFER_POOL.
 *
 *   \section TITT Tested In Two Threads
 *
 *   Two threads are created: CREATE_BUFFER_POOL and READ_BUFFER_POOL. \n
 *   Thread CREATE_BUFFER_POOL creates buffer pool, allocates buffers 
 *   and writes to them. Then READ_BUFFER_POOL reads from the buffers. 
 *   After all CREATE_BUFFER_POOL frees the buffers, allocates them 
 *   for the second time and frees them. \n
 *   CREATE_BUFFER_POOL destroys buffers (<B><I>only in XSAL</I></B>).
 *
 *
 *   \section TC Test cases:
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Number </TD><TD> numb_of_buffers </TD><TD> buffer_size </CENTER></B></TD></TR>
 *   <TR><TD> 1 </TD><TD> 10 </TD><TD> 1 </TD></TR>
 *   <TR><TD> 2 </TD><TD> 11 </TD><TD> 2 </TD></TR>
 *   <TR><TD> 3 </TD><TD> 12 </TD><TD> 3 </TD></TR>
 *   <TR><TD> 4 </TD><TD> 13 </TD><TD> 4 </TD></TR>
 *   <TR><TD> 5 </TD><TD> 14 </TD><TD> 5 </TD></TR>
 *   <TR><TD> 6 </TD><TD> 15 </TD><TD> 6 </TD></TR>
 *   <TR><TD> 7 </TD><TD> 16 </TD><TD> 7 </TD></TR>
 *   <TR><TD> 8 </TD><TD> 17 </TD><TD> 8 </TD></TR>
 *   <TR><TD> 9 </TD><TD> 18 </TD><TD> 9 </TD></TR>
 *   <TR><TD> 10 </TD><TD> 30 </TD><TD> 1*1024 </TD></TR>
 *   <TR><TD> 11 </TD><TD> 50 </TD><TD> 2*1024 </TD></TR>
 *   <TR><TD> 12 </TD><TD> 70 </TD><TD> 4*1024 </TD></TR>
 *   <TR><TD> 13 </TD><TD> 90 </TD><TD> 8*1024 </TD></TR>
 *   </TABLE>
 *
 */


const SAL_Thread_Attr_T* Init_Thread_Attr(
         const char* name, 
         SAL_Thread_Id_T id, 
         SAL_Priority_T prior, 
         SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}


/** \name Local Test Functions
 *
 * \{
 */

/**  Function testing buffer pools in one thread.
 *
 *   - creates buffer pool
 *   - allocates, writes, reads and frees buffers
 *   - destroys buffer pool <B><I>only in XSAL</I></B>
 */

void Buffer_Pool_Fun(void* param)
{
   void* buffer_id[MAX_NUMB_OF_BUFFERS + 1];
   size_t i, j;
   Test_Case_T* par = (Test_Case_T*)param;
   uint8_t* addr;

   Tr_Info_Lo("Buffer_Pool_Fun: Start");
   
   if (!SAL_Create_Buffer_Pool(par->numb_of_buffers, par->buffer_size, &Buffer_Pool_Id_1))
   {
      PBC_Failed("SAL_Create_Buffer_Pool: Buffer Pool has not been created");
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) == NULL)
      {
         PBC_Failed_1("SAL_Alloc_Buffer: Buffer %d has not been allocated", (int)(i+1));
      }
   }

   if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) != NULL)
   {
      PBC_Failed("SAL_Alloc_Buffer: Buffer has been allocated when there is no free buffers");
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {
         addr = (uint8_t*)buffer_id[i] + j;

         *addr = (uint8_t)((par->numb_of_buffers + i) % 256);
      }
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      
      for (j = 0; j < par->buffer_size; j++)    
      {
         addr = (uint8_t*)buffer_id[i] + j;

         if (*addr != (par->numb_of_buffers + i) % 256)
         {
            PBC_Failed("Invalid data in buffer");
         }
                                
      }
   }



   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(buffer_id[i]);
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) == NULL)
      {
         PBC_Failed_1("SAL_Alloc_Buffer: Buffer %d has not been allocated second time", (int)(i+1));
      }
 
   }

   if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) != NULL)
   {
      PBC_Failed("SAL_Alloc_Buffer: Buffer has been allocated when there is no free buffers (second time)");
   }


   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(buffer_id[i]);
   }

#if defined (XSAL)

   SAL_Destroy_Buffer_Pool(Buffer_Pool_Id_1);

#endif

   Tr_Info_Lo("Buffer_Pool_Fun: End");
}


/**  One of the fuction testing buffer pools in two different threads.
 *
 *   - creates and destroys buffer pool, allocates, writes and frees buffers
 */

void Create_Buffer_Pool_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { READ_BUFFER_POOL };
   Test_Case_T* par = (Test_Case_T*)param;
   size_t i, j;
   uint8_t* addr;

   Tr_Info_Lo("Create_Buffer_Pool_Fun: Start");

   if (!SAL_Create_Buffer_Pool(par->numb_of_buffers, par->buffer_size, &Buffer_Pool_Id_2))
   {
      PBC_Failed("SAL_Create_Buffer_Pool: Buffer Pool has not been created");
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      
       if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) == NULL)
      {
         PBC_Failed_1("SAL_Alloc_Buffer: Buffer %d has not been allocated",(int)(i+1));
      }
   }

   if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) != NULL)
   {
      PBC_Failed("SAL_Alloc_Buffer: Buffer has been allocated when there is no free buffers");
   }


   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {
         addr = (uint8_t*)Buffer_Id[i] + j;

         *addr = (uint8_t)((par->numb_of_buffers + i) % 256);
      }
   }
   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(Buffer_Id[i]);
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) == NULL)
      {
         PBC_Failed_1("SAL_Alloc_Buffer: Buffer %d has not been allocated second time",(int)(i+1));
      }
   }

   if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) != NULL)
   {
      PBC_Failed("SAL_Alloc_Buffer: Buffer has been allocated when there is no free buffers (second time)");
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(Buffer_Id[i]);
   }

#if defined (XSAL)

   SAL_Destroy_Buffer_Pool(Buffer_Pool_Id_2);

#endif

   Tr_Info_Lo("Create_Buffer_Pool_Fun: Exit");
}


/**  One of the function testing buffer pools in two different threads.
 *
 *   - reads buffers which were written by another thread
 */

void Read_Buffer_Pool_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { CREATE_BUFFER_POOL };
   static SAL_Thread_Id_T destroyed_list [] = { CREATE_BUFFER_POOL };
   Test_Case_T* par = (Test_Case_T*)param;
   size_t i, j;
   uint8_t* addr;

   Tr_Info_Lo("Read_Buffer_Pool_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {

         addr = (uint8_t*)Buffer_Id[i] + j;

         if (*addr != (par->numb_of_buffers + i) % 256)
         {
            Tr_Fault("Invalid data in buffer");
            PBC_Failed_2("numb_of_buffers: %d, buffer_size: %d", 
                         (int)par->numb_of_buffers, (int)par->buffer_size);
         }
      }
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Read_Buffer_Pool_Fun: End");
}


/**  Function creates all threads.
 *
 *   Function creates two threads testing buffer pools (Create_Buffer_Pool_Fun
 *   and Read_Buffer_Pool_Fun thread functions), one thread testing locally 
 *   buffer pools (Buffer_Pool_Fun) and starts SAL_Run.
 */

SAL_Int_T Run_Thread(Test_Case_T data)
{
   SAL_Int_T status;
   Test_Case_T param;
   SAL_Thread_Id_T destroyed_list[] = { CREATE_BUFFER_POOL, READ_BUFFER_POOL };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   param.buffer_size = data.buffer_size;
   param.numb_of_buffers = data.numb_of_buffers;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("BP", BUFFER_POOL, priority, &thread_attr));

   SAL_Create_Thread(
      Create_Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("CBP", CREATE_BUFFER_POOL, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("RBP", READ_BUFFER_POOL, priority, &thread_attr));

   status = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      Tr_Fault("Timeout has been destroyed");
      status = 1;
   }

#endif
   
   return status;
}


/**  Test of the buffer pools.
 *
 *   Calls Run_Thread function for 13 different buffer pools 
 *   (different sizes and number of buffers).
 */

SAL_Int_T Buffer_Pool_Test()
{
   SAL_Int_T ret = 0;
 
   ret = ret || Run_Thread(Test[0]);
   ret = ret || Run_Thread(Test[1]);
   ret = ret || Run_Thread(Test[2]);
   ret = ret || Run_Thread(Test[3]);
   ret = ret || Run_Thread(Test[4]);
   ret = ret || Run_Thread(Test[5]);
   ret = ret || Run_Thread(Test[6]);
   ret = ret || Run_Thread(Test[7]);
   ret = ret || Run_Thread(Test[8]);

   ret = ret || Run_Thread(Test[9]);
   ret = ret || Run_Thread(Test[10]);
   ret = ret || Run_Thread(Test[11]);
   ret = ret || Run_Thread(Test[12]);

   return ret;
}

/** \}
 */

SAL_Int_T Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   return 0;
}


void Tear_Down()
{
}


int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Buffer_Pool_Test();

   Tear_Down();

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2006, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
