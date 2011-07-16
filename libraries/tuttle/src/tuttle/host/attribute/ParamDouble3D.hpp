#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE3D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE3D_HPP_

#include "Param.hpp"
#include "ParamDouble.hpp"

#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamDouble3D : public Param
	, public ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
public:
	ParamDouble3D( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamDouble3D* clone() const { return new ParamDouble3D( *this ); }

	Ofx3DPointD getDefault() const;
	void        getValue( double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC;
	void        getValueAtTime( const OfxTime time, double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC;
	void        setValue( const double& x, const double& y, const double& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void        setValueAtTime( const OfxTime time, const double& x, const double& y, const double& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
