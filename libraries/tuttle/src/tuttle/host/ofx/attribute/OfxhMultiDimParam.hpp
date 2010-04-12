#ifndef _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <string>
#include <vector>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


template <class T, int DIM>
class OfxhMultiDimParam : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef T Type;
	typedef typename T::BaseType BaseType;

protected:
	std::vector<T*> _controls;

public:
	OfxhMultiDimParam( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance )
	{
	}

	virtual ~OfxhMultiDimParam()
	{
		// _controls are parameters which are declares in setInstance which owns all parameters
	}

	inline std::size_t getSize() const
	{
		return _controls.size();
	}

protected:
	// Deriving implementatation needs to overide these
	inline virtual void getAt( BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->get( dst );
	}

	inline virtual void getAt( const OfxTime time, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->get( time, dst );
	}

public:
	inline virtual void setAt( const BaseType& value, const std::size_t index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( value );
	}

	inline virtual void setAt( const OfxTime time, const BaseType& value, const std::size_t index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( time, value );
	}

	// derived class does not need to implement, default is an approximation
	inline virtual void deriveAt( const OfxTime time, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->derive( time, dst );
	}

	inline virtual void integrateAt( const OfxTime time1, const OfxTime time2, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->integrate( time1, time2, dst );
	}

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[i]->get( *v );
		}
	}

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[i]->get( time, *v );
		}
	}

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[i]->set( v );
		}
	}

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[i]->set( time, v );
		}
	}

	/// implementation of var args function
	virtual void deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[i]->derive( time, *v );
		}
	}

	/// implementation of var args function
	virtual void integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[i]->integrate( time1, time2, *v );
		}
	}

};

}
}
}
}


#endif

