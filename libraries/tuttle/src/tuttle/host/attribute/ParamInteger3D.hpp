#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER3D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER3D_HPP_

#include "Param.hpp"
#include "ParamInteger.hpp"

#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamInteger3D : public Param
	, public ofx::attribute::OfxhMultiDimParam<ParamInteger, 3 >
{
public:
	ParamInteger3D( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger3D* clone() const { return new ParamInteger3D( *this ); }

	Ofx3DPointI getDefault() const;

	void get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void getAtTime( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void set( const int& x, const int& y, const int& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setAtTime( const OfxTime time, const int& x, const int& y, const int& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
