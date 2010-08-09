#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPUSHBUTTON_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPUSHBUTTON_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamPushButton.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamPushButton : public ofx::attribute::OfxhParamPushButton
{
protected:
	ImageEffectNode& _effect;

public:
	ParamPushButton( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPushButton* clone() const { return new ParamPushButton( *this ); }

	void copy( const ParamPushButton& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};


}
}
}

#endif
