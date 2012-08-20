#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>
#include <tuttle/host/attribute/ValueInterpolator.hpp>


namespace tuttle {
namespace host {
namespace attribute {

class ParamDouble : public Param
	, public ofx::attribute::OfxhParamDouble
{
protected:
  double _value;

  /* An unordered list of key frames. Used when this param is
     animated.  If we expected a lot of key frames, a sorted container
     might be more efficient. */
  std::vector< TimeValue<double> > _key_frames;

  /* The class that interpolates key frames when animating this param
     All transitions are animated with the same interpolator. A more
     advanced version of this class might allow any of the interpolators
     in ValueInterpolator.hpp to be used between any adjacent key frames. */
  Interpolator<double> *_interpolator;

public:
	ParamDouble( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
        ~ParamDouble();
	ParamDouble* clone() const { return new ParamDouble( *this ); }

	double getDefault() const;

	void getValue( double& ) const OFX_EXCEPTION_SPEC;
	void getValueAtTime( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void setValue( const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setValueAtTime( const OfxTime time, const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void setValueFromExpression( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	
	void derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC;

	void copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
