#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGB_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGB_HPP_

#include "Param.hpp"

#include "ParamDouble.hpp"
#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>

#include <ofxPixels.h>

namespace tuttle {
namespace host {
namespace attribute {

class ParamRGB : public Param
	, public ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
public:
	ParamRGB( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamRGB* clone() const { return new ParamRGB( *this ); }

	OfxRGBColourD getDefault() const;
	void          get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC;
	void          getAtTime( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC;
	void          set( const double& r, const double& g, const double& b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void          setAtTime( const OfxTime time, const double& r, const double& g, const double& b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
