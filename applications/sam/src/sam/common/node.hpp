#ifndef _SAM_NODE_HPP_
#define	_SAM_NODE_HPP_

#include <tuttle/host/Core.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

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

typedef std::pair<ttl::ofx::attribute::OfxhClipImage*, std::string> ClipAndConnection;

void setParametersForNode( const std::vector<std::string> parameters, ttl::Graph::Node& currentNode, std::vector<ClipAndConnection>& clipsToConnect, const bool isDummyNode = false, bool orderedParams = true )
{
	static const boost::regex re_param( "(?:([a-zA-Z_][a-zA-Z0-9_]*)=)?(.*)" );
	std::size_t paramIdx = 0;
	
	BOOST_FOREACH( const std::string& paramStr, parameters )
	{
		boost::cmatch matches;
		if( !boost::regex_match( paramStr.c_str(), matches, re_param ) )
		{
			BOOST_THROW_EXCEPTION(
				tuttle::exception::Value() <<
				tuttle::exception::user() + "Parameter can't be parsed \"" + paramStr + "\"." );
		}
		if( matches.size() != 3 )
		{
			// should never happen
			BOOST_THROW_EXCEPTION(
				tuttle::exception::Value() <<
				tuttle::exception::user() + "Parameter can't be parsed \"" + paramStr + "\". " + matches.size() + " matches." );
		}
		const std::string attributeName = matches[1];
		const std::string attributeValue = matches[2];
		if( attributeName.size() )
		{
			// if we start using non-ordered param (== named param)
			// we can't use ordered parameters anymore
			orderedParams = false;
		}
		else if( orderedParams == false )
		{
			BOOST_THROW_EXCEPTION(
				tuttle::exception::Value() <<
				tuttle::exception::user() + "Non-keyword parameter after keyword parameter. \"" + paramStr + "\"." );
		}
		//								TUTTLE_COUT( "* " << paramStr );
		//								TUTTLE_COUT( "3: " << paramName << " => " << paramValue );

		if( isDummyNode )
		{
			++paramIdx;
			continue;
		}
		
		// setup the node with parameter value in tuttle.
		if( attributeName.size() )
		{
			// set a value to a named parameter or clip
			using namespace ttl::ofx::attribute;
			OfxhParam* param = NULL;
			param = currentNode.getParamSet().getParamPtrByScriptName( attributeName );

			OfxhClipImage* clip = NULL;

			if( param == NULL )
			{
				// search in clips
				clip = currentNode.getClipImageSet().getClipPtr( attributeName );
			}

			if( param == NULL && clip == NULL )
			{
				std::vector<std::string> allAttr;
				std::vector<std::string> paramMatches;
				std::vector<std::string> clipMatches;
				//if( acceptPartialName ) // if sam-do accept partial names
				{

					BOOST_FOREACH( OfxhParamSet::ParamMap::value_type& p, currentNode.getParamSet().getParamsByScriptName() )
					{
						allAttr.push_back( p.first );
						if( boost::algorithm::starts_with( p.first, attributeName ) )
						{
							paramMatches.push_back( p.first );
							param = p.second;
						}
					}

					BOOST_FOREACH( OfxhClipImageSet::ClipImageMap::value_type& c, currentNode.getClipImageSet().getClipsByName() )
					{
						allAttr.push_back( c.first );
						if( boost::algorithm::starts_with( c.first, attributeName ) )
						{
							clipMatches.push_back( c.first );
							clip = c.second;
						}
					}
					if( paramMatches.size() + clipMatches.size() > 1 )
					{
						std::vector<std::string> matches;
						matches.insert( matches.begin(), paramMatches.begin(), paramMatches.end() );
						matches.insert( matches.end(), clipMatches.begin(), clipMatches.end() );
						BOOST_THROW_EXCEPTION(
							ttl::exception::Value() <<
							ttl::exception::user() + "Ambiguous partial attribute name \"" + attributeName + "\". Possible values are: " + boost::algorithm::join( matches, ", " ) + "." );
					}
				}

				if( paramMatches.size() + clipMatches.size() == 0 )
				{
					BOOST_THROW_EXCEPTION(
						ttl::exception::Value() <<
						ttl::exception::user() + "Attribute name \"" + attributeName + "\" not found. Possible values are: " + boost::algorithm::join( allAttr, ", " ) + "." );
				}
			}

			if( param != NULL )
			{
				param->setValueFromExpression( attributeValue );
			}
			else if( clip != NULL )
			{
				clipsToConnect.push_back( ClipAndConnection( clip, attributeValue ) );
			}
			else
			{
				BOOST_THROW_EXCEPTION(
					ttl::exception::Value() <<
					ttl::exception::user() + "Parameter or clip name " + tuttle::quotes( attributeName ) + " not found." );
			}
		}
		else
		{
			currentNode.getParam( paramIdx ).setValueFromExpression( attributeValue );
		}
		++paramIdx;
	}
}

void connectClips( const std::vector<ttl::Graph::Node*> nodes, const std::vector<ClipAndConnection> clipsToConnect, ttl::Graph::Node& currentNode, ttl::Graph& graph, const std::vector<std::string> idNames )
{
	// connect current node to previous node(s)
	if( nodes.size() > 0 ) // if not the first node
	{
		if( clipsToConnect.size() == 0 )
		{
			// No clip connection specified, so by default
			// we connect the new node to the last previous node

			/// @todo We only check if we have more than one clip (assuming that it's the default "Output" clip...)
			///       instead of checking the number of input clips...
			// if we have an input clip
			if( currentNode.getClipImageSet().getClipsByName().size() > 1 )
			{
				graph.connect( *nodes.back(), currentNode );
			}
		}
		else
		{
			// The user has specified some clips to connect

			BOOST_FOREACH( const ClipAndConnection& clip, clipsToConnect )
			{
				//TUTTLE_TCOUT_VAR3( clip.second, currentNode.getName(), clip.first->getName() );

				if( clip.second.size() <= 1 && ( clip.second == " " || clip.second == "!" || clip.second == "/" || clip.second == "-" ) )
				{
					// these keywords allows to keep this clip unconnected
					//TUTTLE_TCOUT( "Don't connect the clip " << clip.first->getName() );
					continue;
				}
				try
				{
					//TUTTLE_TCOUT( "Connect the clip " << clip.first->getName() );
					// test if it's an index
					const int relativeIndex = std::abs( boost::lexical_cast<int>( clip.second ) );
					const int absIndex = nodes.size() - relativeIndex;
					if( absIndex < 0 || absIndex >= (int) nodes.size() )
					{
						using namespace ttl;
						using tuttle::quotes;
						BOOST_THROW_EXCEPTION(
							exception::Value() <<
							exception::user() + "The relative index \"" + -relativeIndex + "\" for the connection of the clip " + quotes( clip.first->getName() ) + " on node " + quotes( currentNode.getName() ) + " is not valid." );
					}
					graph.connect( *nodes[absIndex], currentNode.getAttribute( clip.first->getName() ) );
				}
				catch( ... )
				{
					// It's not an index so we assume, it's the name/id of the clip.
					// If the node doesn't exist it will throw an exception.
					try
					{
						graph.connect( graph.getNode( clip.second ), currentNode.getAttribute( clip.first->getName() ) );
					}
					catch( ... )
					{
						using namespace ttl;
						using tuttle::quotes;
						
						BOOST_THROW_EXCEPTION(
							exception::Failed()
							<< exception::user() + "unable to connect clip " + quotes( clip.first->getName() ) + ", with the id " + quotes( clip.second ) + ". Possible id's are: " + boost::algorithm::join( idNames, ", " ) );
					}
				}
			}
		}
	}
}

}
#endif
