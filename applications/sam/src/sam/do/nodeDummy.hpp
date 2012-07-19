#ifndef _SAM_DO_NODE_DUMMY_HPP_
#define	_SAM_DO_NODE_DUMMY_HPP_

#include <sam/common/node_io.hpp>

#include "global.hpp"

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/common/exceptions.hpp>

//#include <tuttle/host/attribute/expression.hpp>
//#include <tuttle/host/Graph.hpp>

//#include <boost/program_options.hpp>
//#include <boost/regex.hpp>
//#include <boost/algorithm/string/split.hpp>
//#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#define READER_DUMMY_SHORTNAME "r"
#define READER_DUMMY_NAME      "reader"
#define READER_DUMMY_FULLNAME  "dummy.reader"

#define WRITER_DUMMY_SHORTNAME "w"
#define WRITER_DUMMY_NAME      "writer"
#define WRITER_DUMMY_FULLNAME  "dummy.writer"

namespace sam {
namespace samdo {

void addDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_NAME );
	list.push_back( WRITER_DUMMY_NAME );
}

void addFullNameDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_FULLNAME );
	list.push_back( WRITER_DUMMY_FULLNAME );
}

void addShortNameDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_SHORTNAME );
	list.push_back( WRITER_DUMMY_SHORTNAME );
}

void foundAssociateSpecificDummyNode( std::string& inputNode, const std::string& dummyNodeName, const std::string& dummyNodeFullName, const std::string& dummyNodeShortName, const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& nodeList, const std::vector<std::string>& nodeArgs )
{
	if( !std::strcmp( inputNode.c_str(), dummyNodeFullName.c_str() ) || !std::strcmp( inputNode.c_str(), dummyNodeName.c_str() ) || !std::strcmp( inputNode.c_str(), dummyNodeShortName.c_str() ) )
	{
		if( nodeArgs.size() == 0 )
		{
			BOOST_THROW_EXCEPTION( tuttle::exception::Value()
								   << tuttle::exception::user() + "specify and input name (file or sequence)." );
		}
		boost::filesystem::path p( nodeArgs.at( 0 ) );
		std::string inputExtension = p.extension().string();
		boost::algorithm::to_lower( inputExtension );
		if( inputExtension.size() == 0 )
		{
			BOOST_THROW_EXCEPTION( tuttle::exception::Value()
								   << tuttle::exception::user() + "invalid extension of input file/sequence." );
		}
		inputExtension = inputExtension.substr( 1 ); // remove '.' at begining
		unsigned int numberOfSupportedExtension = std::numeric_limits<unsigned int>::max();
		BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
		{
			const std::string pluginName = node->getRawIdentifier();
			if( boost::algorithm::find_first( pluginName, dummyNodeName ) )
			{
				tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin = tuttle::host::Core::instance().getImageEffectPluginById( pluginName );
				plugin->loadAndDescribeActions();
				const tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plugin->getContexts();
				const tuttle::host::ofx::property::OfxhSet& properties = plugin->getDescriptorInContext( *contexts.begin() ).getProperties();
				if( properties.hasProperty( kTuttleOfxImageEffectPropSupportedExtensions ) )
				{
					const tuttle::host::ofx::property::OfxhProperty& prop = properties.fetchProperty( kTuttleOfxImageEffectPropSupportedExtensions );
					const std::vector<std::string> supportedExtensions = getStringValues( prop );
					BOOST_FOREACH( const std::string& ext, supportedExtensions )
					{
//						TUTTLE_TCOUT_VAR2( ext, inputExtension );
						if( ext == inputExtension )
						{
//							TUTTLE_TCOUT( pluginName << " [" << supportedExtensions.size() << "] can read " << ext );
							if( supportedExtensions.size() < numberOfSupportedExtension )
							{
								// Arbitrary solution...
								// If we found multiple plugins that support the requested format,
								// we select the plugin which supports the lowest number of formats.
								// So by defaut, we select specialized plugin before
								// plugin using generic libraries with all formats.
								numberOfSupportedExtension = supportedExtensions.size();
								inputNode = pluginName;
							}
						}
					}
				}
			}
		}
		if( numberOfSupportedExtension == std::numeric_limits<unsigned int>::max() )
		{
			BOOST_THROW_EXCEPTION( tuttle::exception::Value()
								   << tuttle::exception::user() + "Unsupported extension \"" + inputExtension + "\"." );
		}
		TUTTLE_COUT( _color._yellow << "Replace " << dummyNodeName << " with: " << inputNode << _color._std );
	}
}

void foundAssociateDummyNode( std::string& inputNode, const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& nodeList, const std::vector<std::string>& nodeArgs )
{
	foundAssociateSpecificDummyNode( inputNode, READER_DUMMY_NAME, READER_DUMMY_FULLNAME, READER_DUMMY_SHORTNAME, nodeList, nodeArgs );
	foundAssociateSpecificDummyNode( inputNode, WRITER_DUMMY_NAME, WRITER_DUMMY_FULLNAME, WRITER_DUMMY_SHORTNAME, nodeList, nodeArgs );
}

}
}
#endif
