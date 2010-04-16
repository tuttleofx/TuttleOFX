#ifndef _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <boost/ptr_container/ptr_vector.hpp>
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
	///@todo: IMPORTANT FIX: make this deleted, but be careful of the common descriptor (which must not be delete).
	std::vector<T*> _controls;

public:
	OfxhMultiDimParam( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance )
	{
		// _controls.reserve(DIM); /// @todo tuttle: find a way to lock the vector size to DIM...
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
	void copy( const OfxhMultiDimParam& p ) OFX_EXCEPTION_SPEC
	{
		for(std::size_t index = 0; index < DIM; ++index)
		{
			BaseType dst;
			p._controls[index]->get( dst );
			_controls[index]->set( dst, eChangeNone );
		}
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const OfxhMultiDimParam& param = dynamic_cast<const OfxhMultiDimParam&>(p);
		copy( param );
	}

	inline virtual void setAt( const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( value, eChangeNone );
		this->paramChanged( change );
	}

	inline virtual void setAt( const OfxTime time, const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( time, value, eChangeNone );
		this->paramChanged( change );
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
		for( int index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[index]->get( *v );
		}
	}

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[index]->get( time, *v );
		}
	}

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( int index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index]->set( v, eChangeNone );
		}
		this->paramChanged( change );
	}

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( int index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index]->set( time, v, eChangeNone );
		}
		this->paramChanged( change );
	}

	/// implementation of var args function
	virtual void deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[index]->derive( time, *v );
		}
	}

	/// implementation of var args function
	virtual void integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( int index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[index]->integrate( time1, time2, *v );
		}
	}

};

}
}
}
}


#endif

