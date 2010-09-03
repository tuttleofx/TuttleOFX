#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMCHOICE_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMCHOICE_HPP_

#include "OfxhParam.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamChoice
	: public OfxhParam
	, public OfxhKeyframeParam
{
public:
	OfxhParamChoice( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}
	virtual ~OfxhParamChoice() {}

	int                getIndexFor( const std::string& key ) const;
	const std::string& getValueForId( const int id ) const;

	// Deriving implementatation needs to overide these
	virtual void get( int& ) const OFX_EXCEPTION_SPEC                                                 = 0;
	virtual void getAtTime( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC                       = 0;
	virtual void set( const int&, const EChange change ) OFX_EXCEPTION_SPEC                           = 0;
	virtual void setAtTime( const OfxTime time, const int&, const EChange change ) OFX_EXCEPTION_SPEC = 0;

	void get( std::string& key ) const OFX_EXCEPTION_SPEC
	{
		int id = 0;

		get( id );
		key = getValueForId( id );
	}

	void getAtTime( const OfxTime time, std::string& key ) const OFX_EXCEPTION_SPEC
	{
		int id = 0;

		getAtTime( time, id );
		key = getValueForId( id );
	}

	void set( const std::string& key, const EChange change ) OFX_EXCEPTION_SPEC
	{
		set( getIndexFor( key ), change );
	}

	void setAtTime( const OfxTime time, const std::string& key, const EChange change ) OFX_EXCEPTION_SPEC
	{
		setAtTime( time, getIndexFor( key ), change );
	}

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;
};

}
}
}
}

#endif

