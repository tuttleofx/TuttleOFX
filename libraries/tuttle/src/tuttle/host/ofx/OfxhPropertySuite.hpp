#ifndef _TUTTLE_HOST_OFX_PROPERTYSUITE_HPP_
#define _TUTTLE_HOST_OFX_PROPERTYSUITE_HPP_

#include <ofxProperty.h>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace property
{

/// return the OFX function suite that manages properties
void* getPropertySuite(const int version);
}
}
}
}

#endif
