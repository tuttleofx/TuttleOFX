#ifndef _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <boost/ptr_container/ptr_array.hpp>
#include <string>
#include <vector>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

template <class T, std::size_t DIM>
class OfxhMultiDimParam : public OfxhParam
	, public OfxhKeyframeParam
{
public:
	typedef T Type;
	typedef typename T::BaseType BaseType;

protected:
	boost::ptr_array<T, DIM> _controls; // owns the sub-parameters

public:
	OfxhMultiDimParam( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance )
	{}

	virtual ~OfxhMultiDimParam()
	{}

	inline std::size_t getSize() const
	{
		return _controls.size();
	}

protected:
	// Deriving implementatation needs to overide these
	inline virtual void getAtIndex( BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].get( dst );
	}

	inline virtual void getAtTimeAndIndex( const OfxTime time, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].getAtTime( time, dst );
	}

public:
	void copy( const OfxhMultiDimParam& p ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			_controls[index].copy( p._controls[index] );
		}
	}

	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const OfxhMultiDimParam& param = dynamic_cast<const OfxhMultiDimParam&>( p );

		copy( param );
	}

	inline virtual void setAtIndex( const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].set( value, eChangeNone );
		this->paramChanged( change );
	}

	inline virtual void setAtTimeAndIndex( const OfxTime time, const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].setAtTime( time, value, eChangeNone );
		this->paramChanged( change );
	}

	// derived class does not need to implement, default is an approximation
	inline virtual void deriveAtIndex( const OfxTime time, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].derive( time, dst );
	}

	inline virtual void integrateAtIndex( const OfxTime time1, const OfxTime time2, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].integrate( time1, time2, dst );
	}

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[index].get( *v );
		}
	}

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[index].getAtTime( time, *v );
		}
	}

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index].set( v, eChangeNone );
		}
		this->paramChanged( change );
	}

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index].setAtTime( time, v, eChangeNone );
		}
		this->paramChanged( change );
	}

	/// implementation of var args function
	virtual void deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[index].derive( time, *v );
		}
	}

	/// implementation of var args function
	virtual void integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[index].integrate( time1, time2, *v );
		}
	}

	std::ostream& displayValues( std::ostream& os ) const
	{
		BOOST_STATIC_ASSERT( DIM != 0 );
		os << "[";
		for( std::size_t index = 0; index < DIM-1; ++index )
		{
			_controls[index].displayValues( os );
			os << ",";
		}
		_controls[DIM-1].displayValues( os );
		os << "]";
		return os;
	}

};

}
}
}
}

#endif

