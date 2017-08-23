/*--------------------------------------------------------------------------
 * File:           pm_parser.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Lukasz Ptak
 *
 * Description:    Implementation of the PM Parser functionality.
 *
 *--------------------------------------------------------------------------*/

#include <ctype.h>
#include <limits.h>
#include <malloc.h>
#include "pbc_trace.h"
#include "pm_parser.h"
#include "pm_process.h"
#include <stdlib.h>
#include <string.h>
#include "string_resource.h"
#include "xsal_application_id.h"

EM_FILENUM(PROCMAN_MODULE_ID, 2);

#define PM_BLOCK_SIZE 5

#if 1  /* needs to be set based on BASA */

/**
 *  Structure is used for binding name to the application id.
 */
typedef struct
{
   int app_id;
   char* app_name;
} Application_Name_T;

#undef X
#define X(a,b) {a, b}, /* Create Application_Name_T with tag and ptr to string*/

static const Application_Name_T appNames[] =
{
   { 0, "SAL_NO_APP_ID" },
   SAL_APPLICATION_IDS
};
#else
static const Application_Name_T appNames[] = APP_NAMES;
#endif

/** Configuration file line option counters */
typedef struct Option_Count_Tag
{
   int a, c, d, f, l, n, p, r, x;
} Option_Count_T;

static int Parse_Int(const char *line, int *read_pos, int *result)
{
   char *ptr = NULL;
   int offset = 0;
   int value = 0;
   int ret = 0;

   /* Acts like atoi */
   value = strtol(&line[(*read_pos)], &ptr, 10);
   offset = ptr - &line[(*read_pos)];
   if (value < INT_MAX && value > INT_MIN && offset != 0)
   {
      *result = value;

      /* read_pos points to the position right next to parsed integer */
      (*read_pos) += offset;
      ret = 1;
   }

   return ret;
}

static char* Parse_String(const char *line, int *read_pos)
{
   int is_require_closing_quote = 0;
   int string_size = 0;
   int str_length = 0;
   char *temp_array = NULL;
   char *return_string = NULL;

   /* Skip white-space. */
   while (isspace((unsigned char) line[*read_pos]) != 0)
   {
      (*read_pos)++;
   }

   /* The first character may be a double quote (") or any other character.
    * In the former case, a closing double quote must be present.
    */
   if (line[*read_pos] == '"')
   {
      (*read_pos)++;
      is_require_closing_quote = 1;
   }

   str_length = strlen(line) - (*read_pos);
   temp_array = (char*) alloca(str_length + 1);
   if( temp_array != NULL)
   {
      memset(temp_array, '\0', str_length + 1);

      while (1)
      {
         switch (line[*read_pos])
         {
            case '\0':

               if (is_require_closing_quote)
               {
                  Tr_Fault( "Closing quote not found" );
                  return NULL; /* unexpected end of input */
               }
               else
               {
                  size_t ret_len = strlen(temp_array) + 1;
                  return_string = (char*) malloc(ret_len);
                  Str_Full_Copy(return_string, temp_array, ret_len);
                  return return_string;
               }

            case ' ': /* terminating only if closing quote not required */
            case '\t':

               if (!is_require_closing_quote)
               {
                  size_t ret_len = strlen(temp_array) + 1;
                  return_string = (char*) malloc(ret_len);
                  Str_Full_Copy(return_string, temp_array, ret_len);
                  return return_string;
               }

               break;

            case '"':
               /* If this is the closing quote then exit.  Otherwise treat it
                * as a regular character.
                */
               if (is_require_closing_quote)
               {
                  size_t ret_len = strlen(temp_array) + 1;
                  (*read_pos)++;
                  return_string = (char*) malloc(ret_len);
                  Str_Full_Copy(return_string, temp_array, ret_len);
                  return return_string;
               }

               break;

            case '\\': /* \ -- embedded special character */
               (*read_pos)++;
               if (line[*read_pos] == '\0')
               {
                  Tr_Fault( "\\ -- embedded special character" );
                  return NULL;
               }

               break;

            default:
               break;
         }

         temp_array[string_size++] = line[(*read_pos)++];
      }
   }
   else
   {
      Tr_Fault( "Memory allocation failed");
      return NULL;
   }

}

static int Parse_Option(const char *line, int *read_pos, Option_Count_T *option_count,
   PM_Process_T *process)
{
   int ret = 1;
   char *string_result = NULL;
   int int_result;

   switch (line[(*read_pos)++])
   {
      case 'a':
         string_result = Parse_String(line, read_pos);
         if (string_result)
         {
            /* process->argv is made from 5-elements blocks. If such block becomes full, a new one
             * is attached to the end and is ready to receive new entries.
             */
            if (process->arg_counter % PM_BLOCK_SIZE == 0)
            {
               process->argv = realloc(process->argv, (process->arg_counter + PM_BLOCK_SIZE)
                  * sizeof(char*));
               PBC_Ensure(process->argv != NULL, "Unable to reallocate memory for process arguments");
               process->argv[process->arg_counter] = string_result;
            }
            else
            {
               process->argv[process->arg_counter] = string_result;
            }

            option_count->a++;
            process->arg_counter++;
         }
         else
         {
            ret = 0;
         }
         break;

      case 'c':
         ret = Parse_Int(line, read_pos, &int_result);
         if (ret)
         {
            if (int_result > 31 || int_result < 0)
            {
               ret = 0;
               Tr_Fault( "Configuration must be in [0-31]" );
            }
            else
            {
               process->config |= (1U << int_result);
               option_count->c++;
            }
         }
         break;

      case 'd':
         ret = 0;
         /* -d parameter can appear only once*/
         if (option_count->d == 0)
         {
            string_result = Parse_String(line, read_pos);
            if (string_result)
            {
               process->dir = string_result;
               option_count->d++;
               ret = 1;
            }
         }
         break;

      case 'f':
         ret = 0;
         /* -f parameter can appear only once*/
         if (option_count->f == 0)
         {
            string_result = Parse_String(line, read_pos);
            if (string_result)
            {
               process->path = string_result;
               option_count->f++;
               ret = 1;
            }
         }
         break;

      case 'l':
         ret = Parse_Int(line, read_pos, &int_result);
         if (ret)
         {
            process->load_timeout = int_result;
            option_count->l++;
         }
         break;

      case 'n':
         string_result = Parse_String(line, read_pos);
         if (string_result)
         {
            process->proc_name = string_result;
            option_count->n++;
         }
         else
         {
            ret = 0;
         }
         break;

      case 'p':
         ret = Parse_Int(line, read_pos, &int_result);
         if (ret)
         {
            process->priority = int_result;
            option_count->p++;
         }
         break;

      case 'r':
         process->auto_restart = 1;
         option_count->r++;
         break;

      case 'x':
         ret = Parse_Int(line, read_pos, &int_result);
         if (ret)
         {
            process->terminate_timeout = int_result;
            option_count->x++;
         }
         break;

      default:
         Tr_Fault_1( "Unrecognized option: %c", (char)line[*read_pos - 1] );
         ret = 0;
         break;
   }

   return ret;
}

PM_Process_T * PM_Parse_Line(const char *line_str, int line_number, int *line_status)
{
   int read_pos = 0;
   PM_Process_T *process = NULL;
   bool_t is_valid = true; /* initially assume the line is valid */
   int eoi = 0; /* end of input flag */

   /* Most options may appear only once on the command line, so we need
    * to count them.
    */
   Option_Count_T option_count;

   memset(&option_count, 0, sizeof(Option_Count_T));

   while (!eoi)
   {
      /* Skip white space. */

      while (isspace((unsigned char) line_str[read_pos]) != 0)
      {
         read_pos++;
      }

      /* See what you've got. */

      switch (line_str[read_pos])
      {
         case '\0': /* end of input */
            eoi = 1;
            break;

         case '#': /* comment */
            eoi = 1;
            break;

         case '-': /* command */
            if (process == NULL)
            {
               process = malloc(sizeof(*process));
               PBC_Ensure(process != NULL, "Unable to allocat memory for process structure");
               PM_Constructor(process);
            }
            read_pos++;

            if (!Parse_Option(line_str, &read_pos, &option_count, process))
            {
               is_valid = false;
               eoi = 1;
               *line_status = 0;
            }

            break; /* process the rest of input */

         default:
            is_valid = false;
            eoi = 1;
            *line_status = 0;
            break;
      }
   }

   if (option_count.a == 0 && option_count.c == 0 && option_count.d == 0 && option_count.f == 0
      && option_count.l == 0 && option_count.n == 0 && option_count.p == 0 && option_count.r == 0
      && option_count.x == 0)
   {
      is_valid = false;
   }
   else if (is_valid && !(option_count.d <= 1 && option_count.f == 1 && option_count.l <= 1
      && option_count.n == 1 && option_count.p <= 1 && option_count.r <= 1 && option_count.x <= 1))
   {
      is_valid = false;
      *line_status = 0;
      Tr_Fault_1( "cfg error in line %d", (int)line_number );
   }

   /* try to find appId  */
   if (is_valid && (process != NULL))
   {
      size_t i = 0;
      for (i = 0; i < sizeof(appNames) / sizeof(Application_Name_T); i++)
      {
         if (strcmp(process->proc_name, appNames[i].app_name) == 0)
         {
            process->app_id = appNames[i].app_id;
            break;
         }
      }

      if (process->app_id == 0)
      {
         is_valid = false;
         *line_status = 0;
         Tr_Fault_1( "Wrong application name: %s", process->proc_name );
      }
   }
   else
   {
      is_valid = false;
   }

   if (!is_valid && (process != NULL ))
   {
      PM_Destructor(process);
      free(process);
      process = NULL;
   }

   return process;
}

/*-----------------------------------------------------------------------------
 * History
 *
 * 19-Feb-2015, Vijayalakshmi: Fixed NULL dereference found by klockwork
 * 19-Jul-2013, Larry Piekarski: Removed reference to deprecated util functions.
 * 05-Jun-2012, Kirk Bailey: Fixed NULL dereference found by Klockwork.
 * 22 Sep. 2006, Lukasz Ptak:     Initial revision.
 *-----------------------------------------------------------------------------*/
