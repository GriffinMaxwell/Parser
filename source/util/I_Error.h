/***
 * File: I_Error.h
 * Desc: Interface for reporting errors in the compilation process.
 */

#ifndef _I_ERROR_H
#define _I_ERROR_H

#include <stddef.h>

typedef struct I_Error_t
{
   /*
    * Report that an error occured.
    *
    * @param message - the error message
    */
   void (*report)(struct I_Error_t *interface, size_t line, const char *message);
} I_Error_t;

#define Error_Report(interface, line, message) \
   (interface)->report((interface), (line), (message))

#endif
