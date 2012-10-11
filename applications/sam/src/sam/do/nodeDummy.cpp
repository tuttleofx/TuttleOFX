#include <sam/common/color.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/exceptions.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <cstring>
#include <algorithm>

#include "nodeDummy.hpp"

namespace sam {
Color color;

namespace samdo {

bool Dummy::isDummyReaderNode( const std::string& nodeName )
{
	return ( ( nodeName == READER_DUMMY_FULLNAME ) || ( nodeName == READER_DUMMY_NAME ) || ( nodeName == READER_DUMMY_SHORTNAME ) );
}

bool Dummy::isDummyWriterNode( const std::string& nodeName )
{
	return ( ( nodeName == WRITER_DUMMY_FULLNAME ) || ( nodeName == WRITER_DUMMY_NAME ) || ( nodeName == WRITER_DUMMY_SHORTNAME ) );
}

bool Dummy::isDummyNode( const std::string& nodeName )
{
	return ( isDummyReaderNode( nodeName ) || isDummyWriterNode( nodeName ) );
}

void Dummy::getFullName( std::string& inputNode )
{
	if( isDummyReaderNode( inputNode ) )
	{
		inputNode = READER_DUMMY_NAME;
		return;
	}
	if( isDummyWriterNode( inputNode ) )
	{
		inputNode = WRITER_DUMMY_NAME;
		return;
	}
}

bpo::options_description Dummy::getInfoOptions()
{
	bpo::options_description infoOptions;
	infoOptions.add_options()
		( kHelpOptionString, kHelpOptionMessage )
		( kVersionOptionString, kVersionOptionMessage )
		( kExpertOptionString, kExpertOptionMessage )
		( kPluginsOptionString, kPluginsOptionMessage )
		( kFormatOptionString, kFormatOptionMessage );
	return infoOptions;
}

bpo::options_description Dummy::getConfOptions()
{
	bpo::options_description confOptions;
	confOptions.add_options()
		( kVerboseOptionString, kVerboseOptionMessage )
		( kIdOptionString, bpo::value<std::string > (), kIdOptionMessage )
		( kNbCoresOptionString, bpo::value<std::size_t > (), kNbCoresOptionMessage );
	return confOptions;
}

bpo::options_description Dummy::getOpenFXOptions()
{
	bpo::options_description openFXOptions;
	openFXOptions.add_options()
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
	return openFXOptions;
}

void Dummy::getCommandLineParameters( bpo::variables_map& node_vm, const std::vector<std::string>& nodeArgs )
{
	// Declare the supported options.
	bpo::options_description infoOptions = getInfoOptions();
	bpo::options_description confOptions = getConfOptions();
	// describe openFX options
	bpo::options_description openfxOptions = getOpenFXOptions();

	bpo::positional_options_description param_values;
	param_values.add( kParamValuesOptionLongName, -1 );
	
	bpo::options_description all_options;
	all_options.add( infoOptions ).add( confOptions ).add( openfxOptions );
	
	bpo::store( bpo::command_line_parser( nodeArgs ).options( all_options ).positional( param_values ).run(), node_vm );
}

void Dummy::getParametersFromCommandLine( std::vector<std::string>& parameters, const std::vector<std::string>& nodeArgs )
{
	bpo::variables_map node_vm;
	getCommandLineParameters( node_vm, nodeArgs );
	
	if( node_vm.count( kParamValuesOptionLongName ) )
	{
		parameters = node_vm[kParamValuesOptionLongName].as< std::vector< std::string> > ();
	}
}

void Dummy::getPathsFromCommandLine( std::vector<std::string>& paths, const std::vector<std::string>& nodeArgs )
{
	getParametersFromCommandLine( paths, nodeArgs );
	
	std::vector<std::string>::iterator it = paths.begin();
	for( ; it < paths.end(); it++ )
	{
		if( std::strncmp( (*it).c_str() , "ext=" , 4 ) == 0 )
		{
			paths.erase( it );
			it--;
		}
	}
	if( paths.size() == 0 )
	{
		paths.push_back( "./" );
	}
}

void Dummy::getExtensionsFromCommandLine( std::vector<std::string>& extensions, const std::vector<std::string>& nodeArgs )
{
	getParametersFromCommandLine( extensions, nodeArgs );
	
	std::vector<std::string>::iterator it = extensions.begin();
	for( ; it < extensions.end(); it++ )
	{
		if( std::strncmp( (*it).c_str() , "ext=" , 4 ) != 0 )
		{
			extensions.erase( it );
			it--;
		}
		else
		{
			*it = (*it).erase( 0, 4 ); // erase ext=
		}
	}
}

void Dummy::addDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_NAME );
	list.push_back( WRITER_DUMMY_NAME );
}

void Dummy::addFullNameDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_FULLNAME );
	list.push_back( WRITER_DUMMY_FULLNAME );
}

void Dummy::addShortNameDummyNodeInList( std::vector<std::string>& list )
{
	list.push_back( READER_DUMMY_SHORTNAME );
	list.push_back( WRITER_DUMMY_SHORTNAME );
}

std::vector<std::string> Dummy::getAllSupportedNodes( const std::string& context )
{
	const NodeList& nodeList = ttl::core().getImageEffectPluginCache().getPlugins();
	std::vector<std::string> listOfPlugins;
	
	BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
	{
		try
		{
			const std::string pluginName = node->getRawIdentifier();
			node->loadAndDescribeActions();
			if( node->supportsContext( context ) )
			{
				listOfPlugins.push_back( pluginName );
			}
		}
		catch(...)
		{
			/// @todo Create a list of loading errors?
		}
	}
	
	// sort results
	std::sort( listOfPlugins.begin(), listOfPlugins.end() );
	return listOfPlugins;
}

std::vector<std::string> Dummy::getSupportedExtensions( const std::string& context )
{
	const NodeList& nodeList = ttl::core().getImageEffectPluginCache().getPlugins();
	std::vector<std::string> listOfExtensions;
	
	BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
	{
		try
		{
			node->loadAndDescribeActions();
			if( node->supportsContext( context ) )
			{
				//TUTTLE_COUT( pluginName );
				if( node->getDescriptorInContext( context ).getProperties().hasProperty( kTuttleOfxImageEffectPropSupportedExtensions ) )
				{
					const tuttle::host::ofx::property::OfxhProperty& prop = node->getDescriptorInContext( context ).getProperties().fetchProperty( kTuttleOfxImageEffectPropSupportedExtensions );
	
					for( std::size_t n = 0; n < prop.getDimension(); ++n )
					{
						listOfExtensions.push_back( prop.getStringValue( n ) );
					}
				}
			}
		}
		catch(...)
		{}
	}
	
	// sort results
	std::sort( listOfExtensions.begin(), listOfExtensions.end() );
	
	// delete similar extension
	listOfExtensions.erase( std::unique( listOfExtensions.begin(), listOfExtensions.end() ), listOfExtensions.end() );
	
	return listOfExtensions;
}

void Dummy::printAllSupportedNodes( const std::string& context )
{
	BOOST_FOREACH( const std::string& pluginName, getAllSupportedNodes( context ) )
	{
		TUTTLE_COUT( pluginName );
	}
}

void Dummy::printAllSupportedExtensions( const std::string& context )
{
	TUTTLE_COUT( boost::algorithm::join( getSupportedExtensions( context ), "," ) );
}

void Dummy::displayHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions )
{
	using namespace sam;

	TUTTLE_COUT( color._blue << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "NODE" << color._std );
	TUTTLE_COUT( color._green << "\tsam do " << nodeFullName << " - OpenFX node." << color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "DESCRIPTION" << color._std );
	TUTTLE_COUT( color._green << "\tnode type: dummy (specific to TuttleOFX)" << color._std );
	// internal node help
	TUTTLE_COUT( "" );
	TUTTLE_COUT( "Dummy reader call specific reader detected by the extension of the sequence name." );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "ASSOCIATED PLUGINS" << color._std );
	if( strcmp( nodeFullName.c_str(), READER_DUMMY_NAME ) == 0 )
		printAllSupportedNodes( kOfxImageEffectContextReader );
	if( strcmp( nodeFullName.c_str(), WRITER_DUMMY_NAME ) == 0 )
		printAllSupportedNodes( kOfxImageEffectContextWriter );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "SUPPORTED FORMATS" << color._std );
	if( strcmp( nodeFullName.c_str(), READER_DUMMY_NAME ) == 0 )
		printAllSupportedExtensions( kOfxImageEffectContextReader );
	if( strcmp( nodeFullName.c_str(), WRITER_DUMMY_NAME ) == 0 )
		printAllSupportedExtensions( kOfxImageEffectContextWriter );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "PARAMETERS" << color._std );
	TUTTLE_COUT( color._green << "\t" << std::left << std::setw( 25 ) << "ext" << ": " << color._std << " String\n\t  select extension from supported format list.");
	TUTTLE_COUT( "" );
	//TUTTLE_COUT( color._blue << "CLIPS" << color._std );
	//coutClipsWithDetails( currentNode );

	TUTTLE_COUT( "" );
	TUTTLE_COUT( color._blue << "DISPLAY OPTIONS (override the process)" << color._std );
	TUTTLE_COUT( infoOptions );
	TUTTLE_COUT( color._blue << "CONFIGURE PROCESS" << color._std );
	TUTTLE_COUT( confOptions );

	TUTTLE_COUT( "" );
}

void Dummy::displayHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions, const bpo::options_description &expertOptions )
{
	using namespace sam;
	displayHelp( nodeFullName, infoOptions, confOptions );

	TUTTLE_COUT( color._blue << "EXPERT OPTIONS" << color._std );
	TUTTLE_COUT( expertOptions );
}

void Dummy::foundAssociateSpecificDummyNode( std::string& inputNode, const std::string& dummyNodeName, const NodeList& nodeList, const std::vector<std::string>& nodeArgs )
{
	if( std::strcmp( inputNode.c_str(), dummyNodeName.c_str() ) )
		return;
	
	bpo::options_description infoOptions = getInfoOptions();
	bpo::options_description confOptions = getConfOptions();
	bpo::options_description openfxOptions = getOpenFXOptions();
	
	bpo::variables_map node_vm;
	getCommandLineParameters( node_vm, nodeArgs );
	
	if( node_vm.count( kHelpOptionLongName ) )
	{
		displayHelp( dummyNodeName, infoOptions, confOptions );
		exit( 0 );
	}
	if( node_vm.count( kExpertOptionLongName ) )
	{
		displayHelp( dummyNodeName, infoOptions, confOptions, openfxOptions );
		exit( 0 );
	}
	if( node_vm.count( kPluginsOptionLongName ) )
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
		if( strcmp( dummyNodeName.c_str(), WRITER_DUMMY_NAME ) == 0 )
		{
			inputNode = "tuttle.dummy";
			return;
		}
		
		BOOST_THROW_EXCEPTION( tuttle::exception::Value()
							   << tuttle::exception::user() + "specify and input name (file or sequence)." );
	}
	
	std::vector<std::string> paths;
	bool needRecursiveProcess = false;
	if( kParamValuesOptionLongName )
	{
		paths = node_vm[kParamValuesOptionLongName].as< std::vector< std::string> > ();
		BOOST_FOREACH( const std::string& basePath, paths )
		{
			boost::filesystem::path p( basePath );
			std::string inputExtension = p.extension().string();
			boost::algorithm::to_lower( inputExtension );
			if( inputExtension.size() == 0 )
			{
				needRecursiveProcess = true;
			}
		}
	}
	else
	{
		paths.push_back( "./" );
	}
	
	if( needRecursiveProcess)
	{
		inputNode = "tuttle.dummy";
		return;
	}
	boost::filesystem::path p( paths.at(0) );
	std::string inputExtension = p.extension().string();
	inputExtension = inputExtension.substr( 1 ); // remove '.' at begining
	unsigned int numberOfSupportedExtension = std::numeric_limits<unsigned int>::max();
	BOOST_FOREACH( ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, nodeList )
	{
		const std::string pluginName = node->getRawIdentifier();
		if( boost::algorithm::find_first( pluginName, dummyNodeName ) )
		{
			tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* plugin = tuttle::host::core().getImageEffectPluginById( pluginName );
			plugin->loadAndDescribeActions();
			const tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plugin->getContexts();
			const tuttle::host::ofx::property::OfxhSet& properties = plugin->getDescriptorInContext( *contexts.begin() ).getProperties();
			if( properties.hasProperty( kTuttleOfxImageEffectPropSupportedExtensions ) )
			{
				const tuttle::host::ofx::property::OfxhProperty& prop = properties.fetchProperty( kTuttleOfxImageEffectPropSupportedExtensions );

				for( std::size_t n = 0; n < prop.getDimension(); ++n )
				{
					//TUTTLE_TCOUT_VAR2( prop.getStringValue( n ), inputExtension );
					if( prop.getStringValue( n ) == inputExtension )
					{
						//TUTTLE_TCOUT( pluginName << " [" << prop.getDimension() << "] can read " << prop.getStringValue( n ) );
						if( prop.getDimension() < numberOfSupportedExtension )
						{
							// Arbitrary solution...
							// If we found multiple plugins that support the requested format,
							// we select the plugin which supports the lowest number of formats.
							// So by defaut, we select specialized plugin before
							// plugin using generic libraries with all formats.
							numberOfSupportedExtension = prop.getDimension();
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
	TUTTLE_COUT( color._yellow << "Replace " << dummyNodeName << " with: " << inputNode << color._std );
}

void Dummy::foundAssociateDummyNode( std::string& inputNode, const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& nodeList, const std::vector<std::string>& nodeArgs )
{
	getFullName( inputNode );
	foundAssociateSpecificDummyNode( inputNode, READER_DUMMY_NAME, nodeList, nodeArgs );
	foundAssociateSpecificDummyNode( inputNode, WRITER_DUMMY_NAME, nodeList, nodeArgs );
}

}
}
