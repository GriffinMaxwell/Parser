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
   size_t line;
   char message[ERROR_TESTDOUBLE_MAX_MESSAGE_SIZE];
} Error_TestDouble_t;

/*
 * Initialize a Error_TestDouble.
 */
void Error_TestDouble_Init(Error_TestDouble_t *instance);

/*
 * Get (shallow copy) the most recently reported error
 *
 * @param line - line of the most recent error
 * @param message - message of the most recent error
 */
void Error_TestDouble_GetError(Error_TestDouble_t *instance, size_t *line, char *message);

#endif
