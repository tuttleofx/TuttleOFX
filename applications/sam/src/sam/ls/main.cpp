#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <detector.hpp>

#include <algorithm>
#include <iterator>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;


namespace sam
{
	bool wasSthgDumped = false;
}

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
	return os;
}

template<class T>
void coutVec( const boost::ptr_vector<T>& v )
{
	BOOST_FOREACH( const T& f, v )
	{
		TUTTLE_COUT( f );
		sam::wasSthgDumped = true;
	}
}


int main( int argc, char** argv )
{
	signal(SIGINT, signal_callback_handler);

	using namespace tuttle::common;
	using namespace sam;

	formatters::Formatter::get();
	boost::shared_ptr<Color> color( Color::get() );
	
	sequenceParser::EType filterByType = sequenceParser::eTypeFolder | sequenceParser::eTypeFile | sequenceParser::eTypeSequence; // by default show directories, files and sequences
	sequenceParser::EDetection detectionOptions = (sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles | sequenceParser::eDetectionIgnoreDotFile | sequenceParser::eDetectionSequenceFromFilename);
	sequenceParser::EDisplay displayOptions = sequenceParser::eDisplayNone;
	bool recursiveListing = false;
	bool script = false;
	std::string availableExtensions;
	std::vector<std::string> paths;
	std::vector<std::string> filters;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		(kAllOptionString            , kAllOptionMessage)
		(kDirectoriesOptionString    , kDirectoriesOptionMessage)
		(kExpressionOptionString     , bpo::value<std::string>(), kExpressionOptionMessage)
		(kFilesOptionString          , kFilesOptionMessage )
		(kHelpOptionString           , kHelpOptionMessage)
		(kLongListingOptionString   , kLongListingOptionMessage)
		(kRelativePathOptionString  , kRelativePathOptionMessage)
		(kRecursiveOptionString      , kRecursiveOptionMessage)
		(kPathOptionString    , kPathOptionMessage)
		(kSequencesOptionString           , kSequencesOptionMessage)
		(kColorOptionString            , kColorOptionMessage)
		(kFullDisplayOptionString     , kFullDisplayOptionMessage )
		(kScriptOptionString           , kScriptOptionMessage)
		(kBriefOptionString            , kBriefOptionMessage)
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		(kInputDirOptionString, bpo::value< std::vector<std::string> >(), kInputDirOptionMessage)
		(kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage)
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add(kInputDirOptionLongName, -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);
	
	bpo::variables_map vm;

	try
	{
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

		// get environment options and parse them
		if( const char* env_ls_options = std::getenv("SAM_LS_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_ls_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		if( const char* env_ls_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_ls_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_LOG_ERROR("sam-ls: command line error: " << e.what() );
		exit( 254 );
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR("sam-ls: unknown error in command line.");
		exit( 254 );
	}

	if ( vm.count(kScriptOptionLongName) )
	{
		// disable color, disable directory printing and set relative path by default
		script = true;
		displayOptions |= sequenceParser::eDisplayAbsolutePath;
	}

	if ( vm.count(kColorOptionLongName) && !script )
	{
		color->enable();
		displayOptions |= sequenceParser::eDisplayColor;
	}
	if ( vm.count(kEnableColorOptionLongName) && !script )
	{
		const std::string str = vm[kEnableColorOptionLongName].as<std::string>();
		if( string_to_boolean( str ) )
		{
			color->enable();
			displayOptions |= sequenceParser::eDisplayColor;
		}
		else
		{
			color->disable();
			displayOptions &= ~sequenceParser::eDisplayColor;
		}
	}
	
	if (vm.count(kHelpOptionLongName))
	{
		TUTTLE_COUT( color->_blue  << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color->_std );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( color->_blue  << "NAME" << color->_std );
		TUTTLE_COUT( color->_green << "\tsam-ls - list directory contents" << color->_std );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( color->_blue  << "SYNOPSIS" << color->_std );
		TUTTLE_COUT( color->_green << "\tsam-ls [options] [directories]" << color->_std );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( color->_blue  << "DESCRIPTION" << color->_std );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( "List information about the sequences, files and folders." );
		TUTTLE_COUT( "List the current directory by default, and only sequences." );
		TUTTLE_COUT( "The script option disable color, disable directory printing (in multi-directory case or recursive) and set relative path by default." );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( color->_blue  << "OPTIONS" << color->_std );
		TUTTLE_COUT( "" );
		TUTTLE_COUT( mainOptions );
		return 0;
	}

	if ( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_COUT( color->_green << "list directory contents" << color->_std );
		return 0;
	}

	if (vm.count(kExpressionOptionLongName))
	{
		TUTTLE_LOG_WARNING( "Expression: " << vm["expression"].as<std::string>() );
		bal::split( filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}

	if( vm.count(kDirectoriesOptionLongName ) || vm.count(kFilesOptionLongName) || vm.count(kSequencesOptionLongName) )
	{
		filterByType &= ~sequenceParser::eTypeFolder;
		filterByType &= ~sequenceParser::eTypeFile;
		filterByType &= ~sequenceParser::eTypeSequence;
	}
		
	if( vm.count(kDirectoriesOptionLongName ) )
	{
		filterByType |= sequenceParser::eTypeFolder;
	}
	if (vm.count(kFilesOptionLongName))
	{
		filterByType |= sequenceParser::eTypeFile;
	}
	if (vm.count(kSequencesOptionLongName))
	{
		filterByType |= sequenceParser::eTypeSequence;
	}
	
	if (vm.count(kFullDisplayOptionLongName))
	{
		filterByType |= sequenceParser::eTypeFolder;
		filterByType |= sequenceParser::eTypeFile;
		filterByType |= sequenceParser::eTypeSequence;
	}
	
	if (vm.count(kAllOptionLongName))
	{
		// add .* files
		detectionOptions &= ~sequenceParser::eDetectionIgnoreDotFile;
	}
	
	if (vm.count(kLongListingOptionLongName))
	{
		displayOptions |= sequenceParser::eDisplayProperties;
	}
	
	if (vm.count(kRelativePathOptionLongName) )
	{
		displayOptions |= sequenceParser::eDisplayPath;
	}

	if(vm.count(kPathOptionLongName))
	{
		displayOptions |= sequenceParser::eDisplayAbsolutePath;
	}
	
	// defines paths, but if no directory specify in command line, we add the current path
	if (vm.count(kInputDirOptionLongName))
	{
		paths = vm[kInputDirOptionLongName].as< std::vector<std::string> >();
	}
	else
	{
		paths.push_back( "./" );
	}
	
	if (vm.count(kRecursiveOptionLongName))
	{
		recursiveListing = true;
	}


// 	for(uint i=0; i<filters.size(); i++)
// 	TUTTLE_LOG_TRACE( "filters = " << filters.at(i) );
// 	TUTTLE_LOG_TRACE( "research mask = " << researchMask );
// 	TUTTLE_LOG_TRACE( "options  mask = " << descriptionMask );

	std::list<boost::shared_ptr<sequenceParser::FileObject> > listing;
	try
	{
		std::size_t index = 0;
		BOOST_FOREACH( bfs::path path, paths )
		{
			if( path == bfs::path(".") )
			{
				path = bfs::path("./");
			}
			if( ( paths.size() > 1 || recursiveListing ) && !script )
			{
				if( index > 0 )
				{
					TUTTLE_COUT( "" );
				}
				TUTTLE_COUT( path.string() << ":");
				wasSthgDumped = true;
			}

			coutVec( sequenceParser::fileObjectInDirectory( path.string(), filters, filterByType, detectionOptions, displayOptions ) );

			if(recursiveListing)
			{
				for ( bfs::recursive_directory_iterator end, dir( path ); dir != end; ++dir )
				{
					if( bfs::is_directory( *dir ) )
					{
						bfs::path currentPath = (bfs::path)*dir;
						if( !script )
							TUTTLE_COUT( "\n" << currentPath.string() << ":" );

						coutVec( sequenceParser::fileObjectInDirectory( currentPath.string(), filters, filterByType, detectionOptions, displayOptions ) );

					}
				}
			}
			++index;
		}
	}
	catch ( const bfs::filesystem_error& ex)
	{
		TUTTLE_LOG_ERROR( ex.what() );
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information() );
	}
	
	if(!wasSthgDumped)
	{
		TUTTLE_LOG_ERROR( "No sequence found here." );
	}
	
	return 0;
}
