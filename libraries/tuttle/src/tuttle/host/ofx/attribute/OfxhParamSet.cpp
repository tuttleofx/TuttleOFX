#include "OfxhParamSet.hpp"

#include <boost/foreach.hpp>


namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

OfxhParamSet::OfxhParamSet()
{}

OfxhParamSet::OfxhParamSet( const OfxhParamSet& other )
{
	operator=( other );
}

void OfxhParamSet::initMapFromList()
{
	BOOST_FOREACH( OfxhParam& p, _paramVector )
	{
		_params[p.getName()] = &p;
		_paramsByScriptName[p.getScriptName()] = &p;
	}
}

OfxhParamSet::~OfxhParamSet()
{}

void OfxhParamSet::operator=( const OfxhParamSet& other )
{
	_paramVector = other._paramVector.clone();
	initMapFromList();
}

void OfxhParamSet::copyParamsValues( const OfxhParamSet& other )
{
	if( _paramVector.size() != other._paramVector.size() )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
		    << exception::dev( "You try to copy parameters values, but the two lists are not identical." ) );
	}

	ParamVector::const_iterator oit = other._paramVector.begin(), oitEnd = other._paramVector.end();
	for( ParamVector::iterator it = _paramVector.begin(), itEnd = _paramVector.end();
	     it != itEnd && oit != oitEnd;
	     ++it, ++oit )
	{
		OfxhParam& p        = *it;
		const OfxhParam& op = *oit;
		if( p.getName() != op.getName() )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
			    << exception::dev( "You try to copy parameters values, but it is not the same parameters in the two lists." ) );
		}
		p.copy( op );
	}
	initMapFromList();
}

//void OfxhParamSet::referenceParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC
//{
	//	if( _allParams.find( name ) != _allParams.end() )
	//	{
	//		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists, "Trying to reference a new parameter which already exists." ) );
	//	}
	//	_allParams[name] = instance;
//}

void OfxhParamSet::addParam( OfxhParam* instance ) OFX_EXCEPTION_SPEC
{
	if( _params.find( instance->getName() ) != _params.end() )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists, "Trying to add a new parameter which already exists." ) );
	}
	_paramVector.push_back( instance );
	_params[instance->getName()] = instance;
	_paramsByScriptName[instance->getScriptName()] = instance;
	//	referenceParam( name, instance );
}

}
}
}
}

