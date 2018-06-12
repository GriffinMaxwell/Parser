/***
 * File: Error_TestDouble.cpp
 */

#include "Error_TestDouble.h"

extern "C"
{
   #include <string.h>
   #include "util.h"
}

static void report(I_Error_t *interface, size_t line, const char *message)
{
   REINTERPRET(instance, interface, Error_TestDouble_t *);

   instance->line = line;
   strncpy(instance->message, message, ERROR_TESTDOUBLE_MAX_MESSAGE_SIZE);
}

void Error_TestDouble_Init(Error_TestDouble_t *instance)
{
   instance->interface.report = &report;
}

void Error_TestDouble_GetError(Error_TestDouble_t *instance, size_t *line, char *message)
{
   *line = instance->line;
   message = instance->message;  // Shallow copy
}
