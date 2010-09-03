#ifndef _TUTTLE_HOST_PARAM_HPP_
#define _TUTTLE_HOST_PARAM_HPP_

#include "Attribute.hpp"
#include <tuttle/host/ofx/attribute/OfxhParamAccessor.hpp>

namespace tuttle {
namespace host {
class ImageEffectNode;

namespace attribute {

class Param : public Attribute
	, virtual public ofx::attribute::OfxhParamAccessor
{
public:
	Param( ImageEffectNode& effect );
	virtual ~Param() = 0;

	const std::string& getName() const { return ofx::attribute::OfxhParamAccessor::getName(); }
};

}
}
}

#endif

