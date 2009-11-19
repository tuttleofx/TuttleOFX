/*
Software License :

Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
      contributors may be used to endorse or promote products derived from this
      software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <fstream>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/ofxhBinary.h>
#include <tuttle/host/ofx/ofxhPropertySuite.h>
#include <tuttle/host/ofx/ofxhClip.h>
#include <tuttle/host/ofx/ofxhParam.h>
#include <tuttle/host/ofx/ofxhMemory.h>
#include <tuttle/host/ofx/ofxhImageEffect.h>
#include <tuttle/host/ofx/ofxhPluginAPICache.h>
#include <tuttle/host/ofx/ofxhPluginCache.h>
#include <tuttle/host/ofx/ofxhHost.h>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>
#include "ofx/ofxhAttribute.h"

// my host
#include "HostDescriptor.hpp"
#include "EffectInstance.hpp"
#include "ClipInstance.hpp"
#include "ParamInstance.hpp"

namespace tuttle {
namespace host {
namespace core {

//
// StringInstance
//
StringInstance::StringInstance( EffectInstance* effect,
                                const std::string& name,
                                tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::ParamStringInstance( descriptor, setInstance )
, _effect( effect )
{
    _value = getDefault();
}

const std::string & StringInstance::getDefault() const
{
    return getProperties().getStringProperty( kOfxParamPropDefault );
}

OfxStatus StringInstance::get( std::string& v )
{
    v = _value;
    return kOfxStatOK;
}

OfxStatus StringInstance::get( OfxTime time, std::string& v )
{
    v = _value; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus StringInstance::set( const char* v )
{
    _value = std::string(v);
    return kOfxStatOK;
}

OfxStatus StringInstance::set( OfxTime time, const char* v )
{
    _value = std::string(v); ///< @todo: in time !
    return kOfxStatOK;
}

//
// IntegerInstance
//
IntegerInstance::IntegerInstance( EffectInstance* effect,
                                  const std::string& name,
                                  tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                  tuttle::host::ofx::attribute::ParamInstanceSet & setInstance)
: tuttle::host::ofx::attribute::ParamIntegerInstance( descriptor, setInstance )
, _effect( effect )
{
    _value = getDefault();
}

int IntegerInstance::getDefault() const
{
    return getProperties().getIntProperty( kOfxParamPropDefault );
}

OfxStatus IntegerInstance::get( int& v )
{
    v = _value;
    return kOfxStatOK;
}

OfxStatus IntegerInstance::get( OfxTime time, int& v )
{
    v = _value; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus IntegerInstance::set( int v )
{
    _value = v;
    return kOfxStatOK;
}

OfxStatus IntegerInstance::set( OfxTime time, int v )
{
    _value = v; ///< @todo: in time !
    return kOfxStatOK;
}

//
// DoubleInstance
//
DoubleInstance::DoubleInstance( EffectInstance* effect,
                                const std::string& name,
                                tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::ParamDoubleInstance( descriptor, setInstance )
, _effect( effect )

{
    _value = getDefault();
}

double DoubleInstance::getDefault() const
{
    return getProperties().getDoubleProperty( kOfxParamPropDefault );
}

OfxStatus DoubleInstance::get( double& v )
{
    v = _value;
    return kOfxStatOK;
}

OfxStatus DoubleInstance::get( OfxTime time, double& v )
{
    v = _value; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus DoubleInstance::set( double v )
{
    _value = v;
    return kOfxStatOK;
}

OfxStatus DoubleInstance::set( OfxTime time, double v )
{
    _value = v; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus DoubleInstance::derive( OfxTime time, double& )
{
    return kOfxStatErrMissingHostFeature;
}

OfxStatus DoubleInstance::integrate( OfxTime time1, OfxTime time2, double& )
{
    return kOfxStatErrMissingHostFeature;
}

//
// BooleanInstance
//
BooleanInstance::BooleanInstance( EffectInstance* effect,
                                  const std::string& name,
                                  tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                  tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::ParamBooleanInstance( descriptor, setInstance )
, _effect( effect )

{
    _value = getDefault();
}

bool BooleanInstance::getDefault() const
{
    return static_cast<bool>(getProperties().getIntProperty( kOfxParamPropDefault ));
}

OfxStatus BooleanInstance::get( bool& v )
{
    v = _value;
    return kOfxStatOK;
}

OfxStatus BooleanInstance::get( OfxTime time, bool& v )
{
    v = _value; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus BooleanInstance::set( bool v )
{
    _value = v;
    return kOfxStatOK;
}

OfxStatus BooleanInstance::set( OfxTime time, bool v )
{
    _value = v; ///< @todo: in time !
    return kOfxStatOK;
}

//
// ChoiceInteger
//
ChoiceInstance::ChoiceInstance( EffectInstance* effect,
                                const std::string& name,
                                tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::ParamChoiceInstance( descriptor, setInstance )
, _effect( effect )

{
    _value = getDefault();
}

int ChoiceInstance::getDefault() const
{
    return getProperties().getIntProperty( kOfxParamPropDefault );
}

OfxStatus ChoiceInstance::get( int& v )
{
    v = _value;
    return kOfxStatOK;
}

OfxStatus ChoiceInstance::get( OfxTime time, int& v )
{
    v = _value; ///< @todo: in time !
    return kOfxStatOK;
}

OfxStatus ChoiceInstance::set( int v )
{
    _value = v;
    return kOfxStatOK;
}

OfxStatus ChoiceInstance::set( OfxTime time, int v )
{
    _value = v; ///< @todo: in time !
    return kOfxStatOK;
}

//
// RGBAInstance
//
RGBAInstance::RGBAInstance( EffectInstance* effect,
                            const std::string& name,
                            tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                            tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 4>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new DoubleInstance(effect, name+".r_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".g_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".b_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".a_", descriptor, setInstance) );

    _value = getDefault();
}

OfxRGBAColourD RGBAInstance::getDefault() const
{
    OfxRGBAColourD color;
    getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 4 );
    return color;
}

OfxStatus RGBAInstance::get( double& r, double& g, double& b, double& a )
{
    r = _value.r;
    g = _value.g;
    b = _value.b;
    a = _value.a;
    return kOfxStatOK;
}

OfxStatus RGBAInstance::get( OfxTime time, double& r, double& g, double& b, double& a )
{
    r = _value.r;
    g = _value.g;
    b = _value.b;
    a = _value.a;
    return kOfxStatOK;
}

OfxStatus RGBAInstance::set( double r, double g, double b, double a )
{
    _value.r = r;
    _value.g = g;
    _value.b = b;
    _value.a = a;
    return kOfxStatOK;
}

OfxStatus RGBAInstance::set( OfxTime time, double r, double g, double b, double a )
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
RGBInstance::RGBInstance( EffectInstance* effect,
                          const std::string& name,
                          tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                          tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 3>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new DoubleInstance(effect, name+".r_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".g_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".b_", descriptor, setInstance) );
    _value = getDefault();
}

OfxRGBColourD RGBInstance::getDefault() const
{
    OfxRGBColourD color;
    getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 3 );
    return color;
}

OfxStatus RGBInstance::get( double& r, double& g, double& b )
{
    r = _value.r;
    g = _value.g;
    b = _value.b;
    return kOfxStatOK;
}

OfxStatus RGBInstance::get( OfxTime time, double& r, double& g, double& b )
{
    r = _value.r;
    g = _value.g;
    b = _value.b;
    return kOfxStatOK;
}

OfxStatus RGBInstance::set( double r, double g, double b )
{
    _value.r = r;
    _value.g = g;
    _value.b = b;
    return kOfxStatOK;
}

OfxStatus RGBInstance::set( OfxTime time, double r, double g, double b )
{
    _value.r = r;
    _value.g = g;
    _value.b = b;
    return kOfxStatOK;
}

//
// Double2DInstance
//
Double2DInstance::Double2DInstance( EffectInstance* effect,
                                    const std::string& name,
                                    tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                    tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 2>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new DoubleInstance(effect, name+".x_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".y_", descriptor, setInstance) );
    _value = getDefault();
}

OfxPointD Double2DInstance::getDefault() const
{
    OfxPointD point;
    getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 2 );
    return point;
}

OfxStatus Double2DInstance::get( double& x, double& y )
{
    x = _value.x;
    y = _value.y;
    return kOfxStatOK;
}

OfxStatus Double2DInstance::get( OfxTime time, double& x, double& y )
{
    x = _value.x;
    y = _value.y;
    return kOfxStatOK;
}

OfxStatus Double2DInstance::set( double x, double y )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

OfxStatus Double2DInstance::set( OfxTime time, double x, double y )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

//
// Integer2DInstance
//
Integer2DInstance::Integer2DInstance( EffectInstance* effect,
                                      const std::string& name,
                                      tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                      tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<IntegerInstance, 2>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new IntegerInstance(effect, name+".x_", descriptor, setInstance) );
    _controls.push_back( new IntegerInstance(effect, name+".y_", descriptor, setInstance) );
    _value = getDefault();
}

OfxPointI Integer2DInstance::getDefault() const
{
    OfxPointI point;
    getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 2 );
    return point;
}

OfxStatus Integer2DInstance::get( int& x, int& y )
{
    x = _value.x;
    y = _value.y;
    return kOfxStatOK;
}

OfxStatus Integer2DInstance::get( OfxTime time, int& x, int& y )
{
    x = _value.x;
    y = _value.y;
    return kOfxStatOK;
}

OfxStatus Integer2DInstance::set( int x, int y )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

OfxStatus Integer2DInstance::set( OfxTime time, int x, int y )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

//
// Integer3DInstance
//
Integer3DInstance::Integer3DInstance( EffectInstance* effect,
                                      const std::string& name,
                                      tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                      tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<IntegerInstance, 3>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new IntegerInstance(effect, name+".x_", descriptor, setInstance) );
    _controls.push_back( new IntegerInstance(effect, name+".y_", descriptor, setInstance) );
    _controls.push_back( new IntegerInstance(effect, name+".z_", descriptor, setInstance) );
    _value = getDefault();
}

Ofx3DPointI Integer3DInstance::getDefault() const
{
    Ofx3DPointI point;
    getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 3 );
    return point;
}

OfxStatus Integer3DInstance::get( int& x, int& y, int& z )
{
    x = _value.x;
    y = _value.y;
    z = _value.z;
    return kOfxStatOK;
}

OfxStatus Integer3DInstance::get( OfxTime time, int& x, int& y, int& z )
{
    x = _value.x;
    y = _value.y;
    z = _value.z;
    return kOfxStatOK;
}

OfxStatus Integer3DInstance::set( int x, int y, int z )
{
    _value.x = x;
    _value.y = y;
    _value.z = z;
    return kOfxStatOK;
}

OfxStatus Integer3DInstance::set( OfxTime time, int x, int y, int z )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

//
// Double3DInstance
//
Double3DInstance::Double3DInstance( EffectInstance* effect,
                                    const std::string& name,
                                    tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                    tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 3>( descriptor, setInstance )
, _effect( effect )

{
    _controls.push_back( new DoubleInstance(effect, name+".x_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".y_", descriptor, setInstance) );
    _controls.push_back( new DoubleInstance(effect, name+".z_", descriptor, setInstance) );
    _value = getDefault();
}

Ofx3DPointD Double3DInstance::getDefault() const
{
    Ofx3DPointD point;
    getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 3 );
    return point;
}

OfxStatus Double3DInstance::get( double& x, double& y, double& z )
{
    x = _value.x;
    y = _value.y;
    z = _value.z;
    return kOfxStatOK;
}

OfxStatus Double3DInstance::get( OfxTime time, double& x, double& y, double& z )
{
    x = _value.x;
    y = _value.y;
    z = _value.z;
    return kOfxStatOK;
}

OfxStatus Double3DInstance::set( double x, double y, double z )
{
    _value.x = x;
    _value.y = y;
    _value.z = z;
    return kOfxStatOK;
}

OfxStatus Double3DInstance::set( OfxTime time, double x, double y, double z )
{
    _value.x = x;
    _value.y = y;
    return kOfxStatOK;
}

//
// PushbuttonInstance
//
PushbuttonInstance::PushbuttonInstance( EffectInstance* effect,
                                        const std::string& name,
                                        tuttle::host::ofx::attribute::ParamDescriptor& descriptor,
                                        tuttle::host::ofx::attribute::ParamInstanceSet & setInstance )
: tuttle::host::ofx::attribute::ParamPushbuttonInstance( descriptor, setInstance )
, _effect( effect )

{
}

}
}
}
