#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <limits>

using namespace tuttle::common;
using namespace tuttle::host;
namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

namespace sam
{
	Color _color;
}

static int _notNullImage   = 0;
static int _nullFileSize   = 0;
static int _corruptedImage = 0;
static int _missingFiles   = 0;
//static std::streambuf * const _stdCout = std::cout.rdbuf(); // back up cout's streambuf
//static std::streambuf * const _stdCerr = std::cerr.rdbuf(); // back up cout's streambuf

enum EReturnCode
{
	eReturnCodeOK = 0,
	eReturnCodeErrorInImages = 1,
	eReturnCodeApplicationError = 2
};

enum EImageStatus
{
	eImageStatusDiffNull,
	eImageStatusDiffNotNull,
	eImageStatusFileSizeError,
	eImageStatusNoFile,
	eImageStatusImageError
};

/**
 * @brief Diff the image status.
 */
EImageStatus diffImageStatus( Graph::Node& read1, Graph::Node& read2, Graph::Node& stat, Graph& graph, const bfs::path& filename1, const bfs::path& filename2 )
{
	if( bfs::exists( filename1 ) == 0 || bfs::exists( filename2 ) == 0 )
		return eImageStatusNoFile;

	if( bfs::file_size( filename1 ) == 0 || bfs::file_size( filename2 ) == 0 )
		return eImageStatusFileSizeError;

	try
	{
		// Setup parameters
		read1.getParam( "filename" ).setValue( filename1.string() );
		read2.getParam( "filename" ).setValue( filename2.string() );
		graph.compute( stat );
		std::cout << "diff = ";
		for( unsigned int i = 0; i<4; ++i )
		{
			std::cout << stat.getParam( "quality" ).getDoubleValueAtIndex(i) << "  ";
		}
		std::cout << std::endl;

		for( unsigned int i = 0; i<3; ++i )
		{
			if( stat.getParam( "quality" ).getDoubleValueAtIndex(i) != std::numeric_limits<double>::infinity() )
				return eImageStatusDiffNotNull;
		}
		std::cout << stat << std::endl;

		return eImageStatusDiffNull;
	}
	catch( ... )
	{
		std::cerr << boost::current_exception() << std::endl;
		std::cerr<< boost::current_exception_diagnostic_information() << std::endl;
		return eImageStatusImageError;
	}
}

EImageStatus diffFile( Graph::Node& read1, Graph::Node& read2, Graph::Node& stat, Graph& graph, const bfs::path& filename1, const bfs::path& filename2 )
{
	EImageStatus s = diffImageStatus( read1, read2, stat, graph, filename1, filename2 );

	//std::cout.rdbuf(_stdCout); // restore cout's original streambuf
	switch( s )
	{
		case eImageStatusDiffNull:
			break;
		case eImageStatusDiffNotNull:
			std::cout << "diff not null: ";
			++_notNullImage;
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
	std::cout << filename1 << "  |  " << filename2 << std::endl;
	//std::cout.rdbuf(0); // remove cout's streambuf
	return s;
}

void diffSequence( Graph::Node& read1, Graph::Node& read2, Graph::Node& stat, Graph& graph, const Sequence& seq1, const Sequence& seq2 )
{
	for( Sequence::Time t = seq1.getFirstTime(); t <= seq1.getLastTime(); ++t )
	{
		diffFile( read1, read2, stat, graph, seq1.getAbsoluteFilenameAt(t), seq2.getAbsoluteFilenameAt(t) );
	}
}

void diffSequence( Graph::Node& read1, Graph::Node& read2, Graph::Node& stat, Graph& graph, const Sequence& seq1, const Sequence& seq2, const Sequence::Time first, const Sequence::Time last )
{
	for( Sequence::Time t = first; t <= last; ++t )
	{
		diffFile( read1, read2, stat, graph, seq1.getAbsoluteFilenameAt(t), seq2.getAbsoluteFilenameAt(t) );
	}
}

int main( int argc, char** argv )
{
	using namespace sam;

	//std::cout.rdbuf(0); // remove cout's streambuf
	//std::cerr.rdbuf(0); // remove cerr's streambuf

	try
	{
		std::vector<std::string> inputs;
		std::vector<std::string> readerId;
		bool hasRange    = false;
		bool script      = false;
		bool enableColor = false;
		std::vector<int> range;

		bpo::options_description desc;
		bpo::options_description hidden;

		desc.add_options()
			("help,h",   "display help")
			("reader,n", bpo::value(&readerId)/*->required()*/, "reader node identifier \"tuttle.XXXreader\".")
			("inputs,i", bpo::value(&inputs)/*->required()*/, "input pathname (directory, file or sequence pattern).")
			("range,r",  bpo::value(&range)->multitoken(), "range (used only if input is a sequence pattern).")
			("brief",    "brief summary of the tool")
			("color",    "color the output")
			("script",   "output is formated to using in script files")
		;

		// describe hidden options
		hidden.add_options()
			("enable-color", bpo::value<std::string>(), "enable (or disable) color")
		;

		bpo::options_description cmdline_options;
		cmdline_options.add(desc).add(hidden);

		bpo::positional_options_description pod;
		pod.add("input", -1);

		bpo::variables_map vm;

		try
		{
			//parse the command line, and put the result in vm
			bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

			// get environment options and parse them
			if( const char* env_diff_options = std::getenv("SAM_DIFF_OPTIONS") )
			{
				const std::vector<std::string> vecOptions = bpo::split_unix( env_diff_options, " " );
				bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
			}
			if( const char* env_diff_options = std::getenv("SAM_OPTIONS") )
			{
				const std::vector<std::string> vecOptions = bpo::split_unix( env_diff_options, " " );
				bpo::store(bpo::command_line_parser(vecOptions).options(cmdline_options).positional(pod).run(), vm);
			}
			bpo::notify(vm);
		}
		catch( const bpo::error& e)
		{
			TUTTLE_COUT("sam-diff: command line error: " << e.what() );
			exit( -2 );
		}
		catch(...)
		{
			TUTTLE_COUT("sam-diff: unknown error in command line.");
			exit( -2 );
		}

		if ( vm.count("script") )
		{
			// disable color, disable directory printing and set relative path by default
			script = true;
		}

		if ( vm.count("color") && !script )
		{
			enableColor = true;
		}
		if ( vm.count("enable-color") && !script )
		{
			const std::string str = vm["enable-color"].as<std::string>();
			enableColor = string_to_boolean( str );
		}

		if( enableColor )
		{
			_color.enable();
		}

		if ( vm.count("brief") )
		{
			//std::cout.rdbuf(_stdCout);
			TUTTLE_COUT( _color._green << "diff image files" << _color._std );
			//std::cout.rdbuf(0);
			return 0;
		}

		if( vm.count("help") )
		{
			//std::cout.rdbuf(_stdCout); // restore cout's original streambuf
			TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
			TUTTLE_COUT( _color._green << "\tsam-diff - compute difference between 2 images/sequences" << _color._std << std::endl);
			TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
			TUTTLE_COUT( _color._green << "\tsam-diff [reader] [input] [reader] [input] [options]" << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std << std::endl );

			TUTTLE_COUT( "diff if sequence have black images." );
			TUTTLE_COUT( "This tools process the PSNR of an image, and if it's null, the image is considered black." );

			TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std << std::endl );
			TUTTLE_COUT( desc );
			//std::cout.rdbuf(0); // remove cout's streambuf
			return 0;
		}
		if( !vm.count("reader") )
		{
			TUTTLE_COUT( _color._red  << "sam-diff : no reader specified." << _color._std );
			TUTTLE_COUT( _color._red  << "           run sam-diff -h for more information." << _color._std );
			return 0;
		}
		if( !vm.count("inputs") )
		{
			TUTTLE_COUT( _color._red  << "sam-diff : no input specified." << _color._std );
			TUTTLE_COUT( _color._red  << "           run sam-diff -h for more information." << _color._std );
			return 0;
		}
		readerId = vm["reader"].as< std::vector<std::string> >();
		inputs   = vm["inputs"].as< std::vector<std::string> >();
		if( readerId.size() != 2 || inputs.size() != 2 )
		{
			TUTTLE_COUT( _color._red  << "sam-diff : number of input or reader is/are incorrect." << _color._std );
			TUTTLE_COUT( _color._red  << "           run sam-diff -h for more information." << _color._std );
			return 0;
		}
		if( vm.count("range") )
		{
			range = vm["range"].as< std::vector<int> >();
			hasRange = ( range.size() == 2 );
		}

		Core::instance().preload();
		Graph graph;
		TUTTLE_COUT( "in1: " << readerId.at(0) << " " << inputs.at(0) );
		TUTTLE_COUT( "in2: " << readerId.at(1) << " " << inputs.at(1) );
		Graph::Node& read1 = graph.createNode( readerId.at(0) );
		Graph::Node& read2 = graph.createNode( readerId.at(1) );
		//Graph::Node& viewer = graph.createNode( "tuttle.viewer" );
		Graph::Node& stat = graph.createNode( "tuttle.diff" );
		read1.getParam("explicitConversion").setValue(3); // force reader to use float image buffer
		read2.getParam("explicitConversion").setValue(3); // force reader to use float image buffer
		//graph.connect( viewer, stat );
		graph.connect( read1,  stat );
		graph.connect( read2, stat.getAttribute("SourceB") );

		bfs::path path1 = inputs.at(0);
		bfs::path path2 = inputs.at(1);

			if( bfs::exists( path1 ) )
			{
				if( !bfs::exists( path2 ) )
				{
					TUTTLE_COUT( "could not find file or directory 2 (first is not a sequence)." );
					return 0;
				}/*
				if( bfs::is_directory( path1 ) )
				{
					std::list<boost::shared_ptr<FileObject> > fObjects;
					fObjects = fileObjectsInDir( path );
					BOOST_FOREACH( const boost::shared_ptr<FileObject> fObj, fObjects )
					{
						switch( fObj->getMaskType() )
						{
							case eMaskTypeSequence:
							{
								diffSequence( read1, read2, stat, graph, dynamic_cast<const Sequence&>( *fObj ) );
								break;
							}
							case eMaskTypeFile:
							{
								const File fFile = dynamic_cast<const File&>( *fObj );
								diffFile( read1, read2, stat, graph, fFile.getAbsoluteFilename(), fFile.getAbsoluteFilename() );
								break;
							}
							case eMaskTypeDirectory:
							case eMaskTypeUndefined:
								break;
						}
					}
				}
				else
				{*/

					diffFile( read1, read2, stat, graph, path1, path2 );
				//}
			}
			else
			{
				try
				{
					Sequence s1( path1 );
					Sequence s2( path2 );
					if( hasRange )
					{
						diffSequence( read1, read2, stat, graph, s1, s2, range[0], range[1] );
					}
					else
					{
						diffSequence( read1, read2, stat, graph, s1, s2 );
					}
				}
				catch( ... )
				{
					std::cerr << "Unrecognized pattern " << path1 << " or " << path2 << std::endl;
					return eReturnCodeApplicationError;
				}
			}

	}
	catch( ... )
	{
		//std::cerr.rdbuf(_stdCerr); // restore cout's original streambuf
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		//std::cerr.rdbuf(0); // remove cerr's streambuf
		return -1;
	}
	//std::cout.rdbuf(_stdCerr); // restore cout's original streambuf
	TUTTLE_COUT( "________________________________________" );
	TUTTLE_COUT( "Not null images: "      << _notNullImage  );
	TUTTLE_COUT( "Null file size: "    << _nullFileSize     );
	TUTTLE_COUT( "Corrupted images: "  << _corruptedImage   );
	TUTTLE_COUT( "Holes in sequence: " << _missingFiles     );
	TUTTLE_COUT( "________________________________________" );
	//std::cout.rdbuf(0); // remove cerr's streambuf

	return _notNullImage + _nullFileSize + _corruptedImage + _missingFiles;
}

