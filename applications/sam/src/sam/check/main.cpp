#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <tuttle/host/Graph.hpp>

#include <Detector.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace tuttle::host;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace sp  = sequenceParser;

static int _blackImage     = 0;
static int _nullFileSize   = 0;
static int _corruptedImage = 0;
static int _missingFiles   = 0;
enum EReturnCode
{
	eReturnCodeOK = 0,
	eReturnCodeErrorInImages = 1,
	eReturnCodeApplicationError = 2
};

enum EImageStatus
{
	eImageStatusOK,
	eImageStatusBlack,
	eImageStatusFileSizeError,
	eImageStatusNoFile,
	eImageStatusImageError
};

/**
 * @brief Check the image status.
 */
EImageStatus checkImageStatus( Graph::Node& read, Graph::Node& stat, Graph& graph, const bfs::path& filename )
{
	if( bfs::exists( filename ) == 0 )
		return eImageStatusNoFile;

	if( bfs::file_size( filename ) == 0 )
		return eImageStatusFileSizeError;

	try
	{
		// Setup parameters
		read.getParam( "filename" ).setValue( filename.string() );
		graph.compute( stat );

		for( unsigned int i = 0; i<4; ++i )
		{
			if( stat.getParam( "outputChannelMax" ).getDoubleValueAtIndex(i) != 0 )
				return eImageStatusOK;
			if( stat.getParam( "outputChannelMin" ).getDoubleValueAtIndex(i) != 0 )
				return eImageStatusOK;
		}
		TUTTLE_LOG_INFO( "stat:" << stat );
		return eImageStatusBlack;
	}
	catch( ... )
	{
		return eImageStatusImageError;
	}
}

EImageStatus checkFile( Graph::Node& read, Graph::Node& stat, Graph& graph, const bfs::path& filename )
{
	EImageStatus s = checkImageStatus( read, stat, graph, filename );

	std::string message = "";
	switch( s )
	{
		case eImageStatusOK:
			break;
		case eImageStatusBlack:
			message = "Black image: ";
			++_blackImage;
			break;
		case eImageStatusFileSizeError:
			message = "Null file size: ";
			++_nullFileSize;
			break;
		case eImageStatusNoFile:
			message = "Missing file: ";
			++_missingFiles;
			break;
		case eImageStatusImageError:
			message = "Corrupted image: ";
			++_corruptedImage;
			break;
	}
	TUTTLE_LOG_INFO( message << filename );
	return s;
}

void checkSequence( Graph::Node& read, Graph::Node& stat, Graph& graph, const sp::Sequence& seq )
{
	for( sp::Time t = seq.getFirstTime(); t <= seq.getLastTime(); ++t )
	{
		checkFile( read, stat, graph, seq.getAbsoluteFilenameAt(t) );
	}
}

void checkSequence( Graph::Node& read, Graph::Node& stat, Graph& graph, const sp::Sequence& seq, const sp::Time first, const sp::Time last )
{
	for( sp::Time t = first; t <= last; ++t )
	{
		checkFile( read, stat, graph, seq.getAbsoluteFilenameAt(t) );
	}
}

int main( int argc, char** argv )
{
	signal(SIGINT, signal_callback_handler);

	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );

	std::vector<std::string> inputs;
	std::string readerId;
	bool hasRange    = false;
	bool script      = false;
	std::vector<int> range;
	
	bpo::options_description desc;
	bpo::options_description hidden;
	
	formatter->init_logging();
	
	desc.add_options()
			( kHelpOptionString,   kHelpOptionMessage )
			( kReaderOptionString, bpo::value(&readerId)/*->required()*/, kReaderOptionMessage )
			( kInputOptionString,  bpo::value(&inputs)/*->required()*/,kInputOptionMessage )
			( kRangeOptionString,  bpo::value(&range)->multitoken(), kRangeOptionMessage )
			( kBriefOptionString,  kBriefOptionMessage )
			( kColorOptionString,  kColorOptionMessage )
			( kScriptOptionString, kScriptOptionMessage );
	
	// describe hidden options
	hidden.add_options()
			( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );
	
	bpo::options_description cmdline_options;
	cmdline_options.add( desc ).add( hidden );
	
	bpo::positional_options_description pod;
	pod.add(kInputOptionLongName, -1);
	
	bpo::variables_map vm;
	
	try
	{
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);
		
		// get environment options and parse them
		if( const char* env_check_options = std::getenv( "SAM_CHECK_OPTIONS" ) )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_check_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		if( const char* env_check_options = std::getenv( "SAM_OPTIONS" ) )
		{
			const std::vector<std::string> vecOptions = bpo::split_unix( env_check_options, " " );
			bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_LOG_ERROR( "sam-check: command line error: " << e.what() );
		exit( 254 );
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR( "sam-check: unknown error in command line." );
		exit( 254 );
	}
	
	if( vm.count( kScriptOptionLongName ) )
	{
		// disable color, disable directory printing and set relative path by default
		script = true;
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
	
	if( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_LOG_INFO( color->_green << "check image files" << color->_std );
		std::cout.rdbuf(0);
		return 0;
	}
	
	if( vm.count(kHelpOptionLongName) || vm.count(kInputDirOptionLongName) == 0 )
	{
		TUTTLE_LOG_INFO( color->_blue  << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-check - detect black images in sequences" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-check [reader] [input] [options]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "DESCRIPTION" << color->_std << std::endl );
		TUTTLE_LOG_INFO( "" );
		
		TUTTLE_LOG_INFO( "Check if sequence have black images." );
		TUTTLE_LOG_INFO( "This tools process the PSNR of an image, and if it's null, the image is considered black." );
		
		TUTTLE_LOG_INFO( color->_blue  << "OPTIONS" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( desc );
		return 0;
	}
	if( !vm.count(kReaderOptionLongName) )
	{
		TUTTLE_LOG_ERROR( "sam-check : no reader specified." );
		TUTTLE_LOG_ERROR( "            run sam-check -h for more information." );
		return 0;
	}
	if( !vm.count(kInputOptionLongName) )
	{
		TUTTLE_LOG_ERROR( "sam-check : no input specified." );
		TUTTLE_LOG_ERROR( "            run sam-check -h for more information." );
		return 0;
	}
	
	readerId = vm[kReaderOptionLongName].as<std::string>();
	inputs   = vm[kInputOptionLongName].as< std::vector<std::string> >();
	
	if( vm.count(kRangeOptionLongName) )
	{
		range = vm[kRangeOptionLongName].as< std::vector<int> >();
		hasRange = ( range.size() == 2 );
	}

	try
	{
		core().preload();
		Graph graph;
		Graph::Node& read = graph.createNode( readerId );
		Graph::Node& stat = graph.createNode( "tuttle.imagestatistics" );
		read.getParam("explicitConversion").setValue(3); // force reader to use float image buffer
		graph.connect( read, stat );

		BOOST_FOREACH( const bfs::path path, inputs )
		{
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{
					boost::ptr_vector<sp::FileObject> fObjects;
					sp::Detector detector;
					fObjects = detector.fileObjectInDirectory( path.string() );
					BOOST_FOREACH( sp::FileObject& fObj, fObjects )
					{
						switch( fObj.getMaskType() )
						{
							case sp::eMaskTypeSequence:
							{
								checkSequence( read, stat, graph, dynamic_cast<const sp::Sequence&>( fObj ) );
								break;
							}
							case sp::eMaskTypeFile:
							{
								const sp::File fFile = dynamic_cast<const sp::File&>( fObj );
								checkFile( read, stat, graph, fFile.getAbsoluteFilename() );
								break;
							}
							case sp::eMaskTypeDirectory:
							case sp::eMaskTypeUndefined:
								break;
						}
					}
				}
				else
				{
					checkFile( read, stat, graph, path );
				}
			}
			else
			{
				try
				{
					sp::Sequence s( path );
					if( hasRange )
					{
						checkSequence( read, stat, graph, s, range[0], range[1] );
					}
					else
					{
						checkSequence( read, stat, graph, s );
					}
				}
				catch( ... )
				{
					TUTTLE_LOG_ERROR( "Unrecognized pattern \"" << path << "\"" );
					return eReturnCodeApplicationError;
				}
			}
		}
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information() );
		return 255;
	}
	TUTTLE_LOG_WARNING( "________________________________________" );
	TUTTLE_LOG_WARNING( "Black images: "      << _blackImage       );
	TUTTLE_LOG_WARNING( "Null file size: "    << _nullFileSize     );
	TUTTLE_LOG_WARNING( "Corrupted images: "  << _corruptedImage   );
	TUTTLE_LOG_WARNING( "Holes in sequence: " << _missingFiles     );
	TUTTLE_LOG_WARNING( "________________________________________" );

	return _blackImage + _nullFileSize + _corruptedImage + _missingFiles;
}

