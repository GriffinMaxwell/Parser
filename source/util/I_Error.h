/***
 * File: I_Error.h
 * Desc: Interface for reporting errors in the compilation process.
 */

#ifndef _I_ERROR_H
#define _I_ERROR_H

typedef struct I_Error_t
{
   /*
    * Report that an error occured.
    *
    * @param message - the error message
    */
   void (*report)(struct I_Error_t *interface, const char *message);
} I_Error_t;

#define Error_Report(interface, message) \
   (interface)->report((interface), (message))

#endif
