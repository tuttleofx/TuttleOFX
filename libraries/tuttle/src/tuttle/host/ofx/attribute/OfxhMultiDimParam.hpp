#ifndef _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_MULTIDIMPARAM_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <boost/program_options/parsers.hpp>

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

	void setValueFromExpression( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
	{
		std::vector<std::string> allExp = boost::program_options::split_unix( value, "," );

		if( allExp.size() == 1 )
		{
			for( std::size_t i = 0; i < getSize(); ++i )
			{
				_controls[i].setValueFromExpression( value, change );
			}
		}
		else
		{
			if( getSize() != allExp.size() )
			{
				BOOST_THROW_EXCEPTION( exception::Value()
					<< exception::user() + "Set " + allExp.size() + " values on a multidimensional parameter of size " + DIM + "."
					);
			}
			for( std::size_t i = 0; i < getSize(); ++i )
			{
				_controls[i].setValueFromExpression( allExp[i], change );
			}
		}
		this->paramChanged( change );
	}


protected:
	// Deriving implementatation needs to overide these
	inline virtual void getValueAtIndex( BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		BOOST_ASSERT( _controls.size() > index );
		_controls[index].getValue( dst );
	}

	inline virtual void getValueAtTimeAndIndex( const OfxTime time, BaseType& dst, const std::size_t index ) const OFX_EXCEPTION_SPEC
	{
		BOOST_ASSERT( _controls.size() > index );
		_controls[index].getValueAtTime( time, dst );
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
	
	inline void setValue( const BaseType& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t i = 0; i < getSize(); ++i )
		{
			_controls[i].setValue( value, eChangeNone );
		}
		this->paramChanged( change );
	}
	
	inline void setValueAtTime( const OfxTime time, const BaseType& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t i = 0; i < getSize(); ++i )
		{
			_controls[i].setValueAtTime( time, value, eChangeNone );
		}
		this->paramChanged( change );
	}

	inline virtual void setValueAtIndex( const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].setValue( value, eChangeNone );
		this->paramChanged( change );
	}

	inline virtual void setValueAtTimeAndIndex( const OfxTime time, const BaseType& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index].setValueAtTime( time, value, eChangeNone );
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
			_controls[index].getValue( *v );
		}
	}

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[index].getValueAtTime( time, *v );
		}
	}

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index].setValue( v, eChangeNone );
		}
		this->paramChanged( change );
	}

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
	{
		for( std::size_t index = 0; index < DIM; ++index )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[index].setValueAtTime( time, v, eChangeNone );
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

