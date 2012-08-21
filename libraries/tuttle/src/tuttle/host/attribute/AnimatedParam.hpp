#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAM_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAM_HPP_

/* Template class for an animated parameter.

Used for ParamDouble and ParamInteger */

#include "Param.hpp"
#include "expression.hpp"
#include <tuttle/host/INode.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>
#include <tuttle/host/attribute/ValueInterpolator.hpp>
#include <set>

namespace tuttle {
namespace host {
namespace attribute {

template<typename T, typename OFX_PARAM>
class AnimatedParam : public Param, public OFX_PARAM
{
protected:
  T _value;

  /* An ordered set of key frames. Used when this param is
     animated. */
  std::set< TimeValue<T> > _key_frames;

  /* The class that interpolates key frames when animating this param
     All transitions yare animated with the same interpolator. A more
     advanced version of this class might allow any of the interpolators
     in ValueInterpolator.hpp to be used between any adjacent key frames. */
  Interpolator<T> *_interpolator;

public:
  AnimatedParam<T,OFX_PARAM>( INode&                           effect,
		    const std::string&                         name,
		    const ofx::attribute::OfxhParamDescriptor& descriptor,
		    const std::size_t                          index )

  :Param( effect )
    ,OFX_PARAM( descriptor, name, effect.getParamSet() )
{
	_interpolator = new LinearInterpolator<T>();
}

  ~AnimatedParam<T,OFX_PARAM>() {
    delete _interpolator;
  }

  /* This interface isn't ideal because it requires casting the
     OfxhParam to a ParamDouble or ParamInteger. However, I'm not sure
     it's a sane idea to alter the OfxhParam base class. */
void setInterpolator(const enum EInterpolatorType etype) {
  delete _interpolator;
  switch (etype) {
  case eSmoothInterpolator:
    _interpolator = new SmoothInterpolator<T>();
    break;
  case eFastInterpolator:
    _interpolator = new FastInterpolator<T>();
    break;
  case eSlowInterpolator:
    _interpolator = new SlowInterpolator<T>();
    break;
  case eLinearInterpolator:
  default:
    _interpolator = new LinearInterpolator<T>();
    break;
  }
}

void getValue( T& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void getValueAtTime( const OfxTime time, T& v ) const OFX_EXCEPTION_SPEC
{
  if (_key_frames.size() == 0)
    v = _value;
  else {

  /* Find the key frames surrounding this time
    
     Set prev to the first key frame before "time" or the first one
     after "time" if there's not one before.
    
     Set next to the first key frame after "time" or the first one
     before "time" if there's not one after */

    TimeValue<T> temp, prev, next;
    typename std::set< TimeValue<T> >::iterator it;

    // Find the first item at or before time
    temp.time = time;
    it = _key_frames.lower_bound(temp);

    if (it->time == time || it == _key_frames.begin()) {
      // There's a key frame at exactly this time
      // or this is the first key frame. Return this key frame value
      v = it->value;
      return;
    } else if (it == _key_frames.end()) {
      // There's no key frame at or after this time
      // Return the last key frame value
      it--;
      v = it->value;
      return;
    } else {
      // The time is between two values.
      // Get the previous key frame and interpolate
      next = *it;
      it--;
      prev = *it;
      _interpolator->getValue(prev,next,(const OfxTime)time,v);
    }
  }
}

void setValue( const T& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
    /* Setting a single value for this param clears the animation */
         _key_frames.clear();
	_value = v;
	this->paramChanged( change );
}

void setValueAtTime( const OfxTime time, const T& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
  TimeValue<T> new_tv;
  std::pair<typename std::set< TimeValue<T> >::iterator, bool> ret;

  new_tv.time = time;
  new_tv.value = v;
  ret = _key_frames.insert(new_tv);

  // If a key frame already exists, change the value
  if (!ret.second) {

    // Try to find the position right before the existing element
    // and insert the new key frame there
    typename std::set< TimeValue<T> >::iterator it;
    it = ret.first;
    if (it == _key_frames.begin())
      it++;
    else
      it--;

    _key_frames.erase(ret.first);
    if (it != _key_frames.end())
      _key_frames.insert(it, new_tv);
    else
      _key_frames.insert(new_tv);
  }

  this->paramChanged( change );
}

void setValueFromExpression( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = extractValueFromExpression<T>( value );
	this->paramChanged( change );
}

void derive( const OfxTime time, T& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void integrate( const OfxTime time1, const OfxTime time2, T& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

  void copy( const AnimatedParam<T,OFX_PARAM>& p ) OFX_EXCEPTION_SPEC
{
        typename std::set< TimeValue<T> >::iterator it;
	_value = p._value;
	_key_frames = p._key_frames;
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

  void copy( const ofx::attribute::OfxhParam& p ) OFX_EXCEPTION_SPEC
{
  const AnimatedParam<T,OFX_PARAM>& param = dynamic_cast<const AnimatedParam<T,OFX_PARAM>&>( p );

	copy( param );
}

AnimatedParam<T,OFX_PARAM>* clone() const {
  return new AnimatedParam<T,OFX_PARAM>( *this );
}

/* ======= BEGIN OfxhKeyframeParam functions =======

Since this implementation is backed by a set, indexes may change and
they are no longer valid after a key frame is set or deleted.  */
void getNumKeys( unsigned int& outNumKeys ) const OFX_EXCEPTION_SPEC
{
  outNumKeys = _key_frames.size();
}
void getKeyTime( const int nth, OfxTime& outTime ) const OFX_EXCEPTION_SPEC {
  typename std::set< TimeValue<T> >::iterator it;
  int count;

  // Find the nth element
  for (count = 0,it  = _key_frames.begin();count<nth && it != _key_frames.end();count++,it++) {}
  if (it != _key_frames.end())
    outTime = it->time;
  else
    BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrBadIndex ) );    
}

void getKeyIndex( const OfxTime time, const int direction, int& outIndex ) const OFX_EXCEPTION_SPEC
{
    typename std::set< TimeValue<T> >::iterator it;
    size_t count = 0;

    for (it = _key_frames.begin(),count=0;it != _key_frames.end();count++,it++) {

      if (direction == 0) {
	// Find an exact match
	if (it->time == time) {
	  outIndex = count;
	  return;
	} else if (it->time > time) {
	  BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatFailed ) );    
	}

      } else if (direction < 0) {
	// Find the key frame directly before time
	if (it->time >= time) {
	  if (it == _key_frames.begin())
	    BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatFailed ) );
	  else {
	    outIndex = count-1;
	    return;
	  }
	}

      } else { // direction > 0
	// Find a key frame after time
	if (it->time > time) {
	  outIndex = count;
	  return;
	}
      }
    }
    BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatFailed ) );
}

void deleteKey( const OfxTime time ) OFX_EXCEPTION_SPEC
{
  TimeValue<T> temp;
  typename std::set< TimeValue<T> >::iterator it;
  
  temp.time = time;
  it = _key_frames.find(temp);
  if (it != _key_frames.end())
    _key_frames.erase(it);
  else
    BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrBadIndex ) );
}
void deleteAllKeys() OFX_EXCEPTION_SPEC
{
  _key_frames.clear();
}


};

/* ======= END OfxhKeyframeParam functions ======= */

typedef AnimatedParam<double,ofx::attribute::OfxhParamDouble> AnimatedParamDouble;
typedef AnimatedParam<int,ofx::attribute::OfxhParamInteger> AnimatedParamInteger;
}
}
}

#endif
