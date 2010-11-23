#ifndef _TUTTLE_HOST_PARAM_HPP_
#define _TUTTLE_HOST_PARAM_HPP_

#include "Attribute.hpp"
#include <tuttle/host/ofx/attribute/OfxhParamAccessor.hpp>

namespace tuttle {
namespace host {

class INode;

namespace attribute {

class Param : public Attribute
	, virtual public ofx::attribute::OfxhParamAccessor
{
public:
	Param( INode& effect );
	virtual ~Param() = 0;

	const std::string& getName() const { return ofx::attribute::OfxhParamAccessor::getName(); }
};

}
}
}

#endif

