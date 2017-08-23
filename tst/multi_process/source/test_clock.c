#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE2_ID, 5);

#define APP_ID 2
#define DELAY_US 10000
#define DELAY_MS (DELAY_US/1000)
#define MAX_NUMB_OF_THREADS 1
#define MAX_TRIES 10

SAL_Int_T Clock_Test_Suite()
{
    int err_count = 0;
    int try_count;

    for (try_count = 0; try_count < MAX_TRIES; try_count++)
    {
       SAL_Clock_T clock1;
       SAL_Clock_T clock2;
       SAL_Int_T delta;
       SAL_Int_T elapsed;

       clock1 = SAL_Clock();
       SAL_Delay(DELAY_US);
       clock2 = SAL_Clock();
       delta = SAL_Delta_Time(clock1, clock2);
       elapsed = SAL_Elapsed_Time(clock1);

       if ((delta < DELAY_MS) || 
           ((delta != elapsed) && (elapsed - delta) != 1))
       {
          Tr_Fault_4("Delay test failure Clock 1 %d Clock 2 %d delta %d elapsed %d", clock1, clock2, delta, elapsed);
           err_count++;
       }

       elapsed = SAL_Elapsed_Time(clock2+DELAY_MS);

       if (elapsed != -DELAY_MS)
       {
          Tr_Fault_3("Elapsed test failure Clock 2 %d DELAY_MS %d elapsed %d", clock2, DELAY_MS, elapsed);
           err_count++;
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
      ret = Clock_Test_Suite();

      Tear_Down();

      Tr_Info_Hi_1("Test %s\n", ((ret)?("FAILED"):("PASSED")));
   }

   return ret;
}
