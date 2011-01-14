#ifndef _ofxsParam_H_
#define _ofxsParam_H_

/*
 * OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
 * Copyright (C) 2004-2005 The Open Effects Association Ltd
 * Author Bruno Nicoletti bruno@thefoundry.co.uk
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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
 *
 */

/** @file This file contains core code that wraps OFX parameters with C++ classes.
 *
 * This file only holds code that is visible to a plugin implementation, and so hides much
 * of the direct OFX objects and any library side only functions.
 *
 * The classes that skin parameters are broken into two sets, those used during the description phase,
 * eg OFX::IntParamDescriptor and those representing instances eg, OFX::IntParamInstance. The members on
 * each represent the actions that can be carried out on those particular OFX objects.
 *
 */

#include "ofxsCore.h"
#include "ofxsInteract.h"
#include "ofxsUtilities.h"

#include <extensions/nuke/camera.h>

#include <boost/throw_exception.hpp>
#include <boost/assert.hpp>

#include <memory>
#include <iostream>


/** @brief Nasty macro used to define empty protected copy ctors and assign ops */
#define mDeclareProtectedAssignAndCC( CLASS ) \
    CLASS& operator=( const CLASS& ) { assert( false ); return *this; }    \
    CLASS( const CLASS & ) { assert( false ); }

/** @brief The core 'OFX Support' namespace, used by plugin implementations. All code for these are defined in the common support libraries.
 */
namespace OFX {

/* forward class declarations of the  descriptors */
class ParamDescriptor;
class ValueParamDescriptor;
class IntParamDescriptor;
class Int2DParamDescriptor;
class Int3DParamDescriptor;
class DoubleParamDescriptor;
class Double2DParamDescriptor;
class Double3DParamDescriptor;
class StringParamDescriptor;
class RGBAParamDescriptor;
class RGBParamDescriptor;
class BooleanParamDescriptor;
class ChoiceParamDescriptor;
class GroupParamDescriptor;
class PageParamDescriptor;
class PushButtonParamDescriptor;
class CustomParamDescriptor;
class ParamSetDescriptor;

/* forward class declarations of the instances */
class Param;
class ValueParam;
class IntParam;
class Int2DParam;
class Int3DParam;
class DoubleParam;
class Double2DParam;
class Double3DParam;
class RGBAParam;
class RGBParam;
class StringParam;
class BooleanParam;
class ChoiceParam;
class CustomParam;
class GroupParam;
class PageParam;
class PushButtonParam;
class ParamSet;

/** @brief Enumerates the different types of parameter */
enum ParamTypeEnum
{
	eDummyParam,
	eStringParam,
	eIntParam,
	eInt2DParam,
	eInt3DParam,
	eDoubleParam,
	eDouble2DParam,
	eDouble3DParam,
	eRGBParam,
	eRGBAParam,
	eBooleanParam,
	eChoiceParam,
	eCustomParam,
	eGroupParam,
	ePageParam,
	ePushButtonParam,
	eParametricParam,
	eCameraParam
};

/** @brief Enumerates the different types of cache invalidation */
enum CacheInvalidationEnum
{
	eCacheInvalidateValueChange,
	eCacheInvalidateValueChangeToEnd,
	eCacheInvalidateValueAll
};

/** @brief Enumerates how we search for keys in an animating parameter */
enum KeySearchEnum
{
	eKeySearchBackwards,
	eKeySearchNear,
	eKeySearchForwards
};

/** @brief Enumerates the differing types of string params */
enum StringTypeEnum
{
	eStringTypeSingleLine,
	eStringTypeMultiLine,
	eStringTypeFilePath,
	eStringTypeDirectoryPath,
	eStringTypeLabel
};

/** @brief Enumerates the differing types of double params */
enum DoubleTypeEnum
{
	eDoubleTypePlain,
	eDoubleTypeAngle,
	eDoubleTypeScale,
	eDoubleTypeTime,
	eDoubleTypeAbsoluteTime,
	eDoubleTypeNormalisedX,
	eDoubleTypeNormalisedY,
	eDoubleTypeNormalisedXAbsolute,
	eDoubleTypeNormalisedYAbsolute,
	eDoubleTypeNormalisedXY,
	eDoubleTypeNormalisedXYAbsolute
};

/** @brief turns a ParamTypeEnum into the char * that raw OFX uses */
const char* mapParamTypeEnumToString( ParamTypeEnum v );

////////////////////////////////////////////////////////////////////////////////
/** @brief Base class for all param descriptors */
class ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( ParamDescriptor );
	ParamDescriptor( void ) { assert( false ); }

protected:
	std::string _paramName;
	ParamTypeEnum _paramType;
	PropertySet _paramProps;

	/** @brief hidden constructors */
	ParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief dtor */
	virtual ~ParamDescriptor();

	inline ParamTypeEnum getType( void ) const { return _paramType; }

	inline PropertySet& getProps() { return _paramProps; }
	inline const PropertySet& getProps() const { return _paramProps; }

	/** @brief name */
	inline const std::string& getName( void ) const { return _paramName; }

	/** @brief Get the property set */
	inline PropertySet& getPropertySet() { return _paramProps; }
	inline const PropertySet& getPropertySet() const { return _paramProps; }

	/** @brief set the label properties in a plugin */
	void setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel );
	void setLabel(const std::string &label){ setLabels(label, label, label); }

	/** @brief set the param hint */
	void setHint( const std::string& hint );

	/** @brief set the script name, default is the name it was created with */
	void setScriptName( const std::string& hint );

	/** @brief set the secretness of the param, defaults to false */
	void setIsSecret( bool v );

	/** @brief set the group param that is the parent of this one, default is to be ungrouped at the root level */
	void setParent( const GroupParamDescriptor& v );
	inline void setParent( const GroupParamDescriptor* v ) { setParent(*v); }

	/** @brief whether the param is enabled, defaults to true */
	void setEnabled( bool v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Used to implement dummy parameters for page positioning commands */
class DummyParamDescriptor : public ParamDescriptor
{
public:
	/** @brief ctor */
	DummyParamDescriptor( const std::string& name )
		: ParamDescriptor( name, eDummyParam, 0 )
	{}
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a value holding param */
class ValueParamDescriptor : public ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( ValueParamDescriptor );
	ValueParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ValueParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;
	std::auto_ptr<ParamInteractWrap> _interact;

public:
	/** @brief dtor */
	virtual ~ValueParamDescriptor();

	/** @brief set whether the param can animate, defaults to true in most cases */
	void setAnimates( bool v );

	/** @brief set whether the param is persistant, defaults to true */
	void setIsPersistant( bool v );

	/** @brief Set's whether the value of the param is significant (ie: affects the rendered image), defaults to true */
	void setEvaluateOnChange( bool v );

	/** @brief Set's how any cache should be invalidated if the parameter is changed, defaults to eCacheInvalidateValueChange */
	void setCacheInvalidation( CacheInvalidationEnum v );

	/// @brief Set whether the param should appear on any undo stack
	void setCanUndo( bool v );

	void setInteractDescriptor( ParamInteractWrap* desc );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a string param */
class StringParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( StringParamDescriptor );
	StringParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	StringParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( const std::string& v );

	/** @brief sets the kind of the string param, defaults to eStringSingleLine */
	void setStringType( StringTypeEnum v );

	/** @brief if the string param is a file path, say that we are picking an existing file, rather than posibly specifying a new one, defaults to true */
	void setFilePathExists( bool v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an integer param */
class IntParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( IntParamDescriptor );
	IntParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	IntParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( int v );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int min, int max );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int min, int max );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an 2d integer param */
class Int2DParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( Int2DParamDescriptor );
	Int2DParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Int2DParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the dimension labels */
	void setDimensionLabels( const std::string& x,
	                         const std::string& y );

	/** @brief set the default value, default is 0 */
	void setDefault( int x, int y );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int minX, int minY,
	               int maxX, int maxY );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int minX, int minY,
	                      int maxX, int maxY );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an 3d integer param */
class Int3DParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( Int3DParamDescriptor );
	Int3DParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Int3DParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the dimension labels */
	void setDimensionLabels( const std::string& x,
	                         const std::string& y,
	                         const std::string& z );

	/** @brief set the default value, default is 0 */
	void setDefault( int x, int y, int z );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int minX, int minY, int minZ,
	               int maxX, int maxY, int maxZ );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int minX, int minY, int minZ,
	                      int maxX, int maxY, int maxZ );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Common base to all double param types */
class BaseDoubleParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( BaseDoubleParamDescriptor );
	BaseDoubleParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	BaseDoubleParamDescriptor( const std::string& name, ParamTypeEnum type, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the type of the double param, defaults to eDoubleTypePlain */
	void setDoubleType( DoubleTypeEnum v );

	/** @brief set the sensitivity of any gui slider */
	void setIncrement( double v );

	/** @brief set the number of digits printed after a decimal point in any gui */
	void setDigits( int v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a double param */
class DoubleParamDescriptor : public BaseDoubleParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( DoubleParamDescriptor );
	DoubleParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	DoubleParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief if the double type is Absolute time, show a time marker on the time line if possible */
	void setShowTimeMarker( bool v );

	/** @brief set the default value, default is 0 */
	void setDefault( double v );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( double min, double max );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double min, double max );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a 2D double param */
class Double2DParamDescriptor : public BaseDoubleParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( Double2DParamDescriptor );
	Double2DParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Double2DParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the dimension labels */
	void setDimensionLabels( const std::string& x,
	                         const std::string& y );

	/** @brief set the default value, default is 0 */
	void setDefault( double x, double y );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( double minX, double minY,
	               double maxX, double maxY );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double minX, double minY,
	                      double maxX, double maxY );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a 3D double param */
class Double3DParamDescriptor : public BaseDoubleParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( Double3DParamDescriptor );
	Double3DParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Double3DParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	friend class ParamSetDescriptor;

public:
	/** @brief set the dimension labels */
	void setDimensionLabels( const std::string& x,
	                         const std::string& y,
	                         const std::string& z );

	/** @brief set the default value, default is 0 */
	void setDefault( double x, double y, double z );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( double minX, double minY, double minZ,
	               double maxX, double maxY, double maxZ );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double minX, double minY, double minZ,
	                      double maxX, double maxY, double maxZ );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an RGB colour param */
class RGBParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( RGBParamDescriptor );
	RGBParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	RGBParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief set the default value */
	void setDefault( double r, double g, double b );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an RGBA colour param */
class RGBAParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( RGBAParamDescriptor );
	RGBAParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	RGBAParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief set the default value */
	void setDefault( double r, double g, double b, double a );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a boolean param */
class BooleanParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( BooleanParamDescriptor );
	BooleanParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	BooleanParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief set the default value */
	void setDefault( bool v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a choice param */
class ChoiceParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( ChoiceParamDescriptor );
	ChoiceParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ChoiceParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief set the default value */
	void setDefault( int v );

	/** @brief append an option, default is to have not there */
	void appendOption( const std::string& v );

	/** @brief how many options do we have */
	int getNOptions( void ) const;

	/** @brief clear all the options so as to add some new ones in */
	void resetOptions( void );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a group param, not much to it really */
class GroupParamDescriptor : public ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( GroupParamDescriptor );
	GroupParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	GroupParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	void setOpen( const bool open = true );
	
	// so it can make one
	friend class ParamSetDescriptor;

public:
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a page param, not much to it really */
class PageParamDescriptor : public ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( PageParamDescriptor );
	PageParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	PageParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief adds a child parameter. Note the two existing pseudo params, gColumnSkip  and gRowSkip */
	void addChild( const ParamDescriptor& p );

	/** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
	static DummyParamDescriptor gSkipRow;

	/** @brief dummy page positioning parameter to be passed to @ref OFX::PageParamDescriptor::addChild */
	static DummyParamDescriptor gSkipColumn;
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a push button param, not much to it at all */
class PushButtonParamDescriptor : public ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( PushButtonParamDescriptor );
	PushButtonParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	PushButtonParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a push button param, not much to it at all */
class ParametricParamDescriptor : public ParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( ParametricParamDescriptor );
	ParametricParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ParametricParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	OfxParamHandle _ofxParamHandle;
	ParamSetDescriptor* _paramSet;
	std::auto_ptr<ParamInteractWrap> _interact;
	
	// so it can make one
	friend class ParamSetDescriptor;
	void setParamSet( ParamSetDescriptor& paramSet );

public:
	void setDimension( const int dimension );

	void setRange( const double min, const double max );

	void setLabel( const std::string label );

	void setDimensionLabel( const std::string label, const int id );

	void setUIColour( const int id, const OfxRGBColourD color );

	void addControlPoint( const int id, const OfxTime time, const double x, const double y, const bool addKey );

	void setIdentity( const int id );

	void setIdentity();

	void setInteractDescriptor( ParamInteractWrap* desc );

};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a custom param, haven't added animation support yet */
class CustomParamDescriptor : public ValueParamDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( CustomParamDescriptor );
	CustomParamDescriptor( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	CustomParamDescriptor( const std::string& name, OfxPropertySetHandle props );

	// so it can make one
	friend class ParamSetDescriptor;

public:
	/** @brief set the default value of the param */
	void setDefault( const std::string& v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Describes a set of properties */
class ParamSetDescriptor
{
protected:
	mDeclareProtectedAssignAndCC( ParamSetDescriptor );

	/** @brief calls the raw OFX routine to define a param */
	void defineRawParam( const std::string& name, ParamTypeEnum paramType, OfxPropertySetHandle& props );

	/** @brief Define a param descriptor of the given type */
	template <class T>
	bool defineParamDescriptor( const std::string& name, ParamTypeEnum paramType, T*& paramPtr )
	{
		paramPtr = NULL;
		// have we made it already in this param set and is it of the correct type
		if( ParamDescriptor * param  = findPreviouslyDefinedParam( name ) )
		{
			if( param->getType() == paramType )
			{
				TUTTLE_COUT_WARNING( "Parameter already defined ! (" + name + ")" );
				paramPtr = (T*) param; // could be a dynamic cast here
				return true;
			}
			else
			{
				BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrExists, "Parameter already defined with another type ! (" + name + ")" ) );
				return false;
			}
		}
		else
		{
			// ok define one and add it in
			OfxPropertySetHandle props;
			defineRawParam( name, paramType, props );

			// make out support descriptor class
			paramPtr = new T( name, props );

			// add it to our map of described ones
			_definedParams[name] = paramPtr;
		}
		return true;
	}

protected:
	/** @brief Properties that belong to this param set */
	PropertySet _paramSetProps;

	/** @brief Parameter set handle */
	OfxParamSetHandle _paramSetHandle;

	/** @brief Set of all previously defined parameters, defined on demand */
	std::map<std::string, ParamDescriptor*> _definedParams;

	/** @brief Hidden ctor */
	ParamSetDescriptor( void );

	/** @brief find a param in the map */
	ParamDescriptor* findPreviouslyDefinedParam( const std::string& name );

	/** @brief set the param set handle */
	void setOfxParamSetHandle( OfxParamSetHandle h );

public:

	OfxParamSetHandle getOfxParamSetHandle()
	{
		return _paramSetHandle;
	}

	virtual ~ParamSetDescriptor();
	/** @brief tries to fetch a ParamDescriptor, returns 0 if it isn't there*/
	ParamDescriptor* getParamDescriptor( const std::string& name ) const;

	/** @brief estabilishes the order of page params. Do it by calling it in turn for each page */
	void setPageParamOrder( PageParamDescriptor& p );

	/** @brief Define an integer param */
	IntParamDescriptor* defineIntParam( const std::string& name );

	/** @brief Define a 2D integer param */
	Int2DParamDescriptor* defineInt2DParam( const std::string& name );

	/** @brief Define a 3D integer param */
	Int3DParamDescriptor* defineInt3DParam( const std::string& name );

	/** @brief Define an double param */
	DoubleParamDescriptor* defineDoubleParam( const std::string& name );

	/** @brief Define a 2D double param */
	Double2DParamDescriptor* defineDouble2DParam( const std::string& name );

	/** @brief Define a 3D double param */
	Double3DParamDescriptor* defineDouble3DParam( const std::string& name );

	/** @brief Define a string param */
	StringParamDescriptor* defineStringParam( const std::string& name );

	/** @brief Define a RGBA param */
	RGBAParamDescriptor* defineRGBAParam( const std::string& name );

	/** @brief Define an RGB  param */
	RGBParamDescriptor* defineRGBParam( const std::string& name );

	/** @brief Define a Boolean  param */
	BooleanParamDescriptor* defineBooleanParam( const std::string& name );

	/** @brief Define a Choice param */
	ChoiceParamDescriptor* defineChoiceParam( const std::string& name );

	/** @brief Define a group param */
	GroupParamDescriptor* defineGroupParam( const std::string& name );

	/** @brief Define a Page param */
	PageParamDescriptor* definePageParam( const std::string& name );

	/** @brief Define a push button param */
	PushButtonParamDescriptor* definePushButtonParam( const std::string& name );

	/** @brief Define a parametric param */
	ParametricParamDescriptor* defineParametricParam( const std::string& name );

	/** @brief Define a custom param */
	CustomParamDescriptor* defineCustomParam( const std::string& name );
};

////////////////////////////////////////////////////////////////////////////////
class Attribute
{
private:
	Attribute( const Attribute& v ) { assert( false ); }
	Attribute& operator=( const Attribute& ) { assert( false ); return *this; }
protected:
	Attribute() { assert( false ); }
	
public:
	Attribute( const std::string& name )
	: _name(name)
	{}

	/** @brief get name */
	inline const std::string& getName() const { return _name; }

	inline PropertySet& getProps() { return _props; }
	inline const PropertySet& getProps() const { return _props; }
	
protected:
	std::string _name;
	PropertySet _props;
};

/** @brief Base class for all param instances */
class Param : public Attribute
{
private:
	// don't ever use these!
	Param& operator=( const Param& ) { assert( false ); return *this; }
	Param( const Param& v ) : _paramSet( v._paramSet ) { assert( false ); }
protected:
	Param() { assert( false ); }

protected:
	ParamTypeEnum _paramType;
	OfxParamHandle _paramHandle;
	const ParamSet* _paramSet; // who do I belong to

	/** @brief hidden constructor */
	Param( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle );

	friend class ParamSet;

	OfxParamHandle getOfxHandle() { return _paramHandle; }

public:
	/** @brief dtor */
	virtual ~Param() = 0;

	/** @brief, set the label properties in a plugin */
	void setLabels( const std::string& label, const std::string& shortLabel, const std::string& longLabel );

	/** @brief return the derived type of this parameter */
	ParamTypeEnum getParamType() const { return _paramType; }

	/** @brief set the secretness of the param, defaults to false */
	void setIsSecret( bool v );

	/** @brief set the param hint */
	void setHint( const std::string& hint );

	/** @brief whether the param is enabled */
	void setEnabled( bool v );

	void setIsSecretAndDisabled( bool v ) { setEnabled(!v); setIsSecret(v); }

	/** @brief fetch the labels */
	void getLabels( std::string& label, std::string& shortLabel, std::string& longLabel ) const;

	/** @brief get whether the param is secret */
	bool getIsSecret( void ) const;

	/** @brief whether the param is enabled */
	bool getIsEnable( void ) const;

	/** @brief get the param hint */
	std::string getHint( void ) const;

	/** @brief get the script name */
	std::string getScriptName( void ) const;

	/** @brief get the group param that is the parent of this one */
	const GroupParam* getParent( void ) const;
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a value holding param */
class ValueParam : public Param
{
protected:
	mDeclareProtectedAssignAndCC( ValueParam );
	ValueParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ValueParam( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief dtor */
	virtual ~ValueParam() = 0;

	/** @brief Set's whether the value of the param is significant (ie: affects the rendered image) */
	void setEvaluateOnChange( bool v );

	/** @brief is the param animating */
	bool getIsAnimating( void ) const;

	/** @brief is the param animating */
	bool getIsAutoKeying( void ) const;

	/** @brief is the param animating */
	bool getIsPersistant( void ) const;

	/** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
	bool getEvaluateOnChange( void ) const;

	/** @brief Get's whether the value of the param is significant (ie: affects the rendered image) */
	CacheInvalidationEnum getCacheInvalidation( void ) const;

	/** @brief if the param is animating, the number of keys in it, otherwise 0 */
	unsigned int getNumKeys( void ) const;

	/** @brief get the time of the nth key, nth must be between 0 and getNumKeys-1 */
	double getKeyTime( int nthKey ) const;

	/** @brief find the index of a key by a time */
	int getKeyIndex( double        time,
	                 KeySearchEnum searchDir ) const;

	/** @brief deletes a key at the given time */
	void deleteKeyAtTime( double time );

	/** @brief delete all the keys */
	void deleteAllKeys( void );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an integer param */
class IntParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( IntParam );
	IntParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	IntParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value */
	void setDefault( int v );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int min, int max );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int min, int max );

	/** @brief het the default value */
	void getDefault( int& v ) const;

	/** @brief het the default value */
	int getDefault( void ) const { int v; getDefault( v ); return v; }

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void getRange( int& min, int& max ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( int& min, int& max ) const;

	/** @brief get value */
	void getValue( int& v ) const;

	/** @brief and a nicer one */
	int getValue( void ) const { int v; getValue( v ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, int& v ) const;

	/** @brief and a nicer one */
	int getValueAtTime( double t ) const { int v; getValueAtTime( t, v ); return v; }

	/** @brief set value */
	void setValue( int v );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, int v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an integer param */
class Int2DParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( Int2DParam );
	Int2DParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Int2DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( int x, int y );

	/** @brief set the default value, default is 0 */
	void setDefault( const OfxPointI& v ) { setDefault( v.x, v.y ); }

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int minX, int minY,
	               int maxX, int maxY );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int minX, int minY,
	                      int maxX, int maxY );

	/** @brief het the default value */
	void getDefault( int& x, int& y ) const;

	/** @brief get the default value */
	OfxPointI getDefault( void ) const { OfxPointI v; getDefault( v.x, v.y ); return v; }

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void getRange( int& minX, int& minY,
	               int& maxX, int& maxY ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( int& minX, int& minY,
	                      int& maxX, int& maxY ) const;

	/** @brief get value */
	void getValue( int& x, int& y ) const;

	/** @brief get the  value */
	OfxPointI getValue( void ) const { OfxPointI v; getValue( v.x, v.y ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, int& x, int& y ) const;

	/** @brief get the  value */
	OfxPointI getValueAtTime( double t ) const { OfxPointI v; getValueAtTime( t, v.x, v.y ); return v; }

	/** @brief set value */
	void setValue( int x, int y );

	/** @brief set the current value */
	void setValue( const OfxPointI& v ) { setValue( v.x, v.y ); }

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, int x, int y );

	/** @brief set the current value */
	void setValueAtTime( double t, const OfxPointI& v ) { setValueAtTime( t, v.x, v.y ); }
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an integer param */
class Int3DParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( Int3DParam );
	Int3DParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Int3DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( int x, int y, int z );

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void setRange( int minX, int minY, int minZ,
	               int maxX, int maxY, int maxZ );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( int minX, int minY, int minZ,
	                      int maxX, int maxY, int maxZ );

	/** @brief het the default value */
	void getDefault( int& x, int& y, int& z ) const;

	/** @brief set the hard min/max range, default is INT_MIN, INT_MAX */
	void getRange( int& minX, int& minY, int& minZ,
	               int& maxX, int& maxY, int& maxZ ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( int& minX, int& minY, int& minZ,
	                      int& maxX, int& maxY, int& maxZ ) const;

	/** @brief get value */
	void getValue( int& x, int& y, int& z ) const;

	/** @brief get the value at a time */
	void getValueAtTime( double t, int& x, int& y, int& z ) const;

	/** @brief set value */
	void setValue( int x, int y, int z );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, int x, int y, int z );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Common base to all double param types */
class BaseDoubleParam : public ValueParam
{
protected:
	mDeclareProtectedAssignAndCC( BaseDoubleParam );
	BaseDoubleParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	BaseDoubleParam( const ParamSet* paramSet, const std::string& name, ParamTypeEnum type, OfxParamHandle handle );

	friend class ParamSet;

public:
	virtual ~BaseDoubleParam() = 0;
	
	/** @brief set the sensitivity of any gui slider */
	void setIncrement( double v );

	/** @brief set the number of digits printed after a decimal point in any gui */
	void setDigits( int v );

	/** @brief get the sensitivity of any gui slider */
	void getIncrement( double& v ) const;

	/** @brief get the number of digits printed after a decimal point in any gui */
	void getDigits( int& v ) const;

	/** @brief get the type of the double param */
	void getDoubleType( DoubleTypeEnum& v ) const;
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an doubleeger param */
class DoubleParam : public BaseDoubleParam
{
private:
	mDeclareProtectedAssignAndCC( DoubleParam );
	DoubleParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	DoubleParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value */
	void setDefault( double v );

	/** @brief if the double type is Absolute time, show a time marker on the time line if possible */
	void setShowTimeMarker( bool v );

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void setRange( double min, double max );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double min, double max );

	/** @brief het the default value */
	void getDefault( double& v ) const;

	/** @brief het the default value */
	double getDefault( void ) const { double v; getDefault( v ); return v; }

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void getRange( double& min, double& max ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( double& min, double& max ) const;

	/** @brief get value */
	void getValue( double& v ) const;

	/** @brief get value */
	double getValue( void ) const { double v; getValue( v ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, double& v ) const;

	/** @brief get value */
	double getValueAtTime( double t ) const { double v; getValueAtTime( t, v ); return v; }

	/** @brief set value */
	void setValue( double v );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, double v );

	/** @brief differentiate the param */
	void differentiate( double t, double& v ) const;

	/** @brief differentiate the param  */
	double differentiate( double t ) const { double v; differentiate( t, v ); return v; }

	/** @brief integrate the param */
	void integrate( double t1, double t2, double& v ) const;

	/** @brief integrate the param */
	double integrate( double t1, double t2 ) const { double v; integrate( t1, t2, v ); return v; }
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an doubleeger param */
class Double2DParam : public BaseDoubleParam
{
private:
	mDeclareProtectedAssignAndCC( Double2DParam );
	Double2DParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Double2DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( double x, double y );

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void setRange( double minX, double minY,
	               double maxX, double maxY );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double minX, double minY,
	                      double maxX, double maxY );

	/** @brief get the default value */
	void getDefault( double& x, double& y ) const;

	/** @brief get the default value */
	inline OfxPointD getDefault() const { OfxPointD v; getDefault( v.x, v.y ); return v; }

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void getRange( double& minX, double& minY,
	               double& maxX, double& maxY ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( double& minX, double& minY,
	                      double& maxX, double& maxY ) const;

	/** @brief get value */
	void getValue( double& x, double& y ) const;

	/** @brief get value */
	inline OfxPointD getValue() const { OfxPointD v; getValue( v.x, v.y ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, double& x, double& y ) const;

	/** @brief set value */
	void setValue( double x, double y );

	/** @brief set value */
	inline void setValue( const OfxPointD& p ) { setValue( p.x, p.y ); }

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, double x, double y );

	/** @brief set the value at a time, implicitly adds a keyframe */
	inline void setValueAtTime( double t, const OfxPointD& p ) { setValueAtTime( t, p.x, p.y ); }

	/** @brief differentiate the param */
	void differentiate( double t, double& x, double& y ) const;

	/** @brief differentiate the param  */
	OfxPointD differentiate( double t ) const { OfxPointD v; differentiate( t, v.x, v.y ); return v; }

	/** @brief integrate the param */
	void integrate( double t1, double t2, double& x, double& y ) const;

	/** @brief integrate the param */
	OfxPointD integrate( double t1, double t2 ) const { OfxPointD v; integrate( t1, t2, v.x, v.y ); return v; }
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an doubleeger param */
class Double3DParam : public BaseDoubleParam
{
private:
	mDeclareProtectedAssignAndCC( Double3DParam );
	Double3DParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	Double3DParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( double x, double y, double z );

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void setRange( double minX, double minY, double minZ,
	               double maxX, double maxY, double maxZ );

	/** @brief set the display min and max, default is to be the same as the range param */
	void setDisplayRange( double minX, double minY, double minZ,
	                      double maxX, double maxY, double maxZ );

	/** @brief het the default value */
	void getDefault( double& x, double& y, double& z ) const;

	/** @brief set the hard min/max range, default is DOUBLE_MIN, DOUBLE_MAX */
	void getRange( double& minX, double& minY, double& minZ,
	               double& maxX, double& maxY, double& maxZ ) const;

	/** @brief set the display min and max, default is to be the same as the range param */
	void getDisplayRange( double& minX, double& minY, double& minZ,
	                      double& maxX, double& maxY, double& maxZ ) const;

	/** @brief get value */
	void getValue( double& x, double& y, double& z ) const;

	/** @brief get the value at a time */
	void getValueAtTime( double t, double& x, double& y, double& z ) const;

	/** @brief set value */
	void setValue( double x, double y, double z );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, double x, double y, double z );

	/** @brief differentiate the param */
	void differentiate( double t, double& x, double& y, double& z ) const;

	/** @brief differentiate the param  */
	Ofx3DPointD differentiate( double t ) const { Ofx3DPointD v; differentiate( t, v.x, v.y, v.z ); return v; }

	/** @brief integrate the param */
	void integrate( double t1, double t2, double& x, double& y, double& z ) const;

	/** @brief integrate the param */
	Ofx3DPointD integrate( double t1, double t2 ) const { Ofx3DPointD v; integrate( t1, t2, v.x, v.y, v.z ); return v; }
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an RGB param */
class RGBParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( RGBParam );
	RGBParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	RGBParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( double r, double g, double b );

	/** @brief get default value */
	void getDefault( double& r, double& g, double& b ) const;

	/** @brief get value */
	void getValue( double& r, double& g, double& b ) const;

	/** @brief get the value at a time */
	void getValueAtTime( double t, double& r, double& g, double& b ) const;

	/** @brief set value */
	void setValue( double r, double g, double b );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, double r, double g, double b );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up an RGB param */
class RGBAParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( RGBAParam );
	RGBAParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	RGBAParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief set the default value, default is 0 */
	void setDefault( double r, double g, double b, double a );

	/** @brief get default value */
	void getDefault( double& r, double& g, double& b, double& a ) const;
	OfxRGBAColourD getDefault() const { OfxRGBAColourD c; getDefault(c.r, c.g, c.b, c.a); return c; }

	/** @brief get value */
	void getValue( double& r, double& g, double& b, double& a ) const;
	OfxRGBAColourD getValue() const { OfxRGBAColourD c; getValue(c.r, c.g, c.b, c.a); return c; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, double& r, double& g, double& b, double& a ) const;
	OfxRGBAColourD getValueAtTime( double t ) const { OfxRGBAColourD c; getValueAtTime(t, c.r, c.g, c.b, c.a); return c; }

	/** @brief set value */
	void setValue( double r, double g, double b, double a );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, double r, double g, double b, double a );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a string param */
class StringParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( StringParam );
	StringParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	StringParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	friend class ParamSet;

public:
	/** @brief get the default value */
	void getDefault( std::string& v ) const;
	std::string getDefault() const { std::string s; getDefault(s); return s; }

	/** @brief set the default value */
	void setDefault( const std::string& v );

	/** @brief get value */
	void getValue( std::string& v ) const;
	std::string getValue() const { std::string s; getValue(s); return s; }

	/** @brief set value */
	void setValue( const std::string& v );

	/** @brief get the value at a time */
	void getValueAtTime( double t, std::string& v ) const;
	std::string getValueAtTime( double t ) const { std::string s; getValueAtTime( t, s ); return s; }

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, const std::string& v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a choice param */
class ChoiceParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( ChoiceParam );
	ChoiceParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ChoiceParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;
public:
	/** @brief set the default value */
	void setDefault( int v );

	/** @brief get the default value */
	void getDefault( int& v ) const;

	/** @brief how many options do we have */
	int getNOptions( void ) const;

	/** @brief append an option, default is to have not there */
	void appendOption( const std::string& v );

	/** @brief clear all the options so as to add some new ones in */
	void resetOptions( void );

	/** @brief get value */
	void getValue( int& v ) const;

	/** @brief get value */
	inline int getValue() const { int v; getValue( v ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, int& v ) const;

	/** @brief set value */
	void setValue( int v );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, int v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a boolean param */
class BooleanParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( BooleanParam );
	BooleanParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	BooleanParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
	/** @brief set the default value */
	void setDefault( bool v );

	/** @brief get the default value */
	void getDefault( bool& v ) const;

	/** @brief get the default value */
	bool getDefault( void ) const { bool v; getDefault( v ); return v; }

	/** @brief get value */
	void getValue( bool& v ) const;

	/** @brief get value */
	bool getValue( void ) const { bool v; getValue( v ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, bool& v ) const;

	/** @brief get value */
	bool getValueAtTime( double t ) const { bool v; getValueAtTime( t, v ); return v; }

	/** @brief set value */
	void setValue( bool v );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, bool v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a group param */
class GroupParam : public Param
{
private:
	mDeclareProtectedAssignAndCC( GroupParam );
	GroupParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	GroupParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a group param */
class PageParam : public Param
{
private:
	mDeclareProtectedAssignAndCC( PageParam );
	PageParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	PageParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a custom param, not animation support yet */
class CustomParam : public ValueParam
{
private:
	mDeclareProtectedAssignAndCC( CustomParam );
	CustomParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	CustomParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
	/** @brief set the default value of the param */
	void setDefault( const std::string& v );

	/** @brief get the default value of the param */
	void getDefault( std::string& v ) const;

	/** @brief get value */
	void getValue( std::string& v ) const;

	/** @brief get value */
	inline std::string getValue() const { std::string v; getValue( v ); return v; }

	/** @brief get the value at a time */
	void getValueAtTime( double t, std::string& v ) const;

	/** @brief get the value at a time */
	inline std::string getValueAtTime( double t ) const { std::string v; getValueAtTime( t, v ); return v; }

	/** @brief set value */
	void setValue( const std::string& v );

	/** @brief set the value at a time, implicitly adds a keyframe */
	void setValueAtTime( double t, const std::string& v );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a push button param, not much to it at all */
class PushButtonParam : public Param
{
private:
	mDeclareProtectedAssignAndCC( PushButtonParam );
	PushButtonParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	PushButtonParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a parametric param */
class ParametricParam : public Param
{
private:
	mDeclareProtectedAssignAndCC( ParametricParam );
	ParametricParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	ParametricParam( const ParamSet* paramSet, const std::string& name, OfxParamHandle handle );

	// so it can make one
	friend class ParamSet;

public:
	double getValue( const int curveIndex,
									   const OfxTime time,
									   const double parametricPosition );
	int getNControlPoints( const int curveIndex,
										   const OfxTime time );
	std::pair<double, double> getNthControlPoints( const int curveIndex,
										   const OfxTime time,
											const int nthCtl );
	void setNthControlPoints( const int curveIndex,
												const OfxTime time,
												const int nthCtl,
												const double key,
												const double value,
												const bool addAnimationKey );
	void setNthControlPoints( const int curveIndex,
										   const OfxTime time,
											const int nthCtl,
											const std::pair<double, double> ctrlPoint,
											const bool addAnimationKey );
	void addControlPoint( const int curveIndex,
											 const OfxTime time,
											 const double key,
											 const double value,
											 const bool addAnimationKey );
	void deleteControlPoint( const int curveIndex,
											 const int nthCtl );
	void deleteControlPoint( const int curveIndex );
};

////////////////////////////////////////////////////////////////////////////////
/** @brief Wraps up a camera param */
class CameraParam : public Param
{
private:
	mDeclareProtectedAssignAndCC( CameraParam );
	CameraParam( void ) { assert( false ); }

protected:
	/** @brief hidden constructor */
	CameraParam( OfxImageEffectHandle imageEffectHandle, const ParamSet* paramSet, const std::string& name, NukeOfxCameraHandle handle );

	// so it can make one
	friend class ParamSet;

public:
	Param& getParameter( const std::string& name );

private:
	OfxImageEffectHandle _imageEffectHandle;
};

template<class ParamType>
inline ParamTypeEnum mapParamTypeToEnum()
{
	return eDummyParam; // as default value...
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<StringParam>()
{
	return eStringParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<IntParam>()
{
	return eIntParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<Int2DParam>()
{
	return eInt2DParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<Int3DParam>()
{
	return eInt3DParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<DoubleParam>()
{
	return eDoubleParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<Double2DParam>()
{
	return eDouble2DParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<Double3DParam>()
{
	return eDouble3DParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<RGBParam>()
{
	return eRGBParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<RGBAParam>()
{
	return eRGBAParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<BooleanParam>()
{
	return eBooleanParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<ChoiceParam>()
{
	return eChoiceParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<CustomParam>()
{
	return eCustomParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<GroupParam>()
{
	return eGroupParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<PageParam>()
{
	return ePageParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<PushButtonParam>()
{
	return ePushButtonParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<ParametricParam>()
{
	return eParametricParam;
}
template<>
inline ParamTypeEnum mapParamTypeToEnum<CameraParam>()
{
	return eCameraParam;
}

template<class ParamType>
inline ParamTypeEnum mapParamTypeToEnumFrom( const ParamType& )
{
	return mapParamTypeToEnum<ParamType>();
}
template<class ParamType>
inline ParamTypeEnum mapParamTypeToEnumFrom( const ParamType* )
{
	return mapParamTypeToEnum<ParamType>();
}

////////////////////////////////////////////////////////////////////////////////
/** @brief A set of parameters in a plugin instance */
class ParamSet
{
public:
	typedef ParamSet This;
protected:
	mDeclareProtectedAssignAndCC( ParamSet );
	ParamTypeEnum getParamType( const std::string& name ) const;

private:
	/** @brief Properties that belong to this param set */
	PropertySet _paramSetProps;

	/** @brief Parameter set handle */
	OfxParamSetHandle _paramSetHandle;

	/** @brief Set of all previously fetched parameters, created on demand */
	mutable std::map<std::string, Param*> _fetchedParams;

	/** @brief see if we have a param of the given name in out map */
	Param* findPreviouslyFetchedParam( const std::string& name );

	/** @brief calls the raw OFX routine to define a param */
	void fetchRawParam( const std::string& name, ParamTypeEnum paramType, OfxParamHandle& handle );
	void fetchRawCameraParam( OfxImageEffectHandle pluginHandle, const std::string& name, NukeOfxCameraHandle& handle );

	/** @brief Fetch a param of the given name and type */
	template <class T>
	T* fetchParam( const std::string& name )
	{
		ParamTypeEnum paramType = mapParamTypeToEnum<T>();
		T* paramPtr = NULL;

		// have we made it already in this param set and is it an int?
		if( Param * param  = findPreviouslyFetchedParam( name ) )
		{
			if( param->getParamType() == paramType )
			{
				paramPtr = (T*) param; // could be a dynamic cast here
			}
			else
				BOOST_THROW_EXCEPTION( OFX::Exception::TypeRequest( "Fetching param and attempting to return the wrong type" ) );
		}
		else
		{
			// ok define one and add it in
			OfxParamHandle paramHandle;
			fetchRawParam( name, paramType, paramHandle );

			// make out support descriptor class
			paramPtr = new T( this, name, paramHandle );

			// add it to our map of described ones
			_fetchedParams[name] = paramPtr;
		}
		return paramPtr;
	}
	
protected:
	template<class T>
	inline T* fetchAttribute( OfxImageEffectHandle pluginHandle, const std::string& name )
	{
		// maybe the correct fonction you want to use is fetchParam(std::string)
		// if you use this function you need to redefine it for each type
		// because we need OfxImageEffectHandle because we use specific suites
		BOOST_ASSERT( false );
	}

protected:
	/** @brief Hidden ctor */
	ParamSet( void );

	/** @brief set the param set handle */
	void setParamSetHandle( OfxParamSetHandle h );

public:
	virtual ~ParamSet();

	bool paramExists( const std::string& name ) const;

	/// open an undoblock
	void beginEditBlock( const std::string& name );

	/// close an undoblock
	void endEditBlock();

	Param* getParam( const std::string& name );

	/** @brief Fetch an integer param */
	IntParam* fetchIntParam( const std::string& name );

	/** @brief Fetch a 2D integer param */
	Int2DParam* fetchInt2DParam( const std::string& name );

	/** @brief Fetch a 3D integer param */
	Int3DParam* fetchInt3DParam( const std::string& name );

	/** @brief Fetch an double param */
	DoubleParam* fetchDoubleParam( const std::string& name );

	/** @brief Fetch a 2D double param */
	Double2DParam* fetchDouble2DParam( const std::string& name );

	/** @brief Fetch a 3D double param */
	Double3DParam* fetchDouble3DParam( const std::string& name );

	/** @brief Fetch a string param */
	StringParam* fetchStringParam( const std::string& name );

	/** @brief Fetch a RGBA param */
	RGBAParam* fetchRGBAParam( const std::string& name );

	/** @brief Fetch an RGB  param */
	RGBParam* fetchRGBParam( const std::string& name );

	/** @brief Fetch a Boolean  param */
	BooleanParam* fetchBooleanParam( const std::string& name );

	/** @brief Fetch a Choice param */
	ChoiceParam* fetchChoiceParam( const std::string& name );

	/** @brief Fetch a group param */
	GroupParam* fetchGroupParam( const std::string& name );
	const GroupParam* fetchGroupParam( const std::string& name ) const { return const_cast<This&>(*this).fetchGroupParam(name); }

	/** @brief Fetch a page param */
	PageParam* fetchPageParam( const std::string& name );

	/** @brief Fetch a push button param */
	PushButtonParam* fetchPushButtonParam( const std::string& name );

	/** @brief Fetch a custom param */
	CustomParam* fetchCustomParam( const std::string& name );

	/** @brief Fetch a parametric param */
	ParametricParam* fetchParametricParam( const std::string& name );
};

/** @brief Fetch a parametric param */
template<>
inline CameraParam* ParamSet::fetchAttribute<CameraParam>( OfxImageEffectHandle pluginHandle, const std::string& name )
{
	typedef CameraParam T;
	const ParamTypeEnum paramType = mapParamTypeToEnum<T>();
	T* paramPtr = NULL;

	// have we made it already in this param set and is it an int?
	if( Param * param  = findPreviouslyFetchedParam( name ) )
	{
		if( param->getParamType() == paramType )
		{
			paramPtr = (T*) param; // could be a dynamic cast here
		}
		else
		{
			BOOST_THROW_EXCEPTION( OFX::Exception::TypeRequest( "Fetching param and attempting to return the wrong type" ) );
		}
	}
	else
	{
		// ok define one and add it in
		NukeOfxCameraHandle paramHandle;
		fetchRawCameraParam( pluginHandle, name, paramHandle );

		// make out support descriptor class
		paramPtr = new T( pluginHandle, this, name, paramHandle );

		// add it to our map of described ones
		_fetchedParams[name] = paramPtr;
	}
	return paramPtr;
}

}

// undeclare the protected assign and CC macro
#undef mDeclareProtectedAssignAndCC

#endif
