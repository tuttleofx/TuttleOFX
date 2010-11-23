#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPUSHBUTTON_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPUSHBUTTON_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamPushButton.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamPushButton : public Param
	, public ofx::attribute::OfxhParamPushButton
{
public:
	ParamPushButton( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPushButton* clone() const { return new ParamPushButton( *this ); }

	void copy( const ParamPushButton& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
