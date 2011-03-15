
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>

#define STD_COLOR   "\E[0m"
#define COLOR_BLUE  "\E[1;34m"
#define COLOR_GREEN "\E[0;32m"
#define COLOR_RED   "\E[0;31m"

namespace bpo = boost::program_options;
namespace bal = boost::algorithm;
namespace tth = tuttle::host;

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

void getPluginProperties( const std::string& plugName)
{
	tth::ofx::imageEffect::OfxhImageEffectPlugin* plug = tth::Core::instance().getImageEffectPluginById( plugName );
	
	if( !plug )
	{
		TUTTLE_COUT( COLOR_RED << "no plugin match to: " << plugName << STD_COLOR );
		return;
	}

	plug->loadAndDescribeActions();
	TUTTLE_COUT("Identifier:\t\t"		<< plug->getIdentifier() );
	TUTTLE_COUT("Raw identifier:\t\t"	<< plug->getRawIdentifier() );
	TUTTLE_COUT("Minor version:\t\t"	<< plug->getVersionMinor() );
	TUTTLE_COUT("Major version:\t\t" 	<< plug->getVersionMajor() );
	TUTTLE_COUT("API version:\t\t"		<< plug->getApiVersion() );
	
	// list contexts of plugin
	TUTTLE_COUT(COLOR_GREEN <<"Contexts:");
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet contexts = plug->getContexts();
	tth::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet::iterator itContext;

	for (itContext = contexts.begin(); itContext != contexts.end(); itContext++)
	{
		TUTTLE_COUT( *itContext );
	}

	// list properties of plugin for the first context
	itContext = contexts.begin();
	const tth::ofx::property::OfxhSet properties = plug->getDescriptorInContext( *itContext ) .getProperties();
	TUTTLE_COUT( COLOR_RED << "Number of properties: " << properties.getSize() );
	TUTTLE_COUT( "RW ModifiedBy\tType\tSize\t"<<std::setw (50)  << std::left << "Property Name" << "\tDefault Values");
	tth::ofx::property::PropertyMap propMap = properties.getMap();
	for( tth::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tth::ofx::property::OfxhProperty& prop = *( itProperty->second );
		TUTTLE_COUT(	COLOR_GREEN <<
				( prop.getPluginReadOnly() ? "r- " : "rw " ) <<
				( prop.getModifiedBy() == tth::ofx::property::eModifiedByHost ? "host  \t" : "plugin\t" ) <<
				( tth::ofx::property::mapTypeEnumToString( prop.getType() ) ) << "\t[" <<
				prop.getDimension() << "]\t" <<
				COLOR_BLUE <<
				std::setw (50)  << std::left <<
				itProperty->first << COLOR_GREEN << "\t{ " <<
				getDefaultValues(prop) << " }"
		);
	}
}

bool isNotFiltered( std::string plugName, std::vector<std::string>& filters)
{
	if(filters.size()==0)
		return true;
	
	for(uint i=0; i<filters.size(); i++)
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
	std::vector<std::string>	filters;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("help,h"		, "show this help")
		("all,a"		, "show list of all plugins available")
		("filter,f"		, bpo::value<std::string>(), "filtering the output for research a plugin. ex: -f \"*blur,*tuttle*\"")
		("verbose,v"		, "explain what is being done")
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

	bpo::positional_options_description pd;
	pd.add("", -1);
	
	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

	bpo::notify(vm);

	if (vm.count("help"))
	{
	    TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
	    TUTTLE_COUT( "NAME\n\tsam-plugins - show informations about OpenFX plugins\n" );
	    TUTTLE_COUT( "SYNOPSIS\n\tsam-plugins [options] [specific_OpenFX_plugin]\n" );
	    TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
	    return 1;
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
		
		for(uint i=0; i<plugs.size(); i++)
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
// 	for(uint i=0; i< plugins.size(); i++)
// 	  TUTTLE_COUT( plugins.at(i) );
	
	try
	{
		TUTTLE_COUT( COLOR_RED << "################################################################################" << STD_COLOR);
		tth::Core::instance().preload();
		BOOST_FOREACH( std::string plugin, plugins )
		{
			TUTTLE_COUT( COLOR_BLUE << "PLUGIN DESCRIPTION" );
			getPluginProperties( plugin );
			TUTTLE_COUT( COLOR_RED << "################################################################################" << STD_COLOR);
		}
	}
	catch(... )
	{
	    TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}

	return 0;
}

