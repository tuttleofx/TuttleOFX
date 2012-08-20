#include "ParamDouble.hpp"
#include "expression.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamDouble::ParamDouble( INode&                           effect,
                          const std::string&                         name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor,
                          const std::size_t                          index )
	: Param( effect )
	, ofx::attribute::OfxhParamDouble( descriptor, name, effect.getParamSet(), index )
{
	_value = getDefault();
	_interpolator = new SmoothInterpolator<double>();
}
ParamDouble::~ParamDouble() {
        delete _interpolator;
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault, _index );
}

void ParamDouble::getValue( double& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamDouble::getValueAtTime( const OfxTime time, double& v ) const OFX_EXCEPTION_SPEC
{
  if (_key_frames.size() == 0)
    v = _value;
  else {

  /* Find the key frames surrounding this time
    
     Set prev to the first key frame before "time" or the first one
     after "time" if there's not one before.
    
     Set next to the first key frame after "time" or the first one
     before "time" if there's not one after */

    TimeValue<double> prev;
    TimeValue<double> next;
    prev.time = _key_frames[0].time;
    prev.value = _key_frames[0].value;
    next.time = _key_frames[0].time;
    next.value  = _key_frames[0].value;

    for (size_t i=1;i<_key_frames.size();i++) {
      if ((_key_frames[i].time <= time && _key_frames[i].time > prev.time) ||
	  (prev.time > time && _key_frames[i].time < prev.time)) {
	prev.time = _key_frames[i].time;
	prev.value = _key_frames[i].value;
      }
      if ((_key_frames[i].time >= time && _key_frames[i].time < next.time) ||
	  (next.time < time &&  _key_frames[i].time > next.time)) {
	next.time = _key_frames[i].time;
	next.value = _key_frames[i].value;
      }
    }

    /* Find the value between these key frames */
    _interpolator->getValue(prev,next,(const OfxTime)time,v);
  }
}

void ParamDouble::setValue( const double& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
    /* Setting a single value for this param clears the animation */
         _key_frames.clear();
	_value = v;
	this->paramChanged( change );
}

void ParamDouble::setValueAtTime( const OfxTime time, const double& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
  TimeValue<double> new_tv;

  /* If there's already a key frame at this time, change the value */
  for (size_t i=0;i<_key_frames.size();i++) {
    if (_key_frames[i].time == time) {
      _key_frames[i].value = v;
      this->paramChanged( change );
      return;
    }
  }

  /* Otherwise, add a new key frame */
  new_tv.time = time;
  new_tv.value = v;
  _key_frames.push_back(new_tv);
  this->paramChanged( change );
}

void ParamDouble::setValueFromExpression( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = extractValueFromExpression<double>( value );
	this->paramChanged( change );
}

void ParamDouble::derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ParamDouble::integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ParamDouble::copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC
{
	_value = p._value;
	_key_frames.clear();
	for (size_t i = 0;i<p._key_frames.size();i++) {
	  _key_frames.push_back(p._key_frames[i]);
	}
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamDouble::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamDouble& param = dynamic_cast<const ParamDouble&>( p );

	copy( param );
}

}
}
}

