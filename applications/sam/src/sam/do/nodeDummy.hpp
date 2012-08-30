#ifndef _SAM_DO_NODE_DUMMY_HPP_
#define	_SAM_DO_NODE_DUMMY_HPP_

#include <sam/common/node_io.hpp>
#include <sam/common/options.hpp>

#include "global.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>


#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <cstring>
#include <algorithm>

#define READER_DUMMY_SHORTNAME "r"
#define READER_DUMMY_NAME      "reader"
#define READER_DUMMY_FULLNAME  "dummy.reader"

#define WRITER_DUMMY_SHORTNAME "w"
#define WRITER_DUMMY_NAME      "writer"
#define WRITER_DUMMY_FULLNAME  "dummy.writer"

namespace bpo = boost::program_options;

namespace sam {
namespace samdo {

typedef std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*> NodeList;
typedef ttl::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet   NodeContext;

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

void printAllSupportedNodes( const std::string& context )
{
	const NodeList& nodeList = ttl::Core::instance().getImageEffectPluginCache().getPlugins();
	std::vector<std::string> listOfPlugins;
	
	BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
	{
		const std::string pluginName = node->getRawIdentifier();
		node->loadAndDescribeActions();
		if( node->supportsContext( context ) )
		{
			listOfPlugins.push_back( pluginName );
		}
	}
	
	// sort results
	std::sort( listOfPlugins.begin(), listOfPlugins.end() );
	
	BOOST_FOREACH( std::string plugin, listOfPlugins )
	{
		TUTTLE_COUT( plugin );
	}
}

void printAllSupportedExtensions( const std::string& context )
{
	const NodeList& nodeList = ttl::Core::instance().getImageEffectPluginCache().getPlugins();
	std::vector<std::string> listOfExtensions;
	
	BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
	{
		const std::string pluginName = node->getRawIdentifier();
		node->loadAndDescribeActions();
		
		if( node->supportsContext( context ) )
		{
			//TUTTLE_COUT( pluginName );
			if( node->getDescriptorInContext( context ).getProperties().hasProperty( kTuttleOfxImageEffectPropSupportedExtensions ) )
			{
				const std::vector<std::string> extensions = getStringValues( node->getDescriptorInContext( context ).getProperties().fetchStringProperty( kTuttleOfxImageEffectPropSupportedExtensions ) );
				BOOST_FOREACH( std::string ext, extensions )
				{
					listOfExtensions.push_back( ext );
				}
			}
		}
	}
	
	// sort results
	std::sort( listOfExtensions.begin(), listOfExtensions.end() );
	
	// delete similar extension
	listOfExtensions.erase( std::unique( listOfExtensions.begin(), listOfExtensions.end() ), listOfExtensions.end() );
	
	TUTTLE_COUT( boost::algorithm::join( listOfExtensions, "," ) );
}

void displayDummyNodeHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions )
{
	using namespace sam;

	TUTTLE_COUT( _color._blue << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << _color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "NODE" << _color._std );
	TUTTLE_COUT( _color._green << "\tsam do " << nodeFullName << " - OpenFX node." << _color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "DESCRIPTION" << _color._std );
	TUTTLE_COUT( _color._green << "\tnode type: dummy (specific to TuttleOFX)" << _color._std );
	// internal node help
	TUTTLE_COUT( "" );
	TUTTLE_COUT( "Dummy reader call specific reader detected by the extension of the sequence name." );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "ASSOCIATED PLUGINS" << _color._std );
	if( strcmp( nodeFullName.c_str(), READER_DUMMY_NAME ) == 0 )
		printAllSupportedNodes( kOfxImageEffectContextReader );
	if( strcmp( nodeFullName.c_str(), WRITER_DUMMY_NAME ) == 0 )
		printAllSupportedNodes( kOfxImageEffectContextWriter );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "SUPPORTED FORMATS" << _color._std );
	if( strcmp( nodeFullName.c_str(), READER_DUMMY_NAME ) == 0 )
		printAllSupportedExtensions( kOfxImageEffectContextReader );
	if( strcmp( nodeFullName.c_str(), WRITER_DUMMY_NAME ) == 0 )
		printAllSupportedExtensions( kOfxImageEffectContextWriter );
	//TUTTLE_COUT( "" );
	//TUTTLE_COUT( _color._blue << "PARAMETERS" << _color._std );
	//coutParametersWithDetails( currentNode );
	//TUTTLE_COUT( "" );
	//TUTTLE_COUT( _color._blue << "CLIPS" << _color._std );
	//coutClipsWithDetails( currentNode );

	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "DISPLAY OPTIONS (override the process)" << _color._std );
	TUTTLE_COUT( infoOptions );
	TUTTLE_COUT( _color._blue << "CONFIGURE PROCESS" << _color._std );
	TUTTLE_COUT( confOptions );

	TUTTLE_COUT( "" );
}

void displayDummyNodeHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions, const bpo::options_description &expertOptions )
{
	using namespace sam;
	displayDummyNodeHelp( nodeFullName, infoOptions, confOptions );

	TUTTLE_COUT( _color._blue << "EXPERT OPTIONS" << _color._std );
	TUTTLE_COUT( expertOptions );
}

void foundAssociateSpecificDummyNode( std::string& inputNode, const std::string& dummyNodeName, const std::string& dummyNodeFullName, const std::string& dummyNodeShortName, const NodeList& nodeList, const std::vector<std::string>& nodeArgs )
{
	if( !std::strcmp( inputNode.c_str(), dummyNodeFullName.c_str() ) || !std::strcmp( inputNode.c_str(), dummyNodeName.c_str() ) || !std::strcmp( inputNode.c_str(), dummyNodeShortName.c_str() ) )
	{
		
		// Declare the supported options.
		bpo::options_description infoOptions;
		infoOptions.add_options()
			( kHelpOptionString, kHelpOptionMessage )
			( kVersionOptionString, kVersionOptionMessage )
			( kExpertOptionString, kExpertOptionMessage )
			( kPluginsOptionString, kPluginsOptionMessage )
			( kFormatOptionString, kFormatOptionMessage );
		bpo::options_description confOptions;
		confOptions.add_options()
			( kVerboseOptionString, kVerboseOptionMessage )
			( kIdOptionString, bpo::value<std::string > (), kIdOptionMessage )
			( kNbCoresOptionString, bpo::value<std::size_t > (), kNbCoresOptionMessage );
		// describe openFX options
		bpo::options_description openfxOptions;
		openfxOptions.add_options()
			( kAttributesOptionString, kAttributesOptionMessage )
			( kPropertiesOptionString, kPropertiesOptionMessage )
			( kClipsOptionString, kClipsOptionMessage )
			( kClipOptionString, bpo::value<std::string > (), kClipOptionMessage )
			( kParametersOptionString, kParametersOptionMessage )
			( kParamInfosOptionString, bpo::value<std::string > (), kParamInfosOptionMessage )
			( kParamValuesOptionString, bpo::value<std::vector<std::string> >(), kParamValuesOptionMessage )
			// for auto completion
			( kParametersReduxOptionString, kParametersReduxOptionMessage )
			( kParamTypeOptionString, bpo::value<std::string > (), kParamTypeOptionMessage )
			( kParamPossibleValuesOptionString, bpo::value<std::string > (), kParamPossibleValuesOptionMessage )
			( kParamDefaultOptionString, bpo::value<std::string > (), kParamDefaultOptionMessage )
			( kParamGroupOptionString, kParamGroupOptionMessage );
		
		bpo::positional_options_description param_values;
		param_values.add( kParamValuesOptionLongName, -1 );
		
		bpo::options_description all_options;
		all_options.add( infoOptions ).add( confOptions ).add( openfxOptions );
		
		bpo::variables_map node_vm;
		bpo::store( bpo::command_line_parser( nodeArgs ).options( all_options ).positional( param_values ).run(), node_vm );
		
		if( node_vm.count( kHelpOptionLongName ) )
		{
			displayDummyNodeHelp( dummyNodeName, infoOptions, confOptions );
			exit( 0 );
		}
		if( node_vm.count( kExpertOptionLongName ) )
		{
			displayDummyNodeHelp( dummyNodeName, infoOptions, confOptions, openfxOptions );
			exit( 0 );
		}
		if( kPluginsOptionLongName )
		{
			if( strcmp( dummyNodeName.c_str(), READER_DUMMY_NAME ) == 0 )
				printAllSupportedNodes( kOfxImageEffectContextReader );
			if( strcmp( dummyNodeName.c_str(), WRITER_DUMMY_NAME ) == 0 )
				printAllSupportedNodes( kOfxImageEffectContextWriter );
			exit( 0 );
		}
		
		if( node_vm.count( kFormatOptionLongName ) )
		{
			if( strcmp( dummyNodeName.c_str(), READER_DUMMY_NAME ) == 0 )
				printAllSupportedExtensions( kOfxImageEffectContextReader );
			if( strcmp( dummyNodeName.c_str(), WRITER_DUMMY_NAME ) == 0 )
				printAllSupportedExtensions( kOfxImageEffectContextWriter );
			exit( 0 );
		}
		
		if( node_vm.count( kVersionOptionLongName ) )
		{
			TUTTLE_COUT( "\tsam do " << dummyNodeName );
			TUTTLE_COUT( "Version 1.0" );
			TUTTLE_COUT( "" );
			exit( 0 );
		}
		
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
