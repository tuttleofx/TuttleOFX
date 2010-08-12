#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSET_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSET_HPP_

#include "OfxhParamSetAccessor.hpp"
#include "OfxhParam.hpp"
#include <tuttle/host/ofx/OfxhIObject.hpp>

#include <boost/ptr_container/ptr_list.hpp>
#include <map>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


/// A set of parameters
///
/// As we are the owning object we delete the params inside ourselves. It was tempting
/// to make params autoref objects and have shared ownership with the client code
/// but that adds complexity for no strong gain.
class OfxhParamSet :
	public OfxhParamSetAccessor,
	virtual public OfxhIObject
{
public:
	typedef OfxhParamSet This;
	typedef std::map<std::string, OfxhParam*> ParamMap;
	typedef boost::ptr_list<OfxhParam> ParamList;
protected:
	ParamMap _params;        ///< params by name
	ParamList _paramList;    ///< params list

public:
	/// ctor
	///
	/// The propery set being passed in belongs to the owning
	/// plugin instance.
	explicit OfxhParamSet();

	explicit OfxhParamSet( const OfxhParamSet& other );

	/// dtor.
	virtual ~OfxhParamSet();

	void operator=( const OfxhParamSet& other );
	
	void copyParamsValues( const OfxhParamSet& other );
	
	bool operator==( const This& other ) const { return _paramList == other._paramList; }

	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	/// obtain a handle on this set for passing to the C api
	OfxParamSetHandle getParamSetHandle() const { return ( OfxParamSetHandle ) this; }

	const ParamMap& getParams() const { return _params; }
	ParamMap&       getParams()       { return _params; }

	const ParamList& getParamList() const { return _paramList; }
	ParamList&       getParamList()       { return _paramList; }

	// get the param
	OfxhParam& getParam( const std::string& name )
	{
		ParamMap::iterator it = _params.find( name );
		if( it == _params.end() )
			BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrBadIndex, std::string( "Param not found. (" ) + name + ")" ) );
		return *it->second;
	}
	const OfxhParam& getParam( const std::string& name ) const { return const_cast<This*>(this)->getParam( name ); }

#ifndef SWIG
	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChanged( const attribute::OfxhParam& param, const EChange change ) = 0;

	/// reference a param
	virtual void referenceParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC;

	/// add a param
	virtual void addParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC;

	/// make a parameter instance
	///
	/// Client host code needs to implement this
	virtual OfxhParam* newParam( const OfxhParamDescriptor& Descriptor ) OFX_EXCEPTION_SPEC = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	virtual void editBegin( const std::string& name ) OFX_EXCEPTION_SPEC = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	virtual void editEnd() OFX_EXCEPTION_SPEC = 0;

private:
	void initMapFromList();
#endif
};


}
}
}
}

#endif

