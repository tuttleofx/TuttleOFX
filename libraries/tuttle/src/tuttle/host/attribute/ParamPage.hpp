#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPAGE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMPAGE_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamPage.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamPage : public Param
	, public ofx::attribute::OfxhParamPage
{
public:
	ParamPage( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPage* clone() const { return new ParamPage( *this ); }

	void copy( const ParamPage& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
