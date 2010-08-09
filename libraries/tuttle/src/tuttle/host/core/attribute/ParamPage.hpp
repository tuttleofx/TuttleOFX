#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPAGE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPAGE_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamPage.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamPage : public ofx::attribute::OfxhParamPage
{
protected:
	ImageEffectNode& _effect;

public:
	ParamPage( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPage* clone() const { return new ParamPage(*this); }

	void copy( const ParamPage& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};


}
}
}

#endif
