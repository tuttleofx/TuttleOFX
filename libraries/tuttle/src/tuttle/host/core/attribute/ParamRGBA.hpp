#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMRGBA_HPP_

#include "ParamDouble.hpp"
#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {


class ParamRGBA : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 4 >
{
protected:
	ImageEffectNode& _effect;

public:
	ParamRGBA( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamRGBA* clone() const { return new ParamRGBA( *this ); }

	OfxRGBAColourD getDefault() const;
	void get( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void set( const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC;
};


}
}
}

#endif
