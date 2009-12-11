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
#ifndef _TUTTLE_PARAM_INSTANCE_HPP
#define _TUTTLE_PARAM_INSTANCE_HPP

#include <iostream>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/core/EffectInstance.hpp>

namespace tuttle {
namespace host {
namespace core {

class PushbuttonInstance : public tuttle::host::ofx::attribute::ParamPushbuttonInstance
{
//	PushbuttonInstance( const PushbuttonInstance& );

protected:
	EffectInstance& _effect;

public:
	PushbuttonInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual PushbuttonInstance* clone() const;
};

class StringInstance : public tuttle::host::ofx::attribute::ParamStringInstance
{
//	StringInstance( const StringInstance& );

protected:
	EffectInstance& _effect;
	std::string _value; /// @todo link de parametres

public:
	StringInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual StringInstance* clone() const;

	const std::string& getDefault() const;
	OfxStatus          get( std::string& );
	OfxStatus          get( OfxTime time, std::string& );
	OfxStatus          set( const char* );
	OfxStatus          set( OfxTime time, const char* );
};

class IntegerInstance : public tuttle::host::ofx::attribute::ParamIntegerInstance
{
//	IntegerInstance( const IntegerInstance& );

protected:
	EffectInstance& _effect;
	int _value; /// @todo link de parametres

public:
	IntegerInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual IntegerInstance* clone() const;

	int       getDefault() const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( int );
	OfxStatus set( OfxTime time, int );
};

class DoubleInstance : public tuttle::host::ofx::attribute::ParamDoubleInstance
{
//	DoubleInstance(const DoubleInstance&);

protected:
	EffectInstance& _effect;
	double _value;

public:
	DoubleInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual DoubleInstance* clone() const;

	double    getDefault() const;
	OfxStatus get( double& );
	OfxStatus get( OfxTime time, double& );
	OfxStatus set( double );
	OfxStatus set( OfxTime time, double );
	OfxStatus derive( OfxTime time, double& );
	OfxStatus integrate( OfxTime time1, OfxTime time2, double& );
};

class BooleanInstance : public tuttle::host::ofx::attribute::ParamBooleanInstance
{
//	BooleanInstance(const BooleanInstance&);

protected:
	EffectInstance& _effect;

	bool _value;

public:
	BooleanInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual BooleanInstance* clone() const;

	bool      getDefault() const;
	OfxStatus get( bool& );
	OfxStatus get( OfxTime time, bool& );
	OfxStatus set( bool );
	OfxStatus set( OfxTime time, bool );
};

class ChoiceInstance : public tuttle::host::ofx::attribute::ParamChoiceInstance
{
//	ChoiceInstance(const ChoiceInstance&);

protected:
	EffectInstance& _effect;

	int _value;

public:
	ChoiceInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ChoiceInstance* clone() const;

	int       getDefault() const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( int );
	OfxStatus set( OfxTime time, int );
};

class RGBAInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<DoubleInstance, 4 >
{
//	RGBAInstance(const RGBAInstance&);

protected:
	EffectInstance& _effect;

	OfxRGBAColourD _value;

public:
	RGBAInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual RGBAInstance* clone() const;

	OfxRGBAColourD getDefault() const;
	OfxStatus      get( double& r, double& g, double& b, double& a );
	OfxStatus      get( OfxTime time, double& r, double& g, double& b, double& a );
	OfxStatus      set( double r, double g, double b, double a );
	OfxStatus      set( OfxTime time, double r, double g, double b, double a );
};

class RGBInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<DoubleInstance, 3 >
{
//	RGBInstance(const RGBInstance&);

protected:
	EffectInstance& _effect;

	OfxRGBColourD _value;

public:
	RGBInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual RGBInstance* clone() const;

	OfxRGBColourD getDefault() const;
	OfxStatus     get( double& r, double& g, double& b );
	OfxStatus     get( OfxTime time, double& r, double& g, double& b );
	OfxStatus     set( double r, double g, double b );
	OfxStatus     set( OfxTime time, double r, double g, double b );
};

class Double2DInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<DoubleInstance, 2 >
{
//	Double2DInstance(const Double2DInstance&);

protected:
	EffectInstance& _effect;

	OfxPointD _value;

public:
	Double2DInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual Double2DInstance* clone() const;

	OfxPointD getDefault() const;
	OfxStatus get( double& x, double& y );
	OfxStatus get( OfxTime time, double& x, double& y );
	OfxStatus set( double x, double y );
	OfxStatus set( OfxTime time, double x, double y );
};

class Integer2DInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<IntegerInstance, 2 >
{
//	Integer2DInstance(const Integer2DInstance&);

protected:
	EffectInstance& _effect;

	OfxPointI _value;

public:
	Integer2DInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual Integer2DInstance* clone() const;

	OfxPointI getDefault() const;
	OfxStatus get( int& x, int& y );
	OfxStatus get( OfxTime time, int& x, int& y );
	OfxStatus set( int x, int y );
	OfxStatus set( OfxTime time, int x, int y );
};

class Integer3DInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<IntegerInstance, 3 >
{
//	Integer3DInstance(const Integer3DInstance&);

protected:
	EffectInstance& _effect;

	Ofx3DPointI _value;

public:
	Integer3DInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual Integer3DInstance* clone() const;

	Ofx3DPointI getDefault() const;
	OfxStatus   get( int& x, int& y, int& z );
	OfxStatus   get( OfxTime time, int& x, int& y, int& z );
	OfxStatus   set( int x, int y, int z );
	OfxStatus   set( OfxTime time, int x, int y, int z );
};

class Double3DInstance : public tuttle::host::ofx::attribute::OfxhMultiDimParam<DoubleInstance, 3 >
{
//	Double3DInstance(const Double3DInstance&);

protected:
	EffectInstance& _effect;

	Ofx3DPointD _value;

public:
	Double3DInstance( EffectInstance& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual Double3DInstance* clone() const;

	Ofx3DPointD getDefault() const;
	OfxStatus   get( double& x, double& y, double& z );
	OfxStatus   get( OfxTime time, double& x, double& y, double& z );
	OfxStatus   set( double x, double y, double z );
	OfxStatus   set( OfxTime time, double x, double y, double z );
};

}
}
}

#endif
