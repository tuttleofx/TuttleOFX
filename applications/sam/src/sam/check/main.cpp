#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>
#include <sam/common/options.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace tuttle::common;
using namespace tuttle::host;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

namespace sam
{
	Color _color;
}

static int _blackImage     = 0;
static int _nullFileSize   = 0;
static int _corruptedImage = 0;
static int _missingFiles   = 0;
static std::streambuf * const _stdCout = std::cout.rdbuf(); // back up cout's streambuf
static std::streambuf * const _stdCerr = std::cerr.rdbuf(); // back up cout's streambuf

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
		std::cout.rdbuf(_stdCout); // restore cout's original streambuf
		for( unsigned int i = 0; i<4; ++i )
		{
			if( stat.getParam( "outputChannelMax" ).getDoubleValueAtIndex(i) != 0 )
				return eImageStatusOK;
			if( stat.getParam( "outputChannelMin" ).getDoubleValueAtIndex(i) != 0 )
				return eImageStatusOK;
		}
		std::cout << "stat:" << stat << std::endl;
		std::cout.rdbuf(0); // remove cout's streambuf
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

	std::cout.rdbuf(_stdCout); // restore cout's original streambuf
	switch( s )
	{
		case eImageStatusOK:
			break;
		case eImageStatusBlack:
			std::cout << "Black image: ";
			++_blackImage;
			break;
		case eImageStatusFileSizeError:
			std::cout << "Null file size: ";
			++_nullFileSize;
			break;
		case eImageStatusNoFile:
			std::cout << "Missing file: ";
			++_missingFiles;
			break;
		case eImageStatusImageError:
			std::cout << "Corrupted image: ";
			++_corruptedImage;
			break;
	}
	std::cout << filename << std::endl;
	std::cout.rdbuf(0); // remove cout's streambuf
	return s;
}

void checkSequence( Graph::Node& read, Graph::Node& stat, Graph& graph, const Sequence& seq )
{
	for( Sequence::Time t = seq.getFirstTime(); t <= seq.getLastTime(); ++t )
	{
		checkFile( read, stat, graph, seq.getAbsoluteFilenameAt(t) );
	}
}

void checkSequence( Graph::Node& read, Graph::Node& stat, Graph& graph, const Sequence& seq, const Sequence::Time first, const Sequence::Time last )
{
	for( Sequence::Time t = first; t <= last; ++t )
	{
		checkFile( read, stat, graph, seq.getAbsoluteFilenameAt(t) );
	}
}

int main( int argc, char** argv )
{
	using namespace sam;

	std::cout.rdbuf(0); // remove cout's streambuf
	std::cerr.rdbuf(0); // remove cerr's streambuf

	try
	{
		std::vector<std::string> inputs;
		std::string readerId;
		bool hasRange    = false;
		bool script      = false;
		bool enableColor = false;
		std::vector<int> range;

		bpo::options_description desc;
		bpo::options_description hidden;

		desc.add_options()
			(kHelpOptionString.c_str(),   kHelpOptionMessage.c_str())
			(kReaderOptionString.c_str(), bpo::value(&readerId)/*->required()*/, kReaderOptionMessage.c_str())
			(kInputOptionString.c_str(),  bpo::value(&inputs)/*->required()*/,kInputOptionMessage.c_str())
			(kRangeOptionString.c_str(),  bpo::value(&range)->multitoken(), kRangeOptionMessage.c_str() )
			(kBriefOptionString.c_str(),    kBriefOptionMessage.c_str())
			(kColorOptionString.c_str(),    kColorOptionMessage.c_str())
			(kScriptOptionString.c_str(),  kScriptOptionMessage.c_str())
		;

		// describe hidden options
		hidden.add_options()
			(kEnableColorOptionString.c_str(), bpo::value<std::string>(), kEnableColorOptionMessage.c_str())
		;

		bpo::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		bpo::positional_options_description pod;
		pod.add(kInputOptionLongName.c_str(), -1);

		bpo::variables_map vm;

		try
		{
			//parse the command line, and put the result in vm
			bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

			// get environment options and parse them
			if( const char* env_check_options = std::getenv("SAM_CHECK_OPTIONS") )
			{
				const std::vector<std::string> vecOptions = bpo::split_unix( env_check_options, " " );
				bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
			}
			if( const char* env_check_options = std::getenv("SAM_OPTIONS") )
			{
				const std::vector<std::string> vecOptions = bpo::split_unix( env_check_options, " " );
				bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
			}
			bpo::notify(vm);
		}
		catch( const bpo::error& e)
		{
			TUTTLE_COUT("sam-check: command line error: " << e.what() );
			exit( -2 );
		}
		catch(...)
		{
			TUTTLE_COUT("sam-check: unknown error in command line.");
			exit( -2 );
		}

		if ( vm.count(kScriptOptionLongName.c_str()) )
		{
			// disable color, disable directory printing and set relative path by default
			script = true;
		}

		if ( vm.count(kColorOptionLongName.c_str()) && !script )
		{
			enableColor = true;
		}
		if ( vm.count(kEnableColorOptionLongName.c_str()) && !script )
		{
			const std::string str = vm[kEnableColorOptionLongName.c_str()].as<std::string>();
			enableColor = string_to_boolean( str );
		}

		if( enableColor )
		{
			_color.enable();
		}

		if ( vm.count(kBriefOptionLongName.c_str()) )
		{
			std::cout.rdbuf(_stdCout);
			TUTTLE_COUT( _color._green << "check image files" << _color._std );
			std::cout.rdbuf(0);
			return 0;
		}

		if( vm.count(kHelpOptionLongName.c_str()) || vm.count(kInputDirOptionLongName.c_str()) == 0 )
		{
			std::cout.rdbuf(_stdCout); // restore cout's original streambuf
			TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
			TUTTLE_COUT( _color._green << "\tsam-check - detect black images in sequences" << _color._std << std::endl);
			TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
			TUTTLE_COUT( _color._green << "\tsam-check [reader] [input] [options]" << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std << std::endl );

			TUTTLE_COUT( "Check if sequence have black images." );
			TUTTLE_COUT( "This tools process the PSNR of an image, and if it's null, the image is considered black." );

			TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std << std::endl );
			TUTTLE_COUT( desc );
			std::cout.rdbuf(0); // remove cout's streambuf
			return 0;
		}
		if( !vm.count(kReaderOptionLongName.c_str()) )
		{
			TUTTLE_COUT( _color._red  << "sam-check : no reader specified." << _color._std );
			TUTTLE_COUT( _color._red  << "            run sam-check -h for more information." << _color._std );
			return 0;
		}
		if( !vm.count(kInputOptionLongName.c_str()) )
		{
			TUTTLE_COUT( _color._red  << "sam-check : no input specified." << _color._std );
			TUTTLE_COUT( _color._red  << "            run sam-check -h for more information." << _color._std );
			return 0;
		}
		readerId = vm[kReaderOptionLongName.c_str()].as<std::string>();
		inputs   = vm[kInputOptionLongName.c_str()].as< std::vector<std::string> >();
		if( vm.count(kRangeOptionLongName.c_str()) )
		{
			range = vm[kRangeOptionLongName.c_str()].as< std::vector<int> >();
			hasRange = ( range.size() == 2 );
		}

		Core::instance().preload();
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
					std::list<boost::shared_ptr<FileObject> > fObjects;
					fObjects = fileObjectsInDir( path );
					BOOST_FOREACH( const boost::shared_ptr<FileObject> fObj, fObjects )
					{
						switch( fObj->getMaskType() )
						{
							case eMaskTypeSequence:
							{
								checkSequence( read, stat, graph, dynamic_cast<const Sequence&>( *fObj ) );
								break;
							}
							case eMaskTypeFile:
							{
								const File fFile = dynamic_cast<const File&>( *fObj );
								checkFile( read, stat, graph, fFile.getAbsoluteFilename() );
								break;
							}
							case eMaskTypeDirectory:
							case eMaskTypeUndefined:
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
					Sequence s( path );
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
					std::cerr << "Unrecognized pattern \"" << path << "\"" << std::endl;
					return eReturnCodeApplicationError;
				}
			}
		}
	}
	catch( ... )
	{
		std::cerr.rdbuf(_stdCerr); // restore cout's original streambuf
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		std::cerr.rdbuf(0); // remove cerr's streambuf
		return -1;
	}
	std::cout.rdbuf(_stdCerr); // restore cout's original streambuf
	TUTTLE_COUT( "________________________________________" );
	TUTTLE_COUT( "Black images: "      << _blackImage       );
	TUTTLE_COUT( "Null file size: "    << _nullFileSize     );
	TUTTLE_COUT( "Corrupted images: "  << _corruptedImage   );
	TUTTLE_COUT( "Holes in sequence: " << _missingFiles     );
	TUTTLE_COUT( "________________________________________" );
	std::cout.rdbuf(0); // remove cerr's streambuf

	return _blackImage + _nullFileSize + _corruptedImage + _missingFiles;
}

