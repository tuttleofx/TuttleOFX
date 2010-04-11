#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMGROUP_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMGROUP_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamGroup.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamGroup : public ofx::attribute::OfxhParamGroup
{
protected:
	ImageEffectNode& _effect;

public:
	ParamGroup( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamGroup* clone() const { return new ParamGroup(*this); }
	
	void copy( const ParamGroup& p ) OFX_EXCEPTION_SPEC {}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamGroup& param = dynamic_cast<const ParamGroup&>(p);
		copy( param );
	}
};


}
}
}

#endif
