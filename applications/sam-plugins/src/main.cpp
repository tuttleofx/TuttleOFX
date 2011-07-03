
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

static const std::string	kColorStd	( "\E[0;0m"  );
static const std::string	kColorBlue	( "\E[1;34m" );
static const std::string	kColorGreen	( "\E[0;32m" );
static const std::string	kColorRed	( "\E[0;31m" );

namespace			bpo		= boost::program_options;
namespace			bal		= boost::algorithm;
namespace			tth		= tuttle::host;

bool				color		= false;
bool				properties	= false;
bool				clips		= false;
bool				parameters	= false;

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
	if( color )
	{
		TUTTLE_COUT( kColorRed );
	}
	else
	{
		TUTTLE_COUT( " " );
	}
	if(context.size() == 0)
	{
		TUTTLE_COUT( "Number of properties : " << properties.getSize() );
	}
	else
	{
		TUTTLE_COUT( "Number of properties for \"" << context << "\": " << properties.getSize() );
	}

	TUTTLE_COUT( "RW ModifiedBy\tType\tSize\t"<<std::setw (50)  << std::left << "Property Name" << "\tDefault Values");
	tth::ofx::property::PropertyMap propMap = properties.getMap();
	for( tth::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tth::ofx::property::OfxhProperty& prop = *( itProperty->second );
		color ?
		TUTTLE_COUT(	kColorGreen <<
				( prop.getPluginReadOnly() ? "r- " : "rw " ) <<
				( prop.getModifiedBy() == tth::ofx::property::eModifiedByHost ? "host  \t" : "plugin\t" ) <<
				( tth::ofx::property::mapTypeEnumToString( prop.getType() ) ) << "\t[" <<
				prop.getDimension() << "]\t" <<
				kColorBlue <<
				std::setw (50)  << std::left <<
				itProperty->first << kColorGreen << "\t{ " <<
				getDefaultValues(prop) << " }"
		)
		:
		TUTTLE_COUT(	( prop.getPluginReadOnly() ? "r- " : "rw " ) <<
				( prop.getModifiedBy() == tth::ofx::property::eModifiedByHost ? "host  \t" : "plugin\t" ) <<
				( tth::ofx::property::mapTypeEnumToString( prop.getType() ) ) << "\t[" <<
				prop.getDimension() << "]\t" <<
				std::setw (50)  << std::left <<
				itProperty->first << "\t{ " <<
				getDefaultValues(prop) << " }"
		)
		;
	}
}


void getPluginProperties( const std::string& plugName )
{
	tth::ofx::imageEffect::OfxhImageEffectPlugin* plug = tth::Core::instance().getImageEffectPluginById( plugName );
	
	if( !plug )
	{
		TUTTLE_COUT( kColorRed << "no plugin match to: " << plugName << kColorStd );
		return;
	}

	plug->loadAndDescribeActions();
	TUTTLE_COUT("Identifier:\t\t"		<< plug->getIdentifier() );
	TUTTLE_COUT("Raw identifier:\t\t"	<< plug->getRawIdentifier() );
	TUTTLE_COUT("Minor version:\t\t"	<< plug->getVersionMinor() );
	TUTTLE_COUT("Major version:\t\t" 	<< plug->getVersionMajor() );
	TUTTLE_COUT("API version:\t\t"		<< plug->getApiVersion() );

	// list contexts of plugin
	color ? TUTTLE_COUT(kColorGreen <<"Contexts:") : TUTTLE_COUT("Contexts:");
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plug->getContexts();
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet::iterator itContext;
	std::string strContexts;
	for (itContext = contexts.begin(); itContext != contexts.end(); itContext++)
	{
		strContexts += *itContext + ", ";
	}
	strContexts.erase(strContexts.size()-2, 2);
	TUTTLE_COUT( "[ " << strContexts << " ]");
	itContext = contexts.begin();

	// list properties of plugin for the first context
	if(properties)
	{
		color ? TUTTLE_COUT( "\n"<< kColorRed << "Properties" << kColorStd) : TUTTLE_COUT( "\nProperties") ;

		const tth::ofx::property::OfxhSet properties = plug->getDescriptorInContext( *itContext ).getProperties();
		printProperties( properties );
	}
	tth::ofx::imageEffect::OfxhImageEffectNode* plugInst = NULL;
	if(clips | parameters)
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
			TUTTLE_COUT( "Plugin contexts not supported by the host. (" + plugName + ")" );
			return;
		}
	}
	if(clips)
	{
		color ? TUTTLE_COUT( "\n"<< kColorRed << "Clips") : TUTTLE_COUT( "\nClips") ;
		typedef std::map<std::string, tth::ofx::attribute::OfxhClipImageDescriptor*> ContextMap;

		// get contexts
		ContextMap::const_iterator it = plugInst->getDescriptor( ).getClips().begin();
		std::string strClipContexts;
		for ( ; it != plugInst->getDescriptor( ).getClips().end(); it++ )
			strClipContexts += (*it).first + ", " ;
		strClipContexts.erase( strClipContexts.size()-2, 2 );
		color ? TUTTLE_COUT( kColorGreen << "[ " << strClipContexts << " ]" << kColorStd ): TUTTLE_COUT( "[ " << strClipContexts << " ]" );

		// get propeties in each context
		ContextMap::const_iterator it2 = plugInst->getDescriptor( ).getClips().begin();
		for ( ; it2 != plugInst->getDescriptor().getClips().end(); it2++ )
			printProperties( (*it2).second->getProperties(), (*it2).first );
	}
	if(parameters)
	{
		color ? TUTTLE_COUT( "\n"<< kColorRed << "Parameters") : TUTTLE_COUT( "\nParameters") ;
		typedef std::map<std::string, tth::ofx::attribute::OfxhParamDescriptor*> ParamDescriptorMap;

		// get contexts
		ParamDescriptorMap::const_iterator it = plugInst->getDescriptor( ).getParams().begin();
		std::string strParamsContexts;
		for ( ; it != plugInst->getDescriptor( ).getParams().end(); it++ )
			strParamsContexts += (*it).first + ", " ;
		strParamsContexts.erase( strParamsContexts.size()-2, 2 );
		color ? TUTTLE_COUT( kColorGreen << "[ " << strParamsContexts << " ]" << kColorStd) : TUTTLE_COUT( "[ " << strParamsContexts << " ]" );

		// get propeties in each context
		ParamDescriptorMap::const_iterator it2 = plugInst->getDescriptor( ).getParams().begin();
		for ( ; it2 != plugInst->getDescriptor().getParams().end(); it2++ )
			printProperties( (*it2).second->getProperties(), (*it2).first );

/*


		const tth::ofx::property::OfxhSet paramProperties = plugInst->getDescriptor( ).getParamList().front().getProperties();
		printProperties( paramProperties, "" );*/
	}
}

bool isNotFiltered( std::string plugName, std::vector<std::string>& filters)
{
	if(filters.size()==0)
		return true;
	
	for(unsigned int i=0; i<filters.size(); i++)
	{
		std::string filter(filters.at(i));
		filter = boost::regex_replace( filter, boost::regex( "\\*" ), "(.*)"  );
		filter = boost::regex_replace( filter, boost::regex( "\\?" ), "(.)"  );

		if (regex_match(plugName, boost::regex(filter)))
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
		("help,h"		, "show this help")
		("all,a"		, "show list of all plugins available")
		("filter,f"		, bpo::value<std::string>(), "filtering the output for research a plugin. ex: -f \"*blur,*tuttle*\"")
		("color"		, "color the output")
		("verbose,v"		, "explain what is being done")
		("properties"		, "list properties of the OpenFX plugin")
		("clips"		, "list clips of the OpenFX plugin")
		("parameters"		, "list parameters of the OpenFX plugin")
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
	bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

	// get environnement options and parse them
	if( const char* env_ptr = std::getenv("SAM_PLUGINS_OPTIONS") )
	{
		std::vector<std::string> envOptions;
		std::string env( env_ptr );
		envOptions.push_back( env );
		bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
	}

	bpo::notify(vm);

	if (vm.count("help"))
	{
		TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
		TUTTLE_COUT( "NAME\n\tsam-plugins - show informations about OpenFX plugins\n" );
		TUTTLE_COUT( "SYNOPSIS\n\tsam-plugins [options] [specific_OpenFX_plugin]\n" );
		TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
		return 0;
	}
	
	// defines plugins
	if (vm.count("plugin"))
	{
		plugins = vm["plugin"].as< std::vector<std::string> >();
	}
	
	if (vm.count("filter"))
	{
		bal::split( filters, vm["filter"].as<std::string>(), bal::is_any_of(","));
	}
	
	if (vm.count("all") | (plugins.size() == 0) )
	{
		tth::Core::instance().preload();
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::Core::instance().getImageEffectPluginCache().getPlugins();
		
		for(unsigned int i=0; i<plugs.size(); i++)
		{
			std::string plugName = plugs.at(i)->getRawIdentifier();
			if(isNotFiltered( plugName, filters))
				TUTTLE_COUT( plugs.at(i)->getRawIdentifier() );
		}
		return 0;
	}

	if (vm.count("verbose"))
	{
		verbose = true;
	}

	if (vm.count("color"))
	{
		color = true;
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
		// get the plugins names for research partials names (rawreader need to make reference to the plug fr.tuttle.reader)
		const std::vector<tth::ofx::imageEffect::OfxhImageEffectPlugin*> plugs = tth::Core::instance().getImageEffectPluginCache().getPlugins();

		unsigned int founded;
		BOOST_FOREACH( std::string plugin, plugins )
		{
			std::vector< std::string > termsPlugin;
			bal::split( termsPlugin, plugin, bal::is_any_of("."));

			for( unsigned int i=0; i<plugs.size(); i++ )
			{
				founded = 0;
				for( unsigned int t=0; t<termsPlugin.size(); t++ )
				{
					size_t found1 = plugs.at(i)->getRawIdentifier().find( "."+termsPlugin.at(t) );
					size_t found2 = plugs.at(i)->getRawIdentifier().find( termsPlugin.at(t)+"." );
					if ( ( found1 != std::string::npos ) | ( found2 != std::string::npos ) )
					{
						founded++;
					}
					else
						break;
					if( founded == termsPlugin.size() )
					{
						/*TUTTLE_COUT("plug is " << plugs.at(i)->getRawIdentifier() );*/
						foundPlugins.push_back( plugs.at(i)->getRawIdentifier() );
					}
				}
			}
		}

		color ?
		TUTTLE_COUT( kColorRed << "################################################################################" << kColorStd )
		:
		TUTTLE_COUT( "################################################################################")
		;

		BOOST_FOREACH( std::string plugin, foundPlugins )
		{
			color ?
			TUTTLE_COUT( kColorBlue << "PLUGIN DESCRIPTION" )
			:
			TUTTLE_COUT( "PLUGIN DESCRIPTION" )
			;
			getPluginProperties( plugin );
			color ?
			TUTTLE_COUT( kColorRed << "################################################################################" << kColorStd )
			:
			TUTTLE_COUT( "################################################################################")
			;
		}
	}
	catch(... )
	{
		TUTTLE_COUT( kColorRed );
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
		TUTTLE_COUT( kColorStd );
	}

	return 0;
}

