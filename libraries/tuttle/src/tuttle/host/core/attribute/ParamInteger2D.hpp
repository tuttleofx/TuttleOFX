#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER2D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER2D_HPP_

#include "ParamInteger.hpp"
#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {


class ParamInteger2D : public ofx::attribute::OfxhMultiDimParam<ParamInteger, 2 >
{
protected:
	ImageEffectNode& _effect;

public:
	ParamInteger2D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger2D* clone() const { return new ParamInteger2D( *this ); }

	OfxPointI getDefault() const;
	void get( int& x, int& y ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC;
	void set( const int &x, const int &y ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &x, const int &y ) OFX_EXCEPTION_SPEC;
};


}
}
}

#endif
