/***
 * File: Error_TestDouble.h
 * Desc: Basic test implementation of I_Error that stores the most recent error
 */

#ifndef _ERROR_TESTDOUBLE_H
#define _ERROR_TESTDOUBLE_H

extern "C"
{
   #include "I_Error.h"
}

#define ERROR_TESTDOUBLE_MAX_MESSAGE_SIZE (256)

typedef struct
{
   I_Error_t interface;
   char message[ERROR_TESTDOUBLE_MAX_MESSAGE_SIZE];
} Error_TestDouble_t;

/*
 * Initialize a Error_TestDouble.
 */
void Error_TestDouble_Init(Error_TestDouble_t *instance);

/*
 * Get (shallow copy) the most recently reported error
 *
 * @param message - to point to most recent message
 */
void Error_TestDouble_GetError(Error_TestDouble_t *instance, char *message);

#endif
