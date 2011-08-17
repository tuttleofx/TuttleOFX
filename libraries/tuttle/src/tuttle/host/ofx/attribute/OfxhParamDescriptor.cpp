#include "OfxhParamDescriptor.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

struct TypeMap
{
	const char* paramType;
	property::TypeEnum propType;
	int propDimension;
};

bool isDoubleParam( const std::string& paramType )
{
	return paramType == kOfxParamTypeDouble ||
	       paramType == kOfxParamTypeDouble2D ||
	       paramType == kOfxParamTypeDouble3D;
}

bool isColourParam( const std::string& paramType )
{
	return
	    paramType == kOfxParamTypeRGBA ||
	    paramType == kOfxParamTypeRGB;
}

bool isIntParam( const std::string& paramType )
{
	return paramType == kOfxParamTypeInteger ||
	       paramType == kOfxParamTypeInteger2D ||
	       paramType == kOfxParamTypeInteger3D;
}

static TypeMap typeMap[] = {
	{ kOfxParamTypeInteger, property::eInt, 1 },
	{ kOfxParamTypeDouble, property::eDouble, 1 },
	{ kOfxParamTypeBoolean, property::eInt, 1 },
	{ kOfxParamTypeChoice, property::eInt, 1 },
	{ kOfxParamTypeRGBA, property::eDouble, 4 },
	{ kOfxParamTypeRGB, property::eDouble, 3 },
	{ kOfxParamTypeDouble2D, property::eDouble, 2 },
	{ kOfxParamTypeInteger2D, property::eInt, 2 },
	{ kOfxParamTypeDouble3D, property::eDouble, 3 },
	{ kOfxParamTypeInteger3D, property::eInt, 3 },
	{ kOfxParamTypeString, property::eString, 1 },
	{ kOfxParamTypeCustom, property::eString, 1 },
	{ kOfxParamTypeGroup, property::eNone },
	{ kOfxParamTypePage, property::eNone },
	{ kOfxParamTypePushButton, property::eNone },
	{ 0 }
};

/// is this a standard type
bool isStandardType( const std::string& type )
{
	TypeMap* tm = typeMap;

	while( tm->paramType )
	{
		if( tm->paramType == type )
			return true;
		++tm;
	}
	return false;
}

bool findType( const std::string paramType, property::TypeEnum& propType, int& propDim )
{
	TypeMap* tm = typeMap;

	while( tm->paramType )
	{
		if( tm->paramType == paramType )
		{
			propType = tm->propType;
			propDim  = tm->propDimension;
			return true;
		}
		++tm;
	}
	return false;
}

/**
 * @brief make a parameter, with the given type and name
 */
OfxhParamDescriptor::OfxhParamDescriptor( const std::string& type, const std::string& name )
	: attribute::OfxhAttributeDescriptor( property::OfxhSet() )
{
	const char* ctype = type.c_str();
	const char* cname = name.c_str();

	static const property::OfxhPropSpec paramDescriptorProps[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeParameter },
		{ kOfxParamPropSecret, property::eInt, 1, false, "0" },
		{ kOfxParamPropHint, property::eString, 1, false, "" },
		{ kOfxParamPropParent, property::eString, 1, false, "" },
		{ kOfxParamPropEnabled, property::eInt, 1, false, "1" },
		{ kOfxParamPropDataPtr, property::ePointer, 1, false, 0 },
		{ 0 }
	};

	const property::OfxhPropSpec dynamicParamDescriptorProps[] = {
		{ kOfxParamPropType, property::eString, 1, true, ctype },
		{ kOfxParamPropScriptName, property::eString, 1, false, cname }, ///< @todo TUTTLE_TODO : common property for all Attributes
		{ 0 }
	};

	getEditableProperties().addProperties( paramDescriptorProps );
	getEditableProperties().addProperties( dynamicParamDescriptorProps );

	setAllNames( name );

	getEditableProperties().setStringProperty( kOfxParamPropType, type );
	assert( ctype );
}

/**
 * make a parameter, with the given type and name
 */
void OfxhParamDescriptor::initStandardParamProps( const std::string& type )
{
	property::TypeEnum propType = property::eString;
	int propDim                 = 1;

	findType( type, propType, propDim );


	if( propType != property::eNone )
		initValueParamProps( type, propType, propDim );
	else
		initNoValueParamProps();

	if( type == kOfxParamTypeString )
	{
		static const property::OfxhPropSpec allString[] = {
			{ kOfxParamPropStringMode, property::eString, 1, false, kOfxParamStringIsSingleLine },
			{ kOfxParamPropStringFilePathExists, property::eInt, 1, false, "1" },
			{ 0 }
		};

		getEditableProperties().addProperties( allString );
	}

	if( isDoubleParam( type ) || isIntParam( type ) || isColourParam( type ) )
	{
		initNumericParamProps( type, propType, propDim );
	}

	if( type != kOfxParamTypeGroup && type != kOfxParamTypePage )
	{
		initInteractParamProps( type );
	}

	if( type == kOfxParamTypeChoice )
	{
		static const property::OfxhPropSpec allChoice[] = {
			{ kOfxParamPropChoiceOption, property::eString, 0, false, "" },
			{ 0 }
		};

		getEditableProperties().addProperties( allChoice );
	}
	else if( type == kOfxParamTypeCustom )
	{
		static const property::OfxhPropSpec allCustom[] = {
			{ kOfxParamPropCustomInterpCallbackV1, property::ePointer, 1, false, 0 },
			{ 0 },
		};

		getEditableProperties().addProperties( allCustom );
	}
	else if( type == kOfxParamTypePage )
	{
		static const property::OfxhPropSpec allPage[] = {
			{ kOfxParamPropPageChild, property::eString, 0, false, "" },
			{ 0 }
		};
		getEditableProperties().addProperties( allPage );
	}
	else if( type == kOfxParamTypeGroup )
	{
		static const property::OfxhPropSpec allGroup[] = {
			{ kOfxParamPropGroupOpen, property::eInt, 1, false, "1" },
			{ 0 }
		};

		getEditableProperties().addProperties( allGroup );
	}
}

/**
 * add standard properties to a params that can take an interact
 */
void OfxhParamDescriptor::initInteractParamProps( const std::string& type )
{
	static const property::OfxhPropSpec allButGroupPageProps[] = {
		{ kOfxParamPropInteractV1, property::ePointer, 1, false, 0 },
		{ kOfxParamPropInteractSize, property::eDouble, 2, false, "0" },
		{ kOfxParamPropInteractSizeAspect, property::eDouble, 1, false, "1" },
		{ kOfxParamPropInteractMinimumSize, property::eInt, 2, false, "10" },
		{ kOfxParamPropInteractPreferedSize, property::eInt, 2, false, "10" },
		{ 0 }
	};

	getEditableProperties().addProperties( allButGroupPageProps );
}

/**
 * add standard properties to a value holding param
 */
void OfxhParamDescriptor::initValueParamProps( const std::string& type, property::TypeEnum valueType, int dim )
{
	static const property::OfxhPropSpec invariantProps[] = {
		{ kOfxParamPropAnimates, property::eInt, 1, false, "1" },
		{ kOfxParamPropIsAnimating, property::eInt, 1, false, "0" },
		{ kOfxParamPropIsAutoKeying, property::eInt, 1, false, "0" },
		{ kOfxParamPropPersistant, property::eInt, 1, false, "1" },
		{ kOfxParamPropEvaluateOnChange, property::eInt, 1, false, "1" },
		{ kOfxParamPropPluginMayWrite, property::eInt, 1, false, "0" },
		{ kOfxParamPropCanUndo, property::eInt, 1, false, "1" },
		{ kOfxParamPropCacheInvalidation, property::eString, 1, false, kOfxParamInvalidateValueChange },
		{ 0 }
	};

	property::OfxhPropSpec variantProps[] = {
		{ kOfxParamPropDefault, valueType, dim, false, valueType == property::eString ? "" : "0" },
		{ 0 }
	};

	getEditableProperties().addProperties( invariantProps );
	getEditableProperties().addProperties( variantProps );
}

void OfxhParamDescriptor::initNoValueParamProps()
{
	static const property::OfxhPropSpec invariantProps[] = {
		{ kOfxParamPropAnimates, property::eInt, 1, false, "0" },
		{ kOfxParamPropIsAnimating, property::eInt, 1, false, "0" },
		{ kOfxParamPropIsAutoKeying, property::eInt, 1, false, "0" },
		{ kOfxParamPropPersistant, property::eInt, 1, false, "0" },
		{ kOfxParamPropEvaluateOnChange, property::eInt, 1, false, "0" },
		{ kOfxParamPropPluginMayWrite, property::eInt, 1, false, "0" },
		{ kOfxParamPropCanUndo, property::eInt, 1, false, "0" },
		{ kOfxParamPropCacheInvalidation, property::eString, 1, false, "" },
		{ 0 }
	};

	getEditableProperties().addProperties( invariantProps );
}

/**
 * add standard properties to a value holding param
 */
void OfxhParamDescriptor::initNumericParamProps( const std::string& type, property::TypeEnum valueType, int dim )
{
	static std::string dbl_minstr, dbl_maxstr, int_minstr, int_maxstr;
	bool doneOne = false;

	if( !doneOne )
	{
		// Needed for msvc compilator
		using namespace std;

		std::ostringstream dbl_min, dbl_max, int_min, int_max;
		doneOne = true;
		dbl_min << -numeric_limits<double>::max();
		dbl_max << numeric_limits<double>::max();
		int_min << numeric_limits<int>::min();
		int_max << numeric_limits<int>::max();

		dbl_minstr = dbl_min.str();
		dbl_maxstr = dbl_max.str();
		int_minstr = int_min.str();
		int_maxstr = int_max.str();
	}

	property::OfxhPropSpec allNumeric[] = {
		{ kOfxParamPropDisplayMin, valueType, dim, false, isColourParam( type ) ? "0" : ( valueType == property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropDisplayMax, valueType, dim, false, isColourParam( type ) ? "1" : ( valueType == property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ kOfxParamPropMin, valueType, dim, false, ( valueType == property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropMax, valueType, dim, false, ( valueType == property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ 0 }
	};

	getEditableProperties().addProperties( allNumeric );

	/// if any double or a colour
	if( valueType == property::eDouble )
	{
		static const property::OfxhPropSpec allDouble[] = {
			{ kOfxParamPropIncrement, property::eDouble, 1, false, "1" },
			{ kOfxParamPropDigits, property::eInt, 1, false, "2" },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );
	}

	/// if a double param type
	if( isDoubleParam( type ) )
	{
		static const property::OfxhPropSpec allDouble[] = {
			{ kOfxParamPropDoubleType, property::eString, 1, false, kOfxParamDoubleTypePlain },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );

		if( dim == 1 )
		{
			static const property::OfxhPropSpec allDouble1D[] = {
				{ kOfxParamPropShowTimeMarker, property::eInt, 1, false, "0" },
				{ 0 }
			};

			getEditableProperties().addProperties( allDouble1D );
		}
	}

	/// if a multi dimensional param
	if( isDoubleParam( type ) && ( dim == 2 || dim == 3 ) )
	{
		property::OfxhPropSpec all2D3D[] = {
			{ kOfxParamPropDimensionLabel, property::eString, dim, false, "" },
			{ 0 },
		};

		getEditableProperties().addProperties( all2D3D );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "X", 0 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "Y", 1 );
		if( dim == 3 )
		{
			getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "Z", 2 );
		}
	}

	/// if a multi dimensional param
	if( isColourParam( type ) )
	{
		property::OfxhPropSpec allColor[] = {
			{ kOfxParamPropDimensionLabel, property::eString, dim, false, "" },
			{ 0 },
		};

		getEditableProperties().addProperties( allColor );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "R", 0 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "G", 1 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "B", 2 );
		if( dim == 4 )
		{
			getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "A", 3 );
		}
	}
}

}
}
}
}
