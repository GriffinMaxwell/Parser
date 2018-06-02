/***
 * File: Error_Mock.h
 * Desc: Mock implementation of I_Error
 */

#ifndef _ERROR_MOCK_H
#define _ERROR_MOCK_H

extern "C"
{
   #include "I_Error.h"
}

typedef struct
{
   I_Error_t interface;
} Error_Mock_t;

/*
 * Initialize an Error_Mock.
 */
void Error_Mock_Init(Error_Mock_t *instance);

#endif
