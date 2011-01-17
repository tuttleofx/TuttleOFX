/*
 * OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
 * Copyright (C) 2004-2005 The Open Effects Association Ltd
 * Author Bruno Nicoletti bruno@thefoundry.co.uk
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The Open Effects Association Ltd
 * 1 Wardour St
 * London W1D 6PA
 * England
 *
 *
 */

/** @brief This file contains code that skins the ofx param suite */

#include "ofxsSupportPrivate.h"
#include "ofxsUtilities.h"
#include "ofxParametricParam.h"
#include "extensions/nuke/camera.h"
#include <cstring>

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries. */
namespace OFX {

/** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
DummyParamDescriptor PageParamDescriptor::gSkipRow( kOfxParamPageSkipRow );

/** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
DummyParamDescriptor PageParamDescriptor::gSkipColumn( kOfxParamPageSkipColumn );

/** @brief turns a ParamTypeEnum into the char * that raw OFX uses */
const char* mapParamTypeEnumToString( ParamTypeEnum v )
{
	switch( v )
	{
		case eStringParam: return kOfxParamTypeString ;
		case eIntParam: return kOfxParamTypeInteger ;
		case eInt2DParam: return kOfxParamTypeInteger2D ;
		case eInt3DParam: return kOfxParamTypeInteger3D ;
		case eDoubleParam: return kOfxParamTypeDouble ;
		case eDouble2DParam: return kOfxParamTypeDouble2D ;
		case eDouble3DParam: return kOfxParamTypeDouble3D ;
		case eRGBParam: return kOfxParamTypeRGB ;
		case eRGBAParam: return kOfxParamTypeRGBA ;
		case eBooleanParam: return kOfxParamTypeBoolean ;
		case eChoiceParam: return kOfxParamTypeChoice ;
		case eCustomParam: return kOfxParamTypeCustom ;
		case eGroupParam: return kOfxParamTypeGroup ;
		case ePageParam: return kOfxParamTypePage ;
		case ePushButtonParam: return kOfxParamTypePushButton ;
		case eParametricParam: return kOfxParamTypeParametric ;
		default: assert( false );
	}
	return kOfxParamTypeInteger;
}

bool isEqual( const char* t1, const char* t2 )
{
	return strcmp( t1, t2 ) == 0;
}

ParamTypeEnum mapParamTypeStringToEnum( const char* v )
{
	if( isEqual( kOfxParamTypeString, v ) )
		return eStringParam ;
	else if( isEqual( kOfxParamTypeInteger, v ) )
		return eIntParam ;
	else if( isEqual( kOfxParamTypeInteger2D, v ) )
		return eInt2DParam ;
	else if( isEqual( kOfxParamTypeInteger3D, v ) )
		return eInt3DParam ;
	else if( isEqual( kOfxParamTypeDouble, v ) )
		return eDoubleParam ;
	else if( isEqual( kOfxParamTypeDouble2D, v ) )
		return eDouble2DParam ;
	else if( isEqual( kOfxParamTypeDouble3D, v ) )
		return eDouble3DParam ;
	else if( isEqual( kOfxParamTypeRGB, v ) )
		return eRGBParam ;
	else if( isEqual( kOfxParamTypeRGBA, v ) )
		return eRGBAParam ;
	else if( isEqual( kOfxParamTypeBoolean, v ) )
		return eBooleanParam ;
	else if( isEqual( kOfxParamTypeChoice, v ) )
		return eChoiceParam ;
	else if( isEqual( kOfxParamTypeCustom, v ) )
		return eCustomParam ;
	else if( isEqual( kOfxParamTypeGroup, v ) )
		return eGroupParam ;
	else if( isEqual( kOfxParamTypePage, v ) )
		return ePageParam ;
	else if( isEqual( kOfxParamTypePushButton, v ) )
		return ePushButtonParam ;
	else if( isEqual( kOfxParamTypeParametric, v ) )
		return eParametricParam ;
	else
		assert( false );
	return ePushButtonParam ;
}

////////////////////////////////////////////////////////////////////////////////
// the base class for all param descriptors

/** @brief ctor */
ParamDescriptor::ParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props )
	: _paramName( name ),
	_paramType( type ),
	_paramProps( props )
{
	// validate the properities on this descriptor
	if( type != eDummyParam )
		OFX::Validation::validateParameterProperties( type, getProps(), true );
}

ParamDescriptor::~ParamDescriptor()
{}

/** @brief set the label properties */
void ParamDescriptor::setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel )
{
	getProps().propSetString( kOfxPropLabel, label );
	getProps().propSetString( kOfxPropShortLabel, shortLabel, false );
	getProps().propSetString( kOfxPropLongLabel, longLabel, false );
}

/** @brief set the param hint */
void ParamDescriptor::setHint( const std::string& v )
{
	getProps().propSetString( kOfxParamPropHint, v, false );
}

/** @brief set the script name, default is the name it was defined with */
void ParamDescriptor::setScriptName( const std::string& v )
{
	getProps().propSetString( kOfxParamPropScriptName, v, false );
}

/** @brief set the secretness of the param, defaults to false */
void ParamDescriptor::setIsSecret( bool v )
{
	getProps().propSetInt( kOfxParamPropSecret, v );
}

/** @brief set the secretness of the param, defaults to false */
void ParamDescriptor::setEnabled( bool v )
{
	getProps().propSetInt( kOfxParamPropEnabled, v );
}

/** @brief set the group param that is the parent of this one, default is to be ungrouped at the root level */
void ParamDescriptor::setParent( const GroupParamDescriptor& v )
{
	getProps().propSetString( kOfxParamPropParent, v.getName() );
}

////////////////////////////////////////////////////////////////////////////////
// the base class for all params that can hold a value

/** @brief ctor */
ValueParamDescriptor::ValueParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props )
	: ParamDescriptor( name, type, props )
{}

/** @brief dtor */
ValueParamDescriptor::~ValueParamDescriptor()
{}

/** @brief set whether the param can animate, defaults to true in most cases */
void ValueParamDescriptor::setAnimates( bool v )
{
	getProps().propSetInt( kOfxParamPropAnimates, v );
}

/** @brief set whether the param is persistant, defaults to true */
void ValueParamDescriptor::setIsPersistant( bool v )
{
	getProps().propSetInt( kOfxParamPropPersistant, v );
}

/** @brief Set's whether the value of the param is significant (ie: affects the rendered image), defaults to true */
void ValueParamDescriptor::setEvaluateOnChange( bool v )
{
	getProps().propSetInt( kOfxParamPropEvaluateOnChange, v );
}

/** @brief Set's whether the value of the param is significant (ie: affects the rendered image), defaults to true */
void ValueParamDescriptor::setCanUndo( bool v )
{
	getProps().propSetInt( kOfxParamPropCanUndo, v, 0, false );
}

/** @brief Set's how any cache should be invalidated if the parameter is changed, defaults to eCacheInvalidateValueChange */
void ValueParamDescriptor::setCacheInvalidation( CacheInvalidationEnum v )
{
	switch( v )
	{
		case eCacheInvalidateValueChange:
			getProps().propSetString( kOfxParamPropCacheInvalidation, kOfxParamInvalidateValueChange );
			break;

		case eCacheInvalidateValueChangeToEnd:
			getProps().propSetString( kOfxParamPropCacheInvalidation, kOfxParamInvalidateValueChangeToEnd );
			break;

		case eCacheInvalidateValueAll:
			getProps().propSetString( kOfxParamPropCacheInvalidation, kOfxParamInvalidateAll );
			break;
	}
}

void ValueParamDescriptor::setInteractDescriptor( ParamInteractWrap* desc)
{
	_interact.reset( desc );
	getProps().propSetPointer( kOfxParamPropInteractV1, (void*)desc->getMainEntry() );
    desc->getDescriptor().setParamName(getName());
}

////////////////////////////////////////////////////////////////////////////////
// int param descriptor

/** @brief ctor */
IntParamDescriptor::IntParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eIntParam, props )
{}

/** @brief set the default value, default is 0 */
void IntParamDescriptor::setDefault( int v )
{
	getProps().propSetInt( kOfxParamPropDefault, v );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void IntParamDescriptor::setRange( int min, int max )
{
	getProps().propSetInt( kOfxParamPropMin, min );
	getProps().propSetInt( kOfxParamPropMax, max );
}

/** @brief set the display min and max, default is to be the same as the range param */
void IntParamDescriptor::setDisplayRange( int min, int max )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, min );
	getProps().propSetInt( kOfxParamPropDisplayMax, max );
}

////////////////////////////////////////////////////////////////////////////////
// 2D int param descriptor

/** @brief ctor */
Int2DParamDescriptor::Int2DParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eInt2DParam, props )
{}

/** @brief set the default value, default is 0 */
void Int2DParamDescriptor::setDefault( int x, int y )
{
	getProps().propSetInt( kOfxParamPropDefault, x, 0 );
	getProps().propSetInt( kOfxParamPropDefault, y, 1 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int2DParamDescriptor::setRange( int xmin, int ymin,
                                     int xmax, int ymax )
{
	getProps().propSetInt( kOfxParamPropMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropMax, ymax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int2DParamDescriptor::setDisplayRange( int xmin, int ymin,
                                            int xmax, int ymax )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMax, ymax, 1 );
}

void Int2DParamDescriptor::setDimensionLabels( const std::string& x, const std::string& y )
{
	getProps().propSetString( kOfxParamPropDimensionLabel, x, 0 );
	getProps().propSetString( kOfxParamPropDimensionLabel, y, 1 );
}

////////////////////////////////////////////////////////////////////////////////
// 3D int param descriptor

/** @brief ctor */
Int3DParamDescriptor::Int3DParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eInt3DParam, props )
{}

/** @brief set the default value, default is 0 */
void Int3DParamDescriptor::setDefault( int x, int y, int z )
{
	getProps().propSetInt( kOfxParamPropDefault, x, 0 );
	getProps().propSetInt( kOfxParamPropDefault, y, 1 );
	getProps().propSetInt( kOfxParamPropDefault, z, 2 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int3DParamDescriptor::setRange( int xmin, int ymin, int zmin,
                                     int xmax, int ymax, int zmax )
{
	getProps().propSetInt( kOfxParamPropMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropMin, zmin, 2 );
	getProps().propSetInt( kOfxParamPropMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropMax, ymax, 1 );
	getProps().propSetInt( kOfxParamPropMax, zmax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int3DParamDescriptor::setDisplayRange( int xmin, int ymin, int zmin,
                                            int xmax, int ymax, int zmax )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMin, zmin, 2 );
	getProps().propSetInt( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMax, ymax, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMax, zmax, 2 );
}

void Int3DParamDescriptor::setDimensionLabels( const std::string& x, const std::string& y, const std::string& z )
{
	getProps().propSetString( kOfxParamPropDimensionLabel, x, 0 );
	getProps().propSetString( kOfxParamPropDimensionLabel, y, 1 );
	getProps().propSetString( kOfxParamPropDimensionLabel, z, 2 );
}

////////////////////////////////////////////////////////////////////////////////
// base class for all double param descriptors

/** @brief hidden constructor */
BaseDoubleParamDescriptor::BaseDoubleParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, type, props )
{}

/** @brief set the type of the double param, defaults to eDoubleTypePlain */
void BaseDoubleParamDescriptor::setDoubleType( DoubleTypeEnum v )
{
	switch( v )
	{
		case eDoubleTypePlain:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypePlain );
			break;
		case eDoubleTypeAngle:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeAngle );
			break;
		case eDoubleTypeScale:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeScale );
			break;
		case eDoubleTypeTime:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeTime );
			break;
		case eDoubleTypeAbsoluteTime:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeAbsoluteTime );
			break;
		case eDoubleTypeNormalisedX:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedX );
			break;
		case eDoubleTypeNormalisedY:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedY );
			break;
		case eDoubleTypeNormalisedXAbsolute:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXAbsolute );
			break;
		case eDoubleTypeNormalisedYAbsolute:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedYAbsolute );
			break;
		case eDoubleTypeNormalisedXY:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXY );
			break;
		case eDoubleTypeNormalisedXYAbsolute:
			getProps().propSetString( kOfxParamPropDoubleType, kOfxParamDoubleTypeNormalisedXYAbsolute );
			break;
	}
}

/** @brief set the sensitivity of any gui slider */
void BaseDoubleParamDescriptor::setIncrement( double v )
{
	getProps().propSetDouble( kOfxParamPropIncrement, v );
}

/** @brief set the number of digits printed after a decimal point in any gui */
void BaseDoubleParamDescriptor::setDigits( int v )
{
	getProps().propSetInt( kOfxParamPropDigits, v );
}

////////////////////////////////////////////////////////////////////////////////
// double param descriptor

/** @brief ctor */
DoubleParamDescriptor::DoubleParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: BaseDoubleParamDescriptor( name, eDoubleParam, props )
{}

/** @brief set the default value, default is 0 */
void DoubleParamDescriptor::setDefault( double v )
{
	getProps().propSetDouble( kOfxParamPropDefault, v );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void DoubleParamDescriptor::setRange( double min, double max )
{
	getProps().propSetDouble( kOfxParamPropMin, min );
	getProps().propSetDouble( kOfxParamPropMax, max );
}

/** @brief set the display min and max, default is to be the same as the range param */
void DoubleParamDescriptor::setDisplayRange( double min, double max )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, min );
	getProps().propSetDouble( kOfxParamPropDisplayMax, max );
}

////////////////////////////////////////////////////////////////////////////////
// 2D double param descriptor

/** @brief ctor */
Double2DParamDescriptor::Double2DParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: BaseDoubleParamDescriptor( name, eDouble2DParam, props )
{}

/** @brief set the default value, default is 0 */
void Double2DParamDescriptor::setDefault( double x, double y )
{
	getProps().propSetDouble( kOfxParamPropDefault, x, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, y, 1 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double2DParamDescriptor::setRange( double xmin, double ymin,
                                        double xmax, double ymax )
{
	getProps().propSetDouble( kOfxParamPropMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropMax, ymax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double2DParamDescriptor::setDisplayRange( double xmin, double ymin,
                                               double xmax, double ymax )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, ymax, 1 );
}

void Double2DParamDescriptor::setDimensionLabels( const std::string& x, const std::string& y )
{
	getProps().propSetString( kOfxParamPropDimensionLabel, x, 0 );
	getProps().propSetString( kOfxParamPropDimensionLabel, y, 1 );
}

////////////////////////////////////////////////////////////////////////////////
// 3D double param descriptor

/** @brief ctor */
Double3DParamDescriptor::Double3DParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: BaseDoubleParamDescriptor( name, eDouble3DParam, props )
{}

/** @brief set the default value, default is 0 */
void Double3DParamDescriptor::setDefault( double x, double y, double z )
{
	getProps().propSetDouble( kOfxParamPropDefault, x, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, y, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, z, 2 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double3DParamDescriptor::setRange( double xmin, double ymin, double zmin,
                                        double xmax, double ymax, double zmax )
{
	getProps().propSetDouble( kOfxParamPropMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropMin, zmin, 2 );
	getProps().propSetDouble( kOfxParamPropMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropMax, ymax, 1 );
	getProps().propSetDouble( kOfxParamPropMax, zmax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double3DParamDescriptor::setDisplayRange( double xmin, double ymin, double zmin,
                                               double xmax, double ymax, double zmax )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, zmin, 2 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, ymax, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, zmax, 2 );
}

void Double3DParamDescriptor::setDimensionLabels( const std::string& x, const std::string& y, const std::string& z )
{
	getProps().propSetString( kOfxParamPropDimensionLabel, x, 0 );
	getProps().propSetString( kOfxParamPropDimensionLabel, y, 1 );
	getProps().propSetString( kOfxParamPropDimensionLabel, z, 2 );
}

////////////////////////////////////////////////////////////////////////////////
// RGB param descriptor

/** @brief hidden constructor */
RGBParamDescriptor::RGBParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eRGBParam, props )
{}

/** @brief set the default value */
void RGBParamDescriptor::setDefault( double r, double g, double b )
{
	getProps().propSetDouble( kOfxParamPropDefault, r, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, g, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, b, 2 );
}

////////////////////////////////////////////////////////////////////////////////
// RGBA param descriptor

/** @brief hidden constructor */
RGBAParamDescriptor::RGBAParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eRGBAParam, props )
{}

/** @brief set the default value */
void RGBAParamDescriptor::setDefault( double r, double g, double b, double a )
{
	getProps().propSetDouble( kOfxParamPropDefault, r, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, g, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, b, 2 );
	getProps().propSetDouble( kOfxParamPropDefault, a, 3 );
}

////////////////////////////////////////////////////////////////////////////////
// bool param descriptor

/** @brief hidden constructor */
BooleanParamDescriptor::BooleanParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eBooleanParam, props )
{}

/** @brief set the default value */
void BooleanParamDescriptor::setDefault( bool v )
{
	getProps().propSetInt( kOfxParamPropDefault, int(v) );
}

////////////////////////////////////////////////////////////////////////////////
// choice param descriptor

/** @brief hidden constructor */
ChoiceParamDescriptor::ChoiceParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eChoiceParam, props )
{}

/** @brief set the default value */
void ChoiceParamDescriptor::setDefault( int v )
{
	getProps().propSetInt( kOfxParamPropDefault, v );
}

/** @brief how many options do we have */
int ChoiceParamDescriptor::getNOptions( void ) const
{
	int nCurrentValues = getProps().propGetDimension( kOfxParamPropChoiceOption );

	return nCurrentValues;
}

/** @brief set the default value */
void ChoiceParamDescriptor::appendOption( const std::string& v )
{
	int nCurrentValues = getProps().propGetDimension( kOfxParamPropChoiceOption );

	getProps().propSetString( kOfxParamPropChoiceOption, v, nCurrentValues );
}

/** @brief set the default value */
void ChoiceParamDescriptor::resetOptions( void )
{
	getProps().propReset( kOfxParamPropChoiceOption );
}

////////////////////////////////////////////////////////////////////////////////
// string param descriptor

/** @brief hidden ctor */
StringParamDescriptor::StringParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eStringParam, props )
{}

/** @brief set the default value, default is 0 */
void StringParamDescriptor::setDefault( const std::string& v )
{
	getProps().propSetString( kOfxParamPropDefault, v );
}

/** @brief sets the kind of the string param, defaults to eStringSingleLine */
void StringParamDescriptor::setStringType( StringTypeEnum v )
{
	switch( v )
	{
		case eStringTypeSingleLine:
			getProps().propSetString( kOfxParamPropStringMode,  kOfxParamStringIsSingleLine );
			break;
		case eStringTypeMultiLine:
			getProps().propSetString( kOfxParamPropStringMode,  kOfxParamStringIsMultiLine );
			break;
		case eStringTypeFilePath:
			getProps().propSetString( kOfxParamPropStringMode,  kOfxParamStringIsFilePath );
			break;
		case eStringTypeDirectoryPath:
			getProps().propSetString( kOfxParamPropStringMode,  kOfxParamStringIsDirectoryPath );
			break;
		case eStringTypeLabel:
			getProps().propSetString( kOfxParamPropStringMode,  kOfxParamStringIsLabel );
			break;
	}
}

/** @brief if the string param is a file path, say that we are picking an existing file, defaults to true */
void StringParamDescriptor::setFilePathExists( bool v )
{
	getProps().propSetInt( kOfxParamPropStringFilePathExists, int(v) );
}

////////////////////////////////////////////////////////////////////////////////
// custom param descriptor

/** @brief hidden ctor */
CustomParamDescriptor::CustomParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ValueParamDescriptor( name, eCustomParam, props )
{}

/** @brief set the default value, default is 0 */
void CustomParamDescriptor::setDefault( const std::string& v )
{
	getProps().propSetString( kOfxParamPropDefault, v );
}

////////////////////////////////////////////////////////////////////////////////
// group param descriptor

/** @brief hidden constructor */
GroupParamDescriptor::GroupParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ParamDescriptor( name, eGroupParam, props )
{}

void GroupParamDescriptor::setOpen( const bool open )
{
	getProps().propSetInt( kOfxParamPropGroupOpen, open );
}

////////////////////////////////////////////////////////////////////////////////
// page param descriptor

/** @brief hidden constructor */
PageParamDescriptor::PageParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ParamDescriptor( name, ePageParam, props )
{}

/** @brief adds a child parameter. Note the two existing pseudo params, gColumnSkip  and gRowSkip */
void PageParamDescriptor::addChild( const ParamDescriptor& p )
{
	int nKids = getProps().propGetDimension( kOfxParamPropPageChild );

	getProps().propSetString( kOfxParamPropPageChild, p.getName(), nKids );
}

////////////////////////////////////////////////////////////////////////////////
// pushbutton param descriptor

/** @brief hidden constructor */
PushButtonParamDescriptor::PushButtonParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ParamDescriptor( name, ePushButtonParam, props )
{}

////////////////////////////////////////////////////////////////////////////////
// parametric param descriptor

/** @brief hidden constructor */
ParametricParamDescriptor::ParametricParamDescriptor( const std::string& name, OfxPropertySetHandle props )
	: ParamDescriptor( name, eParametricParam, props )
{
}

void ParametricParamDescriptor::setParamSet( ParamSetDescriptor& paramSet )
{
	_paramSet = &paramSet;
	OFX::Private::gParamSuite->paramGetHandle( _paramSet->getOfxParamSetHandle(), getName().c_str(), &_ofxParamHandle, NULL );
}

void ParametricParamDescriptor::setRange( const double min, const double max )
{
	getProps().propSetDouble( kOfxParamPropParametricRange, min, 0 );
	getProps().propSetDouble( kOfxParamPropParametricRange, max, 1 );
}

void ParametricParamDescriptor::setDimension( const int dimension )
{
	getProps().propSetInt( kOfxParamPropParametricDimension, dimension );
}

void ParametricParamDescriptor::setLabel( const std::string label )
{
	getProps().propSetString( kOfxPropLabel, label );
}

void ParametricParamDescriptor::setDimensionLabel( const std::string label, const int id )
{
	getProps().propSetString( kOfxParamPropDimensionLabel, label, id );
}

void ParametricParamDescriptor::setUIColour( const int id, const OfxRGBColourD color )
{
	getProps().propSetDouble( kOfxParamPropParametricUIColour, color.r, id*3 + 0 );
	getProps().propSetDouble( kOfxParamPropParametricUIColour, color.g, id*3 + 1 );
	getProps().propSetDouble( kOfxParamPropParametricUIColour, color.b, id*3 + 2 );
}

void ParametricParamDescriptor::addControlPoint( const int id, const OfxTime time, const double x, const double y, const bool addKey )
{
	OFX::Private::gParametricParameterSuite->parametricParamAddControlPoint(
		_ofxParamHandle,
		id,
		time,
		x,
		y,
		addKey );
}

void ParametricParamDescriptor::setIdentity( const int id )
{
	addControlPoint( id, 0, 0, 0, false );
	addControlPoint( id, 0, 1, 1, false );
}

void ParametricParamDescriptor::setIdentity()
{
	const int nbCurves = getProps().propGetInt( kOfxParamPropParametricDimension );
	for( int i = 0; i < nbCurves; ++i )
	{
		setIdentity( i );
	}
}

void ParametricParamDescriptor::setInteractDescriptor( ParamInteractWrap* desc )
{
	_interact.reset( desc );
	getProps().propSetPointer( kOfxParamPropParametricInteractBackground, (void*)desc->getMainEntry() );
    desc->getDescriptor().setParamName( getName() );
}

////////////////////////////////////////////////////////////////////////////////
// Descriptor for a set of parameters
/** @brief hidden ctor */
ParamSetDescriptor::ParamSetDescriptor( void )
	: _paramSetHandle( 0 )
{}

ParamDescriptor* ParamSetDescriptor::getParamDescriptor( const std::string& name ) const
{
	std::map<std::string, ParamDescriptor*>::const_iterator it = _definedParams.find( name );
	if( it != _definedParams.end() )
		return it->second;
	return 0;
}

/** @brief set the param set handle */
void ParamSetDescriptor::setOfxParamSetHandle( OfxParamSetHandle h )
{
	// set me handle
	_paramSetHandle = h;

	if( h )
	{
		// fetch me props
		OfxPropertySetHandle props;
		OfxStatus stat = OFX::Private::gParamSuite->paramSetGetPropertySet( h, &props );
		_paramSetProps.propSetHandle( props );
		throwSuiteStatusException( stat );
	}
	else
	{
		_paramSetProps.propSetHandle( 0 );
	}
}

/** @brief dtor */
ParamSetDescriptor::~ParamSetDescriptor()
{
	// delete any descriptor we may have constructed
	std::map<std::string, ParamDescriptor*>::iterator iter;
	for( iter = _definedParams.begin(); iter != _definedParams.end(); ++iter )
	{
		if( iter->second )
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
}

/** @brief estabilishes the order of page params. Do it by calling it in turn for each page */
void ParamSetDescriptor::setPageParamOrder( PageParamDescriptor& p )
{
	int nPages = _paramSetProps.propGetDimension( kOfxPluginPropParamPageOrder );

	_paramSetProps.propSetString( kOfxPluginPropParamPageOrder, p.getName().c_str(), nPages );
}

/** @brief calls the raw OFX routine to define a param */
void ParamSetDescriptor::defineRawParam( const std::string& name, ParamTypeEnum paramType, OfxPropertySetHandle& props )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramDefine( _paramSetHandle, mapParamTypeEnumToString( paramType ), name.c_str(), &props );

	throwSuiteStatusException( stat );
}

/** @brief if a param has been defined in this set, go find it */
ParamDescriptor* ParamSetDescriptor::findPreviouslyDefinedParam( const std::string& name )
{
	// search
	std::map<std::string, ParamDescriptor*>::const_iterator search;
	search = _definedParams.find( name );
	if( search == _definedParams.end() )
		return NULL;
	return search->second;
}

/** @brief Define an integer param, only callable from describe in context */
IntParamDescriptor* ParamSetDescriptor::defineIntParam( const std::string& name )
{
	IntParamDescriptor* param = NULL;

	defineParamDescriptor( name, eIntParam, param );
	return param;
}

/** @brief Define a 2D integer param */
Int2DParamDescriptor* ParamSetDescriptor::defineInt2DParam( const std::string& name )
{
	Int2DParamDescriptor* param = NULL;

	defineParamDescriptor( name, eInt2DParam, param );
	return param;
}

/** @brief Define a 3D integer param */
Int3DParamDescriptor* ParamSetDescriptor::defineInt3DParam( const std::string& name )
{
	Int3DParamDescriptor* param = NULL;

	defineParamDescriptor( name, eInt3DParam, param );
	return param;
}

/** @brief Define an double param, only callable from describe in context */
DoubleParamDescriptor* ParamSetDescriptor::defineDoubleParam( const std::string& name )
{
	DoubleParamDescriptor* param = NULL;

	defineParamDescriptor( name, eDoubleParam, param );
	return param;
}

/** @brief Define a 2D double param */
Double2DParamDescriptor* ParamSetDescriptor::defineDouble2DParam( const std::string& name )
{
	Double2DParamDescriptor* param = NULL;

	defineParamDescriptor( name, eDouble2DParam, param );
	return param;
}

/** @brief Define a 3D double param */
Double3DParamDescriptor* ParamSetDescriptor::defineDouble3DParam( const std::string& name )
{
	Double3DParamDescriptor* param = NULL;

	defineParamDescriptor( name, eDouble3DParam, param );
	return param;
}

/** @brief Define a string param */
StringParamDescriptor* ParamSetDescriptor::defineStringParam( const std::string& name )
{
	StringParamDescriptor* param = NULL;

	defineParamDescriptor( name, eStringParam, param );
	return param;
}

/** @brief Define a RGBA param */
RGBAParamDescriptor* ParamSetDescriptor::defineRGBAParam( const std::string& name )
{
	RGBAParamDescriptor* param = NULL;

	defineParamDescriptor( name, eRGBAParam, param );
	return param;
}

/** @brief Define an RGB  param */
RGBParamDescriptor* ParamSetDescriptor::defineRGBParam( const std::string& name )
{
	RGBParamDescriptor* param = NULL;

	defineParamDescriptor( name, eRGBParam, param );
	return param;
}

/** @brief Define a Boolean  param */
BooleanParamDescriptor* ParamSetDescriptor::defineBooleanParam( const std::string& name )
{
	BooleanParamDescriptor* param = NULL;

	defineParamDescriptor( name, eBooleanParam, param );
	return param;
}

/** @brief Define a Choice param */
ChoiceParamDescriptor* ParamSetDescriptor::defineChoiceParam( const std::string& name )
{
	ChoiceParamDescriptor* param = NULL;

	defineParamDescriptor( name, eChoiceParam, param );
	return param;
}

/** @brief Define a group param */
GroupParamDescriptor* ParamSetDescriptor::defineGroupParam( const std::string& name )
{
	GroupParamDescriptor* param = NULL;

	defineParamDescriptor( name, eGroupParam, param );
	return param;
}

/** @brief Define a Page param */
PageParamDescriptor* ParamSetDescriptor::definePageParam( const std::string& name )
{
	PageParamDescriptor* param = NULL;

	defineParamDescriptor( name, ePageParam, param );
	return param;
}

/** @brief Define a push button  param */
PushButtonParamDescriptor* ParamSetDescriptor::definePushButtonParam( const std::string& name )
{
	PushButtonParamDescriptor* param = NULL;

	defineParamDescriptor( name, ePushButtonParam, param );
	return param;
}

/** @brief Define a parametric param */
ParametricParamDescriptor* ParamSetDescriptor::defineParametricParam( const std::string& name )
{
	ParametricParamDescriptor* param = NULL;

	defineParamDescriptor( name, eParametricParam, param );

	// Parametric parameters need the ParamSet !
	param->setParamSet( *this ); ///< @todo tuttle: more generic way for all param types ?

	return param;
}

/** @brief Define a custom param */
CustomParamDescriptor* ParamSetDescriptor::defineCustomParam( const std::string& name )
{
	CustomParamDescriptor* param = NULL;

	defineParamDescriptor( name, eCustomParam, param );
	return param;
}

////////////////////////////////////////////////////////////////////////////////
/** @brief Base class for all param instances */
Param::Param( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle )
	: Attribute( name ),
	_paramType( type ),
	_paramHandle( handle ),
	_paramSet( paramSet )
{
	// fetch our property handle
	OfxPropertySetHandle propHandle;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetPropertySet( handle, &propHandle );

	throwSuiteStatusException( stat );
	getProps().propSetHandle( propHandle );

	// and validate the properties
	OFX::Validation::validateParameterProperties( type, getProps(), false );
}

/** @brief dtor */
Param::~Param()
{}

/** @brief, set the label properties in a plugin */
void Param::setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel )
{
	getProps().propSetString( kOfxPropLabel, label );
	getProps().propSetString( kOfxPropShortLabel, shortLabel, false );
	getProps().propSetString( kOfxPropLongLabel, longLabel, false );
}

/** @brief set the secretness of the param, defaults to false */
void Param::setIsSecret( bool v )
{
	getProps().propSetInt( kOfxParamPropSecret, v );
}

/** @brief set the param hint */
void Param::setHint( const std::string& v )
{
	getProps().propSetString( kOfxParamPropHint, v, false );
}

/** @brief whether the param is enabled */
void Param::setEnabled( bool v )
{
	getProps().propSetInt( kOfxParamPropEnabled, v );
}

/** @brief fetch the labels */
void Param::getLabels( std::string& label, std::string& shortLabel, std::string& longLabel ) const
{
	label      = getProps().propGetString( kOfxPropLabel );
	shortLabel = getProps().propGetString( kOfxPropShortLabel, false );
	longLabel  = getProps().propGetString( kOfxPropLongLabel, false );
}

/** @brief get whether the param is secret */
bool Param::getIsSecret( void ) const
{
	bool v = getProps().propGetInt( kOfxParamPropSecret ) != 0;

	return v;
}

/** @brief whether the param is enabled */
bool Param::getIsEnable( void ) const
{
	bool v = getProps().propGetInt( kOfxParamPropEnabled ) != 0;

	return v;
}

/** @brief get the param hint */
std::string Param::getHint( void ) const
{
	std::string v = getProps().propGetString( kOfxParamPropHint, false );

	return v;
}

/** @brief get the script name */
std::string Param::getScriptName( void ) const
{
	std::string v = getProps().propGetString( kOfxParamPropScriptName, false );

	return v;
}

/** @brief get the group param that is the parent of this one */
const GroupParam* Param::getParent( void ) const
{
	std::string v = getProps().propGetString( kOfxParamPropParent );

	if( v == "" )
		return NULL;
	return _paramSet->fetchGroupParam( v );
}

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a value holding param */

/** @brief hidden constructor */
ValueParam::ValueParam( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle )
	: Param( paramSet, name, type, handle )
{}

/** @brief dtor */
ValueParam::~ValueParam()
{}

/** @brief Set's whether the value of the param is significant (ie: affects the rendered image) */
void ValueParam::setEvaluateOnChange( bool v )
{
	getProps().propSetInt( kOfxParamPropEvaluateOnChange, v );
}

/** @brief is the param animating */
bool ValueParam::getIsAnimating( void ) const
{
	return getProps().propGetInt( kOfxParamPropIsAnimating ) != 0;
}

/** @brief is the param animating */
bool ValueParam::getIsAutoKeying( void ) const
{
	return getProps().propGetInt( kOfxParamPropIsAutoKeying ) != 0;
}

/** @brief is the param animating */
bool ValueParam::getIsPersistant( void ) const
{
	return getProps().propGetInt( kOfxParamPropPersistant ) != 0;
}

/** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
bool ValueParam::getEvaluateOnChange( void ) const
{
	return getProps().propGetInt( kOfxParamPropEvaluateOnChange ) != 0;
}

/** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
CacheInvalidationEnum ValueParam::getCacheInvalidation( void ) const
{
	std::string v = getProps().propGetString( kOfxParamPropCacheInvalidation );

	if( v == kOfxParamInvalidateValueChange )
		return eCacheInvalidateValueChange;
	else if( v == kOfxParamInvalidateValueChangeToEnd )
		return eCacheInvalidateValueChangeToEnd;
	else // if(v == kOfxParamInvalidateAll)
		return eCacheInvalidateValueAll;
}

/** @brief if the param is animating, the number of keys in it, otherwise 0 */
unsigned int ValueParam::getNumKeys( void ) const
{
	if( !OFX::Private::gParamSuite->paramGetNumKeys )
		throwHostMissingSuiteException( "paramGetNumKeys" );
	unsigned int v = 0;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetNumKeys( _paramHandle, &v );
	throwSuiteStatusException( stat );
	return v;
}

/** @brief get the time of the nth key, nth must be between 0 and getNumKeys-1 */
double ValueParam::getKeyTime( int nthKey ) const
{
	if( !OFX::Private::gParamSuite->paramGetKeyTime )
		throwHostMissingSuiteException( "paramGetKeyTime" );
	double v       = 0;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetKeyTime( _paramHandle, nthKey, &v );

	// oops?
	if( stat == kOfxStatFailed )
		BOOST_THROW_EXCEPTION( std::out_of_range( "ValueParam::getKeyTime key index out of range" ) );
	throwSuiteStatusException( stat );
	return v;
}

/** @brief find the index of a key by a time */
int ValueParam::getKeyIndex( double        time,
                             KeySearchEnum searchDir ) const
{
	if( !OFX::Private::gParamSuite->paramGetKeyIndex )
		throwHostMissingSuiteException( "paramGetKeyIndex" );
	int v = 0;

	// turn enum into -1,0,1
	int dir = searchDir == eKeySearchBackwards ? -1 : ( searchDir == eKeySearchNear ? 0 : 1 );

	// call raw param function
	OfxStatus stat = OFX::Private::gParamSuite->paramGetKeyIndex( _paramHandle, time, dir, &v );

	// oops?
	if( stat == kOfxStatFailed )
		return -1;                                                                                                                                                                                                                                                                               // if search failed, return -1
	throwSuiteStatusException( stat );
	return v;
}

/** @brief deletes a key at the given time */
void ValueParam::deleteKeyAtTime( double time )
{
	if( !OFX::Private::gParamSuite->paramDeleteKey )
		throwHostMissingSuiteException( "paramDeleteKey" );
	OfxStatus stat = OFX::Private::gParamSuite->paramDeleteKey( _paramHandle, time );
	if( stat == kOfxStatFailed )
		return;                                                                                                                                                                                                                                        // if no key at time, fail quietly
	throwSuiteStatusException( stat );
}

/** @brief delete all the keys */
void ValueParam::deleteAllKeys( void )
{
	if( !OFX::Private::gParamSuite->paramDeleteAllKeys )
		throwHostMissingSuiteException( "paramDeleteAllKeys" );
	OfxStatus stat = OFX::Private::gParamSuite->paramDeleteAllKeys( _paramHandle );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up an integer param */

/** @brief hidden constructor */
IntParam::IntParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eIntParam, handle )
{}

/** @brief set the default value */
void IntParam::setDefault( int v )
{
	getProps().propSetInt( kOfxParamPropDefault, v );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void IntParam::setRange( int min, int max )
{
	getProps().propSetInt( kOfxParamPropMin, min );
	getProps().propSetInt( kOfxParamPropMax, max );
}

/** @brief set the display min and max, default is to be the same as the range param */
void IntParam::setDisplayRange( int min, int max )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, min );
	getProps().propSetInt( kOfxParamPropDisplayMax, max );
}

/** @brief het the default value */
void IntParam::getDefault( int& v ) const
{
	v = getProps().propGetInt( kOfxParamPropDefault );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void IntParam::getRange( int& min, int& max ) const
{
	min = getProps().propGetInt( kOfxParamPropMin );
	max = getProps().propGetInt( kOfxParamPropMax );
}

/** @brief set the display min and max, default is to be the same as the range param */
void IntParam::getDisplayRange( int& min, int& max ) const
{
	min = getProps().propGetInt( kOfxParamPropDisplayMin );
	max = getProps().propGetInt( kOfxParamPropDisplayMax );
}

/** @brief get value */
void IntParam::getValue( int& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &v );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void IntParam::getValueAtTime( double t, int& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &v );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void IntParam::setValue( int v )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, v );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void IntParam::setValueAtTime( double t, int v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, v );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// 2D Int params

/** @brief hidden constructor */
Int2DParam::Int2DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eInt2DParam, handle )
{}

/** @brief set the default value */
void Int2DParam::setDefault( int x, int y )
{
	getProps().propSetInt( kOfxParamPropDefault, x, 0 );
	getProps().propSetInt( kOfxParamPropDefault, y, 1 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int2DParam::setRange( int xmin, int ymin,
                           int xmax, int ymax )
{
	getProps().propSetInt( kOfxParamPropMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropMax, ymax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int2DParam::setDisplayRange( int xmin, int ymin,
                                  int xmax, int ymax )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMax, ymax, 1 );
}

/** @brief het the default value */
void Int2DParam::getDefault( int& x, int& y ) const
{
	x = getProps().propGetInt( kOfxParamPropDefault, 0 );
	y = getProps().propGetInt( kOfxParamPropDefault, 1 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int2DParam::getRange( int& xmin, int& ymin,
                           int& xmax, int& ymax ) const
{
	xmin = getProps().propGetInt( kOfxParamPropMin, 0 );
	ymin = getProps().propGetInt( kOfxParamPropMin, 1 );
	xmax = getProps().propGetInt( kOfxParamPropMax, 0 );
	ymax = getProps().propGetInt( kOfxParamPropMax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int2DParam::getDisplayRange( int& xmin, int& ymin,
                                  int& xmax, int& ymax ) const
{
	xmin = getProps().propGetInt( kOfxParamPropDisplayMin, 0 );
	ymin = getProps().propGetInt( kOfxParamPropDisplayMin, 1 );
	xmax = getProps().propGetInt( kOfxParamPropDisplayMax, 0 );
	ymax = getProps().propGetInt( kOfxParamPropDisplayMax, 1 );
}

/** @brief get value */
void Int2DParam::getValue( int& x, int& y ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &x, &y );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Int2DParam::getValueAtTime( double t, int& x, int& y ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &x, &y );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void Int2DParam::setValue( int x, int y )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, x, y );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void Int2DParam::setValueAtTime( double t, int x, int y )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, x, y );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// 3D Int params

/** @brief hidden constructor */
Int3DParam::Int3DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eInt3DParam, handle )
{}

/** @brief set the default value */
void Int3DParam::setDefault( int x, int y, int z )
{
	getProps().propSetInt( kOfxParamPropDefault, x, 0 );
	getProps().propSetInt( kOfxParamPropDefault, y, 1 );
	getProps().propSetInt( kOfxParamPropDefault, z, 2 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int3DParam::setRange( int xmin, int ymin, int zmin,
                           int xmax, int ymax, int zmax )
{
	getProps().propSetInt( kOfxParamPropMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropMin, zmin, 2 );
	getProps().propSetInt( kOfxParamPropMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropMax, ymax, 1 );
	getProps().propSetInt( kOfxParamPropMin, zmax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int3DParam::setDisplayRange( int xmin, int ymin, int zmin,
                                  int xmax, int ymax, int zmax )
{
	getProps().propSetInt( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMin, zmin, 2 );
	getProps().propSetInt( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetInt( kOfxParamPropDisplayMax, ymax, 1 );
	getProps().propSetInt( kOfxParamPropDisplayMax, zmax, 2 );
}

/** @brief het the default value */
void Int3DParam::getDefault( int& x, int& y, int& z ) const
{
	x = getProps().propGetInt( kOfxParamPropDefault, 0 );
	y = getProps().propGetInt( kOfxParamPropDefault, 1 );
	z = getProps().propGetInt( kOfxParamPropDefault, 2 );
}

/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
void Int3DParam::getRange( int& xmin, int& ymin, int& zmin,
                           int& xmax, int& ymax, int& zmax ) const
{
	xmin = getProps().propGetInt( kOfxParamPropMin, 0 );
	ymin = getProps().propGetInt( kOfxParamPropMin, 1 );
	zmin = getProps().propGetInt( kOfxParamPropMin, 2 );
	xmax = getProps().propGetInt( kOfxParamPropMax, 0 );
	ymax = getProps().propGetInt( kOfxParamPropMax, 1 );
	zmax = getProps().propGetInt( kOfxParamPropMax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Int3DParam::getDisplayRange( int& xmin, int& ymin, int& zmin,
                                  int& xmax, int& ymax, int& zmax ) const
{
	xmin = getProps().propGetInt( kOfxParamPropDisplayMin, 0 );
	ymin = getProps().propGetInt( kOfxParamPropDisplayMin, 1 );
	zmin = getProps().propGetInt( kOfxParamPropDisplayMin, 2 );
	xmax = getProps().propGetInt( kOfxParamPropDisplayMax, 0 );
	ymax = getProps().propGetInt( kOfxParamPropDisplayMax, 1 );
	zmax = getProps().propGetInt( kOfxParamPropDisplayMax, 2 );
}

/** @brief get value */
void Int3DParam::getValue( int& x, int& y, int& z ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &x, &y, &z );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Int3DParam::getValueAtTime( double t, int& x, int& y, int& z ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &x, &y, &z );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void Int3DParam::setValue( int x, int y, int z )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, x, y, z );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void Int3DParam::setValueAtTime( double t, int x, int y, int z )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, x, y, z );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// common base to all double params

/** @brief hidden constructor */
BaseDoubleParam::BaseDoubleParam( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle )
	: ValueParam( paramSet, name, type, handle )
{}

BaseDoubleParam::~BaseDoubleParam()
{}

/** @brief set the sensitivity of any gui slider */
void BaseDoubleParam::setIncrement( double v )
{
	getProps().propSetDouble( kOfxParamPropIncrement, v );
}

/** @brief set the number of digits printed after a decimal point in any gui */
void BaseDoubleParam::setDigits( int v )
{
	getProps().propSetInt( kOfxParamPropDigits, v );
}

/** @brief set the sensitivity of any gui slider */
void BaseDoubleParam::getIncrement( double& v ) const
{
	v = getProps().propGetDouble( kOfxParamPropIncrement );
}

/** @brief set the number of digits printed after a decimal point in any gui */
void BaseDoubleParam::getDigits( int& v ) const
{
	v = getProps().propGetInt( kOfxParamPropDigits );
}

/** @brief set the number of digits printed after a decimal point in any gui */
void BaseDoubleParam::getDoubleType( DoubleTypeEnum& v ) const
{
	std::string str = getProps().propGetString( kOfxParamPropDoubleType );

	if( str == kOfxParamDoubleTypePlain )
		v = eDoubleTypePlain;
	else if( str == kOfxParamDoubleTypeAngle )
		v = eDoubleTypeAngle;
	else if( str == kOfxParamDoubleTypeScale )
		v = eDoubleTypeScale;
	else if( str == kOfxParamDoubleTypeTime )
		v = eDoubleTypeTime;
	else if( str == kOfxParamDoubleTypeAbsoluteTime )
		v = eDoubleTypeAbsoluteTime;
	else if( str == kOfxParamDoubleTypeNormalisedX )
		v = eDoubleTypeNormalisedX;
	else if( str == kOfxParamDoubleTypeNormalisedY )
		v = eDoubleTypeNormalisedY;
	else if( str == kOfxParamDoubleTypeNormalisedXAbsolute )
		v = eDoubleTypeNormalisedXAbsolute;
	else if( str == kOfxParamDoubleTypeNormalisedYAbsolute )
		v = eDoubleTypeNormalisedYAbsolute;
	else if( str == kOfxParamDoubleTypeNormalisedXY )
		v = eDoubleTypeNormalisedXY;
	else if( str == kOfxParamDoubleTypeNormalisedXYAbsolute )
		v = eDoubleTypeNormalisedXYAbsolute;
	else
		v = eDoubleTypePlain;
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up an double param */

/** @brief hidden constructor */
DoubleParam::DoubleParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: BaseDoubleParam( paramSet, name, eDoubleParam, handle )
{}

/** @brief set the default value */
void DoubleParam::setDefault( double v )
{
	getProps().propSetDouble( kOfxParamPropDefault, v );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void DoubleParam::setRange( double min, double max )
{
	getProps().propSetDouble( kOfxParamPropMin, min );
	getProps().propSetDouble( kOfxParamPropMax, max );
}

/** @brief set the display min and max, default is to be the same as the range param */
void DoubleParam::setDisplayRange( double min, double max )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, min );
	getProps().propSetDouble( kOfxParamPropDisplayMax, max );
}

/** @brief het the default value */
void DoubleParam::getDefault( double& v ) const
{
	v = getProps().propGetDouble( kOfxParamPropDefault );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void DoubleParam::getRange( double& min, double& max ) const
{
	min = getProps().propGetDouble( kOfxParamPropMin );
	max = getProps().propGetDouble( kOfxParamPropMax );
}

/** @brief set the display min and max, default is to be the same as the range param */
void DoubleParam::getDisplayRange( double& min, double& max ) const
{
	min = getProps().propGetDouble( kOfxParamPropDisplayMin );
	max = getProps().propGetDouble( kOfxParamPropDisplayMax );
}

/** @brief get value */
void DoubleParam::getValue( double& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &v );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void DoubleParam::getValueAtTime( double t, double& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &v );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void DoubleParam::setValue( double v )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, v );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void DoubleParam::setValueAtTime( double t, double v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, v );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void DoubleParam::differentiate( double t, double& v ) const
{
	if( !OFX::Private::gParamSuite->paramGetDerivative )
		throwHostMissingSuiteException( "paramGetDerivative" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetDerivative( _paramHandle, t, &v );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void DoubleParam::integrate( double t1, double t2, double& v ) const
{
	if( !OFX::Private::gParamSuite->paramGetIntegral )
		throwHostMissingSuiteException( "paramGetIntegral" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetIntegral( _paramHandle, t1, t2, &v );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// 2D Double params

/** @brief hidden constructor */
Double2DParam::Double2DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: BaseDoubleParam( paramSet, name, eDouble2DParam, handle )
{}

/** @brief set the default value */
void Double2DParam::setDefault( double x, double y )
{
	getProps().propSetDouble( kOfxParamPropDefault, x, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, y, 1 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double2DParam::setRange( double xmin, double ymin,
                              double xmax, double ymax )
{
	getProps().propSetDouble( kOfxParamPropMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropMax, ymax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double2DParam::setDisplayRange( double xmin, double ymin,
                                     double xmax, double ymax )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, ymax, 1 );
}

/** @brief het the default value */
void Double2DParam::getDefault( double& x, double& y ) const
{
	x = getProps().propGetDouble( kOfxParamPropDefault, 0 );
	y = getProps().propGetDouble( kOfxParamPropDefault, 1 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double2DParam::getRange( double& xmin, double& ymin,
                              double& xmax, double& ymax ) const
{
	xmin = getProps().propGetDouble( kOfxParamPropMin, 0 );
	ymin = getProps().propGetDouble( kOfxParamPropMin, 1 );
	xmax = getProps().propGetDouble( kOfxParamPropMax, 0 );
	ymax = getProps().propGetDouble( kOfxParamPropMax, 1 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double2DParam::getDisplayRange( double& xmin, double& ymin,
                                     double& xmax, double& ymax ) const
{
	xmin = getProps().propGetDouble( kOfxParamPropDisplayMin, 0 );
	ymin = getProps().propGetDouble( kOfxParamPropDisplayMin, 1 );
	xmax = getProps().propGetDouble( kOfxParamPropDisplayMax, 0 );
	ymax = getProps().propGetDouble( kOfxParamPropDisplayMax, 1 );
}

/** @brief get value */
void Double2DParam::getValue( double& x, double& y ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &x, &y );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double2DParam::getValueAtTime( double t, double& x, double& y ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &x, &y );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void Double2DParam::setValue( double x, double y )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, x, y );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void Double2DParam::setValueAtTime( double t, double x, double y )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, x, y );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double2DParam::differentiate( double t, double& x, double& y ) const
{
	if( !OFX::Private::gParamSuite->paramGetDerivative )
		throwHostMissingSuiteException( "paramGetDerivative" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetDerivative( _paramHandle, t, &x, &y );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double2DParam::integrate( double t1, double t2, double& x, double& y ) const
{
	if( !OFX::Private::gParamSuite->paramGetIntegral )
		throwHostMissingSuiteException( "paramGetIntegral" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetIntegral( _paramHandle, t1, t2, &x, &y );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// 3D Double params

/** @brief hidden constructor */
Double3DParam::Double3DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: BaseDoubleParam( paramSet, name, eDouble3DParam, handle )
{}

/** @brief set the default value */
void Double3DParam::setDefault( double x, double y, double z )
{
	getProps().propSetDouble( kOfxParamPropDefault, x, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, y, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, z, 2 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double3DParam::setRange( double xmin, double ymin, double zmin,
                              double xmax, double ymax, double zmax )
{
	getProps().propSetDouble( kOfxParamPropMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropMin, zmin, 2 );
	getProps().propSetDouble( kOfxParamPropMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropMax, ymax, 1 );
	getProps().propSetDouble( kOfxParamPropMin, zmax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double3DParam::setDisplayRange( double xmin, double ymin, double zmin,
                                     double xmax, double ymax, double zmax )
{
	getProps().propSetDouble( kOfxParamPropDisplayMin, xmin, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, ymin, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMin, zmin, 2 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, xmax, 0 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, ymax, 1 );
	getProps().propSetDouble( kOfxParamPropDisplayMax, zmax, 2 );
}

/** @brief het the default value */
void Double3DParam::getDefault( double& x, double& y, double& z ) const
{
	x = getProps().propGetDouble( kOfxParamPropDefault, 0 );
	y = getProps().propGetDouble( kOfxParamPropDefault, 1 );
	z = getProps().propGetDouble( kOfxParamPropDefault, 2 );
}

/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
void Double3DParam::getRange( double& xmin, double& ymin, double& zmin,
                              double& xmax, double& ymax, double& zmax ) const
{
	xmin = getProps().propGetDouble( kOfxParamPropMin, 0 );
	ymin = getProps().propGetDouble( kOfxParamPropMin, 1 );
	zmin = getProps().propGetDouble( kOfxParamPropMin, 2 );
	xmax = getProps().propGetDouble( kOfxParamPropMax, 0 );
	ymax = getProps().propGetDouble( kOfxParamPropMax, 1 );
	zmax = getProps().propGetDouble( kOfxParamPropMax, 2 );
}

/** @brief set the display min and max, default is to be the same as the range param */
void Double3DParam::getDisplayRange( double& xmin, double& ymin, double& zmin,
                                     double& xmax, double& ymax, double& zmax ) const
{
	xmin = getProps().propGetDouble( kOfxParamPropDisplayMin, 0 );
	ymin = getProps().propGetDouble( kOfxParamPropDisplayMin, 1 );
	zmin = getProps().propGetDouble( kOfxParamPropDisplayMin, 2 );
	xmax = getProps().propGetDouble( kOfxParamPropDisplayMax, 0 );
	ymax = getProps().propGetDouble( kOfxParamPropDisplayMax, 1 );
	zmax = getProps().propGetDouble( kOfxParamPropDisplayMax, 2 );
}

/** @brief get value */
void Double3DParam::getValue( double& x, double& y, double& z ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &x, &y, &z );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double3DParam::getValueAtTime( double t, double& x, double& y, double& z ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &x, &y, &z );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void Double3DParam::setValue( double x, double y, double z )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, x, y, z );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void Double3DParam::setValueAtTime( double t, double x, double y, double z )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, x, y, z );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double3DParam::differentiate( double t, double& x, double& y, double& z ) const
{
	if( !OFX::Private::gParamSuite->paramGetDerivative )
		throwHostMissingSuiteException( "paramGetDerivative" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetDerivative( _paramHandle, t, &x, &y, &z );
	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void Double3DParam::integrate( double t1, double t2, double& x, double& y, double& z ) const
{
	if( !OFX::Private::gParamSuite->paramGetIntegral )
		throwHostMissingSuiteException( "paramGetIntegral" );
	OfxStatus stat = OFX::Private::gParamSuite->paramGetIntegral( _paramHandle, t1, t2, &x, &y, &z );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// RGB colour param
/** @brief hidden constructor */
RGBParam::RGBParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eRGBParam, handle )
{}

/** @brief set the default value */
void RGBParam::setDefault( double r, double g, double b )
{
	getProps().propSetDouble( kOfxParamPropDefault, r, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, g, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, b, 2 );
}

/** @brief het the default value */
void RGBParam::getDefault( double& r, double& g, double& b ) const
{
	r = getProps().propGetDouble( kOfxParamPropDefault, 0 );
	g = getProps().propGetDouble( kOfxParamPropDefault, 1 );
	b = getProps().propGetDouble( kOfxParamPropDefault, 2 );
}

/** @brief get value */
void RGBParam::getValue( double& r, double& g, double& b ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &r, &g, &b );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void RGBParam::getValueAtTime( double t, double& r, double& g, double& b ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &r, &g, &b );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void RGBParam::setValue( double r, double g, double b )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, r, g, b );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void RGBParam::setValueAtTime( double t, double r, double g, double b )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, r, g, b );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// RGBA colour param
/** @brief hidden constructor */
RGBAParam::RGBAParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eRGBAParam, handle )
{}

/** @brief set the default value */
void RGBAParam::setDefault( double r, double g, double b, double a )
{
	getProps().propSetDouble( kOfxParamPropDefault, r, 0 );
	getProps().propSetDouble( kOfxParamPropDefault, g, 1 );
	getProps().propSetDouble( kOfxParamPropDefault, b, 2 );
	getProps().propSetDouble( kOfxParamPropDefault, a, 3 );
}

/** @brief het the default value */
void RGBAParam::getDefault( double& r, double& g, double& b, double& a ) const
{
	r = getProps().propGetDouble( kOfxParamPropDefault, 0 );
	g = getProps().propGetDouble( kOfxParamPropDefault, 1 );
	b = getProps().propGetDouble( kOfxParamPropDefault, 2 );
	a = getProps().propGetDouble( kOfxParamPropDefault, 3 );
}

/** @brief get value */
void RGBAParam::getValue( double& r, double& g, double& b, double& a ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &r, &g, &b, &a );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void RGBAParam::getValueAtTime( double t, double& r, double& g, double& b, double& a ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &r, &g, &b, &a );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void RGBAParam::setValue( double r, double g, double b, double a )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, r, g, b, a );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void RGBAParam::setValueAtTime( double t, double r, double g, double b, double a )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, r, g, b, a );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a string param */

/** @brief hidden constructor */
StringParam::StringParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eStringParam, handle )
{}

/** @brief het the default value */
void StringParam::getDefault( std::string& v ) const
{
	v = getProps().propGetString( kOfxParamPropDefault );
}

/** @brief set the default value */
void StringParam::setDefault( const std::string& v )
{
	getProps().propSetString( kOfxParamPropDefault, v );
}

/** @brief get value */
void StringParam::getValue( std::string& v ) const
{
	char* cStr;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &cStr );

	throwSuiteStatusException( stat );
	v = cStr;
}

/** @brief set value */
void StringParam::setValue( const std::string& v )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, v.c_str() );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void StringParam::getValueAtTime( double t, std::string& v ) const
{
	char* cStr;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &cStr );

	throwSuiteStatusException( stat );
	v = cStr;
}

/** @brief set the value at a time, implicitly adds a keyframe */
void StringParam::setValueAtTime( double t, const std::string& v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, v.c_str() );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a Boolean integer param */

/** @brief hidden constructor */
BooleanParam::BooleanParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eBooleanParam, handle )
{}

/** @brief set the default value */
void BooleanParam::setDefault( bool v )
{
	getProps().propSetInt( kOfxParamPropDefault, v );
}

/** @brief het the default value */
void BooleanParam::getDefault( bool& v ) const
{
	v = getProps().propGetInt( kOfxParamPropDefault ) != 0;
}

/** @brief get value */
void BooleanParam::getValue( bool& v ) const
{
	int iVal;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &iVal );

	throwSuiteStatusException( stat );
	v = iVal != 0;
}

/** @brief get the value at a time */
void BooleanParam::getValueAtTime( double t, bool& v ) const
{
	int iVal;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &iVal );

	throwSuiteStatusException( stat );
	v = iVal != 0;
}

/** @brief set value */
void BooleanParam::setValue( bool v )
{
	int iVal       = v;
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, iVal );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void BooleanParam::setValueAtTime( double t, bool v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	int iVal       = v;
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, iVal );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a choice integer param */

/** @brief hidden constructor */
ChoiceParam::ChoiceParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eChoiceParam, handle )
{}

/** @brief set the default value */
void ChoiceParam::setDefault( int v )
{
	getProps().propSetInt( kOfxParamPropDefault, v );
}

/** @brief het the default value */
void ChoiceParam::getDefault( int& v ) const
{
	v = getProps().propGetInt( kOfxParamPropDefault );
}

/** @brief get value */
void ChoiceParam::getValue( int& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &v );

	throwSuiteStatusException( stat );
}

/** @brief get the value at a time */
void ChoiceParam::getValueAtTime( double t, int& v ) const
{
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &v );

	throwSuiteStatusException( stat );
}

/** @brief set value */
void ChoiceParam::setValue( int v )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, v );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void ChoiceParam::setValueAtTime( double t, int v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, v );
	throwSuiteStatusException( stat );
}

/** @brief how many options do we have */
int ChoiceParam::getNOptions( void ) const
{
	int nCurrentValues = getProps().propGetDimension( kOfxParamPropChoiceOption );

	return nCurrentValues;
}

/** @brief set the default value */
void ChoiceParam::appendOption( const std::string& v )
{
	int nCurrentValues = getProps().propGetDimension( kOfxParamPropChoiceOption );

	getProps().propSetString( kOfxParamPropChoiceOption, v, nCurrentValues );
}

/** @brief set the default value */
void ChoiceParam::resetOptions( void )
{
	getProps().propReset( kOfxParamPropChoiceOption );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a custom param */

/** @brief hidden constructor */
CustomParam::CustomParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: ValueParam( paramSet, name, eCustomParam, handle )
{}

/** @brief set the default value */
void CustomParam::setDefault( const std::string& v )
{
	getProps().propSetString( kOfxParamPropDefault, v );
}

/** @brief het the default value */
void CustomParam::getDefault( std::string& v ) const
{
	v = getProps().propGetString( kOfxParamPropDefault );
}

/** @brief get value */
void CustomParam::getValue( std::string& v ) const
{
	char* cStr;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValue( _paramHandle, &cStr );

	throwSuiteStatusException( stat );
	v = cStr;
}

/** @brief get the value at a time */
void CustomParam::getValueAtTime( double t, std::string& v ) const
{
	char* cStr;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetValueAtTime( _paramHandle, t, &cStr );

	throwSuiteStatusException( stat );
	v = cStr;
}

/** @brief set value */
void CustomParam::setValue( const std::string& v )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValue( _paramHandle, v.c_str() );

	throwSuiteStatusException( stat );
}

/** @brief set the value at a time, implicitly adds a keyframe */
void CustomParam::setValueAtTime( double t, const std::string& v )
{
	if( !OFX::Private::gParamSuite->paramSetValueAtTime )
		throwHostMissingSuiteException( "paramSetValueAtTime" );
	OfxStatus stat = OFX::Private::gParamSuite->paramSetValueAtTime( _paramHandle, t, v.c_str() );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a group param
/** @brief hidden constructor */
GroupParam::GroupParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: Param( paramSet, name, eGroupParam, handle )
{}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a page param

/** @brief hidden constructor */
PageParam::PageParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: Param( paramSet, name, ePageParam, handle )
{}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a PushButton param

/** @brief hidden constructor */
PushButtonParam::PushButtonParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: Param( paramSet, name, ePushButtonParam, handle )
{}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a Parametric param

/** @brief hidden constructor */
ParametricParam::ParametricParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle )
	: Param( paramSet, name, eParametricParam, handle )
{}

/** @brief Evaluates a parametric parameter

  \arg curveIndex            which dimension to evaluate
  \arg time                  the time to evaluate to the parametric param at
  \arg parametricPosition    the position to evaluate the parametric param at

  @returns the double value is returned
*/
double ParametricParam::getValue( const int curveIndex,
								   const OfxTime time,
								   const double parametricPosition )
{
	double returnValue = 0.0;
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamGetValue( getOfxHandle(),
                                       curveIndex,
                                       time,
                                       parametricPosition,
                                       &returnValue );
	throwSuiteStatusException( stat );
	return returnValue;
}

/** @brief Returns the number of control points in the parametric param.

  \arg curveIndex            which dimension to check
  \arg time                  the time to check

  @returns the integer value is returned
*/
int ParametricParam::getNControlPoints( const int curveIndex,
                                       const OfxTime time )
{
	int returnValue = 0.0;
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamGetNControlPoints( getOfxHandle(),
                                       curveIndex,
                                       time,
                                       &returnValue );
	throwSuiteStatusException( stat );
	return returnValue;
}

/** @brief Returns the key/value pair of the nth control point.

  \arg curveIndex            which dimension to check
  \arg time                  the time to check
  \arg nthCtl                the nth control point to get the value of

  @returns a pair with key and value
*/
std::pair<double, double> ParametricParam::getNthControlPoints( const int curveIndex,
                                       const OfxTime time,
										const int nthCtl )
{
	std::pair<double, double> returnValue;
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamGetNthControlPoint( getOfxHandle(),
													curveIndex,
													time,
													nthCtl,
													&returnValue.first,
													&returnValue.second );
	throwSuiteStatusException( stat );
	return returnValue;
}

/** @brief Modifies an existing control point on a curve

  \arg curveIndex            which dimension to set
  \arg time                  the time to set the value at
  \arg nthCtl                the control point to modify
  \arg key                   key of the control point
  \arg value                 value of the control point
  \arg addAnimationKey       if the param is an animatable, setting this to true will
							 force an animation keyframe to be set as well as a curve key,
							 otherwise if false, a key will only be added if the curve is already
							 animating.

  @returns
	- ::kOfxStatOK            - all was fine
	- ::kOfxStatErrBadHandle  - if the paramter handle was invalid
	- ::kOfxStatErrUnknown    - if the type is unknown

  This modifies an existing control point. Note that by changing key, the order of the
  control point may be modified (as you may move it before or after anther point). So be
  careful when iterating over a curves control points and you change a key.
*/
void ParametricParam::setNthControlPoints( const int curveIndex,
											const OfxTime time,
											const int nthCtl,
											const double key,
											const double value,
											const bool addAnimationKey )
{
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamSetNthControlPoint( getOfxHandle(),
													curveIndex,
													time,
													nthCtl,
													key,
													value,
													addAnimationKey );
	throwSuiteStatusException( stat );
}

void ParametricParam::setNthControlPoints( const int curveIndex,
                                       const OfxTime time,
										const int nthCtl,
										const std::pair<double, double> ctrlPoint,
										const bool addAnimationKey )
{
	setNthControlPoints( curveIndex,
						time,
						nthCtl,
						ctrlPoint.first,
						ctrlPoint.second,
						addAnimationKey );
}

/** @brief Adds a control point to the curve.

  \arg curveIndex            which dimension to set
  \arg time                  the time to set the value at
  \arg key                   key of the control point
  \arg value                 value of the control point
  \arg addAnimationKey       if the param is an animatable, setting this to true will
							 force an animation keyframe to be set as well as a curve key,
							 otherwise if false, a key will only be added if the curve is already
							 animating.

  This will add a new control point to the given dimension of a parametric parameter. If a key exists
  sufficiently close to 'key', then it will be set to the indicated control point.
*/
void ParametricParam::addControlPoint( const int curveIndex,
										 const OfxTime time,
										 const double key,
										 const double value,
										 const bool addAnimationKey )
{
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamAddControlPoint( getOfxHandle(), curveIndex, time, key, value, addAnimationKey );
	throwSuiteStatusException( stat );
}

/** @brief Deletes the nth control point from a parametric param.

  \arg curveIndex            which dimension to delete
  \arg nthCtl                the control point to delete
*/
void ParametricParam::deleteControlPoint( const int curveIndex,
										 const int nthCtl )
{
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamDeleteControlPoint( getOfxHandle(), curveIndex, nthCtl );
	throwSuiteStatusException( stat );
}

/** @brief Delete all curve control points on the given param.

  \arg curveIndex            which dimension to clear
*/
void ParametricParam::deleteControlPoint( const int curveIndex )
{
	OfxStatus stat = OFX::Private::gParametricParameterSuite->parametricParamDeleteAllControlPoints( getOfxHandle(), curveIndex );
	throwSuiteStatusException( stat );
}

////////////////////////////////////////////////////////////////////////////////
// Wraps up a camera param

/** @brief hidden constructor */
CameraParam::CameraParam( OfxImageEffectHandle imageEffectHandle, const ParamSet* paramSet, const std::string& name, NukeOfxCameraHandle handle )
	: Param( paramSet, name, eCameraParam, (OfxParamHandle)handle )
	, _imageEffectHandle( imageEffectHandle )
{
	// fetch all parameters
//	NukeOfxCameraHandle *camera;
//	OfxPropertySetHandle *propertySet;
//	OfxStatus stat = OFX::Private::gCameraParameterSuite->cameraGetHandle( getOfxHandle(), name.c_str(), camera, propertySet );
//	throwSuiteStatusException( stat );
}

Param& CameraParam::getParameter( const std::string& name )
{
	return *this; // @todo tuttle: use internal list, like a ParamSet or a Group ?
}


////////////////////////////////////////////////////////////////////////////////
//  for a set of parameters
/** @brief hidden ctor */
ParamSet::ParamSet( void )
	: _paramSetHandle( 0 )
{}

/** @brief set the param set handle */
void ParamSet::setParamSetHandle( OfxParamSetHandle h )
{
	// set me handle
	_paramSetHandle = h;

	if( h )
	{
		// fetch me props
		OfxPropertySetHandle props;
		OfxStatus stat = OFX::Private::gParamSuite->paramSetGetPropertySet( h, &props );
		_paramSetProps.propSetHandle( props );
		throwSuiteStatusException( stat );
	}
	else
	{
		_paramSetProps.propSetHandle( 0 );
	}
}

/** @brief dtor */
ParamSet::~ParamSet()
{
	// delete any descriptor we may have constructed
	std::map<std::string, Param*>::iterator iter;
	for( iter = _fetchedParams.begin(); iter != _fetchedParams.end(); ++iter )
	{
		if( iter->second )
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
}

/** @brief calls the raw OFX routine to fetch a param */
void ParamSet::fetchRawParam( const std::string& name, ParamTypeEnum paramType, OfxParamHandle& handle )
{
	OfxPropertySetHandle propHandle;

	OfxStatus stat = OFX::Private::gParamSuite->paramGetHandle( _paramSetHandle, name.c_str(), &handle, &propHandle );

	throwSuiteStatusException( stat );

	PropertySet props( propHandle );

	// make sure it is of our type
	std::string paramTypeStr = props.propGetString( kOfxParamPropType );
	if( paramTypeStr != mapParamTypeEnumToString( paramType ) )
	{
		BOOST_THROW_EXCEPTION( OFX::Exception::TypeRequest( "Parameter exists but is of the wrong type" ) );
	}
}

/** @brief calls the raw OFX routine to fetch a param */
void ParamSet::fetchRawCameraParam( OfxImageEffectHandle pluginHandle, const std::string& name, NukeOfxCameraHandle& handle )
{
	OfxPropertySetHandle propHandle;

	OfxStatus stat = OFX::Private::gCameraParameterSuite->cameraGetHandle( pluginHandle, name.c_str(), &handle, &propHandle );

	throwSuiteStatusException( stat );
}

ParamTypeEnum ParamSet::getParamType( const std::string& name ) const
{
	OfxPropertySetHandle propHandle;
	OfxParamHandle handle;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetHandle( _paramSetHandle, name.c_str(), &handle, &propHandle );

	throwSuiteStatusException( stat );
	PropertySet props( propHandle );
	// make sure it is of our type
	std::string paramTypeStr = props.propGetString( kOfxParamPropType );
	return mapParamTypeStringToEnum( paramTypeStr.c_str() );
}

bool ParamSet::paramExists( const std::string& name ) const
{
	OfxParamHandle handle;
	OfxPropertySetHandle propHandle;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetHandle( _paramSetHandle, name.c_str(), &handle, &propHandle );

	if( stat != kOfxStatOK )
		return false;
	return true;
}

Param* ParamSet::getParam( const std::string& name )
{
	OfxParamHandle handle;
	OfxPropertySetHandle propHandle;
	OfxStatus stat = OFX::Private::gParamSuite->paramGetHandle( _paramSetHandle, name.c_str(), &handle, &propHandle );

	throwSuiteStatusException( stat );

	PropertySet props( propHandle );

	// make sure it is of our type
	std::string paramTypeStr = props.propGetString( kOfxParamPropType );
	ParamTypeEnum t          = mapParamTypeStringToEnum( paramTypeStr.c_str() );
	switch( t )
	{
		case eStringParam:
		{
			return fetchParam<StringParam>( name );
		}
		case eIntParam:
		{
			return fetchParam<IntParam>( name );
		}
		case eInt2DParam:
		{
			return fetchParam<Int2DParam>( name );
		}
		case eInt3DParam:
		{
			return fetchParam<Int3DParam>( name );
		}
		case eDoubleParam:
		{
			return fetchParam<DoubleParam>( name );
		}
		case eDouble2DParam:
		{
			return fetchParam<Double2DParam>( name );
		}
		case eDouble3DParam:
		{
			return fetchParam<Double3DParam>( name );
		}
		case eRGBParam:
		{
			return fetchParam<RGBParam>( name );
		}
		case eRGBAParam:
		{
			return fetchParam<RGBAParam>( name );
		}
		case eBooleanParam:
		{
			return fetchParam<BooleanParam>( name );
		}
		case eChoiceParam:
		{
			return fetchParam<ChoiceParam>( name );
		}
		case eCustomParam:
		{
			return fetchParam<CustomParam>( name );
		}
		case eGroupParam:
		{
			return fetchParam<GroupParam>( name );
		}
		case ePageParam:
		{
			return fetchParam<PageParam>( name );
		}
		case ePushButtonParam:
		{
			return fetchParam<PushButtonParam>( name );
		}
		case eParametricParam:
		{
			return fetchParam<ParametricParam>( name );
		}
		case eCameraParam:
		{
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrFatal, "You can't fetch a camera parameter from here..." ) );
//			return fetchParam<CameraParam>( name );
		}
		default:
			assert( false );
	}
	return 0;
}

/** @brief if a param has been fetched in this set, go find it */
Param* ParamSet::findPreviouslyFetchedParam( const std::string& name )
{
	// search
	std::map<std::string, Param*>::const_iterator search;
	search = _fetchedParams.find( name );
	if( search == _fetchedParams.end() )
		return NULL;
	return search->second;
}

/** @brief Fetch an integer param, only callable from describe in context */
IntParam* ParamSet::fetchIntParam( const std::string& name )
{
	return fetchParam<IntParam>( name );
}

/** @brief Fetch a 2D integer param */
Int2DParam* ParamSet::fetchInt2DParam( const std::string& name )
{
	return fetchParam<Int2DParam>( name );
}

/** @brief Fetch a 3D integer param */
Int3DParam* ParamSet::fetchInt3DParam( const std::string& name )
{
	return fetchParam<Int3DParam>( name );
}

/** @brief Fetch an double param, only callable from describe in context */
DoubleParam* ParamSet::fetchDoubleParam( const std::string& name )
{
	return fetchParam<DoubleParam>( name );
}

/** @brief Fetch a 2D double param */
Double2DParam* ParamSet::fetchDouble2DParam( const std::string& name )
{
	return fetchParam<Double2DParam>( name );
}

/** @brief Fetch a 3D double param */
Double3DParam* ParamSet::fetchDouble3DParam( const std::string& name )
{
	return fetchParam<Double3DParam>( name );
}

/** @brief Fetch a string param */
StringParam* ParamSet::fetchStringParam( const std::string& name )
{
	return fetchParam<StringParam>( name );
}

/** @brief Fetch a RGBA param */
RGBAParam* ParamSet::fetchRGBAParam( const std::string& name )
{
	return fetchParam<RGBAParam>( name );
}

/** @brief Fetch an RGB  param */
RGBParam* ParamSet::fetchRGBParam( const std::string& name )
{
	return fetchParam<RGBParam>( name );
}

/** @brief Fetch a Boolean  param */
BooleanParam* ParamSet::fetchBooleanParam( const std::string& name )
{
	return fetchParam<BooleanParam>( name );
}

/** @brief Fetch a Choice param */
ChoiceParam* ParamSet::fetchChoiceParam( const std::string& name )
{
	return fetchParam<ChoiceParam>( name );
}

/** @brief Fetch a group param */
GroupParam* ParamSet::fetchGroupParam( const std::string& name )
{
	return fetchParam<GroupParam>( name );
}

/** @brief Fetch a Page param */
PageParam* ParamSet::fetchPageParam( const std::string& name )
{
	return fetchParam<PageParam>( name );
}

/** @brief Fetch a push button  param */
PushButtonParam* ParamSet::fetchPushButtonParam( const std::string& name )
{
	return fetchParam<PushButtonParam>( name );
}

/** @brief Fetch a custom param */
CustomParam* ParamSet::fetchCustomParam( const std::string& name )
{
	return fetchParam<CustomParam>( name );
}

/** @brief Fetch a parametric param */
ParametricParam* ParamSet::fetchParametricParam( const std::string& name )
{
	return fetchParam<ParametricParam>( name );
}

/// open an undoblock
void ParamSet::beginEditBlock( const std::string& name )
{
	OfxStatus stat = OFX::Private::gParamSuite->paramEditBegin( _paramSetHandle, name.c_str() );
	throwSuiteStatusException( stat );
}

/// close an undoblock
void ParamSet::endEditBlock()
{
	OfxStatus stat = OFX::Private::gParamSuite->paramEditEnd( _paramSetHandle );
	throwSuiteStatusException( stat );
}

}
