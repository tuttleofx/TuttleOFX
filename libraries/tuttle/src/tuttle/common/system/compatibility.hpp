#ifndef _TUTTLE_COMMON_SYSTEM_COMPATIBILITY_HPP_
#define _TUTTLE_COMMON_SYSTEM_COMPATIBILITY_HPP_

//#include <cstddef>
#include <unistd.h>

// compatibility problems...
namespace std {
#ifdef _MSC_VER
       typedef SSIZE_T ssize_t;
#else
       typedef ::ssize_t ssize_t;
#endif
}


#endif

