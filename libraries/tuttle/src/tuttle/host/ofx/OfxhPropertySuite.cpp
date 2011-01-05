#include "OfxhPropertySuite.hpp"
#include "property/OfxhSet.hpp"

//#define DEBUG_PROPERTIES true

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

namespace {

/// static functions for the suite
template<class T>
OfxStatus propSet( OfxPropertySetHandle properties,
                   const char*          property,
                   int                  index,
                   typename T::APIType  value )
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "propSet - " << properties << " " << property << "[" << index << "] = " << value << " \n";
	#endif
	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		if( !thisSet->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhPropertyTemplate<T>& prop = thisSet->fetchLocalTypedProperty<OfxhPropertyTemplate<T> >( property );

		if( prop.getPluginReadOnly() )
		{
			TUTTLE_COUT_ERROR( "Plugin is trying to set a property (" << property << " at index " << index << ") which is read-only." );
			return kOfxStatErrValue;
		}

		prop.setValue( value, index, eModifiedByPlugin );

		//		#ifdef DEBUG_PROPERTIES
		//		std::cout << "propSet error !!! returning status kOfxStatErrUnknown" << std::endl;
		//		#endif
		//		return kOfxStatErrUnknown;
	}
	catch( OfxhException& e )
	{
		#ifdef DEBUG_PROPERTIES
		std::cout << " returning status " << e.getStatus() << "\n";
		#endif
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

/// static functions for the suite

template<class T>
OfxStatus propSetN( OfxPropertySetHandle properties,
                    const char*          property,
                    int                  count,
                    typename T::APIType* values )
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "propSetN - " << properties << " " << property << " \n";
	#endif

	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		if( !thisSet->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhPropertyTemplate<T>& prop = thisSet->fetchLocalTypedProperty<OfxhPropertyTemplate<T> >( property );

		if( prop.getPluginReadOnly() )
		{
			TUTTLE_COUT_ERROR( "Plugin is trying to set property " << property << " of size " << count << ") which is read-only." );
			return kOfxStatErrValue;
		}

		prop.setValueN( values, count, eModifiedByPlugin );
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

/// static functions for the suite

template<class T>
OfxStatus propGet( OfxPropertySetHandle          properties,
                   const char*                   property,
                   int                           index,
                   typename T::APITypeConstless* value )
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "propGet - " << properties << " " << property << " = ...";
	#endif

	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		if( !thisSet->verifyMagic() )
			return kOfxStatErrBadHandle;
		*value = thisSet->fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getAPIConstlessValue( index );
		//*value = castAwayConst( castToAPIType( prop->getValue( index ) ) );

		#ifdef DEBUG_PROPERTIES
		std::cout << *value << "\n";
		#endif
	}
	catch( OfxhException& e )
	{

		#ifdef DEBUG_PROPERTIES
		std::cout << "\n";
		#endif
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

///@todo tuttle: remove this !
inline int* castToConst( int* s )
{
	return s;
}

inline double* castToConst( double* s )
{
	return s;
}

inline void* * castToConst( void** s )
{
	return s;
}

///@todo tuttle: remove this !
inline const char* * castToConst( char** s )
{
	return const_cast<const char**>( s );
}

/// static functions for the suite

template<class T>
OfxStatus propGetN( OfxPropertySetHandle          properties,
                    const char*                   property,
                    int                           count,
                    typename T::APITypeConstless* values )
{
	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		if( !thisSet->verifyMagic() )
			return kOfxStatErrBadHandle;
		thisSet->fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getValueN( castToConst( values ), count );
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

/**
 * static functions for the suite
 */
OfxStatus propReset( OfxPropertySetHandle properties, const char* property )
{
	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		if( !thisSet->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhProperty& prop = thisSet->fetchLocalProperty( property );

		//		if( prop.getPluginReadOnly() )
		//		{
		//			TUTTLE_COUT_ERROR("Plugin is trying to reset a read-only property " << property );
		//			return kOfxStatErrValue;
		//		}

		prop.reset();

	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

/**
 * static functions for the suite
 */
OfxStatus propGetDimension( OfxPropertySetHandle properties, const char* property, int* count )
{
	try
	{
		OfxhSet* thisSet = reinterpret_cast<OfxhSet*>( properties );
		*count = thisSet->fetchProperty( property ).getDimension();
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch(... )
	{
		return kOfxStatErrUnknown;
	}
	return kOfxStatOK;
}

/**
 * the actual suite that is passed across the API to manage properties
 */
struct OfxPropertySuiteV1 gSuite =
{
	propSet<OfxhPointerValue>,
	propSet<OfxhStringValue>,
	propSet<OfxhDoubleValue>,
	propSet<OfxhIntValue>,
	propSetN<OfxhPointerValue>,
	propSetN<OfxhStringValue>,
	propSetN<OfxhDoubleValue>,
	propSetN<OfxhIntValue>,
	propGet<OfxhPointerValue>,
	propGet<OfxhStringValue>,
	propGet<OfxhDoubleValue>,
	propGet<OfxhIntValue>,
	propGetN<OfxhPointerValue>,
	propGetN<OfxhStringValue>,
	propGetN<OfxhDoubleValue>,
	propGetN<OfxhIntValue>,
	propReset,
	propGetDimension
};

}

/// return the OFX function suite that manages properties
void* getPropertySuite( const int version )
{
	if( version == 1 )
		return static_cast<void*>( &gSuite );
	return NULL;
}

}
}
}
}

