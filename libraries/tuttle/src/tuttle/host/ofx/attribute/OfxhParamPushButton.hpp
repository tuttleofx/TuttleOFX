#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMPUSHBUTTON_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMPUSHBUTTON_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamPushButton
	: public OfxhParam
	, public OfxhKeyframeParam
{
public:
	OfxhParamPushButton( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}
};

}
}
}
}

#endif

