#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_

/* Most of the implementation is in the AnimatedParam template */
#include "AnimatedParam.hpp"

namespace tuttle {
namespace host {
namespace attribute {

class ParamDouble : 
    public AnimatedParamDouble
{
public:
  ParamDouble( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );

  double getDefault() const;
  

};

}
}
}

#endif
