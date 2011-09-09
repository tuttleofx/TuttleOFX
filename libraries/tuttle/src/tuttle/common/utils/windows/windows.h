#ifndef _TUTTLE_WINDOWS_H_
#define _TUTTLE_WINDOWS_H_

#ifdef _MSC_VER

  #define _USE_MATH_DEFINES 1

  #define NOMINMAX 1
  #include <windows.h>

  #ifndef __PRETTY_FUNCTION__
   #ifndef __GNUC__
	#define __PRETTY_FUNCTION__ __FUNCDNAME__
   #endif
  #endif

/*
 * __FUNCDNAME__
 * Valid only within a function and returns the decorated name of the enclosing function (as a string). __FUNCDNAME__ is not expanded if you use the /EP or /P compiler option.
 *
 * __FUNCSIG__
 * Valid only within a function and returns the signature of the enclosing function (as a string). __FUNCSIG__ is not expanded if you use the /EP or /P compiler option.
 * On a 64-bit operating system, the calling convention is __cdecl by default.
 *
 * __FUNCTION__
 * Valid only within a function and returns the undecorated name of the enclosing function (as a string). __FUNCTION__ is not expanded if you use the /EP or /P compiler option.
 */

#endif

#endif
