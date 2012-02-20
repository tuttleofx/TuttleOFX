#ifndef _SAM_NODE_IO_HPP_
#define	_SAM_NODE_IO_HPP_

#include <tuttle/host/Graph.hpp>

#include <vector>
#include <string>

namespace sam {

namespace ttl = tuttle::host;
Color _color;

/// get Default or choice values
std::vector<std::string> getDefaultOrChoiceValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    std::vector<std::string> s;
    if( !(prop.getType() == 3) ) // if Pointer, we don't have _value
    {
	int n = 0;
	for( ; n < (int)( prop.getDimension() ) - 1; ++n )
	{
	    s.push_back( prop.getStringValue( n ) );
	}
	if( prop.getDimension() > 0 )
	{
	    s .push_back( prop.getStringValue( n ) );
	}
    }
    return s;
}

/**
 * @todo
 */
void coutProperties( const ttl::Graph::Node& node )
{
    const ttl::ofx::property::OfxhSet& props = node.getProperties();
    BOOST_FOREACH( ttl::ofx::property::PropertyMap::const_reference clip, props.getMap() )
    {
	TUTTLE_COUT(
	    "\t" <<
	    _color._green <<
	    clip.first << " " <<
	    _color._red <<
	    (clip.second->getDimension() > 1 ? (std::string(" x") + boost::lexical_cast<std::string>(clip.second->getDimension())) : "") <<
	    _color._std
	    );
    }
}

/**
 * @todo
 */
void coutClips( const ttl::Graph::Node& node )
{
//	ttl::ofx::attribute::OfxhClipSet& clips = node.getClipSet();
//	BOOST_FOREACH( ttl::ofx::attribute::OfxhClip& clip, clips.getClipVector() )
//	{
//		/// @todo
//		TUTTLE_COUT("");
//	}
}

void coutParametersWithDetails( const ttl::Graph::Node& node )
{
    const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();
    BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
    {
	if( param.getSecret() )
	    continue; // ignore secret parameters
	TUTTLE_COUT(
	    "\t" <<
	    _color._green <<
	    param.getScriptName() << ":\t" <<
	    _color._red <<
	    param.getParamType() <<
	    (param.getSize() > 1 ? (std::string(" x") + boost::lexical_cast<std::string>(param.getSize())) : "") <<
	    _color._std
	    );
	const std::string& hint = param.getHint();
	if( hint.size() )
	{
	    TUTTLE_COUT( hint );
	}
	TUTTLE_COUT("");
    }
}

void coutParameters( const ttl::Graph::Node& node )
{
    const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();
    BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
    {
	if( param.getSecret() )
	    continue; // ignore secret parameters
	TUTTLE_COUT( param.getScriptName() );
    }
}

}

#endif
