#ifndef _SAM_NODE_HPP_
#define	_SAM_NODE_HPP_

#include <tuttle/host/Core.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/foreach.hpp>

#include <map>

namespace sam {

inline std::string retrieveNodeFullname( const std::string& userId )
{
	std::string userIdLower = userId;
	boost::to_lower( userIdLower );

	std::vector<std::string> results;
	std::vector<std::string> ambiguousResults;

	typedef std::map<std::string, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*> PluginMap;
	const PluginMap& pluginsById = tuttle::host::core().getImageEffectPluginCache().getPluginsByID();

	if( pluginsById.find(userIdLower) != pluginsById.end() )
	{
		return userIdLower;
	}

	BOOST_FOREACH( const PluginMap::value_type& plugin, pluginsById )
	{
		const std::string& plugId = plugin.first;
		boost::iterator_range<std::string::const_iterator> it = boost::algorithm::find_last( plugId, userIdLower );
		if( it )
		{
			if( ( it.end() == plugId.end() ) &&
				( *(it.begin()-1) == '.' )
			  )
			{
				results.push_back( plugId );
			}
			else
			{
				ambiguousResults.push_back( plugId );
			}
		}
	}
	if( results.size() == 1 )
		return results.front();
	if( results.size() > 1 )
	{
		ambiguousResults.insert( ambiguousResults.begin(), results.begin(), results.end() );
	}
	if( ambiguousResults.size() == 1 )
		return ambiguousResults.front();

	if( ambiguousResults.size() > 1 )
	{
		tuttle::exception::user userMsg;
		userMsg + "Ambiguous node name \"" + userId + "\".\n";
		userMsg + "Possible nodes:\n";
		BOOST_FOREACH( const std::string& p, ambiguousResults )
		{
			userMsg + " - \"" + p + "\"\n";
		}
		BOOST_THROW_EXCEPTION( tuttle::exception::Value()
			<< userMsg );
	}

	BOOST_THROW_EXCEPTION( tuttle::exception::Value()
		<< tuttle::exception::user() + "Unrecognized node name \"" + userId + "\"." );
}

}
#endif
