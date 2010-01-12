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
                          ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamString( descriptor, effect ),
	_effect( effect )
{
	_value = getDefault();
}

const std::string& ParamString::getDefault() const
{
	return getProperties().getStringProperty( kOfxParamPropDefault );
}

void ParamString::get( std::string& v ) OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamString::get( OfxTime time, std::string& v ) OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamString::set( const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v );
}

void ParamString::set( OfxTime time, const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v ); ///< @todo: in time !
}

//
// IntegerInstance
//
ParamInteger::ParamInteger( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamInteger( descriptor, effect ),
	_effect( effect )
{
	_value = getDefault();
}


int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamInteger::get( int& v ) OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamInteger::get( OfxTime time, int& v ) OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamInteger::set( const int& v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamInteger::set( OfxTime time, const int& v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// DoubleInstance
//
ParamDouble::ParamDouble( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamDouble( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault );
}

void ParamDouble::get( double& v ) OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamDouble::get( OfxTime time, double& v ) OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamDouble::set( const double& v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamDouble::set( OfxTime time, const double& v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

void ParamDouble::derive( OfxTime time, double& ) OFX_EXCEPTION_SPEC
{
	throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
}

void ParamDouble::integrate( OfxTime time1, OfxTime time2, double& ) OFX_EXCEPTION_SPEC
{
	throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
}

//
// BooleanInstance
//
ParamBoolean::ParamBoolean( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamBoolean( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

bool ParamBoolean::getDefault() const
{
	return static_cast<bool>( getProperties().getIntProperty( kOfxParamPropDefault ) != 0 );
}

void ParamBoolean::get( bool& v ) OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamBoolean::get( OfxTime time, bool& v ) OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamBoolean::set( bool v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamBoolean::set( OfxTime time, bool v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// ChoiceInteger
//
ParamChoice::ParamChoice( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamChoice( descriptor, effect ),
	_effect( effect )

{
	_value = getDefault();
}

int ParamChoice::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamChoice::get( int& v ) OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamChoice::get( OfxTime time, int& v ) OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamChoice::set( int v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamChoice::set( OfxTime time, int v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}

//
// RGBAInstance
//
ParamRGBA::ParamRGBA( ImageEffectNode&                                   effect,
                      const std::string&                                 name,
                      ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 4>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".a_", descriptor ) );

	_value = getDefault();
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
	OfxRGBAColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 4 );
	return color;
}

void ParamRGBA::get( double& r, double& g, double& b, double& a ) OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::get( OfxTime time, double& r, double& g, double& b, double& a ) OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::set( double r, double g, double b, double a ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
}

void ParamRGBA::set( OfxTime time, double r, double g, double b, double a ) OFX_EXCEPTION_SPEC
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
                    ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b_", descriptor ) );
	_value = getDefault();
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 3 );
	return color;
}

void ParamRGB::get( double& r, double& g, double& b ) OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::get( OfxTime time, double& r, double& g, double& b ) OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::set( double r, double g, double b ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
}

void ParamRGB::set( OfxTime time, double r, double g, double b ) OFX_EXCEPTION_SPEC
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
                              ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y_", descriptor ) );
	_value = getDefault();
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamDouble2D::get( double& x, double& y ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::get( OfxTime time, double& x, double& y ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::set( double x, double y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamDouble2D::set( OfxTime time, double x, double y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Integer2DInstance
//
ParamInteger2D::ParamInteger2D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y_", descriptor ) );
	_value = getDefault();
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamInteger2D::get( int& x, int& y ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::get( OfxTime time, int& x, int& y ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::set( int x, int y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamInteger2D::set( OfxTime time, int x, int y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Integer3DInstance
//
ParamInteger3D::ParamInteger3D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y_", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".z_", descriptor ) );
	_value = getDefault();
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamInteger3D::get( int& x, int& y, int& z ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::get( OfxTime time, int& x, int& y, int& z ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::set( int x, int y, int z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamInteger3D::set( OfxTime time, int x, int y, int z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// Double3DInstance
//
ParamDouble3D::ParamDouble3D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y_", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".z_", descriptor ) );
	_value = getDefault();
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
	Ofx3DPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamDouble3D::get( double& x, double& y, double& z ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::get( OfxTime time, double& x, double& y, double& z ) OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::set( double x, double y, double z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamDouble3D::set( OfxTime time, double x, double y, double z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

//
// PushbuttonInstance
//
ParamPushButton::ParamPushButton( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPushButton( descriptor, effect ),
	_effect( effect )
{}

//
// PushbuttonInstance
//
ParamGroup::ParamGroup( ImageEffectNode&                                   effect,
					  const std::string&                                 name,
					  ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamGroup( descriptor, effect ),
	_effect( effect )
{}

//
// PushbuttonInstance
//
ParamPage::ParamPage( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPage( descriptor, effect ),
	_effect( effect )
{}

}
}
}
