
#include <tuttle/host/Graph.hpp>
#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace tuttle::common;
using namespace tuttle::host;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

static int _blackImage = 0;
static int _nullFileSize = 0;
static int _corruptedImage = 0;
static int _missingFiles = 0;
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
EImageStatus checkImageStatus( Graph::Node& read, Graph::Node& stat, Graph& graph, const fs::path& filename )
{
	if( fs::exists( filename ) == 0 )
		return eImageStatusNoFile;
	
	if( fs::file_size( filename ) == 0 )
		return eImageStatusFileSizeError;

	try
	{
		// Setup parameters
		read.getParam( "filename" ).set( filename.string() );
		graph.compute( stat, 0 );
		std::cout.rdbuf(_stdCout); // restore cout's original streambuf
		for( unsigned int i = 0; i<4; ++i )
		{
			if( stat.getParam( "outputChannelMax" ).getDoubleAtIndex(i) != 0 )
				return eImageStatusOK;
			if( stat.getParam( "outputChannelMin" ).getDoubleAtIndex(i) != 0 )
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

EImageStatus checkFile( Graph::Node& read, Graph::Node& stat, Graph& graph, const fs::path& filename )
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
	std::cout.rdbuf(0); // remove cout's streambuf
	std::cerr.rdbuf(0); // remove cerr's streambuf

	try
	{
		std::vector<std::string> inputs;
		std::string readerId;
		bool hasRange = false;
		std::vector<int> range;

		po::options_description desc(
			"Check image files.\n"
			"\n"
			"Usage:\n"
			"\tsam-check -n fr.tuttle.pngreader /path/to/my/dir/images.####.png -r 50 100\n"
			"\n"
			"Return code:\n"
			"\t* the number of corrupted images\n"
			"\t* 0 if no error\n"
			"\t* -1 error in the application itself\n"
		);
		
		desc.add_options()
		("help,h", "Display help")
		("reader,n", po::value(&readerId)/*->required()*/, "Reader node identifier \"fr.tuttle.XXXreader\".")
		("input,i", po::value(&inputs)/*->required()*/, "Input pathname (directory, file or sequence pattern).")
		("range,r", po::value(&range)->multitoken(), "Range (used only if input is a sequence pattern).")
		;

		po::positional_options_description pArgs;
		pArgs.add("input", -1);

		po::variables_map vm;
		po::store( po::command_line_parser(argc, argv)
					.options(desc)
					.positional(pArgs).run(), vm );
		po::notify(vm);

		if( vm.count("help") || vm.count("input") == 0 )
		{
			std::cout.rdbuf(_stdCout); // restore cout's original streambuf
			std::cout << desc << std::endl;
			std::cout.rdbuf(0); // remove cout's streambuf
			return 0;
		}
		readerId = vm["reader"].as<std::string>();
		inputs = vm["input"].as< std::vector<std::string> >();
		if( vm.count("range") )
		{
			range = vm["range"].as< std::vector<int> >();
			hasRange = ( range.size() == 2 );
		}

		Core::instance().preload();
		Graph graph;
		Graph::Node& read = graph.createNode( readerId );
		Graph::Node& stat = graph.createNode( "fr.tuttle.imagestatistics" );
		read.getParam("explicitConversion").set(3); // force reader to use float image buffer
		graph.connect( read, stat );

		BOOST_FOREACH( const fs::path path, inputs )
		{
			if( fs::exists( path ) )
			{
				if( fs::is_directory( path ) )
				{
					std::vector<Sequence> sequences = sequencesInDir( path );
					BOOST_FOREACH( const Sequence& s, sequences )
					{
						checkSequence( read, stat, graph, s );
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
	std::cout << "________________________________________" << std::endl;
	std::cout << "Black images: " << _blackImage << std::endl;
	std::cout << "Null file size: " << _nullFileSize << std::endl;
	std::cout << "Corrupted images: " << _corruptedImage << std::endl;
	std::cout << "Holes in sequence: " << _missingFiles << std::endl;
	std::cout << "________________________________________" << std::endl;
	std::cout.rdbuf(0); // remove cerr's streambuf

	return _blackImage + _nullFileSize + _corruptedImage + _missingFiles;
}

