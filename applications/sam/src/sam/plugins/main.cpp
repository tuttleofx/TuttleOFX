#include <sam/common/color.hpp>

#include <tuttle/common/clip/Sequence.hpp>

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

sam::Color          _color;
bool                color       = false;
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
	if( context.size() == 0 )
	{
		TUTTLE_COUT( _color._red << "Number of properties : " << properties.getSize() << _color._std );
	}
	else
	{
		TUTTLE_COUT( _color._red << "Number of properties for \"" << context << "\": " << properties.getSize() << _color._std );
	}

	TUTTLE_COUT( _color._red << "RW" << _color._std << " " << _color._red << "ModifiedBy" << _color._std << "\t" << _color._red << "Type" << _color._std << "\t" << _color._red << "Size" << _color._std << "\t" << _color._red <<std::setw (50)  << std::left << "Property Name" << _color._std << "\t" << _color._red << "Default Values" << _color._std );
	tth::ofx::property::PropertyMap propMap = properties.getMap();
	for( tth::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tth::ofx::property::OfxhProperty& prop = *( itProperty->second );
		TUTTLE_COUT(
			_color._green <<
			( prop.getPluginReadOnly() ? (_color._green + "r-" + _color._std + " ") : (_color._green + "rw" + _color._std + " " ) ) <<
			( prop.getModifiedBy() == tth::ofx::property::eModifiedByHost ? ( _color._green + "host" + _color._std + "  \t" ) : ( _color._green + "plugin" + _color._std + "\t" ) ) <<
			_color._green << ( tth::ofx::property::mapTypeEnumToString( prop.getType() ) ) << _color._std << "\t" << _color._green << "[" <<
			prop.getDimension() << "]" << _color._std << "\t" <<
			_color._blue <<
			std::setw (50)  << std::left <<
			itProperty->first << _color._std << "\t" << _color._green << "{ " <<
			getDefaultValues(prop) << " }" << _color._std
		);
	}
}


void getPluginProperties( const std::string& plugName )
{
	tth::ofx::imageEffect::OfxhImageEffectPlugin* plug = tth::Core::instance().getImageEffectPluginById( plugName );
	
	if( !plug )
	{
		TUTTLE_COUT( _color._red << "no plugin match to: " << plugName << _color._std );
		return;
	}

	plug->loadAndDescribeActions();
	TUTTLE_COUT("Identifier:\t\t"		<< plug->getIdentifier() );
	TUTTLE_COUT("Raw identifier:\t\t"	<< plug->getRawIdentifier() );
	TUTTLE_COUT("Minor version:\t\t"	<< plug->getVersionMinor() );
	TUTTLE_COUT("Major version:\t\t" 	<< plug->getVersionMajor() );
	TUTTLE_COUT("API version:\t\t"		<< plug->getApiVersion() );

	// list contexts of plugin
	TUTTLE_COUT( _color._green << "Contexts:" << _color._std );

	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plug->getContexts();
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet::iterator itContext;
	std::string strContexts;
	for( itContext = contexts.begin(); itContext != contexts.end(); ++itContext )
	{
		strContexts += *itContext + ", ";
	}
	strContexts.erase(strContexts.size()-2, 2);
	TUTTLE_COUT( "[ " << strContexts << " ]" );
	itContext = contexts.begin();

	// list properties of plugin for the first context
	if( properties )
	{
		TUTTLE_COUT( std::endl << _color._red << "Properties" << _color._std );

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
			TUTTLE_CERR( _color._error << "Plugin contexts not supported by the host. (" + plugName + ")" << _color._std );
			return;
		}
	}
	if( clips )
	{
		TUTTLE_COUT( std::endl << _color._red << "Clips" << _color._std );

		typedef std::map<std::string, tth::ofx::attribute::OfxhClipImageDescriptor*> ContextMap;

		// get contexts
		ContextMap::const_iterator it = plugInst->getDescriptor( ).getClips().begin();
		std::string strClipContexts;
		for( ; it != plugInst->getDescriptor( ).getClips().end(); ++it )
		{
			strClipContexts += (*it).first + ", " ;
		}
		strClipContexts.erase( strClipContexts.size()-2, 2 );

		TUTTLE_COUT( _color._green << "[ " << strClipContexts << " ]" << _color._std );


		// get propeties in each context
		ContextMap::const_iterator it2 = plugInst->getDescriptor( ).getClips().begin();
		for( ; it2 != plugInst->getDescriptor().getClips().end(); ++it2 )
		{
			printProperties( (*it2).second->getProperties(), (*it2).first );
		}
	}
	
	if( parameters )
	{
		TUTTLE_COUT( std::endl << _color._red << "Parameters" << _color._std );

		typedef std::map<std::string, tth::ofx::attribute::OfxhParamDescriptor*> ParamDescriptorMap;

		// get contexts
		ParamDescriptorMap::const_iterator it = plugInst->getDescriptor( ).getParams().begin();
		std::string strParamsContexts;
		for( ; it != plugInst->getDescriptor( ).getParams().end(); ++it )
		{
			strParamsContexts += (*it).first + ", ";
		}
		strParamsContexts.erase( strParamsContexts.size()-2, 2 );

		TUTTLE_COUT( _color._green << "[ " << strParamsContexts << " ]" << _color._std );

		// get propeties in each context
		ParamDescriptorMap::const_iterator it2 = plugInst->getDescriptor( ).getParams().begin();
		for( ; it2 != plugInst->getDescriptor().getParams().end(); it2++ )
		{
			printProperties( (*it2).second->getProperties(), (*it2).first );
		}
		/*
		const tth::ofx::property::OfxhSet paramProperties = plugInst->getDescriptor( ).getParamList().front().getProperties();
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
	bool				verbose			= false;
	std::vector<std::string>	plugins;
	std::vector<std::string>	foundPlugins;
	std::vector<std::string>	filters;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("help,h"     , "show this help")
		("all,a"      , "show list of all plugins available")
		("filter,f"   , bpo::value<std::string>(), "filtering the output for research a plugin. ex: -f \"*blur,*tuttle*\"")
		("color"      , "color the output")
		("verbose,v"  , "explain what is being done")
		("properties" , "list properties of the OpenFX plugin")
		("clips"      , "list clips of the OpenFX plugin")
		("parameters" , "list parameters of the OpenFX plugin")
		("brief"            , "brief summary of the tool")
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		("plugin", bpo::value< std::vector<std::string> >(), "input directories")
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add("plugin", -1);
	
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
		TUTTLE_COUT( "sam-plugins: command line error: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_COUT( "sam-plugins: unknown error in command line.");
		exit( -2 );
	}

	if (vm.count("color"))
	{
		color = true;
		_color.enable();
	}

	if( vm.count("help") )
	{
		TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-plugins - show informations about OpenFX plugins" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
		TUTTLE_COUT( _color._green << "\tsam-plugins [options] [specific_OpenFX_plugin]" << _color._std << std::endl );
		TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std );
		TUTTLE_COUT( "List OpenFX in OFX_PLUGIN_PATH by default.");
		TUTTLE_COUT( "And could print properties, parameters and clips for each plugins" << std::endl );
		TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std );
		TUTTLE_COUT( mainOptions );
		return 0;
	}
	
	if ( vm.count("brief") )
	{
		TUTTLE_COUT( _color._green << "show informations about OpenFX plugins" << _color._std );
		return 0;
	}

	// defines plugins
	if( vm.count("plugin") )
	{
		plugins = vm["plugin"].as< std::vector<std::string> >();
	}
	
	if( vm.count("filter") )
	{
		bal::split( filters, vm["filter"].as<std::string>(), bal::is_any_of(","));
	}
	
	if( vm.count("all") | (plugins.size() == 0) )
	{
		tth::Core::instance().preload();
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::Core::instance().getImageEffectPluginCache().getPlugins();
		
		for( std::size_t i = 0; i < plugs.size(); ++i )
		{
			const std::string plugName = plugs.at(i)->getRawIdentifier();
			if( isNotFiltered( plugName, filters) )
			{
				TUTTLE_COUT( plugName );
			}
		}
		return 0;
	}

	if (vm.count("verbose"))
	{
		verbose = true;
	}

	if (vm.count("properties"))
	{
		properties = true;
	}

	if (vm.count("clips"))
	{
		clips = true;
	}

	if (vm.count("parameters"))
	{
		parameters = true;
	}
//	for(unsigned int i=0; i< plugins.size(); i++)
//		TUTTLE_COUT( plugins.at(i) );
	
	try
	{
		tth::Core::instance().preload();
		// get the plugins names for research partials names (rawreader need to make reference to the plug tuttle.reader)
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::Core::instance().getImageEffectPluginCache().getPlugins();

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

		TUTTLE_COUT( _color._red << "################################################################################" << _color._std );

		BOOST_FOREACH( const std::string& plugin, foundPlugins )
		{
			TUTTLE_COUT(_color._blue << "PLUGIN DESCRIPTION" << _color._std );
			getPluginProperties( plugin );
			TUTTLE_COUT( _color._red << "################################################################################" << _color._std );
		}
	}
	catch( ... )
	{
		TUTTLE_CERR( _color._red << boost::current_exception_diagnostic_information() << _color._std );
	}

	return 0;
}

