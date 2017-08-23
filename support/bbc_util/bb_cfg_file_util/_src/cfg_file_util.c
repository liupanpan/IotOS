/*===========================================================================*/
/**
 * @file cfg_file_util.c
 *
 * Utility for managing the runtime configuration using the contents of a file
 *
 * %full_filespec:cfg_file_util.c~kok_inf#3:csrc:kok_basa#2 %
 * @version %version:kok_inf#3 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Mar 28 16:29:19 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Utility for managing the runtime configuration using the contents of a file
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - None
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "cfg_file_util.h"
#include <ctype.h>
#include "reuse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
#define SS_MAX_INPUT_LINE 64

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*
 * QNX doesn't have a standard getline function
 */
#   if defined(QNX_NTO)
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
   char * str;
   ssize_t ret = -1;
   if ((NULL == *lineptr) || (0 == *n))
   {
      *lineptr = malloc(256);
      *n = 256;
   }

   if (*lineptr)
   {
      str = fgets(*lineptr, *n, stream);
      if (str)
      {
         ret = strlen(str);
      }
   }
   return ret;
}
#endif

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
#ifdef WIN32
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/
/*
 * Please see description in cfg_file_util.h
 */
char const * Get_Config_String_From_File(char const *default_value, char const *file_name)
{
   FILE* file = NULL;
   char const *config_value = NULL;
   bool_t use_default = true;

   if (NULL == default_value)
   {
      default_value = "";
   }

   if (NULL != file_name)
   {
      file = fopen(file_name, "r");
   }

   if (file != NULL)
   {
      char * line = NULL;
      size_t line_size = 0;

      /* Read until the end of the file to find the first actual line of the file */
      while (-1 != getline(&line, &line_size, file))
      {
         size_t i = 0;

         /* Find the first character of this line */
         for (i = 0; (isspace((int) line[i] & 0xff) && i < line_size); i++)
         {
            /* work done in loop construct above */
         }

         /* Is this a comment line? */
         if (line[i] != '#')
         {
            char * config_string = &line[i];

            size_t len = strlen(config_string);

            /* Strip trailing '\n', if present */
            if ((len > 0) && ('\n' == config_string[len - 1]))
            {
               len--;
               config_string[len] = '\0';
            }

            if (len > 0)
            {
               config_value = strdup(config_string); /* use contents of file for address */
               use_default = false;
               break; /* Exit the while loop and terminate search */
            }
         }
         free(line);
         line = NULL;
      }
      free(line);
      fclose(file);
   }

   if (use_default)
   {
      config_value = strdup(default_value);
   }

   return config_value; /* Caller responsible for freeing memory! */
}

/*
 * Please see description in cfg_file_util.h
 */
void Free_Config_String(char const ** config_string)
{
   free((char *) *config_string);
   *config_string = NULL;
}

/*===========================================================================*/
/*!
 * @file cfg_file_util.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 24-Jul-2013 Larry Piekarski
 *   - Created initial file (from a ADP GIT project from Kirk Bailey)
 *
 * - 20-Sep-2013 Kirk Bailey Rev 2
 *   - Task 1164: Fixed warnings due to enabling of -Wextra
 */
/*===========================================================================*/
