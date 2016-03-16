#ifndef _TUTTLE_HOST_OFX_PARAMETERSUITE_HPP_
#define _TUTTLE_HOST_OFX_PARAMETERSUITE_HPP_

#include <ofxParam.h>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// fetch the param suite
void* getParameterSuite(const int version);
}
}
}
}

#endif
