#include "commandLine.hpp"
#include "node_io.hpp"
#include "node.hpp"
#include "global.hpp"
#include "nodeDummy.hpp"

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/attribute/expression.hpp>
#include <tuttle/host/Graph.hpp>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

std::string getDefaultValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    std::string s;
    std::vector<std::string> values = sam::samdo::getDefaultOrChoiceValues( prop );

    for( size_t i=0; i < values.size(); i++ )
    {
	s += values.at( i );
	s += " ";
    }
    return s;
}

std::string getPrintableDefaultValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    std::string s;
    std::vector<std::string> values = sam::samdo::getDefaultOrChoiceValues( prop );

    s += sam::samdo::_color._green;
    s += getDefaultValues( prop );
    s += sam::samdo::_color._std;
    return s;
}

/// get defaults values of plugin properties
std::vector<std::string> getChoiceValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    return sam::samdo::getDefaultOrChoiceValues( prop );
}

/// get default value of plugin properties
std::string getDefaultChoiceValue( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    std::string s;
    if( !(prop.getType() == 3) ) // if Pointer, we don't have _value
    {
	int indexForDefaultValue =  std::atoi( getPrintableDefaultValues( prop ).c_str() );
	s += prop.getStringValue( indexForDefaultValue );
    }
    return s;
}

/// get defaults values of plugin properties
std::string getPropType(const tuttle::host::ofx::property::OfxhProperty& prop)
{
    std::string s;
    if( !(prop.getType() == 3) ) // if Pointer, we don't have _value and _defaultValue properties
    {
	int n = 0;
	if( prop.getDimension() > 0 )
	{
	    s += prop.getStringValue( n );
	}
    }
    return s;
}

std::vector<std::string> getChoiceValuesForParameter( const tuttle::host::ofx::property::OfxhSet properties, std::string context="" )
{
    std::vector<std::string> choiceValues;
    std::string defaultValue;
    tuttle::host::ofx::property::PropertyMap propMap = properties.getMap();
    for( tuttle::host::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
    {
	const tuttle::host::ofx::property::OfxhProperty& prop = *( itProperty->second );
	std::string label = itProperty->first;

	if( std::strcmp( label.c_str() , "OfxParamPropChoiceOption" ) == 0 )
	{
	    choiceValues = sam::samdo::getDefaultOrChoiceValues( prop );
	    return choiceValues;
	}
    }
    return choiceValues;
}

void printProperties( const tuttle::host::ofx::property::OfxhSet properties, std::string context="" )
{
    std::string defaultValue;
    std::vector<std::string> choiceValues;

    tuttle::host::ofx::property::PropertyMap propMap = properties.getMap();
    for( tuttle::host::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
    {
	const tuttle::host::ofx::property::OfxhProperty& prop = *( itProperty->second );

	std::string label = itProperty->first;

	if( std::strcmp( label.c_str() , "OfxParamPropChoiceOption" ) == 0 )
	{
	    choiceValues = getChoiceValues( prop );
	}
	if( std::strcmp( label.c_str() , "OfxParamPropDefault" ) == 0 )
	{
	    defaultValue = getDefaultValues( prop );
	}
	if( std::strcmp( label.c_str() , "OfxParamPropType" ) == 0 )
	{
	    std::string type = getPropType( prop );
	    type.erase(0, 12);
	    if( std::strcmp( type.c_str() , "Group" ) ) // if it isn't a group parameter, we print the parameter.
	    {
		std::string stringDefaultValue = defaultValue;
		if( choiceValues.size() ) // if it's a choice, we take the nth argument
		{
			int indexOfDefaultValue = std::atoi( defaultValue.c_str() );
			stringDefaultValue = choiceValues.at( indexOfDefaultValue ) ;
		}

		TUTTLE_COUT( "\t" << sam::samdo::_color._green << std::left << std::setw (25) << context + ":" << sam::samdo::_color._std << std::setw( 15 ) << type << sam::samdo::_color._yellow << stringDefaultValue << sam::samdo::_color._std );
		if( choiceValues.size() )
		    for( size_t i=0; i < choiceValues.size(); i++ )
			TUTTLE_COUT( "\t\t\t\t\t" << sam::samdo::_color._red << "- " << choiceValues.at( i ) << sam::samdo::_color._std );
	    }
	}
    }
}

int main( int argc, char** argv )
{
    using namespace sam::samdo;
    namespace ttl = tuttle::host;

    try
    {
	if( argc <= 1 ) // no argument
	{
	    TUTTLE_COUT(
		"sam do: missing operands.\n"
		"'sam do --help' for more informations.\n"
	    );
	    exit( -1 );
	}

	bool continueOnError = false;
	bool enableColor = false;
	bool script = false;
	std::vector< std::string> cl_options;
	std::vector< std::vector<std::string> > cl_commands;

	decomposeCommandLine( argc, argv, cl_options, cl_commands );

	// create the graph
	ttl::Graph graph;
	std::list<ttl::Graph::Node*> nodes;
	std::vector<std::ssize_t> range;
	std::vector<double> renderscale;
	std::size_t step;

	// Analyze each part of the command line
	{
	    namespace bpo = boost::program_options;
	    // Analyze sam do flags
	    try
	    {
		// Declare the supported options.
		bpo::options_description infoOptions;
		infoOptions.add_options()
		    ("help,h"       , "show node help")
		    ("version,v"    , "display node version")
		    ("nodes,n"      , "show list of all available nodes")
		    ("color"        , "color the output")
		    ("script"       , "output is formated to using in script files")
		    ("brief"        , "brief summary of the tool")
		;
		bpo::options_description confOptions;
		confOptions.add_options()
		    ("continueOnError" , "continue on error" )
		    ("range,r"      , bpo::value<std::string>(), "frame range to render" )
		    ("renderscale"  , bpo::value<std::string>(), "renderscale" )
		    ("verbose,V"    , "explain what is being done")
		    ("quiet,Q"      , "don't print commands")
		    ("nb-cores"     , bpo::value<std::size_t>(), "set a fix number of CPUs")
		;

		// describe hidden options
		bpo::options_description hidden;
		hidden.add_options()
		    ("enable-color", bpo::value<std::string>(), "enable (or disable) color")
		    // params for auto-completion
		    ("nodes-list", "show list of all available nodes")
		;

		bpo::options_description all_options;
		all_options.add(infoOptions).add(confOptions).add(hidden);

		bpo::variables_map samdo_vm;
		bpo::store( bpo::command_line_parser(cl_options).options(all_options).run(), samdo_vm );
		if( const char* env_do_options = std::getenv("SAM_DO_OPTIONS") )
		{
		    const std::vector<std::string> vecOptions = bpo::split_unix( env_do_options, " " );
		    bpo::store(bpo::command_line_parser(vecOptions).options(all_options).run(), samdo_vm);
		}
		if( const char* env_do_options = std::getenv("SAM_OPTIONS") )
		{
		    const std::vector<std::string> vecOptions = bpo::split_unix( env_do_options, " " );
		    bpo::store(bpo::command_line_parser(vecOptions).options(all_options).run(), samdo_vm);
		}

		bpo::notify( samdo_vm );

		if ( samdo_vm.count("script") )
		{
		    // disable color, disable directory printing and set relative path by default
		    script = true;
		}

		if ( samdo_vm.count("color") && !script )
		{
		    enableColor = true;
		}
		if ( samdo_vm.count("enable-color") && !script )
		{
		    const std::string str = samdo_vm["enable-color"].as<std::string>();
		    enableColor = string_to_boolean( str );
		}

		if( enableColor )
		{
		    _color.enable();
		}

		if( samdo_vm.count("help") )
		{
		    TUTTLE_COUT( std::left );
		    TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );

		    TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
		    TUTTLE_COUT( _color._green << "\tsam do - A command line to execute a list of OpenFX nodes." << _color._std << std::endl );

		    TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
		    TUTTLE_COUT( "\tsam do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]" << std::endl);

		    TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std );
		    TUTTLE_COUT( _color._green << "\tA command line to execute a list of OpenFX nodes." << _color._std );
		    TUTTLE_COUT( _color._green << "\tUse the sperarator // to pipe images between nodes." << _color._std << std::endl );

#define SAM_DO_HELP_MARGING 35
#define SAM_DO_EXAMPLE_TITLE_COUT(x) TUTTLE_COUT( std::endl << _color._yellow << "  " << x << _color._std );
#define SAM_DO_EXAMPLE_LINE_COUT(x,y) TUTTLE_COUT( _color._green << "    " << std::setw(SAM_DO_HELP_MARGING) << x << y << _color._std );

		    TUTTLE_COUT( _color._blue  << "EXAMPLES" << _color._std << std::left );
		    SAM_DO_EXAMPLE_TITLE_COUT( "Plugins options" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Plugin list: ", "sam do --nodes" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Plugin help: ", "sam do blur -h" );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Generators and viewers" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Viewer: "                   , "sam do reader in.@.dpx // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Print: "                    , "sam do reader in.@.dpx // print color=full16ansi" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Constant generator: "       , "sam do constant                // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "White constant generator: " , "sam do constant color=1,1,1,1  // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "HD constant generator: "    , "sam do constant size=1920,1080 // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Checkerboard generator: "   , "sam do checkerboard            // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Checkerboard generator: "   , "sam do checkerboard width=500  // viewer" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Checkerboard generator: "   , "sam do checkerboard width=1920 ratio=2.35 // viewer" );
/*
		    SAM_DO_EXAMPLE_LINE_COUT ( "Colorgradient generator: "  , "sam do colorgradient point0=1190,424 color0=0.246,0.44,0.254,1 \\" );
		    SAM_DO_EXAMPLE_LINE_COUT ( " "		                 , "                     point1=458,726  color1=0.396,0.193,0.444,1 format=HD // viewer" );
*/
		    SAM_DO_EXAMPLE_LINE_COUT ( "Text writing: "             , "sam do constant // text text=\"hello\" size=80 // viewer" );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Image sequence conversion and creation");
		    SAM_DO_EXAMPLE_LINE_COUT ( "Convert Image: "   , "sam do reader in.dpx // writer out.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Convert Sequence: ", "sam do reader in.####.dpx // writer out.####.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Select a range: "  , "sam do reader in.####.dpx // writer out.####.jpg // --range=10,100" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "","r and w are shortcuts for reader and writer" );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Geometry processing during conversion" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Crop: "        , "sam do reader in.####.dpx // crop x1=20 x2=1000 y1=10 y2=300 // writer out.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Fill: "        , "sam do reader in.####.dpx // crop y1=10 y2=1060 mode=fill color=0.43,0.67,0.50 // writer out.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Resize: "      , "sam do reader in.####.dpx // resize size=1920,1080 // writer out.####.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Upscaling: "   , "sam do reader in.####.dpx // resize size=1920,1080 filter=lanczos  // writer out.####.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Downscaling: " , "sam do reader in.####.dpx // resize size=720,576   filter=mitchell // writer out.####.jpg" );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Color processing during conversion" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Lut :"  , "sam do reader in.####.dpx // lut lutFile.3dl // writer out.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "CTL: "  , "sam do reader in.####.dpx // ctl file=ctlCode.ctl // writer out.####.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Gamma: ", "sam do reader in.####.dpx // gamma master=2.2 // writer out.####.jpg" );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Image Sequence Numbering" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Frames with or without padding: ", "image.@.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Frames 1 to 100 padding 4: "     , "image.####.jpg -or- image.@.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Frames 1 to 100 padding 5: "     , "image.#####.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Printf style padding 4: "        , "image.%04d.jpg" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "All Frames in Directory: "       , "/path/to/directory"  );

		    SAM_DO_EXAMPLE_TITLE_COUT( "Processing options" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Multiple CPUs: "             , "sam do reader in.@.dpx // writer out.@.exr // --nb-cores 4" );
		    SAM_DO_EXAMPLE_LINE_COUT ( "Continues whatever happens: ", "sam do reader in.@.dpx // writer out.@.exr // --continueOnError" );

		    TUTTLE_COUT( std::endl << _color._blue  << "DISPLAY OPTIONS (replace the process)" << _color._std );
		    TUTTLE_COUT( infoOptions );
		    TUTTLE_COUT( _color._blue  << "CONFIGURE PROCESS" << _color._std );
		    TUTTLE_COUT( confOptions );
		    exit( 0 );
		}

		if ( samdo_vm.count("brief") )
		{
		    TUTTLE_COUT( _color._green << "A command line to execute a list of OpenFX nodes" << _color._std);
		    return 0;
		}

		if( samdo_vm.count("version") )
		{
		    TUTTLE_COUT( "TuttleOFX Host - version " << TUTTLE_HOST_VERSION_STR );
		    exit( 0 );
		}

		const std::string logFilename = (ttl::Core::instance().getPreferences().getTuttleHomePath() / "sam do.log").string();
		std::ofstream logFile( logFilename.c_str() );
		std::streambuf* strm_buffer = std::cerr.rdbuf(); // save cerr's output buffer
		std::cerr.rdbuf ( logFile.rdbuf() ); // redirect output into the file

		// plugins loading
		ttl::Core::instance().preload();

		if( samdo_vm.count("nodes") || samdo_vm.count("nodes-list") )
		{
		    if( samdo_vm.count("nodes") )
			TUTTLE_COUT( _color._blue  << "NODES" << _color._std );
		    std::vector< std::string > pluginNames;
		    addDummyNodeInList( pluginNames );
			const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& allNodes = ttl::Core::instance().getImageEffectPluginCache().getPlugins();
		    BOOST_FOREACH( const ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, allNodes )
		    {
			std::string plugName = node->getRawIdentifier();
			boost::algorithm::replace_first(plugName, "tuttle.", "" );
			pluginNames.push_back( plugName );
		    }
		    std::sort( pluginNames.begin(), pluginNames.end() );

		    const std::string indent = samdo_vm.count("nodes") ? "\t" : "";
		    BOOST_FOREACH( const std::string& pluginName, pluginNames )
		    {
			TUTTLE_COUT( indent << pluginName );
		    }
		    exit( 0 );
		}

		{
		    if( samdo_vm.count("range") )
		    {
			const std::string rangeStr = samdo_vm["range"].as<std::string>();
			std::vector< std::string > rangeVStr = boost::program_options::split_unix( rangeStr, " ," );
			range.reserve( rangeVStr.size() );
			TUTTLE_TCOUT( rangeVStr.size() );
			BOOST_FOREACH( const std::string& rStr, rangeVStr )
			{
			    range.push_back( tuttle::host::attribute::extractValueFromExpression<std::ssize_t>(rStr) );
			}
		    }
		    if( range.size() == 1 )
		    {
			range.push_back( range[0] );
		    }
		    if( range.size() >= 3 )
			step = range[2];
		    else
			step = 1;
		}
		{
		    if( samdo_vm.count("renderscale") )
		    {
			const std::string renderscaleStr = samdo_vm["renderscale"].as<std::string>();
			std::vector< std::string > renderscaleVStr = boost::program_options::split_unix( renderscaleStr, " ," );
			renderscale.reserve( renderscaleVStr.size() );
			TUTTLE_TCOUT( renderscaleVStr.size() );
			BOOST_FOREACH( const std::string& rStr, renderscaleVStr )
			{
			    renderscale.push_back( tuttle::host::attribute::extractValueFromExpression<double>(rStr) );
			}
		    }
		    if( renderscale.size() == 1 )
		    {
			renderscale.push_back( renderscale[0] );
		    }
		}
		std::cerr.rdbuf( strm_buffer ); // restore old output buffer
		continueOnError = samdo_vm.count("continueOnError");
	    }
	    catch( const boost::program_options::error& e )
	    {
		TUTTLE_CERR( _color._error << "sam do: command line error: " << e.what() << _color._std );
		exit( -2 );
	    }
	    catch( ... )
	    {
		TUTTLE_CERR( _color._error << "sam do: error: " << boost::current_exception_diagnostic_information() << _color._std );
		exit( -2 );
	    }

	    /// @todo Set all sam do options for rendering

	    // Analyse options for each node
	    {
		// Declare the supported options.
		bpo::options_description infoOptions;
		infoOptions.add_options()
		    ("help,h"       , "show node help")
		    ("version,v"    , "display node version")
		    ("attributes,a" , "show all attributes: parameters+clips")
		    ("properties"   , "list properties of the node")
		    ("clips,c"      , "list clips of the node")
		    ("clip,C"       , bpo::value<std::string>(), "display clip informations")
		    ("parameters,p" , "list parameters of the node")
		    ("param,P"      , bpo::value<std::string>(), "display parameter informations")
		;
		bpo::options_description confOptions;
		confOptions.add_options()
		    ("verbose,V"    , "explain what is being done")
		    ("nb-cores"     , bpo::value<std::size_t>(), "set a fix number of CPUs")
		;
		// describe hidden options
		bpo::options_description hiddenOptions;
		hiddenOptions.add_options()
		    ("param-values", bpo::value< std::vector<std::string> >(), "node parameters")
		    // for auto completion
		    ("parameters-list"  , "list parameters of the node")
		    ("parameter-type"   , bpo::value< std::string >(), "parameter type")
		    ("parameter-values" , bpo::value< std::string >(), "possible parameter values")
		    ("parameter-default", bpo::value< std::string >(), "parameter default value")
		;

		// define default options
		bpo::positional_options_description param_values;
		param_values.add("param-values", -1);

		bpo::options_description all_options;
		all_options.add(infoOptions).add(confOptions).add(hiddenOptions);

				const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& allNodes = ttl::Core::instance().getImageEffectPluginCache().getPlugins();
		BOOST_FOREACH( const std::vector<std::string>& command, cl_commands )
		{
		    std::string userNodeName = command[0];
					boost::algorithm::to_lower( userNodeName );
		    std::string nodeFullName = userNodeName;
		    std::vector<std::string> nodeArgs;
		    std::copy( command.begin()+1, command.end(), std::back_inserter(nodeArgs) );

		    try
		    {
			foundAssociateDummyNode( userNodeName, allNodes, nodeArgs );
			nodeFullName = retrieveNodeFullname( userNodeName );

			// parse the command line, and put the result in node_vm
			bpo::variables_map node_vm;
			bpo::store( bpo::command_line_parser(nodeArgs).options(all_options).positional(param_values).run(), node_vm );
			if( const char* env_ptr = std::getenv("SAM_DO_NODE_OPTIONS") )
			{
			    std::vector<std::string> envOptions;
			    std::string env( env_ptr );
			    envOptions.push_back( env );
			    bpo::store( bpo::command_line_parser(envOptions).options(all_options).run(), node_vm );
			}
			{
			    std::string envVarName;
			    envVarName += "SAM_DO_";
			    envVarName += boost::algorithm::replace_all_copy( nodeFullName, ".", "_" );
			    envVarName += "_OPTIONS";
			    if( const char* env_ptr = std::getenv(envVarName.c_str()) )
			    {
				std::vector<std::string> envOptions;
				std::string env( env_ptr );
				envOptions.push_back( env );
				bpo::store( bpo::command_line_parser(envOptions).options(all_options).run(), node_vm );
			    }
			}

			bpo::notify( node_vm );
			//TUTTLE_COUT( "[" << nodeFullName << "]" );

			ttl::Graph::Node& currentNode = graph.createNode( nodeFullName );
			nodes.push_back( &currentNode );

			// Check priority flags:
			// If one flag to display informations is used in command line,
			// it replaces all the process.
			// --help,h --version,v --verbose,V --params --clips --props

			if( node_vm.count("help") )
			{
			    TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std );
			    TUTTLE_COUT( "" );
			    TUTTLE_COUT( _color._blue  << "NODE" << _color._std );
			    TUTTLE_COUT( _color._green << "\tsam do " << nodeFullName << " - OpenFX node." << _color._std );
			    TUTTLE_COUT( "" );
			    TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std );
			    TUTTLE_COUT( _color._green << "\tnode type: " << ttl::mapNodeTypeEnumToString( currentNode.getNodeType() ) << _color._std );
			    // internal node help
			    if( currentNode.getNodeType() == ttl::INode::eNodeTypeImageEffect )
			    {
				if( currentNode.asImageEffectNode().getDescriptor().getProperties().hasProperty( kOfxImageEffectPluginPropGrouping, true ) )
				{
				    TUTTLE_COUT( "\t" << _color._green <<  currentNode.asImageEffectNode().getDescriptor().getProperties().fetchStringProperty( kOfxImageEffectPluginPropGrouping ).getValue() << _color._std );
				}
			    }
			    TUTTLE_COUT( "" );
			    if( currentNode.getProperties().hasProperty( kOfxPropPluginDescription, true ) )
			    {
				TUTTLE_COUT( currentNode.getProperties().fetchStringProperty( kOfxPropPluginDescription ).getValue(0) );
			    }
			    else
			    {
				TUTTLE_COUT( "\tNo description." );
			    }
			    TUTTLE_COUT( "" );
			    TUTTLE_COUT( _color._blue  << "PARAMETERS" << _color._std );
			    ttl::ofx::attribute::OfxhParamSet& params = currentNode.getParamSet();
			    BOOST_FOREACH( ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
			    {
				if( param.getSecret() )
				    continue; // ignore secret parameters
				std::string ofxType = param.getParamType();
				ofxType.erase(0, 12);
				if( std::strcmp( ofxType.c_str() , "Group" ) ) // if it isn't a group parameter, we print the parameter.
				    TUTTLE_COUT( "\t" << _color._green << std::left << std::setw(25) << ( param.getScriptName() + ":" ) << _color._std << std::setw(10) << ofxType << " x" << param.getSize() );
			    }

			    TUTTLE_COUT( std::endl << _color._blue  << "PARAMETERS DETAILS" << _color._std );
			    typedef std::map<std::string, tuttle::host::ofx::attribute::OfxhParamDescriptor*> ParamDescriptorMap;

			    // get contexts
			    ParamDescriptorMap::const_iterator it = currentNode.asImageEffectNode().getDescriptor( ).getParams().begin();
			    std::string strParamsContexts;
			    for( ; it != currentNode.asImageEffectNode().getDescriptor( ).getParams().end(); ++it )
			    {
				strParamsContexts += (*it).first + ", ";
			    }
			    strParamsContexts.erase( strParamsContexts.size()-2, 2 );

			    //TUTTLE_COUT( _color._green << "[ " << strParamsContexts << " ]" << _color._std );

			    // get propeties in each context
			    ParamDescriptorMap::const_iterator it2 = currentNode.asImageEffectNode().getDescriptor( ).getParams().begin();
			    for( ; it2 != currentNode.asImageEffectNode().getDescriptor().getParams().end(); it2++ )
			    {
				printProperties( (*it2).second->getProperties(), (*it2).first );
			    }

			    TUTTLE_COUT( "" );
			    TUTTLE_COUT( _color._blue  << "DISPLAY OPTIONS (replace the process)" << _color._std );
			    TUTTLE_COUT( infoOptions );
			    TUTTLE_COUT( _color._blue  << "CONFIGURE PROCESS" << _color._std );
			    TUTTLE_COUT( confOptions );

			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("version") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT( "Version " << currentNode.getVersionStr() );
			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("attributes") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "ATTRIBUTES" << _color._std );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "\tCLIPS" << _color._std );
			    coutClips( currentNode );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "\tPARAMETERS" << _color._std );
			    coutParametersWithDetails( currentNode );
			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("properties") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "PROPERTIES" << _color._std );
			    coutProperties( currentNode );
			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("clips") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "CLIPS" << _color._std );
			    coutClips( currentNode );
			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("clip") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT("");
			    const std::string clipName = node_vm["clip"].as<std::string>();
			    TUTTLE_COUT( _color._blue << "CLIP: " << _color._green << clipName << _color._std );
			    ttl::attribute::ClipImage& clip = currentNode.getClip( clipName );
			    TUTTLE_COUT(
				clip.getBitDepthString()
				<< ", " <<
				clip.getPixelAspectRatio()
				<< ", " <<
				clip.getNbComponents()
				);
			    TUTTLE_COUT("");
			    exit(0);
			}
			if( node_vm.count("parameters") )
			{
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT("");
			    TUTTLE_COUT( _color._blue << "PARAMETERS" << _color._std );
			    TUTTLE_COUT("");
			    coutParametersWithDetails( currentNode );
			    exit(0);
			}
			if( node_vm.count("parameters-list") )
			{
			    coutParameters( currentNode );
			    exit(0);
			}
			if( node_vm.count("param") )
			{
			    const std::string paramName = node_vm["param"].as<std::string>();
			    TUTTLE_COUT( "\tsam do " << nodeFullName );
			    TUTTLE_COUT( _color._blue << "PARAM: " << _color._green << paramName << _color._std );
			    ttl::ofx::attribute::OfxhParam& param = currentNode.getParam( paramName );
			    TUTTLE_COUT("");
			    TUTTLE_COUT(
				"\t" << _color._red <<
				(param.getSecret() ? "SECRET -- " : "") <<
				param.getScriptName() << ": " << param.getParamType() << " x" << param.getSize() <<
				_color._std
				);
			    TUTTLE_COUT("");
			    const std::string& hint = param.getHint();
			    if( hint.size() )
			    {
				TUTTLE_COUT( "\t" << hint );
			    }
			    TUTTLE_COUT("");
			    exit(0);
			}

			if( node_vm.count("parameter-type") )
			{
			    const std::string paramName = node_vm["parameter-type"].as<std::string>();
			    ttl::ofx::attribute::OfxhParam& param = currentNode.getParam( paramName );
			    TUTTLE_COUT( param.getParamType() );
			    exit(0);
			}

			if( node_vm.count("parameter-values") )
			{
			    const std::string paramName = node_vm["parameter-values"].as<std::string>();
			    ttl::ofx::attribute::OfxhParam& param = currentNode.getParam( paramName );
			    std::vector<std::string> values = getChoiceValuesForParameter( param.getProperties() );
			    for( size_t i=0; i < values.size(); i++ )
			    {
				TUTTLE_COUT( values.at( i ) );
			    }
			    exit(0);
			}
		    //("parameter-values", bpo::value< std::string >(), "possible parameter values")
		    //("parameter-default", bpo::value< std::string >(), "parameter default value")

			// Analyse parameters
			static const boost::regex re_param( "(?:([a-zA-Z_][a-zA-Z0-9_]*)=)?(.*)" );
			if( node_vm.count("param-values") )
			{
			    bool orderedParams = true;
			    std::size_t paramIdx = 0;
			    const std::vector<std::string> params = node_vm["param-values"].as< std::vector<std::string> >();
			    BOOST_FOREACH( const std::string& p, params )
			    {
				boost::cmatch matches;
				if( ! boost::regex_match( p.c_str(), matches, re_param ) )
				{
				    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
					<< tuttle::exception::user() + "Parameter can't be parsed \"" + p + "\"." );
				}
				if( matches.size() != 3 )
				{
				    // should never happen
				    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
					<< tuttle::exception::user() + "Parameter can't be parsed \"" + p + "\". " + matches.size() + " matches." );
				}
				const std::string paramName = matches[1];
				const std::string paramValue = matches[2];
				if( paramName.size() )
				{
				    orderedParams = false;
				}
				else if( orderedParams == false )
				{
				    BOOST_THROW_EXCEPTION( tuttle::exception::Value()
					<< tuttle::exception::user() + "Non-keyword parameter after keyword parameter. \"" + p + "\"." );
				}
//								TUTTLE_COUT( "* " << p );
//								TUTTLE_COUT( "3: " << paramName << " => " << paramValue );

				// setup the node with parameter value in tuttle.
				if( paramName.size() )
				{
				    currentNode.getParam( paramName ).setValueFromExpression( paramValue );
				}
				else
				{
				    currentNode.getParam( paramIdx ).setValueFromExpression( paramValue );
				}
				++paramIdx;
			    }
			}
		    }
		    catch( const boost::program_options::error& e )
		    {
			TUTTLE_CERR( _color._red << "sam do - " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
			TUTTLE_CERR( "Error: " << e.what() << _color._std );
#else
			TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() << _color._std );
#endif
			exit( -2 );
		    }
		    catch( const tuttle::exception::Common& e )
		    {
			TUTTLE_CERR( _color._red << "sam do - " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
			TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) << _color._std );
#else
			TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
			TUTTLE_CERR( "Backtrace: " << boost::trace(e) << _color._std );
#endif
			exit( -2 );
		    }
		    catch( ... )
		    {
			TUTTLE_CERR( _color._red << "sam do - " << nodeFullName );
			TUTTLE_CERR( "Unknown error." );
			TUTTLE_CERR( "\n" );
			TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() << _color._std );
			exit( -2 );
		    }
		}
	    }
	}

	// display nodes
//		BOOST_FOREACH( const std::string& option, cl_options )
//		{
//			TUTTLE_COUT( "| " << option );
//		}
	BOOST_FOREACH( const std::vector<std::string>& node, cl_commands )
	{
	    TUTTLE_COUT_DEBUG( "[" << node[0] << "]" );
	    for( std::size_t i = 1; i < node.size(); ++i )
	    {
		const std::string& s = node[i];
		if( s[0] == '-' )
		{
		    TUTTLE_COUT_DEBUG( s );
		}
		else
		{
		    TUTTLE_COUT_DEBUG( "* " << s );
		}
	    }
	}

	// Connect all nodes linearly
	graph.connect( nodes );

	// Execute the graph
	ttl::ComputeOptions options;
		if( range.size() >= 2 )
		{
			options._timeRanges.push_back( ttl::TimeRange( range[0], range[1], step ) );
		}
		if( renderscale.size() == 2 )
		{
			options._renderScale.x = renderscale[0];
			options._renderScale.y = renderscale[1];
		}
	options._continueOnError = continueOnError;
	options._returnBuffers = false;

	graph.compute( *nodes.back(), options );
    }
    catch( const tuttle::exception::Common& e )
    {
	TUTTLE_CERR( _color._red << "sam do - error" );
#ifdef TUTTLE_PRODUCTION
	TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) << _color._std );
#else
	TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
	TUTTLE_CERR( "Backtrace: " << boost::trace(e) << _color._std );
#endif
	exit( -2 );
    }
    catch( const boost::program_options::error& e )
    {
	TUTTLE_CERR( _color._red << "sam do - error" );
	TUTTLE_CERR( "Error: " << e.what() << _color._std );
	exit( -2 );
    }
    catch( ... )
    {
	TUTTLE_CERR( _color._red << "sam do - error" );
	TUTTLE_CERR( boost::current_exception_diagnostic_information() << _color._std );
	exit( -2 );
    }
    return 0;
}

