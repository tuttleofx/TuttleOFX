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
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamPushButton : public tuttle::host::ofx::attribute::OfxhParamPushButton
{
protected:
	ImageEffectNode& _effect;

public:
	ParamPushButton( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamPushButton* clone() const;
};

class ParamString : public tuttle::host::ofx::attribute::OfxhParamString
{
protected:
	ImageEffectNode& _effect;
	std::string _value; /// @todo link de parametres

public:
	ParamString( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamString* clone() const;

	const std::string& getDefault() const;
	OfxStatus          get( std::string& );
	OfxStatus          get( OfxTime time, std::string& );
	OfxStatus          set( const char* );
	OfxStatus          set( const std::string& value ) { _value = value; return kOfxStatOK; }
	OfxStatus          set( OfxTime time, const char* );
};

class ParamInteger : public tuttle::host::ofx::attribute::OfxhParamInteger
{
protected:
	ImageEffectNode& _effect;
	int _value; /// @todo link de parametres

public:
	ParamInteger( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamInteger* clone() const;

	int       getDefault() const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( const int& );
	OfxStatus set( OfxTime time, const int& );
};

class ParamDouble : public tuttle::host::ofx::attribute::OfxhParamDouble
{
protected:
	ImageEffectNode& _effect;
	double _value;

public:
	ParamDouble( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamDouble* clone() const;

	double    getDefault() const;
	OfxStatus get( double& );
	OfxStatus get( OfxTime time, double& );
	OfxStatus set( const double& );
	OfxStatus set( OfxTime time, const double& );
	OfxStatus derive( OfxTime time, double& );
	OfxStatus integrate( OfxTime time1, OfxTime time2, double& );
};

class ParamBoolean : public tuttle::host::ofx::attribute::OfxhParamBoolean
{
protected:
	ImageEffectNode& _effect;

	bool _value;

public:
	ParamBoolean( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamBoolean* clone() const;

	bool      getDefault() const;
	OfxStatus get( bool& );
	OfxStatus get( OfxTime time, bool& );
	OfxStatus set( bool );
	OfxStatus set( OfxTime time, bool );
};

class ParamChoice : public tuttle::host::ofx::attribute::OfxhParamChoice
{
protected:
	ImageEffectNode& _effect;

	int _value;

public:
	ParamChoice( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamChoice* clone() const;

	int       getDefault() const;
	OfxStatus get( int& );
	OfxStatus get( OfxTime time, int& );
	OfxStatus set( int );
	OfxStatus set( OfxTime time, int );
};

class ParamRGBA : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 4 >
{
protected:
	ImageEffectNode& _effect;

	OfxRGBAColourD _value;

public:
	ParamRGBA( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamRGBA* clone() const;

	OfxRGBAColourD getDefault() const;
	OfxStatus      get( double& r, double& g, double& b, double& a );
	OfxStatus      get( OfxTime time, double& r, double& g, double& b, double& a );
	OfxStatus      set( double r, double g, double b, double a );
	OfxStatus      set( OfxTime time, double r, double g, double b, double a );
};

class ParamRGB : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
protected:
	ImageEffectNode& _effect;

	OfxRGBColourD _value;

public:
	ParamRGB( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamRGB* clone() const;

	OfxRGBColourD getDefault() const;
	OfxStatus     get( double& r, double& g, double& b );
	OfxStatus     get( OfxTime time, double& r, double& g, double& b );
	OfxStatus     set( double r, double g, double b );
	OfxStatus     set( OfxTime time, double r, double g, double b );
};

class ParamDouble2D : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 2 >
{
protected:
	ImageEffectNode& _effect;

	OfxPointD _value;

public:
	ParamDouble2D( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamDouble2D* clone() const;

	OfxPointD getDefault() const;
	OfxStatus get( double& x, double& y );
	OfxStatus get( OfxTime time, double& x, double& y );
	OfxStatus set( double x, double y );
	OfxStatus set( OfxTime time, double x, double y );
};

class ParamInteger2D : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamInteger, 2 >
{
protected:
	ImageEffectNode& _effect;

	OfxPointI _value;

public:
	ParamInteger2D( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamInteger2D* clone() const;

	OfxPointI getDefault() const;
	OfxStatus get( int& x, int& y );
	OfxStatus get( OfxTime time, int& x, int& y );
	OfxStatus set( int x, int y );
	OfxStatus set( OfxTime time, int x, int y );
};

class ParamInteger3D : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamInteger, 3 >
{
protected:
	ImageEffectNode& _effect;

	Ofx3DPointI _value;

public:
	ParamInteger3D( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamInteger3D* clone() const;

	Ofx3DPointI getDefault() const;
	OfxStatus   get( int& x, int& y, int& z );
	OfxStatus   get( OfxTime time, int& x, int& y, int& z );
	OfxStatus   set( int x, int y, int z );
	OfxStatus   set( OfxTime time, int x, int y, int z );
};

class ParamDouble3D : public tuttle::host::ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
protected:
	ImageEffectNode& _effect;

	Ofx3DPointD _value;

public:
	ParamDouble3D( ImageEffectNode& effect, const std::string& name, tuttle::host::ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamDouble3D* clone() const;

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
