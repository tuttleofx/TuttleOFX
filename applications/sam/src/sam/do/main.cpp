#include "commandLine.hpp"
#include "global.hpp"
#include "nodeDummy.hpp"

#include <sam/common/node.hpp>
#include <sam/common/node_io.hpp>
#include <sam/common/options.hpp>
#include <sam/common/utility.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/attribute/expression.hpp>
#include <tuttle/host/Graph.hpp>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <Detector.hpp>

#include <iostream>

///
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
//////


namespace bpo = boost::program_options;
namespace ttl = tuttle::host;

namespace bfs = boost::filesystem;
namespace sp  = sequenceParser;

//namespace logging = boost::log;

void displayHelp( bpo::options_description &infoOptions, bpo::options_description &confOptions )
{
	using namespace sam;
	TUTTLE_COUT( std::left << _color._blue << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << _color._std << std::endl );

	TUTTLE_COUT( _color._blue << "NAME" << _color._std );
	TUTTLE_COUT( _color._green << "\tsam do - A command line to execute a list of OpenFX nodes." << _color._std << std::endl );

	TUTTLE_COUT( _color._blue << "SYNOPSIS" << _color._std );
	TUTTLE_COUT( "\tsam do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]" << std::endl );

	TUTTLE_COUT( _color._blue << "DESCRIPTION" << _color._std );
	TUTTLE_COUT( _color._green << "\tA command line to execute a list of OpenFX nodes." << _color._std );
	TUTTLE_COUT( _color._green << "\tUse the sperarator // to pipe images between nodes." << _color._std << std::endl );

	TUTTLE_COUT( _color._blue << "EXAMPLES" << _color._std << std::left );
	SAM_EXAMPLE_TITLE_COUT( "Plugins options" );
	SAM_EXAMPLE_LINE_COUT( "Plugin list: ", "sam do --nodes" );
	SAM_EXAMPLE_LINE_COUT( "Plugin help: ", "sam do blur -h" );

	SAM_EXAMPLE_TITLE_COUT( "Generators and viewers" );
	SAM_EXAMPLE_LINE_COUT( "Viewer: ", "sam do reader in.@.dpx // viewer" );
	SAM_EXAMPLE_LINE_COUT( "Print: ", "sam do reader in.@.dpx // print color=full16ansi" );
	SAM_EXAMPLE_LINE_COUT( "Constant generator: ", "sam do constant                // viewer" );
	SAM_EXAMPLE_LINE_COUT( "White constant generator: ", "sam do constant color=1,1,1,1  // viewer" );
	SAM_EXAMPLE_LINE_COUT( "HD constant generator: ", "sam do constant size=1920,1080 // viewer" );
	SAM_EXAMPLE_LINE_COUT( "Checkerboard generator: ", "sam do checkerboard            // viewer" );
	SAM_EXAMPLE_LINE_COUT( "Checkerboard generator: ", "sam do checkerboard width=500  // viewer" );
	SAM_EXAMPLE_LINE_COUT( "Checkerboard generator: ", "sam do checkerboard width=1920 ratio=2.35 // viewer" );
	/*
	 SAM_EXAMPLE_LINE_COUT ( "Colorgradient generator: "  , "sam do colorgradient point0=1190,424 color0=0.246,0.44,0.254,1 \\" );
	 SAM_EXAMPLE_LINE_COUT ( " "                      , "                     point1=458,726  color1=0.396,0.193,0.444,1 format=HD // viewer" );
	 */
	SAM_EXAMPLE_LINE_COUT( "Text writing: ", "sam do constant // text text=\"hello\" size=80 // viewer" );

	SAM_EXAMPLE_TITLE_COUT( "Image sequence conversion and creation" );
	SAM_EXAMPLE_LINE_COUT( "Convert Image: ", "sam do reader in.dpx // writer out.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Convert Sequence: ", "sam do reader in.####.dpx // writer out.####.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Select a range: ", "sam do reader in.####.dpx // writer out.####.jpg // --range=10,100" );
	SAM_EXAMPLE_LINE_COUT( "", "r and w are shortcuts for reader and writer" );

	SAM_EXAMPLE_TITLE_COUT( "Geometry processing during conversion" );
	SAM_EXAMPLE_LINE_COUT( "Crop: ", "sam do reader in.####.dpx // crop x1=20 x2=1000 y1=10 y2=300 // writer out.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Fill: ", "sam do reader in.####.dpx // crop y1=10 y2=1060 mode=fill color=0.43,0.67,0.50 // writer out.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Resize: ", "sam do reader in.####.dpx // resize size=1920,1080 // writer out.####.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Upscaling: ", "sam do reader in.####.dpx // resize size=1920,1080 filter=lanczos  // writer out.####.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Downscaling: ", "sam do reader in.####.dpx // resize size=720,576   filter=mitchell // writer out.####.jpg" );

	SAM_EXAMPLE_TITLE_COUT( "Color processing during conversion" );
	SAM_EXAMPLE_LINE_COUT( "Lut :", "sam do reader in.####.dpx // lut lutFile.3dl // writer out.jpg" );
	SAM_EXAMPLE_LINE_COUT( "CTL: ", "sam do reader in.####.dpx // ctl file=ctlCode.ctl // writer out.####.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Gamma: ", "sam do reader in.####.dpx // gamma master=2.2 // writer out.####.jpg" );

	SAM_EXAMPLE_TITLE_COUT( "Image Sequence Numbering" );
	SAM_EXAMPLE_LINE_COUT( "Frames with or without padding: ", "image.@.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Frames 1 to 100 padding 4: ", "image.####.jpg -or- image.@.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Frames 1 to 100 padding 5: ", "image.#####.jpg" );
	SAM_EXAMPLE_LINE_COUT( "Printf style padding 4: ", "image.%04d.jpg" );
	SAM_EXAMPLE_LINE_COUT( "All Frames in Directory: ", "/path/to/directory" );

	SAM_EXAMPLE_TITLE_COUT( "Processing options" );
	SAM_EXAMPLE_LINE_COUT( "Range process: ", "sam do reader in.@.dpx // writer out.@.exr // --range 50,100" );
	SAM_EXAMPLE_LINE_COUT( "Single process: ", "sam do reader in.@.dpx // writer out.@.exr // --range 59" );
	SAM_EXAMPLE_LINE_COUT( "Multiple CPUs: ", "sam do reader in.@.dpx // writer out.@.exr // --nb-cores 4" );
	SAM_EXAMPLE_LINE_COUT( "Continues whatever happens: ", "sam do reader in.@.dpx // writer out.@.exr // --continueOnError" );

	TUTTLE_COUT( std::endl << _color._blue << "DISPLAY OPTIONS (replace the process)" << _color._std );
	TUTTLE_COUT( infoOptions );
	TUTTLE_COUT( _color._blue << "CONFIGURE PROCESS" << _color._std );
	TUTTLE_COUT( confOptions );

}

void displayHelp( bpo::options_description &infoOptions, bpo::options_description &confOptions, bpo::options_description &expertOptions )
{
	using namespace sam;
	displayHelp( infoOptions, confOptions );

	TUTTLE_COUT( _color._blue << "EXPERT OPTIONS" << _color._std );
	TUTTLE_COUT( expertOptions );
}

void displayNodeHelp( std::string& nodeFullName, ttl::Graph::Node& currentNode, bpo::options_description &infoOptions, bpo::options_description &confOptions )
{
	using namespace sam;

	TUTTLE_COUT( _color._blue << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << _color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "NODE" << _color._std );
	TUTTLE_COUT( _color._green << "\tsam do " << nodeFullName << " - OpenFX node." << _color._std );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "DESCRIPTION" << _color._std );
	TUTTLE_COUT( _color._green << "\tnode type: " << ttl::mapNodeTypeEnumToString( currentNode.getNodeType() ) << _color._std );
	// internal node help
	if( currentNode.getNodeType() == ttl::INode::eNodeTypeImageEffect )
	{
		if( currentNode.asImageEffectNode().getDescriptor().getProperties().hasProperty( kOfxImageEffectPluginPropGrouping ) )
		{
			TUTTLE_COUT( "\t" << _color._green << currentNode.asImageEffectNode().getPluginGrouping() << _color._std );
		}
	}
	TUTTLE_COUT( "" );
	if( currentNode.getProperties().hasProperty( kOfxPropPluginDescription ) )
	{
		TUTTLE_COUT( currentNode.getProperties().fetchStringProperty( kOfxPropPluginDescription ).getValue( 0 ) );
	}
	else
	{
		TUTTLE_COUT( "\tNo description." );
	}
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "PARAMETERS" << _color._std );
	coutParametersWithDetails( currentNode );
	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "CLIPS" << _color._std );
	coutClipsWithDetails( currentNode );

	TUTTLE_COUT( "" );
	TUTTLE_COUT( _color._blue << "DISPLAY OPTIONS (override the process)" << _color._std );
	TUTTLE_COUT( infoOptions );
	TUTTLE_COUT( _color._blue << "CONFIGURE PROCESS" << _color._std );
	TUTTLE_COUT( confOptions );

	TUTTLE_COUT( "" );
}

void displayNodeHelp( std::string& nodeFullName, ttl::Graph::Node& currentNode, bpo::options_description &infoOptions, bpo::options_description &confOptions, bpo::options_description &expertOptions )
{
	using namespace sam;
	displayNodeHelp( nodeFullName, currentNode, infoOptions, confOptions );

	TUTTLE_COUT( _color._blue << "EXPERT OPTIONS" << _color._std );
	TUTTLE_COUT( expertOptions );
}

int addListOfSequencesInListOfProcess( boost::ptr_vector<sp::FileObject>& inputList, boost::ptr_vector<sp::FileObject>& outputList, const std::vector<std::string>& extensions )
{
	sam::samdo::Dummy dummy;
	std::vector<std::string> exts = dummy.getSupportedExtensions( kOfxImageEffectContextReader );
	int count = 0;
	BOOST_FOREACH( sp::FileObject& fo, inputList )
	{
		bool isSupportedExtension = false;
		std::string filename;
		std::string ext;
		if( fo.getMaskType() == sp::eMaskTypeSequence )
		{
			filename = static_cast<const sp::Sequence&>(fo).getAbsoluteStandardPattern();
		}
		if( fo.getMaskType() == sp::eMaskTypeFile )
		{
			filename = static_cast<const sp::File&>(fo).getAbsoluteFilename();
		}
		ext = bfs::path( filename ).extension().string();
		ext.erase( 0, 1 ); // erase the dot

		BOOST_FOREACH( std::string& e, exts )
		{
			if( e == ext )
			{
				if( extensions.size() )
				{
					BOOST_FOREACH( const std::string& filterExt, extensions )
					{
						if( filterExt == ext )
						{
							isSupportedExtension = true;
						}
					}
				}
				else
				{
					isSupportedExtension = true;
				}
			}
		}
		if( isSupportedExtension )
		{
			count++;
			outputList.push_back( new_clone( fo ) );
		}
	}
	return count;
}

std::string getAbsoluteFilename( const sp::FileObject& fo )
{
	if( fo.getMaskType() == sp::eMaskTypeSequence )
		return static_cast<const sp::Sequence&>(fo).getAbsoluteStandardPattern();
	if( fo.getMaskType() == sp::eMaskTypeFile )
		return static_cast<const sp::File&>(fo).getAbsoluteFilename();
	return "";
}

bool isContextSupported( const ttl::Graph::Node* node , const std::string& context )
{
	const ttl::ofx::property::OfxhProperty& prop = node->getProperties().fetchProperty( kOfxImageEffectPropSupportedContexts );
	std::vector<std::string> contexts = sam::getStringValues( prop );

	BOOST_FOREACH( std::string c, contexts )
	{
		if( c == context )
			return true;
	}
	return false;
}


void init()
{
	boost::shared_ptr< logging::core > core = logging::core::get();
	
	// Create a backend and attach a couple of streams to it
	boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
	backend->add_stream( boost::shared_ptr< std::ostream >(&std::clog ));
	backend->add_stream( boost::shared_ptr< std::ostream >(new std::ofstream("sample.log")));
	
	// Enable auto-flushing after each log record written
	backend->auto_flush(true);
	
	// Wrap it into the frontend and register in the core.
	// The backend requires synchronization in the frontend.
	typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));
	
	sink->set_formatter
			(
				expr::format("[%1%] %2%")
				//% expr::attr< unsigned int >("LineID")
				% logging::trivial::severity
				% expr::smessage
				);
	
	core->add_sink(sink);
	
	/*
	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
	
	sink->locked_backend()->add_stream(
				boost::make_shared< std::ofstream >("sample.log"));
	
	// This makes the sink to write log records that look like this:
	// 1: <normal> A normal severity message
	// 2: <error> An error severity message
	sink->set_formatter
			(
				expr::format("[%1%] %2%")
				//% expr::attr< unsigned int >("LineID")
				% logging::trivial::severity
				% expr::smessage
				);
	
	logging::core::get()->add_sink(sink);*/
}

int main( int argc, char** argv )
{
	using namespace sam;
	using namespace sam::samdo;

	
	init();
	logging::add_common_attributes();
	
	using namespace logging::trivial;
	src::severity_logger< severity_level > lg;
	
	/*
	BOOST_LOG_SEV(lg, trace) << "A trace severity message";
	BOOST_LOG_SEV(lg, debug) << "A debug severity message";
	BOOST_LOG_SEV(lg, info) << "An informational severity message";
	BOOST_LOG_SEV(lg, warning) << "A warning severity message";
	BOOST_LOG_SEV(lg, error) << "An error severity message";
	BOOST_LOG_SEV(lg, fatal) << "A fatal severity message";*/
	/*
	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::info
	);
	*/
	try
	{
		bool continueOnError = false;
		bool stopOnMissingFile = false;
		bool disableProcess = false;
		bool forceIdentityNodesProcess = false;
		bool enableColor = false;
		bool enableVerbose = false;
		bool script = false;
		std::vector<std::string> cl_options;
		std::vector<std::vector<std::string> > cl_commands;

		decomposeCommandLine( argc, argv, cl_options, cl_commands );

		// create the graph
		ttl::Graph graph;
		std::vector<ttl::Graph::Node*> nodes;
		nodes.reserve( 50 );
		
		std::vector<ttl::Graph::Node*> facticesNodes;
		
		ttl::ComputeOptions options;
		std::vector<std::ssize_t> range;
		std::vector<double> renderscale;
		std::size_t step;
		
		std::vector<std::string> idNames; // list of id setted in the command line
		
		// plugins loading
		ttl::core().preload();
		const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& allNodes = ttl::core().getImageEffectPluginCache().getPlugins();

		// Analyze each part of the command line
		{
			// Analyze sam do flags
			try
			{
				// Declare the supported options.
				bpo::options_description infoOptions;
				infoOptions.add_options()
					( kHelpOptionString, kHelpOptionMessage )
					( kVersionOptionString, kVersionOptionMessage )
					( kNodesOptionString, kNodesOptionMessage )
					( kColorOptionString, kColorOptionMessage )
					( kScriptOptionString, kScriptOptionMessage )
					( kBriefOptionString, kBriefOptionMessage )
					( kExpertOptionString, kExpertOptionMessage );
				bpo::options_description confOptions;
				confOptions.add_options()
					( kContinueOnErrorOptionString, bpo::value<bool>(), kContinueOnErrorOptionMessage )
					( kStopOnMissingFileOptionString, bpo::value<bool>(), kStopOnMissingFileOptionMessage )
					( kDisableProcessOptionString, kDisableProcessOptionMessage )
					( kForceIdentityNodesProcessOptionString, kForceIdentityNodesProcessOptionMessage )
					( kRangeOptionString, bpo::value< std::string > (), kRangeOptionMessage )
					( kFirstImageOptionString, bpo::value< int > (), kFirstImageOptionMessage )
					( kLastImageOptionString, bpo::value< int > (), kLastImageOptionMessage )
					( kRenderScaleOptionString, bpo::value<std::string > (), kRenderScaleOptionMessage )
					( kVerboseOptionString, kVerboseOptionMessage )
					( kQuietOptionString, kQuietOptionMessage )
					( kNbCoresOptionString, bpo::value<std::size_t > (), kNbCoresOptionMessage );

				// describe hidden options
				bpo::options_description hidden;
				hidden.add_options()( kEnableColorOptionString, bpo::value<std::string > (), kEnableColorOptionMessage )
					// params for auto-completion
					( kNodesListOptionString, kNodesListOptionMessage );

				bpo::options_description all_options;
				all_options.add( infoOptions ).add( confOptions ).add( hidden );

				bpo::variables_map samdo_vm;
				bpo::store( bpo::command_line_parser( cl_options ).options( all_options ).run(), samdo_vm );
				if( const char* env_do_options = std::getenv( "SAM_DO_OPTIONS" ) )
				{
					const std::vector<std::string> vecOptions = bpo::split_unix( env_do_options, " " );
					bpo::store( bpo::command_line_parser( vecOptions ).options( all_options ).run(), samdo_vm );
				}
				if( const char* env_do_options = std::getenv( "SAM_OPTIONS" ) )
				{
					const std::vector<std::string> vecOptions = bpo::split_unix( env_do_options, " " );
					bpo::store( bpo::command_line_parser( vecOptions ).options( all_options ).run(), samdo_vm );
				}

				bpo::notify( samdo_vm );

				if( samdo_vm.count( kScriptOptionLongName ) )
				{
					// disable color, disable directory printing and set relative path by default
					script = true;
				}
				if( samdo_vm.count( kColorOptionLongName ) && !script )
				{
					enableColor = true;
				}
				if( samdo_vm.count( kEnableColorOptionLongName ) && !script )
				{
					const std::string str = samdo_vm[kEnableColorOptionLongName].as<std::string > ();
					enableColor = string_to_boolean( str );
				}
				if( samdo_vm.count( kDisableProcessOptionLongName ) )
				{
					disableProcess = true;
					enableVerbose = true;
				}
				if( samdo_vm.count( kVerboseOptionLongName ) )
				{
					enableVerbose = true;
				}
				if( samdo_vm.count( kQuietOptionLongName ) )
				{
					enableVerbose = false;
				}
				if( enableColor )
				{
					_color.enable();
				}

				// Display options //
				
				if( samdo_vm.count( kHelpOptionLongName ) )
				{
					displayHelp( infoOptions, confOptions );
					exit( 0 );
				}

				if( samdo_vm.count( kExpertOptionLongName ) )
				{
					displayHelp( infoOptions, confOptions, hidden );
					exit( 0 );
				}

				if( samdo_vm.count( kBriefOptionLongName ) )
				{
					TUTTLE_COUT( _color._green << "A command line to execute a list of OpenFX nodes" << _color._std );
					return 0;
				}

				if( samdo_vm.count( kVersionOptionLongName ) )
				{
					TUTTLE_COUT( "TuttleOFX Host - version " << TUTTLE_HOST_VERSION_STR );
					exit( 0 );
				}

				// Missing operand check //
				
				// If it's not the last display option
				// and if there is no command argument
				// an argument is missing.
				if( !( samdo_vm.count( kNodesOptionLongName ) || samdo_vm.count( kNodesListOptionLongName ) ) &&
				    cl_commands.size() == 0 )
				{
					// No display option and no sub-command to execute
					TUTTLE_COUT( _color._red << "sam do: missing operand." << _color._std << std::endl );
					displayHelp( infoOptions, confOptions );
					exit( -1 );
				}
				
				
				// start to analyse and execute all sub-commands //
				
				const std::string logFilename = ( ttl::core().getPreferences().getTuttleHomePath() / "sam-do.log" ).string();
				std::ofstream logFile( logFilename.c_str() );
				std::streambuf* strm_buffer = std::cerr.rdbuf(); // save cerr's output buffer
				std::cerr.rdbuf( logFile.rdbuf() ); // redirect output into the file
				
				if( samdo_vm.count( kNodesOptionLongName ) || samdo_vm.count( kNodesListOptionLongName ) )
				{
					TUTTLE_COUT( _color._blue << "NODES" << _color._std );
					std::vector<std::string> pluginNames;
					
					Dummy dummy;
					dummy.addDummyNodeInList( pluginNames );

					BOOST_FOREACH( const ttl::ofx::imageEffect::OfxhImageEffectPlugin* node, allNodes )
					{
						std::string plugName = node->getRawIdentifier();
						boost::algorithm::replace_first( plugName, "tuttle.", "" );
						pluginNames.push_back( plugName );
					}
					std::sort( pluginNames.begin(), pluginNames.end() );

					const std::string indent = samdo_vm.count( kNodesOptionLongName ) ? "\t" : "";

					BOOST_FOREACH( const std::string& pluginName, pluginNames )
					{
						TUTTLE_COUT( indent << pluginName );
					}
					exit( 0 );
				}

				{
					if( samdo_vm.count( kRangeOptionLongName ) && ( samdo_vm.count( kFirstImageOptionLongName ) || samdo_vm.count( kLastImageOptionLongName ) ) )
					{
						TUTTLE_COUT( _color._red << "sam do: could not use " << kRangeOptionLongName << "and " << kFirstImageOptionLongName << " or " << kLastImageOptionLongName << " option." << _color._std << std::endl );
						exit( -1 );
					}
					
					if( samdo_vm.count( kFirstImageOptionLongName ) )
					{
						int startFrame = samdo_vm[kFirstImageOptionLongName	].as< int > ();
						options.setBegin( startFrame );
					}
					if( samdo_vm.count( kLastImageOptionLongName ) )
					{
						int stopFrame  = samdo_vm[kLastImageOptionLongName].as< int > ();
						options.setEnd( stopFrame );
					}
					if( samdo_vm.count( kRangeOptionLongName ) )
					{
						const std::string rangeStr = samdo_vm[kRangeOptionLongName].as< std::string > ();
						std::vector<std::string> rangeVStr = boost::program_options::split_unix( rangeStr, " ," );
						range.reserve( rangeVStr.size() );
						TUTTLE_TCOUT( rangeVStr.size() );

						BOOST_FOREACH( const std::string& rStr, rangeVStr )
						{
							range.push_back( tuttle::host::attribute::extractValueFromExpression<std::ssize_t > ( rStr ) );
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
					if( samdo_vm.count( kRenderScaleOptionLongName ) )
					{
						const std::string renderscaleStr = samdo_vm[kRenderScaleOptionLongName].as<std::string > ();
						std::vector<std::string> renderscaleVStr = boost::program_options::split_unix( renderscaleStr, " ," );
						renderscale.reserve( renderscaleVStr.size() );
						TUTTLE_TCOUT( renderscaleVStr.size() );

						BOOST_FOREACH( const std::string& rStr, renderscaleVStr )
						{
							renderscale.push_back( tuttle::host::attribute::extractValueFromExpression<double>( rStr ) );
						}
					}
					if( renderscale.size() == 1 )
					{
						renderscale.push_back( renderscale[0] );
					}
				}
				std::cerr.rdbuf( strm_buffer ); // restore old output buffer
				if( samdo_vm.count( kContinueOnErrorOptionLongName ) )
				{
					continueOnError = samdo_vm[kContinueOnErrorOptionLongName].as< bool > ();
				}
				if( samdo_vm.count( kStopOnMissingFileOptionLongName ) )
				{
					stopOnMissingFile = samdo_vm[kStopOnMissingFileOptionLongName].as< bool > ();
				}

				forceIdentityNodesProcess = samdo_vm.count( kForceIdentityNodesProcessOptionLongName );
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
					( kHelpOptionString, kHelpOptionMessage )
					( kVersionOptionString, kVersionOptionMessage )
					( kExpertOptionString, kExpertOptionMessage );
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

				// define default options
				bpo::positional_options_description param_values;
				param_values.add( kParamValuesOptionLongName, -1 );

				bpo::options_description all_options;
				all_options.add( infoOptions ).add( confOptions ).add( openfxOptions );

				BOOST_FOREACH( const std::vector<std::string>& command, cl_commands )
				{

					std::string userNodeName = command[0];
					boost::algorithm::to_lower( userNodeName );
					std::string nodeFullName = userNodeName;
					std::vector<std::string> nodeArgs;
					std::copy( command.begin() + 1, command.end(), std::back_inserter( nodeArgs ) );

					try
					{
						Dummy dummy;
						if( dummy.isDummyNode( userNodeName ) )
						{
							nodeFullName = "tuttle.dummy";
						}
						nodeFullName = retrieveNodeFullname( nodeFullName );

						// parse the command line, and put the result in node_vm
						bpo::variables_map node_vm;
						bpo::store( bpo::command_line_parser( nodeArgs ).options( all_options ).positional( param_values ).run(), node_vm );
						if( const char* env_ptr = std::getenv( "SAM_DO_NODE_OPTIONS" ) )
						{
							std::vector<std::string> envOptions;
							std::string env( env_ptr );
							envOptions.push_back( env );
							bpo::store( bpo::command_line_parser( envOptions ).options( all_options ).run(), node_vm );
						}
						{
							std::string envVarName;
							envVarName += "SAM_DO_";
							envVarName += boost::algorithm::replace_all_copy( nodeFullName, ".", "_" );
							envVarName += "_OPTIONS";
							if( const char* env_ptr = std::getenv( envVarName.c_str() ) )
							{
								std::vector<std::string> envOptions;
								std::string env( env_ptr );
								envOptions.push_back( env );
								bpo::store( bpo::command_line_parser( envOptions ).options( all_options ).run(), node_vm );
							}
						}

						bpo::notify( node_vm );
						//TUTTLE_COUT( "[" << nodeFullName << "]" );

						// Check priority flags:
						// If one flag to display informations is used in command line,
						// it replaces all the process.
						// --help,h --version,v --verbose,V --params --clips --props

						ttl::Graph::Node& currentNode = graph.createNode( nodeFullName );
						//if( dummy.isDummyNode( userNodeName ) )

						if( node_vm.count( kHelpOptionLongName ) )
						{
							if( dummy.isDummyNode( userNodeName ) )
								dummy.displayHelp( userNodeName, _color );
							else
								displayNodeHelp( nodeFullName, currentNode, infoOptions, confOptions );
							exit( 0 );
						}
						if( node_vm.count( kExpertOptionString ) )
						{
							if( dummy.isDummyNode( userNodeName ) )
								dummy.displayExpertHelp( userNodeName, _color );
							else
								displayNodeHelp( nodeFullName, currentNode, infoOptions, confOptions, openfxOptions );
							exit( 0 );
						}

						if( node_vm.count( kVersionOptionLongName ) )
						{
							TUTTLE_COUT( "\tsam do " << nodeFullName );
							TUTTLE_COUT( "Version " << currentNode.getVersionStr() );
							TUTTLE_COUT( "" );
							exit( 0 );
						}
						if( node_vm.count( kAttributesOptionLongName ) )
						{
							TUTTLE_COUT( "\tsam do " << nodeFullName );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( _color._blue << "ATTRIBUTES" << _color._std );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( _color._blue << "- CLIPS" << _color._std );
							coutClipsWithDetails( currentNode );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( _color._blue << "- PARAMETERS" << _color._std );
							coutParametersWithDetails( currentNode );
							TUTTLE_COUT( "" );
							exit( 0 );
						}
						if( node_vm.count( kPropertiesOptionLongName ) )
						{
							if( !script )
							{
								TUTTLE_COUT( "\tsam do " << nodeFullName );
								TUTTLE_COUT( "" );
								TUTTLE_COUT( _color._blue << "PROPERTIES" << _color._std );
							}
							coutProperties( currentNode );
							if( !script )
								TUTTLE_COUT( "" );
							exit( 0 );
						}
						if( node_vm.count( kClipsOptionLongName ) )
						{
							if( !script )
							{
								TUTTLE_COUT( "\tsam do " << nodeFullName );
								TUTTLE_COUT( "" );
								TUTTLE_COUT( _color._blue << "CLIPS" << _color._std );
							}
							coutClips( currentNode );
							if( !script )
								TUTTLE_COUT( "" );
							exit( 0 );
						}
						if( node_vm.count( kClipOptionLongName ) )
						{
							const std::string clipName = node_vm["clip"].as<std::string>();
							ttl::attribute::ClipImage& clip = currentNode.getClip(clipName);
							
							//const String& prop = fetchStringProperty( kOfxImageEffectPropSupportedPixelDepths );
							//std::vector<std::string>& bitDepths     = prop.getValues();
							
							/*std::vector<std::string> bitDepths = clip.getSupportedBitDepth();
							BOOST_FOREACH( std::string& s, bitDepths )
							{
								s = s.substr( 11 ); // remove 'OfxBitDepth'
							}*/
							
							std::vector<std::string> components = clip.getSupportedComponents();
							BOOST_FOREACH( std::string& s, components )
							{
								s = s.substr( 17 ); // remove 'OfxImageComponent'
							}
							
							if( !script )
							{
								TUTTLE_COUT( "\tsam do " << nodeFullName );
								TUTTLE_COUT( "" );

								TUTTLE_COUT( _color._blue << "CLIP: " << _color._green << clipName << _color._std );
								//TUTTLE_COUT( "supported bit depth: " << boost::algorithm::join( bitDepths, ", " ) );
								TUTTLE_COUT( "supported components: " << boost::algorithm::join( components, ", " ) );
								TUTTLE_COUT( "pixel aspect ratio: " << clip.getPixelAspectRatio() );
								TUTTLE_COUT( "number of components: " << clip.getNbComponents() );
								TUTTLE_COUT( "" );
							}
							else
							{
								//TUTTLE_COUT( boost::algorithm::join( bitDepths, ", " ));
								TUTTLE_COUT( boost::algorithm::join( components, ", " ) );
								TUTTLE_COUT( clip.getPixelAspectRatio() );
								TUTTLE_COUT( clip.getNbComponents());
							}
							
							exit( 0 );
						}
						if( node_vm.count( kParametersOptionLongName ) )
						{
							if( !script )
							{
								TUTTLE_COUT( "\tsam do " << nodeFullName );
								TUTTLE_COUT( "" );
								TUTTLE_COUT( _color._blue << "PARAMETERS" << _color._std );
								TUTTLE_COUT( "" );
								coutParametersWithDetails( currentNode );
								exit( 0 );
							}
							else
							{
								coutParameters( currentNode );
								exit( 0 );
							}
						}
						if( node_vm.count( kParametersReduxOptionLongName ) )
						{
							coutParameters( currentNode );
							exit( 0 );
						}
						if( node_vm.count( kParamInfosOptionLongName ) )
						{
							const std::string attributeName = node_vm[kParamInfosOptionLongName].as<std::string > ();
							TUTTLE_COUT( "\tsam do " << nodeFullName );
							TUTTLE_COUT( _color._blue << "PARAM: " << _color._green << attributeName << _color._std );
							ttl::ofx::attribute::OfxhParam& param = currentNode.getParamByScriptName( attributeName );
							TUTTLE_COUT( "" );
							TUTTLE_COUT( "\t" << _color._red << ( param.getSecret() ? "SECRET -- " : "" ) << param.getScriptName() << ": " << param.getParamTypeName() << " x" << param.getSize() << _color._std );
							TUTTLE_COUT( "" );
							const std::string& hint = param.getHint();
							if( hint.size() )
							{
								TUTTLE_COUT( "\t" << hint );
							}
							TUTTLE_COUT( "" );
							exit( 0 );
						}

						if( node_vm.count( kParamTypeOptionLongName ) )
						{
							const std::string attributeName = node_vm[kParamTypeOptionLongName].as<std::string > ();
							ttl::ofx::attribute::OfxhParam& param = currentNode.getParamByScriptName( attributeName );
							TUTTLE_COUT( param.getParamTypeName() );
							exit( 0 );
						}

						if( node_vm.count( kParamPossibleValuesOptionLongName ) )
						{
							const std::string attributeName = node_vm[kParamPossibleValuesOptionLongName].as<std::string > ();
							ttl::ofx::attribute::OfxhParam& param = currentNode.getParamByScriptName( attributeName );
							coutParameterValues( std::cout, param );
							exit( 0 );
						}
						if( node_vm.count( kParamDefaultOptionLongName ) )
						{
							const std::string attributeName = node_vm[kParamDefaultOptionLongName].as<std::string > ();
							ttl::ofx::attribute::OfxhParam& param = currentNode.getParamByScriptName( attributeName );
							TUTTLE_COUT( getFormattedStringValue( param.getProperties().fetchProperty( kOfxParamPropDefault ) ) );
							exit( 0 );
						}
						if( node_vm.count( kParamGroupOptionLongName ) )
						{
							if( currentNode.getNodeType() == ttl::INode::eNodeTypeImageEffect )
							{
								if( currentNode.asImageEffectNode().getDescriptor().getProperties().hasProperty( kOfxImageEffectPluginPropGrouping ) )
								{
									TUTTLE_COUT( currentNode.asImageEffectNode().getPluginGrouping() );
								}
							}
							exit( 0 );
						}

						if( node_vm.count( kIdOptionLongName ) )
						{
							const std::string nodeId = node_vm[kIdOptionLongName].as<std::string > ();
							idNames.push_back( nodeId );
							graph.renameNode( currentNode, nodeId );
						}
						
						// Analyse attributes: parameters / clips
						typedef std::pair<ttl::ofx::attribute::OfxhClipImage*, std::string> ClipAndConnection;
						std::vector<ClipAndConnection> clipsToConnect;

						if( node_vm.count( kParamValuesOptionLongName ) )
						{
							const std::vector<std::string> params = node_vm[kParamValuesOptionLongName].as<std::vector<std::string> >();
							bool orderedParams = true;
							setParametersForNode( params, currentNode, clipsToConnect, dummy.isDummyNode( userNodeName ), orderedParams );
						}
						
						// if it's a dummy, keeping parameters in node
						if( dummy.isDummyNode( userNodeName ) )
						{
							currentNode.getParamByScriptName( "originalnode" ).setValue( command.at(0) );
							currentNode.getParamByScriptName( "expression" ).setValue( boost::algorithm::join( nodeArgs, " " ) );
							facticesNodes.push_back( &currentNode );
						}

						// connect current node to previous node(s)
						connectClips( nodes, clipsToConnect, currentNode, graph, idNames );
						
						nodes.push_back( &currentNode );
					}
					catch( boost::program_options::error& e )
					{
						TUTTLE_CERR( _color._red << "[sam do] " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
						TUTTLE_CERR( "Error: " << e.what() << _color._std );
#else
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() << _color._std );
#endif
						exit( -2 );
					}
					catch( tuttle::exception::Common& e )
					{
						TUTTLE_CERR( _color._red << "[sam do] " << nodeFullName );
#ifdef TUTTLE_PRODUCTION
						TUTTLE_CERR( "Error: " << *boost::get_error_info<tuttle::exception::user > ( e ) << _color._std );
#else
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() );
						TUTTLE_CERR( "Backtrace: " << boost::trace( e ) << _color._std );
#endif
						exit( -2 );
					}
					catch( ... )
					{
						TUTTLE_CERR( _color._red << "[sam do] " << nodeFullName );
						TUTTLE_CERR( "Unknown error." );
						TUTTLE_CERR( "\n" );
						TUTTLE_CERR( "Debug: " << boost::current_exception_diagnostic_information() << _color._std );
						exit( -2 );
					}
				}
			}
		}

		if( enableVerbose )
		{
			TUTTLE_COUT( _color._blue << "********** graph composition **********" << _color._std );
			BOOST_FOREACH( const std::vector<std::string>& node, cl_commands )
			{
				TUTTLE_COUT( "[" << node[0] << "]" );
				for( std::size_t i = 1; i < node.size(); ++i )
				{
					const std::string& s = node[i];
					TUTTLE_COUT( ( ( s[0] == '-' ) ? "" : "* " ) << s );
				}
			}
			TUTTLE_COUT( _color._blue << "***************************************" << _color._std );
		}
		
		if( nodes.size() == 0 )
			// nothing to do!
			exit( -1 );

		// Setup compute options
		if( range.size() >= 2 )
		{
			options.addTimeRange( range[0], range[1], step );
		}
		
		if( renderscale.size() == 2 )
		{
			options.setRenderScale( renderscale[0], renderscale[1] );
		}
		options.setContinueOnError( continueOnError );
		options.setContinueOnMissingFile( !stopOnMissingFile );
		options.setForceIdentityNodesProcess( forceIdentityNodesProcess );
		
		size_t numberOfLoop = std::numeric_limits<size_t>::max();
		boost::ptr_vector< boost::ptr_vector< sp::FileObject > > listOfSequencesPerReaderNode;
		std::vector< std::string > listOfSequencesPerWriterNode;
		bool writerHaveExtensionInParameter = false;
		
		if( facticesNodes.size() )
		{
			// create a detector from the sequence parsing library
			sp::Detector detector;
			sp::EMaskType    researchMask    = sp::eMaskTypeSequence | sp::eMaskTypeFile ;
			sp::EMaskOptions descriptionMask = sp::eMaskOptionsAbsolutePath | sp::eMaskOptionsRecursive;
			
			std::vector<std::string> filters;
			
			Dummy dummy;
			
			BOOST_FOREACH( ttl::Graph::Node* n, facticesNodes )
			{
				std::string name = n->getParamByScriptName( "originalnode" ).getStringValue();
				dummy.getFullName( name );
				
				// get base paths from dummy parameters
				std::string command = n->getParam("expression").getStringValue();
				std::vector<std::string> paths;
				const std::vector<std::string> nodeArgs = bpo::split_unix( command, " " );
				dummy.getPathsFromCommandLine( paths, nodeArgs );
				
				if( dummy.isDummyReaderNode( name ) )
				{
					std::vector<std::string> extensions;
					dummy.getExtensionsFromCommandLine( extensions, nodeArgs );
					
					boost::ptr_vector<sp::FileObject> listOfSequencesForThisNode;
					
					// browse all paths in parameters
					BOOST_FOREACH( std::string& inputPath, paths )
					{
						size_t count = 0;
						if( enableVerbose )
							TUTTLE_COUT( "[sam-do] " <<n->getName() << " browse: " << inputPath );
					
						// get filenames adn sequences
						boost::ptr_vector<sp::FileObject> listOfSequences = detector.fileObjectInDirectory( inputPath, filters, researchMask, descriptionMask );
						count += addListOfSequencesInListOfProcess( listOfSequences, listOfSequencesForThisNode, extensions );
					
						bfs::path p( inputPath );
						if( ! p.extension().string().size() )
						{
							for ( bfs::recursive_directory_iterator end, dir( inputPath ); dir != end; ++dir )
							{
								if( bfs::is_directory( *dir ) )
								{
									bfs::path currentPath = (bfs::path)*dir;
									boost::ptr_vector<sp::FileObject> listOfSequences = detector.fileObjectInDirectory( currentPath.string(), filters, researchMask, descriptionMask );
									count += addListOfSequencesInListOfProcess( listOfSequences, listOfSequencesForThisNode, extensions );
								}
							}
						}
					}
					// found the minimum number of sequence in each dummy reader
					numberOfLoop = std::min( numberOfLoop, listOfSequencesForThisNode.size() );
					
					listOfSequencesPerReaderNode.push_back( new_clone( listOfSequencesForThisNode ) );
				}
				if( dummy.isDummyWriterNode( name ) )
				{
					// in case of dummy writer, only add the first path, else throw an exception
					if( paths.size() > 1 )
						BOOST_THROW_EXCEPTION( tuttle::exception::Value()
											   << tuttle::exception::user() + "unable to set mutli path in writer." );
					
					if( paths.size() )
					{
						listOfSequencesPerWriterNode.push_back( paths.at(0) ); // push only the first path
						bfs::path p( paths.at(0) );
						if( p.extension().string().size() )
							writerHaveExtensionInParameter = true;
					}
					else
						listOfSequencesPerWriterNode.push_back( "" );
				}
			}
			
			
		}
		
		// if graph not have dummy reader
		// or no dummy node, process only 1 graph
		if( ! listOfSequencesPerReaderNode.size() ||
			! facticesNodes.size() ||
			( ( listOfSequencesPerReaderNode.size() || listOfSequencesPerWriterNode.size() ) && !numberOfLoop ) )
		{
			numberOfLoop = 1;
		}
		
		// check if the graph haave real readers or writers
		// and get filenames list
		std::vector<std::string> filenames;
		bool haveReader = false;
		bool haveWriter = false;
		BOOST_FOREACH( ttl::Graph::Node* n, nodes )
		{
			if( isContextSupported( n, kOfxImageEffectContextReader ) )
			{
				filenames.push_back( n->getParamByScriptName( "filename" ).getStringValue() );
				haveReader = true;
			}
			if( isContextSupported( n, kOfxImageEffectContextWriter ) )
			{
				haveWriter = true;
			}
		}
		
		TUTTLE_TCOUT( "[sam-do] number of loop: " << numberOfLoop );
		TUTTLE_TCOUT( "[sam-do] writer have extension: " << writerHaveExtensionInParameter );
		TUTTLE_TCOUT( "[sam-do] graph have reader : " << haveReader );
		TUTTLE_TCOUT( "[sam-do] graph have writer : " << haveWriter );
		TUTTLE_TCOUT( "[sam-do] number of sequence per reader : " << listOfSequencesPerReaderNode.size() );
		TUTTLE_TCOUT( "[sam-do] number of sequence per writer : " << listOfSequencesPerWriterNode.size() );
		
		if( ( writerHaveExtensionInParameter || haveWriter || haveReader ) && numberOfLoop > 1 )
		{
			BOOST_THROW_EXCEPTION( tuttle::exception::Value()
								   << tuttle::exception::user() + "several sequences can’t be transformed into a single sequence or a movie for the moment." );
		}
		
		// Execute the graph

		Dummy dummy;
		
		for( size_t loop = 0; loop < numberOfLoop; ++loop )
		{
			if( enableVerbose )
				TUTTLE_COUT( "[sam-do] " << _color._red << "graph processing " << loop << _color._std );
			
			// copy graph and node for each iteration
			ttl::Graph graphTmp( graph );
			std::vector<ttl::Graph::Node*> nodesTmp = nodes;
			
			bool processGraph = true;
			size_t countDummyReader = 0;
			size_t countDummyWriter = 0;
			
			
			// replace each dummy node per real reader or writer node on graph
			for( size_t nNode = 0; nNode < facticesNodes.size(); ++nNode )
			{
				ttl::Graph::Node* readerToReplace = facticesNodes.at( nNode );
			
				std::string filename;
			
				std::string name = readerToReplace->getParamByScriptName( "originalnode" ).getStringValue();
				dummy.getFullName( name );
			
				// get sequence name (or filename)
				if( dummy.isDummyReaderNode( name ) )
				{
					std::string baseSrcPath = readerToReplace->getParam("expression").getStringValue();
					bfs::path refPath( baseSrcPath );
					
					if( refPath.extension().string().size() )
					{
						// if writer have an extension in the parameter, keep this filename
						filename = baseSrcPath;
					}
					else
					{
						sp::FileObject& fo = listOfSequencesPerReaderNode.at( countDummyReader ).at( loop );
						filename = getAbsoluteFilename( fo );
						countDummyReader++;
					}
					TUTTLE_TCOUT_VAR( filename );
				}
				else // writer dummy
				{
					std::string baseDstPath = readerToReplace->getParam("expression").getStringValue();
					bfs::path refPath( baseDstPath );
					
					if( refPath.extension().string().size() )
					{
						// if writer have an extension in the parameter, keep this filename
						filename = baseDstPath;
					}
					else
					{
						// need to combine filename:
						// get root from dummy writer, and add sequence pattern from reader
						
						std::string baseSrcPath;
						if( ! listOfSequencesPerReaderNode.size() )
						{
							if( ! filenames.size() )
								BOOST_THROW_EXCEPTION( tuttle::exception::Value()
													   << tuttle::exception::user() + "no pattern was found to set the output filename." );
							baseSrcPath = filenames.at(0);
						}
						else
						{
							baseSrcPath = facticesNodes.at(0)->getParam("expression").getStringValue();
						}
						std::vector<std::string> srcPaths;
						const std::vector<std::string> srcNodeArgs = bpo::split_unix( baseSrcPath, " " );
						dummy.getPathsFromCommandLine( srcPaths, srcNodeArgs );
						
						std::vector<std::string> extensions;
						const std::vector<std::string> dstNodeArgs = bpo::split_unix( baseDstPath, " " );
						
						dummy.getExtensionsFromCommandLine( extensions, dstNodeArgs );
						if( extensions.size() > 1 )
							BOOST_THROW_EXCEPTION( tuttle::exception::Value()
												   << tuttle::exception::user() + "many ext parameter are specified." );
						
						if( ! listOfSequencesPerReaderNode.size() )
						{
							filename = bfs::path(baseSrcPath).filename().string();
						}
						else
						{
							if( listOfSequencesPerReaderNode.size() && numberOfLoop == 1 )
							{
								filename = readerToReplace->getParamByScriptName( "expression" ).getStringValue();
							}
							else
							{
								sp::FileObject& fo = listOfSequencesPerReaderNode.at( 0 ).at( loop );
								filename = getAbsoluteFilename( fo );
							}
							
							bfs::path filepath( baseSrcPath );
							if( filepath.extension().string().size() )
							{
								filename = filepath.filename().string();
							}
							else
							{
								filename.erase( 0, srcPaths.at(0).length() );
							}
						}
						TUTTLE_TCOUT_VAR3( baseSrcPath, filename, srcPaths.at(0) );
						// change extension if necessary
						std::string ext;
						if( extensions.size() > 0 )
						{
							bool isSupportedExtension = false;
							ext = extensions.at(0);
							std::vector<std::string> exts = dummy.getSupportedExtensions( kOfxImageEffectContextWriter );
							
							BOOST_FOREACH( std::string& e, exts )
							{
								if( e == ext )
								{
									isSupportedExtension = true;
								}
							}
							if( !isSupportedExtension )
								BOOST_THROW_EXCEPTION( tuttle::exception::Value()
													   << tuttle::exception::user() + "unknown extension." );
						}
						
						bfs::path path = listOfSequencesPerWriterNode.at( countDummyWriter );
						path /= filename;
						
						if( ext.length() )
							path.replace_extension( ext );
						
						filename = path.string();
					}
					
					countDummyWriter++;
				}
				
				
				// replace dummy node with the correct reader or writer node
				std::vector<std::string> args = bpo::split_unix( filename, " " );
				dummy.foundAssociateDummyNode( name, allNodes, args, _color );

				if( enableVerbose )
					TUTTLE_COUT( readerToReplace->getName() << " => " << name << " : " << args.at(0) );
				
				ttl::Graph::Node& currentNode = graphTmp.createNode( name );
				
				graphTmp.replaceNodeConnections( *readerToReplace, currentNode );
				
				// Analyse attributes: parameters / clips
				typedef std::pair<ttl::ofx::attribute::OfxhClipImage*, std::string> ClipAndConnection;
				std::vector<ClipAndConnection> clipsToConnect;

				bool orderedParams = true;
				setParametersForNode( args, currentNode, clipsToConnect, false, orderedParams );
				
				if( isContextSupported( &currentNode, kOfxImageEffectContextReader ) )
				{
					nodesTmp.insert( nodesTmp.begin(), &currentNode );
				}
				else
					nodesTmp.push_back( &currentNode );
			}
			if( processGraph )
			{
				if( enableVerbose )
					TUTTLE_COUT( "[sam-do] graph processing" );
				if( !disableProcess )
					graphTmp.compute( *nodesTmp.back(), options );
			}
		}
		
		
	}
	catch( const tuttle::exception::Common& e )
	{
#ifdef TUTTLE_PRODUCTION
		TUTTLE_CERR( "[sam-do] Error: " << *boost::get_error_info<tuttle::exception::user > ( e ) << _color._std );
#else
		TUTTLE_CERR( "[sam-do] Debug: " << boost::current_exception_diagnostic_information() );
		TUTTLE_CERR( "[sam-do] Backtrace: " << boost::trace( e ) << _color._std );
#endif
		exit( -2 );
	}
	catch( const boost::program_options::error& e )
	{
		TUTTLE_CERR( "[sam-do] Error: " << e.what() << _color._std );
		exit( -2 );
	}
	catch( ... )
	{
		TUTTLE_CERR( _color._red << "[sam-do] Error" << _color._std );
#ifndef TUTTLE_PRODUCTION
		TUTTLE_CERR( _color._red << boost::current_exception_diagnostic_information() << _color._std );
#endif
		exit( -2 );
	}
	return 0;
}

