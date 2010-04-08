#include <iostream>
#include <fstream>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/OfxhClip.hpp>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhAttribute.hpp>

// my host
#include "HostDescriptor.hpp"
#include "ImageEffectNode.hpp"
#include "ClipImage.hpp"
#include "Param.hpp"

namespace tuttle {
namespace host {
namespace core {

//
// StringInstance
//
ParamString::ParamString( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamString( descriptor, name, effect ),
	_effect( effect )
{
	_value = getDefault();
}

const std::string& ParamString::getDefault() const
{
	return getProperties().getStringProperty( kOfxParamPropDefault );
}

void ParamString::get( std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamString::get( const OfxTime time, std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamString::set( const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v );
}

void ParamString::set( const OfxTime time, const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v ); ///< @todo: in time !
}

//
// IntegerInstance
//
ParamInteger::ParamInteger( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamInteger( descriptor, name, effect ),
	_effect( effect )
{
	_value = getDefault();
}


int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamInteger::get( int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamInteger::get( const OfxTime time, int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamInteger::set( const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamInteger::set( const OfxTime time, const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// DoubleInstance
//
ParamDouble::ParamDouble( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamDouble( descriptor, name, effect ),
	_effect( effect )
{
	_value = getDefault();
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault );
}

void ParamDouble::get( double& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamDouble::get( const OfxTime time, double& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamDouble::set( const double &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamDouble::set( const OfxTime time, const double &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

void ParamDouble::derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC
{
	throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
}

void ParamDouble::integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC
{
	throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
}

//
// BooleanInstance
//
ParamBoolean::ParamBoolean( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamBoolean( descriptor, name, effect ),
	_effect( effect )

{
	_value = getDefault();
}

bool ParamBoolean::getDefault() const
{
	return static_cast<bool>( getProperties().getIntProperty( kOfxParamPropDefault ) != 0 );
}

void ParamBoolean::get( bool& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamBoolean::get( const OfxTime time, bool& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamBoolean::set( const bool &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamBoolean::set( const OfxTime time, const bool &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// ChoiceInteger
//
ParamChoice::ParamChoice( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamChoice( descriptor, name, effect ),
	_effect( effect )

{
	_value = getDefault();
}

int ParamChoice::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamChoice::get( int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamChoice::get( const OfxTime time, int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamChoice::set( const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamChoice::set( const OfxTime time, const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// RGBAInstance
//
ParamRGBA::ParamRGBA( ImageEffectNode&                                   effect,
                      const std::string&                                 name,
                      const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 4>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".a", descriptor ) );

	_value = getDefault();
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
	OfxRGBAColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 4 );
	return color;
}

void ParamRGBA::get( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::get( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::set( const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
}

void ParamRGBA::set( const OfxTime time, const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
}

//
// RGBInstance
//
ParamRGB::ParamRGB( ImageEffectNode&                                   effect,
                    const std::string&                                 name,
                    const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b", descriptor ) );
	_value = getDefault();
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 3 );
	return color;
}

void ParamRGB::get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::get( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::set( const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
}

void ParamRGB::set( const OfxTime time, const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
}

//
// Double2DInstance
//
ParamDouble2D::ParamDouble2D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y", descriptor ) );
	_value = getDefault();
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;
	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamDouble2D::get( double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::get( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::set( const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamDouble2D::set( const OfxTime time, const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Integer2DInstance
//
ParamInteger2D::ParamInteger2D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor ) );
	_value = getDefault();
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamInteger2D::get( int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::get( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::set( const int &x, const int &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamInteger2D::set( const OfxTime time, const int &x, const int &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Integer3DInstance
//
ParamInteger3D::ParamInteger3D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".z", descriptor ) );
	_value = getDefault();
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamInteger3D::get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::get( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::set( const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamInteger3D::set( const OfxTime time, const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Double3DInstance
//
ParamDouble3D::ParamDouble3D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".z", descriptor ) );
	_value = getDefault();
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
	Ofx3DPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamDouble3D::get( double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::get( const OfxTime time, double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::set( const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamDouble3D::set( const OfxTime time, const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// PushbuttonInstance
//
ParamPushButton::ParamPushButton( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPushButton( descriptor, name, effect ),
	_effect( effect )
{}

//
// PushbuttonInstance
//
ParamGroup::ParamGroup( ImageEffectNode&                                   effect,
					  const std::string&                                 name,
					  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamGroup( descriptor, name, effect ),
	_effect( effect )
{}

//
// PushbuttonInstance
//
ParamPage::ParamPage( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPage( descriptor, name, effect ),
	_effect( effect )
{}

}
}
}
