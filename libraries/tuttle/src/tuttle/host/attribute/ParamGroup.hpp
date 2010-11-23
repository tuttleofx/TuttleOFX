#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMGROUP_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMGROUP_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamGroup.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamGroup : public Param
	, public ofx::attribute::OfxhParamGroup
{
public:
	ParamGroup( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamGroup* clone() const { return new ParamGroup( *this ); }

	void copy( const ParamGroup& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
