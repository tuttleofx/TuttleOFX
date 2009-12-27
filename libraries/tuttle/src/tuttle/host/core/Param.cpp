/*
 * Software License :
 *
 * Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.
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
 */

#include <iostream>
#include <fstream>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhProperty.hpp>
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
                          tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamString( descriptor, effect ),
	_effect( effect )
{
	_value = getDefault();
}

ParamString* ParamString::clone() const
{
	return new ParamString( *this );
}

const std::string& ParamString::getDefault() const
{
	return getProperties().getStringProperty( kOfxParamPropDefault );
}

OfxStatus ParamString::get( std::string& v )
{
	v = _value;
	return kOfxStatOK;
}

OfxStatus ParamString::get( OfxTime time, std::string& v )
{
	v = _value; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamString::set( const char* v )
{
	_value = std::string( v );
	return kOfxStatOK;
}

OfxStatus ParamString::set( OfxTime time, const char* v )
{
	_value = std::string( v ); ///< @todo: in time !
	return kOfxStatOK;
}

//
// IntegerInstance
//
ParamInteger::ParamInteger( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamInteger( descriptor, effect ),
	_effect( effect )
{
	_value = getDefault();
}

ParamInteger* ParamInteger::clone() const
{
	return new ParamInteger( *this );
}

int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

OfxStatus ParamInteger::get( int& v )
{
	v = _value;
	return kOfxStatOK;
}

OfxStatus ParamInteger::get( OfxTime time, int& v )
{
	v = _value; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamInteger::set( const int& v )
{
	_value = v;
	return kOfxStatOK;
}

OfxStatus ParamInteger::set( OfxTime time, const int& v )
{
	_value = v; ///< @todo: in time !
	return kOfxStatOK;
}

//
// DoubleInstance
//
ParamDouble::ParamDouble( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamDouble( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

ParamDouble* ParamDouble::clone() const
{
	return new ParamDouble( *this );
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault );
}

OfxStatus ParamDouble::get( double& v )
{
	v = _value;
	return kOfxStatOK;
}

OfxStatus ParamDouble::get( OfxTime time, double& v )
{
	v = _value; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamDouble::set( const double& v )
{
	_value = v;
	return kOfxStatOK;
}

OfxStatus ParamDouble::set( OfxTime time, const double& v )
{
	_value = v; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamDouble::derive( OfxTime time, double& )
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus ParamDouble::integrate( OfxTime time1, OfxTime time2, double& )
{
	return kOfxStatErrMissingHostFeature;
}

//
// BooleanInstance
//
ParamBoolean::ParamBoolean( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamBoolean( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

ParamBoolean* ParamBoolean::clone() const
{
	return new ParamBoolean( *this );
}

bool ParamBoolean::getDefault() const
{
	return static_cast<bool>( getProperties().getIntProperty( kOfxParamPropDefault ) );
}

OfxStatus ParamBoolean::get( bool& v )
{
	v = _value;
	return kOfxStatOK;
}

OfxStatus ParamBoolean::get( OfxTime time, bool& v )
{
	v = _value; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamBoolean::set( bool v )
{
	_value = v;
	return kOfxStatOK;
}

OfxStatus ParamBoolean::set( OfxTime time, bool v )
{
	_value = v; ///< @todo: in time !
	return kOfxStatOK;
}

//
// ChoiceInteger
//
ParamChoice::ParamChoice( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamChoice( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

ParamChoice* ParamChoice::clone() const
{
	return new ParamChoice( *this );
}

int ParamChoice::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

OfxStatus ParamChoice::get( int& v )
{
	v = _value;
	return kOfxStatOK;
}

OfxStatus ParamChoice::get( OfxTime time, int& v )
{
	v = _value; ///< @todo: in time !
	return kOfxStatOK;
}

OfxStatus ParamChoice::set( int v )
{
	_value = v;
	return kOfxStatOK;
}

OfxStatus ParamChoice::set( OfxTime time, int v )
{
	_value = v; ///< @todo: in time !
	return kOfxStatOK;
}

//
// RGBAInstance
//
ParamRGBA::ParamRGBA( ImageEffectNode&                                   effect,
                      const std::string&                                 name,
                      tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 4>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".a_", descriptor ) );

	_value = getDefault();
}

ParamRGBA* ParamRGBA::clone() const
{
	return new ParamRGBA( *this );
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
	OfxRGBAColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 4 );
	return color;
}

OfxStatus ParamRGBA::get( double& r, double& g, double& b, double& a )
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
	return kOfxStatOK;
}

OfxStatus ParamRGBA::get( OfxTime time, double& r, double& g, double& b, double& a )
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
	return kOfxStatOK;
}

OfxStatus ParamRGBA::set( double r, double g, double b, double a )
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
	return kOfxStatOK;
}

OfxStatus ParamRGBA::set( OfxTime time, double r, double g, double b, double a )
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
	return kOfxStatOK;
}

//
// RGBInstance
//
ParamRGB::ParamRGB( ImageEffectNode&                                   effect,
                    const std::string&                                 name,
                    tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b_", descriptor ) );
	_value = getDefault();
}

ParamRGB* ParamRGB::clone() const
{
	return new ParamRGB( *this );
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 3 );
	return color;
}

OfxStatus ParamRGB::get( double& r, double& g, double& b )
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	return kOfxStatOK;
}

OfxStatus ParamRGB::get( OfxTime time, double& r, double& g, double& b )
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	return kOfxStatOK;
}

OfxStatus ParamRGB::set( double r, double g, double b )
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	return kOfxStatOK;
}

OfxStatus ParamRGB::set( OfxTime time, double r, double g, double b )
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	return kOfxStatOK;
}

//
// Double2DInstance
//
ParamDouble2D::ParamDouble2D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y_", descriptor ) );
	_value = getDefault();
}

ParamDouble2D* ParamDouble2D::clone() const
{
	return new ParamDouble2D( *this );
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

OfxStatus ParamDouble2D::get( double& x, double& y )
{
	x = _value.x;
	y = _value.y;
	return kOfxStatOK;
}

OfxStatus ParamDouble2D::get( OfxTime time, double& x, double& y )
{
	x = _value.x;
	y = _value.y;
	return kOfxStatOK;
}

OfxStatus ParamDouble2D::set( double x, double y )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

OfxStatus ParamDouble2D::set( OfxTime time, double x, double y )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

//
// Integer2DInstance
//
ParamInteger2D::ParamInteger2D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y_", descriptor ) );
	_value = getDefault();
}

ParamInteger2D* ParamInteger2D::clone() const
{
	return new ParamInteger2D( *this );
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

OfxStatus ParamInteger2D::get( int& x, int& y )
{
	x = _value.x;
	y = _value.y;
	return kOfxStatOK;
}

OfxStatus ParamInteger2D::get( OfxTime time, int& x, int& y )
{
	x = _value.x;
	y = _value.y;
	return kOfxStatOK;
}

OfxStatus ParamInteger2D::set( int x, int y )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

OfxStatus ParamInteger2D::set( OfxTime time, int x, int y )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

//
// Integer3DInstance
//
ParamInteger3D::ParamInteger3D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".z_", descriptor ) );
	_value = getDefault();
}

ParamInteger3D* ParamInteger3D::clone() const
{
	return new ParamInteger3D( *this );
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

OfxStatus ParamInteger3D::get( int& x, int& y, int& z )
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
	return kOfxStatOK;
}

OfxStatus ParamInteger3D::get( OfxTime time, int& x, int& y, int& z )
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
	return kOfxStatOK;
}

OfxStatus ParamInteger3D::set( int x, int y, int z )
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
	return kOfxStatOK;
}

OfxStatus ParamInteger3D::set( OfxTime time, int x, int y, int z )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

//
// Double3DInstance
//
ParamDouble3D::ParamDouble3D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".z_", descriptor ) );
	_value = getDefault();
}

ParamDouble3D* ParamDouble3D::clone() const
{
	return new ParamDouble3D( *this );
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
	Ofx3DPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

OfxStatus ParamDouble3D::get( double& x, double& y, double& z )
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
	return kOfxStatOK;
}

OfxStatus ParamDouble3D::get( OfxTime time, double& x, double& y, double& z )
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
	return kOfxStatOK;
}

OfxStatus ParamDouble3D::set( double x, double y, double z )
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
	return kOfxStatOK;
}

OfxStatus ParamDouble3D::set( OfxTime time, double x, double y, double z )
{
	_value.x = x;
	_value.y = y;
	return kOfxStatOK;
}

//
// PushbuttonInstance
//
ParamPushButton::ParamPushButton( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor )
	: tuttle::host::ofx::attribute::OfxhParamPushButton( descriptor, effect ),
	_effect( effect )

{}

ParamPushButton* ParamPushButton::clone() const
{
	return new ParamPushButton( *this );
}

}
}
}
