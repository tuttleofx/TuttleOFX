#include "OfxhParamSet.hpp"

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
	for( ParamList::iterator it = _paramList.begin(), itEnd = _paramList.end();
	     it != itEnd;
	     ++it )
	{
		_params[it->getName()] = &( *it );
	}
}

OfxhParamSet::~OfxhParamSet()
{}

void OfxhParamSet::operator=( const OfxhParamSet& other )
{
	_paramList = other._paramList.clone();
	initMapFromList();
}

void OfxhParamSet::copyParamsValues( const OfxhParamSet& other )
{
	if( _paramList.size() != other._paramList.size() )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev( "You try to copy parameters values, but the two lists are not identical." ) );
	}

	ParamList::const_iterator oit = other._paramList.begin(), oitEnd = other._paramList.end();
	for( ParamList::iterator it = _paramList.begin(), itEnd = _paramList.end();
	     it != itEnd && oit != oitEnd;
	     ++it, ++oit )
	{
		OfxhParam& p = *it;
		const OfxhParam& op = *oit;
		if( p.getName() != op.getName() )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::dev( "You try to copy parameters values, but it is not the same parameters in the two lists." ) );
		}
		p.copy(op);
	}
	initMapFromList();
}

void OfxhParamSet::referenceParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC
{
//	if( _allParams.find( name ) != _allParams.end() )
//	{
//		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists, "Trying to reference a new parameter which already exists." ) );
//	}
//	_allParams[name] = instance;
}

void OfxhParamSet::addParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC
{
	if( _params.find( name ) != _params.end() )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists, "Trying to add a new parameter which already exists." ) );
	}
	_paramList.push_back( instance );
	_params[name] = instance;
//	referenceParam( name, instance );
}


}
}
}
}

