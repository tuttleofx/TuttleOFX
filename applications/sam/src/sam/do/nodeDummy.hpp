#ifndef _SAM_DO_NODE_DUMMY_HPP_
#define	_SAM_DO_NODE_DUMMY_HPP_

#include "node_io.hpp"
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

void addFullNameDummyNodeInList ( std::vector<std::string>& list )
{
    list.push_back( READER_DUMMY_FULLNAME );
    list.push_back( WRITER_DUMMY_FULLNAME );
}

void addShortNameDummyNodeInList ( std::vector<std::string>& list )
{
    list.push_back( READER_DUMMY_SHORTNAME );
    list.push_back( WRITER_DUMMY_SHORTNAME );
}

void foundAssociateSpecificDummyNode( std::string& inputNode, const std::string& dummyNodeName, const std::string& dummyNodeFullName, const std::string& dummyNodeShortName, const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& nodeList, const std::vector<std::string>& nodeArgs )
{
    if( ! std::strcmp( inputNode.c_str(), dummyNodeFullName.c_str() ) || ! std::strcmp( inputNode.c_str(), dummyNodeName.c_str() ) || ! std::strcmp( inputNode.c_str(), dummyNodeShortName.c_str() ) )
    {
	if( nodeArgs.size() == 0 )
	{
	    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
		<< tuttle::exception::user() + "specify and input name (file or sequence)." );
	}
	boost::filesystem::path p (nodeArgs.at(0));
	std::string extension = p.extension().string();
	if( extension.size() ==0 )
	{
	    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
		<< tuttle::exception::user() + "invalid extension of input file/sequence." );
	}
	unsigned int numberOfSupportedExtension = std::numeric_limits<unsigned int>::max();
	for( std::size_t i = 0; i < nodeList.size(); ++i )
	{
	    const std::string plugName = nodeList.at(i)->getRawIdentifier();
	    if( boost::algorithm::find_first( plugName, dummyNodeName ) )
	    {
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plug = tuttle::host::Core::instance().getImageEffectPluginById( plugName );
		plug->loadAndDescribeActions();
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plug->getContexts();
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet::iterator itContext;

		const tuttle::host::ofx::property::OfxhSet properties = plug->getDescriptorInContext( *contexts.begin() ).getProperties();
		tuttle::host::ofx::property::PropertyMap propMap = properties.getMap();
		for( tuttle::host::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
		{
		    const tuttle::host::ofx::property::OfxhProperty& prop = *( itProperty->second );
		    if( ! std::strcmp( itProperty->first.c_str(), "TuttleOfxImageEffectPropSupportedExtensions" ) )
		    {
			std::vector<std::string> ext = getDefaultOrChoiceValues(prop);
			for( unsigned int extIdx = 0; extIdx < ext.size(); extIdx++)
			{
			    std::string extname = ".";
			    extname += ext.at(extIdx);
			    if ( ! std::strcmp( extension.c_str(), extname.c_str() ) )
			    {
				//TUTTLE_COUT(plugName << "[" << ext.size() << "] can read " << ext.at(extIdx) );
				if( ext.size() < numberOfSupportedExtension )
				{
				    numberOfSupportedExtension = ext.size();
				    inputNode = plugName;
				}
			    }
			}
		    }
		}
	    }
	}
	if( numberOfSupportedExtension == std::numeric_limits<unsigned int>::max() )
	{
	    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
		<< tuttle::exception::user() + "Unsupported extension \"" + extension + "\"." );
	}
	TUTTLE_COUT( _color._yellow << "Replace "<< dummyNodeName << "with :" << inputNode  << _color._std );
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
