#ifndef _TUTTLE_PARAM_INSTANCE_HPP
#define _TUTTLE_PARAM_INSTANCE_HPP

#include <iostream>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamString : public ofx::attribute::OfxhParamString
{
protected:
	ImageEffectNode& _effect;
	std::string _value; /// @todo link de parametres

public:
	ParamString( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamString* clone() const { return new ParamString( *this ); }

	const std::string& getDefault() const;
	void          get( std::string& ) const OFX_EXCEPTION_SPEC;
	void          get( const OfxTime time, std::string& ) const OFX_EXCEPTION_SPEC;
	void          set( const char* ) OFX_EXCEPTION_SPEC;
	void          set( const std::string& value ) OFX_EXCEPTION_SPEC { _value = value; }
	void          set( const OfxTime time, const char* ) OFX_EXCEPTION_SPEC;
	void          set( const OfxTime time, const std::string& value ) OFX_EXCEPTION_SPEC { _value = value; }
	void          copy( const ParamString& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamString& param = dynamic_cast<const ParamString&>(p);
		copy( param );
	}
};

class ParamInteger : public ofx::attribute::OfxhParamInteger
{
protected:
	ImageEffectNode& _effect;
	int _value; /// @todo link de parametres

public:
	ParamInteger( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger* clone() const { return new ParamInteger( *this ); }

	int       getDefault() const;
	void get( int& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC;
	void set( const int &) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &) OFX_EXCEPTION_SPEC;
	void          copy( const ParamInteger& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamInteger& param = dynamic_cast<const ParamInteger&>(p);
		copy( param );
	}
};

class ParamDouble : public ofx::attribute::OfxhParamDouble
{
protected:
	ImageEffectNode& _effect;
	double _value;

public:
	ParamDouble( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamDouble* clone() const { return new ParamDouble( *this ); }

	double    getDefault() const;
	void get( double& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void set( const double& ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double& ) OFX_EXCEPTION_SPEC;
	void derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC;
	void          copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamDouble& param = dynamic_cast<const ParamDouble&>(p);
		copy( param );
	}
};

class ParamBoolean : public ofx::attribute::OfxhParamBoolean
{
protected:
	ImageEffectNode& _effect;

	bool _value;

public:
	ParamBoolean( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamBoolean* clone() const { return new ParamBoolean( *this ); }

	bool      getDefault() const;
	void get( bool& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, bool& ) const OFX_EXCEPTION_SPEC;
	void set( const bool& ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const bool& ) OFX_EXCEPTION_SPEC;
	void copy( const ParamBoolean& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamBoolean& param = dynamic_cast<const ParamBoolean&>(p);
		copy( param );
	}
};

class ParamChoice : public ofx::attribute::OfxhParamChoice
{
protected:
	ImageEffectNode& _effect;

	int _value;

public:
	ParamChoice( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamChoice* clone() const { return new ParamChoice( *this ); }

	int       getDefault() const;
	void get( int& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC;
	void set( const int& ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int& ) OFX_EXCEPTION_SPEC;
	void          copy( const ParamChoice& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamChoice& param = dynamic_cast<const ParamChoice&>(p);
		copy( param );
	}
};

class ParamRGBA : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 4 >
{
protected:
	ImageEffectNode& _effect;

	OfxRGBAColourD _value;

public:
	ParamRGBA( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamRGBA* clone() const { return new ParamRGBA( *this ); }

	OfxRGBAColourD getDefault() const;
	void get( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC;
	void set( const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC;
	void copy( const ParamRGBA& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamRGBA& param = dynamic_cast<const ParamRGBA&>(p);
		copy( param );
	}
};

class ParamRGB : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
protected:
	ImageEffectNode& _effect;

	OfxRGBColourD _value;

public:
	ParamRGB( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamRGB* clone() const { return new ParamRGB( *this ); }

	OfxRGBColourD getDefault() const;
	void get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC;
	void set( const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC;
	void copy( const ParamRGB& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamRGB& param = dynamic_cast<const ParamRGB&>(p);
		copy( param );
	}
};

class ParamDouble2D : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 2 >
{
protected:
	ImageEffectNode& _effect;

	OfxPointD _value;

public:
	ParamDouble2D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamDouble2D* clone() const { return new ParamDouble2D( *this ); }

	OfxPointD getDefault() const;
	void get( double& x, double& y ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC;
	void set( const double &x, const double &y ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &x, const double &y ) OFX_EXCEPTION_SPEC;
	void copy( const ParamDouble2D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamDouble2D& param = dynamic_cast<const ParamDouble2D&>(p);
		copy( param );
	}
};

class ParamInteger2D : public ofx::attribute::OfxhMultiDimParam<ParamInteger, 2 >
{
protected:
	ImageEffectNode& _effect;

	OfxPointI _value;

public:
	ParamInteger2D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger2D* clone() const { return new ParamInteger2D( *this ); }

	OfxPointI getDefault() const;
	void get( int& x, int& y ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC;
	void set( const int &x, const int &y ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &x, const int &y ) OFX_EXCEPTION_SPEC;
	void copy( const ParamInteger2D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamInteger2D& param = dynamic_cast<const ParamInteger2D&>(p);
		copy( param );
	}
};

class ParamInteger3D : public ofx::attribute::OfxhMultiDimParam<ParamInteger, 3 >
{
protected:
	ImageEffectNode& _effect;

	Ofx3DPointI _value;

public:
	ParamInteger3D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger3D* clone() const { return new ParamInteger3D( *this ); }

	Ofx3DPointI getDefault() const;
	void get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void set( const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC;
	void copy( const ParamInteger3D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamInteger3D& param = dynamic_cast<const ParamInteger3D&>(p);
		copy( param );
	}
};

class ParamDouble3D : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 3 >
{
protected:
	ImageEffectNode& _effect;

	Ofx3DPointD _value;

public:
	ParamDouble3D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamDouble3D* clone() const { return new ParamDouble3D( *this ); }

	Ofx3DPointD getDefault() const;
	void get( double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC;
	void set( const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC;
	void copy( const ParamDouble3D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamDouble3D& param = dynamic_cast<const ParamDouble3D&>(p);
		copy( param );
	}
};


class ParamPushButton : public ofx::attribute::OfxhParamPushButton
{
protected:
	ImageEffectNode& _effect;

public:
	ParamPushButton( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPushButton* clone() const { return new ParamPushButton( *this ); }

	void copy( const ParamPushButton& p ) OFX_EXCEPTION_SPEC {}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamPushButton& param = dynamic_cast<const ParamPushButton&>(p);
		copy( param );
	}
};

class ParamGroup : public ofx::attribute::OfxhParamGroup
{
protected:
	ImageEffectNode& _effect;

public:
	ParamGroup( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamGroup* clone() const { return new ParamGroup(*this); }
	
	void copy( const ParamGroup& p ) OFX_EXCEPTION_SPEC {}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamGroup& param = dynamic_cast<const ParamGroup&>(p);
		copy( param );
	}
};

class ParamPage : public ofx::attribute::OfxhParamPage
{
protected:
	ImageEffectNode& _effect;

public:
	ParamPage( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamPage* clone() const { return new ParamPage(*this); }

	void copy( const ParamPage& p ) OFX_EXCEPTION_SPEC {}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamPage& param = dynamic_cast<const ParamPage&>(p);
		copy( param );
	}
};

}
}
}

#endif
