/***
 * File: Error_Mock.cpp
 */

#include "MockSupport.h"
#include "Error_Mock.h"

extern "C"
{
#include "util.h"
}

static void report(I_Error_t *interface, const char *message)
{
   REINTERPRET(instance, interface, Error_Mock_t *);

   mock()
      .actualCall("report")
      .onObject(instance)
      .withParameter("message", message);
}

void Error_Mock_Init(Error_Mock_t *instance)
{
   instance->interface.report = &report;
}
