#include "commandLine.hpp"
#include "node_io.hpp"

#include <sam/common/color.hpp>

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/attribute/expression.hpp>
#include <tuttle/host/Graph.hpp>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

namespace sam {
namespace samdo {

Color _color;

struct NodeCommand
{
	NodeCommand( const std::vector<std::string>& commandLine )
	: _name( commandLine[0] )
	{
		BOOST_ASSERT( commandLine.size() );
	}

	std::string _name;
	std::vector< std::pair<std::string, std::string> > _params;
	std::vector< std::pair<std::string, std::string> > _flags;
};

}
}


int main( int argc, char** argv )
{
	namespace ttl = tuttle::host;
	using namespace sam::samdo;
	
	try
	{
		if( argc <= 1 ) // no argument
		{
			TUTTLE_COUT(
				"sam-do: missing operands.\n"
				"'sam-do --help' for more informations.\n"
			);
			exit( -1 );
		}
		
		bool                      enableColor         = false;
		bool                      script              = false;
		std::vector< std::string> cl_options;
		std::vector< std::vector<std::string> > cl_commands;
		
		decomposeCommandLine( argc, argv, cl_options, cl_commands );

		std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*> allNodes;

		// create the graph
		ttl::Graph graph;
		std::list<ttl::Graph::Node*> nodes;
		std::vector<std::ssize_t> range;
		std::size_t step;
		
		// Analyze each part of the command line
		{
			namespace bpo = boost::program_options;
			// Analyze sam-do flags
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
					("range,r"      , bpo::value<std::string>(), "frame range to render" )
					("verbose,V"    , "explain what is being done")
					("quiet,Q"      , "don't print commands")
					("nb-cores"     , bpo::value<std::size_t>(), "set a fix number of CPUs")
				;

				// describe hidden options
				bpo::options_description hidden;
				hidden.add_options()
					("enable-color", bpo::value<std::string>(), "enable (or disable) color")
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
					std::string str = samdo_vm["enable-color"].as<std::string>();

					if( str == "1" || boost::iequals(str, "y") || boost::iequals(str, "Y") || boost::iequals(str, "yes") || boost::iequals(str, "Yes") || boost::iequals(str, "true") || boost::iequals(str, "True") )
					{
						enableColor = true;
					}
					else
					{
						enableColor = false;
					}
				}

				if( enableColor )
				{
					_color.enable();
				}

				if( samdo_vm.count("help") )
				{
					TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );

					TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
					TUTTLE_COUT( _color._green << "\tsam-do - A command line to execute a list of OpenFX nodes." << _color._std << std::endl );

					TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
					TUTTLE_COUT( "\tsam-do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]" << std::endl);

					TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std );
					TUTTLE_COUT( _color._green << "\tA command line to execute a list of OpenFX nodes." << _color._std );
					TUTTLE_COUT( _color._green << "\tUse the sperarator // to pipe images between nodes." << _color._std << std::endl );


					TUTTLE_COUT( _color._blue  << "EXAMPLES" << _color._std );
					//TUTTLE_COUT( "\tsam-do r foo.####.dpx // w foo.####.jpg" );
					TUTTLE_COUT( _color._green << "\tsam-do --nodes" << _color._std );
					TUTTLE_COUT( _color._green << "\tsam-do blur -h" << _color._std );
					/// @todo version with read / write (without format specification)
					TUTTLE_COUT( _color._green << "\tsam-do --verbose dpxreader foo.####.dpx // blur 3 // resize scale=0.5 // jpegwriter foo.####.jpg // --range=10,20" << _color._std );
					TUTTLE_COUT( _color._green << "\tsam-do dpxreader foo.dpx // sobel // print // -Q" << _color._std << std::endl );

					TUTTLE_COUT( _color._blue  << "DISPLAY OPTIONS (replace the process)" << _color._std );
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

				// plugins loading
				ttl::Core::instance().preload();
				allNodes = ttl::Core::instance().getImageEffectPluginCache().getPlugins();

				if( samdo_vm.count("nodes") )
				{
					TUTTLE_COUT( "NODES" );
					for( std::size_t i = 0; i < allNodes.size(); ++i )
					{
						const std::string plugName = allNodes.at(i)->getRawIdentifier();

						std::vector< std::string > termsPlugin;
						boost::algorithm::split( termsPlugin, plugName, boost::algorithm::is_any_of("."));

						TUTTLE_COUT( "\t" << termsPlugin.back() );
					}
					exit( 0 );
				}

				{
					if( samdo_vm.count("range") )
					{
						const std::string rangeStr = samdo_vm["range"].as<std::string>();
						std::vector< std::string > rangeVStr = boost::program_options::split_unix( rangeStr, " ," );
						range.reserve( rangeVStr.size() );
						BOOST_FOREACH( const std::string& rStr, rangeVStr )
						{
							range.push_back( tuttle::host::attribute::extractValueFromExpression<std::ssize_t>(rStr) );
						}
					}
					/// @todo remove this and use the full time range of nodes to render
					/// @{
					if( range.size() < 1 )
						range.push_back( 0 );
					/// @}

					if( range.size() < 2 )
						range.push_back( range[0] );
					
					if( range.size() >= 3 )
						step = range[2];
					else
						step = 1;
				}
			}
			catch( const boost::program_options::error& e )
			{
				TUTTLE_CERR( _color._error << "sam-do: command line error: " << e.what() << _color._std );
				exit( -2 );
			}
			catch( ... )
			{
				TUTTLE_CERR( _color._error << "sam-do: error: " << boost::current_exception_diagnostic_information() << _color._std );
				exit( -2 );
			}

			/// @todo Set all sam-do options for rendering

			// Analyse options for each node
			{
				// Declare the supported options.
				bpo::options_description infoOptions( "\tDisplay options (replace the process)" );
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
				bpo::options_description confOptions( "\tConfigure process" );
				confOptions.add_options()
					("verbose,V"    , "explain what is being done")
					("nb-cores"     , bpo::value<std::size_t>(), "set a fix number of CPUs")
				;
				// describe hidden options
				bpo::options_description hiddenOptions;
				hiddenOptions.add_options()
					("param-values", bpo::value< std::vector<std::string> >(), "node parameters")
				;

				// define default options
				bpo::positional_options_description param_values;
				param_values.add("param-values", -1);

				bpo::options_description all_options;
				all_options.add(infoOptions).add(confOptions).add(hiddenOptions);
				
				BOOST_FOREACH( const std::vector<std::string>& command, cl_commands )
				{
					std::string userNodeName = command[0];
					std::string nodeFullName = command[0];
//					boost::algorithm::to_lower( userNodeName );
					std::vector<std::string> nodeArgs;
					std::copy( command.begin()+1, command.end(), std::back_inserter(nodeArgs) );
					
					try
					{
						std::vector< std::string > detectedPlugins;
						for( std::size_t i = 0; i < allNodes.size(); ++i )
						{
							const std::string plugName = allNodes.at(i)->getRawIdentifier();
							if( plugName == userNodeName )
							{
								detectedPlugins.clear();
								detectedPlugins.push_back( plugName );
								break;
							}
							if( boost::algorithm::find_first(plugName, userNodeName ) )
							{
								detectedPlugins.push_back( plugName );
							}
						}
						if( detectedPlugins.size() != 1 )
						{
							if( detectedPlugins.size() < 1 )
							{
								BOOST_THROW_EXCEPTION( tuttle::exception::Value()
									<< tuttle::exception::user() + "Unrecognized node name \"" + userNodeName + "\"." );
							}
							else
							{
								tuttle::exception::user userMsg;
								userMsg + "Ambiguous node name \"" + userNodeName + "\".\n";
								userMsg + "Possible nodes:\n";
								BOOST_FOREACH( const std::string& p, detectedPlugins )
								{
									userMsg + " - \"" + p + "\"\n";
								}
								BOOST_THROW_EXCEPTION( tuttle::exception::Value()
									<< userMsg );
							}
						}
						nodeFullName = detectedPlugins.front();

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
							TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]" );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "NODE" );
							TUTTLE_COUT( "\tsam-do " << nodeFullName << " - OpenFX node." );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "DESCRIPTION" );
							TUTTLE_COUT( "\tnode type: " << ttl::mapNodeTypeEnumToString( currentNode.getNodeType() ) );
							// internal node help
							if( currentNode.getNodeType() == ttl::INode::eNodeTypeImageEffect )
							{
								if( currentNode.asImageEffectNode().getDescriptor().getProperties().hasProperty( kOfxImageEffectPluginPropGrouping, true ) )
								{
									TUTTLE_COUT( "\t" << currentNode.asImageEffectNode().getDescriptor().getProperties().fetchStringProperty( kOfxImageEffectPluginPropGrouping ).getValue() );
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
							TUTTLE_COUT("");
							TUTTLE_COUT( "PARAMETERS" );
							TUTTLE_COUT("");
							ttl::ofx::attribute::OfxhParamSet& params = currentNode.getParamSet();
							BOOST_FOREACH( ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
							{
								if( param.getSecret() )
									continue; // ignore secret parameters
								TUTTLE_COUT(
									"\t" <<
									param.getScriptName() << ":\t" << param.getParamType() << " x" << param.getSize()
									);
							}
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "OPTIONS" );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( infoOptions );
							TUTTLE_COUT( confOptions );
							TUTTLE_COUT("");
							exit(0);
						}
						if( node_vm.count("version") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT( "Version " << currentNode.getVersionStr() );
							TUTTLE_COUT("");
							exit(0);
						}
						if( node_vm.count("attributes") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT("");
							TUTTLE_COUT( "ATTRIBUTES" );
							TUTTLE_COUT("");
							TUTTLE_COUT( "\tCLIPS" );
							coutClips( currentNode );
							TUTTLE_COUT("");
							TUTTLE_COUT( "\tPARAMETERS" );
							coutParameters( currentNode );
							TUTTLE_COUT("");
							exit(0);
						}
						if( node_vm.count("properties") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT("");
							TUTTLE_COUT( "PROPERTIES" );
							coutProperties( currentNode );
							TUTTLE_COUT("");
							exit(0);
						}
						if( node_vm.count("clips") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT("");
							TUTTLE_COUT( "CLIPS" );
							coutClips( currentNode );
							TUTTLE_COUT("");
							exit(0);
						}
						if( node_vm.count("clip") )
						{
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT("");
							const std::string clipName = node_vm["clip"].as<std::string>();
							TUTTLE_COUT( "CLIP: " << clipName );
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
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT("");
							TUTTLE_COUT( "PARAMETERS" );
							TUTTLE_COUT("");
							coutParameters( currentNode );
							exit(0);
						}
						if( node_vm.count("param") )
						{
							const std::string paramName = node_vm["param"].as<std::string>();
							TUTTLE_COUT( "\tsam-do " << nodeFullName );
							TUTTLE_COUT( "PARAM: " << paramName );
							ttl::ofx::attribute::OfxhParam& param = currentNode.getParam( paramName );
							TUTTLE_COUT("");
							TUTTLE_COUT(
								"\t" <<
								(param.getSecret() ? "SECRET -- " : "") <<
								param.getScriptName() << ": " << param.getParamType() << " x" << param.getSize()
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
						TUTTLE_CERR( "sam-do - " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
						TUTTLE_CERR( "Error: " << e.what() );
#else
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
#endif
						exit( -2 );
					}
					catch( const tuttle::exception::Common& e )
					{
						TUTTLE_CERR( "sam-do - " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
						TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) );
#else
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
						TUTTLE_CERR( "Backtrace: " << boost::trace(e) );
#endif
						exit( -2 );
					}
					catch( ... )
					{
						TUTTLE_CERR( "sam-do - " << nodeFullName );
						TUTTLE_CERR( "Unknown error." );
						TUTTLE_CERR( "\n" );
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
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
//		BOOST_FOREACH( const std::vector<std::string>& node, cl_commands )
//		{
//			TUTTLE_COUT( "[" << node[0] << "]" );
//			for( std::size_t i = 1; i < node.size(); ++i )
//			{
//				const std::string& s = node[i];
//				if( s[0] == '-' )
//				{
//					TUTTLE_COUT( s );
//				}
//				else
//				{
//					TUTTLE_COUT( "* " << s );
//				}
//			}
//		}

		// Connect all nodes linearly
		graph.connect( nodes );

		// Execute the graph
		graph.compute( *nodes.back(), range[0], range[1] );
	}
	catch( const tuttle::exception::Common& e )
	{
		TUTTLE_CERR( "sam-do" );
#ifdef TUTTLE_PRODUCTION
		TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user>(e) );
#else
		TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
		TUTTLE_CERR( "Backtrace: " << boost::trace(e) );
#endif
		exit( -2 );
	}
	catch( const boost::program_options::error& e )
	{
		TUTTLE_CERR( "sam-do");
		TUTTLE_CERR( "Error: " << e.what() );
		exit( -2 );
	}
	catch( ... )
	{
		TUTTLE_CERR( "sam-do");
		TUTTLE_CERR( boost::current_exception_diagnostic_information() );
		exit( -2 );
	}
	return 0;
}

