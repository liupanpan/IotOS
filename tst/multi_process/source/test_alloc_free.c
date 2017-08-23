#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE2_ID, 4);

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 1
#define MAX_TRIES 10000
#define NUM_TEST_PTRS 100

static void * test_ptrs[NUM_TEST_PTRS];

SAL_Int_T Alloc_Free_Test_Suite()
{
    int err_count = 0;
    int ptr_count;
    int try_count;

    for (try_count = 0; try_count < MAX_TRIES; try_count++)
    {
        for (ptr_count = 0; ptr_count < NUM_TEST_PTRS; ptr_count++)
        {
            test_ptrs[ptr_count] = SAL_Alloc(ptr_count);
            if (NULL == test_ptrs[ptr_count])
            {
                err_count++;
            }
        }
        for (ptr_count = 0; ptr_count < NUM_TEST_PTRS; ptr_count++)
        {
            SAL_Free(test_ptrs[ptr_count]);
        }
    }
    return err_count;
}

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

   ret = Set_Up();           

   if (ret != 0)
   {
      Tr_Fault("Set_Up failed");
   }
   else
   {
      ret = Alloc_Free_Test_Suite();

      Tear_Down();

      Tr_Info_Hi_1("Test %s\n", ((ret)?("FAILED"):("PASSED")));
   }

   return ret;
}
