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
#ifndef _TUTTLE_PARAM_INSTANCE_HPP
#define _TUTTLE_PARAM_INSTANCE_HPP

#include <iostream>
#include <tuttle/host/ofx/ofxhParam.h>
#include <tuttle/host/core/EffectInstance.hpp>

namespace tuttle {
namespace host {
namespace core {

class PushbuttonInstance : public tuttle::host::ofx::attribute::ParamPushbuttonInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
public:
	PushbuttonInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
};

class StringInstance : public tuttle::host::ofx::attribute::ParamStringInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	std::string _value; /// @todo link de parametres
public:
	StringInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	const std::string & getDefault( ) const;
	OfxStatus get( std::string& );
	OfxStatus get( OfxTime time, std::string& );
	OfxStatus set( const char* );
	OfxStatus set( OfxTime time, const char* );
};

class IntegerInstance : public tuttle::host::ofx::attribute::ParamIntegerInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	int _value; /// @todo link de parametres
public:
	IntegerInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	int getDefault( ) const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( int );
	OfxStatus set( OfxTime time, int );
};

class DoubleInstance : public tuttle::host::ofx::attribute::ParamDoubleInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	double _value;
public:
	DoubleInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	double getDefault( ) const;
	OfxStatus get( double& );
	OfxStatus get( OfxTime time, double& );
	OfxStatus set( double );
	OfxStatus set( OfxTime time, double );
	OfxStatus derive( OfxTime time, double& );
	OfxStatus integrate( OfxTime time1, OfxTime time2, double& );
};

class BooleanInstance : public tuttle::host::ofx::attribute::ParamBooleanInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	bool _value;
public:
	BooleanInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	bool getDefault( ) const;
	OfxStatus get( bool& );
	OfxStatus get( OfxTime time, bool& );
	OfxStatus set( bool );
	OfxStatus set( OfxTime time, bool );
};

class ChoiceInstance : public tuttle::host::ofx::attribute::ParamChoiceInstance
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	int _value;
public:
	ChoiceInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	int getDefault( ) const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( int );
	OfxStatus set( OfxTime time, int );
};

class RGBAInstance : public tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 4 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	OfxRGBAColourD _value;
public:
	RGBAInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	OfxRGBAColourD getDefault( ) const;
	OfxStatus get( double& r, double& g, double& b, double& a );
	OfxStatus get( OfxTime time, double& r, double& g, double& b, double& a );
	OfxStatus set( double r, double g, double b, double a );
	OfxStatus set( OfxTime time, double r, double g, double b, double a );
};

class RGBInstance : public tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 3 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	OfxRGBColourD _value;
public:
	RGBInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	OfxRGBColourD getDefault( ) const;
	OfxStatus get( double& r, double& g, double& b );
	OfxStatus get( OfxTime time, double& r, double& g, double& b );
	OfxStatus set( double r, double g, double b );
	OfxStatus set( OfxTime time, double r, double g, double b );
};

class Double2DInstance : public tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 2 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	OfxPointD _value;
public:
	Double2DInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	OfxPointD getDefault( ) const;
	OfxStatus get( double& x, double& y );
	OfxStatus get( OfxTime time, double& x, double& y );
	OfxStatus set( double x, double y );
	OfxStatus set( OfxTime time, double x, double y );
};

class Integer2DInstance : public tuttle::host::ofx::attribute::MultiDimParam<IntegerInstance, 2 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	OfxPointI _value;
public:
	Integer2DInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	OfxPointI getDefault( ) const;
	OfxStatus get( int& x, int& y );
	OfxStatus get( OfxTime time, int& x, int& y );
	OfxStatus set( int x, int y );
	OfxStatus set( OfxTime time, int x, int y );
};

class Integer3DInstance : public tuttle::host::ofx::attribute::MultiDimParam<IntegerInstance, 3 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	Ofx3DPointI _value;
public:
	Integer3DInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	Ofx3DPointI getDefault( ) const;
	OfxStatus get( int& x, int& y, int& z );
	OfxStatus get( OfxTime time, int& x, int& y, int& z );
	OfxStatus set( int x, int y, int z );
	OfxStatus set( OfxTime time, int x, int y, int z );
};

class Double3DInstance : public tuttle::host::ofx::attribute::MultiDimParam<DoubleInstance, 3 >
{
protected:
	tuttle::host::core::EffectInstance* _effect;
	tuttle::host::ofx::attribute::ParamDescriptor& _descriptor;
	Ofx3DPointD _value;
public:
	Double3DInstance( EffectInstance* effect, const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet & setInstance );
	Ofx3DPointD getDefault( ) const;
	OfxStatus get( double& x, double& y, double& z );
	OfxStatus get( OfxTime time, double& x, double& y, double& z );
	OfxStatus set( double x, double y, double z );
	OfxStatus set( OfxTime time, double x, double y, double z );
};

}
}
}

#endif
