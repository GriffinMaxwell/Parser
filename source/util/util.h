/***
 * File: util.h
 * Desc: Util Macros
 */

#ifndef _UTIL_H
#define _UTIL_H

#define REINTERPRET(new_name, cast_me, new_type) \
   new_type new_name = (new_type)(cast_me)

#endif
