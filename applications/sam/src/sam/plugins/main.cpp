#include <sam/common/options.hpp>
#include <sam/common/utility.hpp>

#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>

namespace           bpo         = boost::program_options;
namespace           bal         = boost::algorithm;
namespace           tth         = tuttle::host;

bool                properties  = false;
bool                clips       = false;
bool                parameters  = false;

/// get defaults values of plugin properties
std::string getDefaultValues(const tth::ofx::property::OfxhProperty& prop)
{
	std::string s;
	if( !(prop.getType() == 3) ) // if Pointer, we don't have _value and _defaultValue properties
	{
		int n = 0;
		for( ; n < (int)( prop.getDimension() ) - 1; ++n )
		{
			s += prop.getStringValue( n );
			s += ", ";
		}
		if( prop.getDimension() > 0 )
		{
			s += prop.getStringValue( n );
		}
	}
	return s;
}

void printProperties( const tth::ofx::property::OfxhSet properties, std::string context="" )
{
	using namespace tuttle::common;
	boost::shared_ptr<Color> color( Color::get() );
	if( context.size() == 0 )
	{
		TUTTLE_LOG_INFO( color->_red << "Number of properties : " << properties.getSize() << color->_std );
	}
	else
	{
		TUTTLE_LOG_INFO( color->_red << "Number of properties for \"" << context << "\": " << properties.getSize() << color->_std );
	}

	std::string propertyId = "Property Name";
	propertyId.resize( 50, ' ' );
	TUTTLE_LOG_INFO( 
				color->_red << "RW" << color->_std << " " <<
				color->_red << "ModifiedBy" << color->_std << "\t" <<
				color->_red << "Type" << color->_std << "\t" <<
				color->_red << "Size" << color->_std << "\t" <<
				color->_red << propertyId << color->_std << "\t" <<
				color->_red << "Default Values" << color->_std );
	
	tth::ofx::property::PropertyMap propMap = properties.getMap();
	for( tth::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tth::ofx::property::OfxhProperty& prop = *( itProperty->second );
		std::string propertyLabel = itProperty->first;
		propertyLabel.resize( 50, ' ' );
		
		TUTTLE_LOG_INFO(
			color->_green <<
			( prop.getPluginReadOnly() ? (color->_green + "r-" + color->_std + " ") : (color->_green + "rw" + color->_std + " " ) ) <<
			( prop.getModifiedBy() == tth::ofx::property::eModifiedByHost ? ( color->_green + "host" + color->_std + "  \t" ) : ( color->_green + "plugin" + color->_std + "\t" ) ) <<
			color->_green << ( tth::ofx::property::mapTypeEnumToString( prop.getType() ) ) << color->_std << "\t" << color->_green << "[" <<
			prop.getDimension() << "]" << color->_std << "\t" <<
			color->_blue << propertyLabel << color->_std << "\t" << color->_green << "{ " <<
			getDefaultValues(prop) << " }" << color->_std
		);
	}
}


void getPluginProperties( const std::string& plugName )
{
	using namespace tuttle::common;
	boost::shared_ptr<Color> color( Color::get() );
	tth::ofx::imageEffect::OfxhImageEffectPlugin* plug = tth::core().getImageEffectPluginById( plugName );
	
	if( !plug )
	{
		TUTTLE_LOG_ERROR( color->_red << "no plugin match to: " << plugName << color->_std );
		return;
	}

	plug->loadAndDescribeActions();
	TUTTLE_LOG_INFO("Identifier:\t\t"		<< plug->getIdentifier() );
	TUTTLE_LOG_INFO("Raw identifier:\t\t"	<< plug->getRawIdentifier() );
	TUTTLE_LOG_INFO("Minor version:\t\t"	<< plug->getVersionMinor() );
	TUTTLE_LOG_INFO("Major version:\t\t" 	<< plug->getVersionMajor() );
	TUTTLE_LOG_INFO("API version:\t\t"		<< plug->getApiVersion() );

	// list contexts of plugin
	TUTTLE_LOG_INFO( color->_green << "Contexts:" << color->_std );

	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plug->getContexts();
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet::iterator itContext;
	std::string strContexts;
	for( itContext = contexts.begin(); itContext != contexts.end(); ++itContext )
	{
		strContexts += *itContext + ", ";
	}
	strContexts.erase(strContexts.size()-2, 2);
	TUTTLE_LOG_INFO( "[ " << strContexts << " ]" );
	itContext = contexts.begin();

	// list properties of plugin for the first context
	if( properties )
	{
		TUTTLE_LOG_INFO( std::endl << color->_red << "Properties" << color->_std );

		const tth::ofx::property::OfxhSet properties = plug->getDescriptorInContext( *itContext ).getProperties();
		printProperties( properties );
	}
	tth::ofx::imageEffect::OfxhImageEffectNode* plugInst = NULL;
	if( clips | parameters )
	{
		if( plug->supportsContext( kOfxImageEffectContextReader ) )
		{
			plugInst = plug->createInstance( kOfxImageEffectContextReader );
		}
		else if( plug->supportsContext( kOfxImageEffectContextWriter ) )
		{
			plugInst = plug->createInstance( kOfxImageEffectContextWriter );
		}
		else if( plug->supportsContext( kOfxImageEffectContextGenerator ) )
		{
			plugInst = plug->createInstance( kOfxImageEffectContextGenerator );
		}
		else if( plug->supportsContext( kOfxImageEffectContextFilter ) )
		{
			plugInst = plug->createInstance( kOfxImageEffectContextFilter );
		}
		else if( plug->supportsContext( kOfxImageEffectContextGeneral ) )
		{
			plugInst = plug->createInstance( kOfxImageEffectContextGeneral );
		}
		else
		{
			TUTTLE_LOG_ERROR( "Plugin contexts not supported by the host. (" + plugName + ")" );
			return;
		}
	}
	if( clips )
	{
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( std::endl << color->_red << "Clips" << color->_std );

		typedef std::map<std::string, tth::ofx::attribute::OfxhClipImageDescriptor*> ContextMap;

		// get contexts
		ContextMap::const_iterator it = plugInst->getDescriptor().getClips().begin();
		std::string strClipContexts;
		for( ; it != plugInst->getDescriptor().getClips().end(); ++it )
		{
			strClipContexts += (*it).first + ", " ;
		}
		strClipContexts.erase( strClipContexts.size()-2, 2 );

		TUTTLE_LOG_INFO( color->_green << "[ " << strClipContexts << " ]" << color->_std );


		// get propeties in each context
		ContextMap::const_iterator it2 = plugInst->getDescriptor().getClips().begin();
		for( ; it2 != plugInst->getDescriptor().getClips().end(); ++it2 )
		{
			printProperties( (*it2).second->getProperties(), (*it2).first );
		}
	}
	
	if( parameters )
	{
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_red << "Parameters" << color->_std );

		typedef std::map<std::string, tth::ofx::attribute::OfxhParamDescriptor*> ParamDescriptorMap;

		// get contexts
		ParamDescriptorMap::const_iterator it = plugInst->getDescriptor().getParams().begin();
		std::string strParamsContexts;
		for( ; it != plugInst->getDescriptor().getParams().end(); ++it )
		{
			strParamsContexts += (*it).first + ", ";
		}
		strParamsContexts.erase( strParamsContexts.size()-2, 2 );

		TUTTLE_LOG_INFO( color->_green << "[ " << strParamsContexts << " ]" << color->_std );

		// get propeties in each context
		ParamDescriptorMap::const_iterator it2 = plugInst->getDescriptor().getParams().begin();
		for( ; it2 != plugInst->getDescriptor().getParams().end(); it2++ )
		{
			printProperties( (*it2).second->getProperties(), (*it2).first );
		}
		/*
		const tth::ofx::property::OfxhSet paramProperties = plugInst->getDescriptor().getParamList().front().getProperties();
		printProperties( paramProperties, "" );
		*/
	}
}

bool isNotFiltered( std::string plugName, std::vector<std::string>& filters)
{
	if( filters.size() == 0 )
		return true;
	
	for( std::size_t i = 0; i < filters.size(); ++i )
	{
		std::string filter(filters.at(i));
		filter = boost::regex_replace( filter, boost::regex( "\\*" ), "(.*)"  );
		filter = boost::regex_replace( filter, boost::regex( "\\?" ), "(.)"  );

		if( regex_match( plugName, boost::regex(filter) ) )
			return true;
	}
	return false;
}

int main( int argc, char** argv )
{
	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );
	
	std::vector<std::string> plugins;
	std::vector<std::string> foundPlugins;
	std::vector<std::string> filters;
	
	formatter->init_logging();
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		( kHelpOptionString       , kHelpOptionMessage )
		( kAllOptionString        , kAllOptionMessage )
		( kFilterOptionString     , bpo::value<std::string>(), kFilterOptionMessage )
		( kColorOptionString      , kColorOptionMessage )
		( kPropertiesOptionString , kPropertiesOptionMessage )
		( kClipsOptionString      , kClipsOptionMessage )
		( kParametersOptionString , kParametersOptionMessage )
		( kBriefOptionString      , kBriefOptionMessage )
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		( kInputDirOptionString, bpo::value< std::vector<std::string> >(), kInputDirOptionMessage )
		( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add(kInputDirOptionLongName, -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	try
	{
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

		// get environnement options and parse them
		if( const char* env_plugins_options = std::getenv("SAM_PLUGINS_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_plugins_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		if( const char* env_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_LOG_ERROR( "sam-plugins: command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR( "sam-plugins: unknown error in command line.");
		exit( -2 );
	}

	if( vm.count( kColorOptionLongName ) )
	{
		color->enable();
	}
	
	if( vm.count( kEnableColorOptionLongName ) )
	{
		const std::string str = vm[kEnableColorOptionLongName].as<std::string>();
		if( string_to_boolean(str) )
		{
			color->enable();
		}
		else
		{
			color->disable();
		}
	}

	if( vm.count(kHelpOptionLongName) )
	{
		TUTTLE_LOG_INFO( color->_blue  << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-plugins - show informations about OpenFX plugins" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-plugins [options] [specific_OpenFX_plugin]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "DESCRIPTION" << color->_std );
		TUTTLE_LOG_INFO( "List OpenFX in OFX_PLUGIN_PATH by default.");
		TUTTLE_LOG_INFO( "And could print properties, parameters and clips for each plugins" );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "OPTIONS" << color->_std );
		TUTTLE_LOG_INFO( mainOptions );
		return 0;
	}
	
	if ( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_LOG_INFO( color->_green << "show informations about OpenFX plugins" << color->_std );
		return 0;
	}

	// defines plugins
	if( vm.count(kInputDirOptionLongName) )
	{
		plugins = vm[kInputDirOptionLongName].as< std::vector<std::string> >();
	}
	
	if( vm.count(kFilterOptionLongName) )
	{
		bal::split( filters, vm[kFilterOptionLongName].as<std::string>(), bal::is_any_of(","));
	}
	
	if( vm.count(kAllOptionLongName) | (plugins.size() == 0) )
	{
		tth::core().preload();
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::core().getImageEffectPluginCache().getPlugins();
		
		for( std::size_t i = 0; i < plugs.size(); ++i )
		{
			const std::string plugName = plugs.at(i)->getRawIdentifier();
			if( isNotFiltered( plugName, filters) )
			{
				TUTTLE_LOG_INFO( plugName );
			}
		}
		return 0;
	}


	if (vm.count(kPropertiesOptionLongName))
	{
		properties = true;
	}

	if (vm.count(kClipsOptionLongName))
	{
		clips = true;
	}

	if (vm.count(kParametersOptionLongName))
	{
		parameters = true;
	}
//	for(unsigned int i=0; i< plugins.size(); i++)
//		TUTTLE_COUT( plugins.at(i) );
	
	try
	{
		tth::core().preload();
		// get the plugins names for research partials names (rawreader need to make reference to the plug tuttle.reader)
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::core().getImageEffectPluginCache().getPlugins();

		unsigned int founded;
		BOOST_FOREACH( const std::string& plugin, plugins )
		{
			std::vector< std::string > termsPlugin;
			bal::split( termsPlugin, plugin, bal::is_any_of("."));

			for( std::size_t i=0; i<plugs.size(); i++ )
			{
				founded = 0;
				for( std::size_t t=0; t<termsPlugin.size(); t++ )
				{
					std::size_t found1 = plugs.at(i)->getRawIdentifier().find( "."+termsPlugin.at(t) );
					std::size_t found2 = plugs.at(i)->getRawIdentifier().find( termsPlugin.at(t)+"." );
					if( ( found1 == std::string::npos ) && ( found2 == std::string::npos ) )
					{
						break;
					}
					++founded;
					if( founded == termsPlugin.size() )
					{
						/*TUTTLE_COUT("plug is " << plugs.at(i)->getRawIdentifier() );*/
						foundPlugins.push_back( plugs.at(i)->getRawIdentifier() );
					}
				}
			}
		}

		TUTTLE_LOG_INFO( color->_red << "################################################################################" << color->_std );

		BOOST_FOREACH( const std::string& plugin, foundPlugins )
		{
			TUTTLE_LOG_INFO(color->_blue << "PLUGIN DESCRIPTION" << color->_std );
			getPluginProperties( plugin );
			TUTTLE_LOG_INFO( color->_red << "################################################################################" << color->_std );
		}
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( color->_red << boost::current_exception_diagnostic_information() << color->_std );
	}

	return 0;
}

