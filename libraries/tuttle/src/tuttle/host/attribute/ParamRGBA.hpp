#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_

#include "Param.hpp"
#include "ParamDouble.hpp"

#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>

#include <ofxPixels.h>

namespace tuttle {
namespace host {
namespace attribute {

class ParamRGBA : public Param
	, public ofx::attribute::OfxhMultiDimParam<ParamDouble, 4 >
{
public:
	ParamRGBA( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamRGBA* clone() const { return new ParamRGBA( *this ); }

	OfxRGBAColourD getDefault() const;
	void           getValue( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void           getValueAtTime( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void           setValue( const double& r, const double& g, const double& b, const double& a, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void           setValueAtTime( const OfxTime time, const double& r, const double& g, const double& b, const double& a, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
