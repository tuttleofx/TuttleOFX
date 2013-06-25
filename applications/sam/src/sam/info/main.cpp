#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <magick/MagickCore.h>

#include <Detector.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>

#define FIRST_COLUMN_WIDTH 23

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace sp  = sequenceParser;

int	firstImage	= 0;
int	lastImage	= 0;

namespace sam
{
	bool wasSthgDumped = false;
}


void printImageProperties( std::string path )
{
	try
	{
		Image         *image;
		ImageInfo     *image_info;
		
		// Read a file into image object
		image_info = AcquireImageInfo();
		GetImageInfo( image_info );

		strcpy( image_info -> filename, path.c_str() );

		ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
		GetExceptionInfo( exceptionsInfo );
		
		image = ReadImage( image_info, exceptionsInfo );
		
		std::string imageType;
		switch( image->type )
		{
			case UndefinedType             : imageType = "Undefined type of image"; break;
			case BilevelType               : imageType = "Bilevel image"; break;
			case GrayscaleType             : imageType = "Grayscale image"; break;
			case GrayscaleMatteType        : imageType = "Grayscale image with opacity"; break;
			case PaletteType               : imageType = "Indexed color image"; break;
			case PaletteMatteType          : imageType = "Indexed color image with opacity"; break;
			case TrueColorType             : imageType = "Truecolor image"; break;
			case TrueColorMatteType        : imageType = "Truecolor image with opacity"; break;
			case ColorSeparationType       : imageType = "Cyan/Yellow/Magenta/Black (CYMK) image"; break;
			case ColorSeparationMatteType  : imageType = "Cyan/Yellow/Magenta/Black (CYMK) image with opacity"; break;
			case OptimizeType              : imageType = "Optimize image"; break;
			case 11                        : imageType = "Indexed bilevel image with opacity"; break; // PaletteBilevelMatteType
		}

		std::string resolutionType;
		switch( image->units )
		{
			case UndefinedResolution            : resolutionType = " [unknown units]"; break;
			case PixelsPerInchResolution        : resolutionType = " [dpi]"; break;
			case PixelsPerCentimeterResolution  : resolutionType = " [pixels/cm]"; break;
		}

		std::string colorSpaceType;
		switch( image->colorspace )
		{
			case UndefinedColorspace      : colorSpaceType = "unknown color space"; break;
			case RGBColorspace            : colorSpaceType = "RGB"; break;
			case GRAYColorspace           : colorSpaceType = "Gray"; break;
			case TransparentColorspace    : colorSpaceType = "Transparent"; break;
			case OHTAColorspace           : colorSpaceType = "OHTA"; break;
			case XYZColorspace            : colorSpaceType = "XYZ"; break;
			case YCbCrColorspace          : colorSpaceType = "Y Cb Cr"; break;
			case YCCColorspace            : colorSpaceType = "YCC"; break;
			case YIQColorspace            : colorSpaceType = "YIQ"; break;
			case YPbPrColorspace          : colorSpaceType = "Y Pb Pr"; break;
			case YUVColorspace            : colorSpaceType = "YUV"; break;
			case CMYKColorspace           : colorSpaceType = "CMYK"; break;
			case sRGBColorspace           : colorSpaceType = "sRGB"; break;
			case LabColorspace            : colorSpaceType = "Lab"; break;
			case 14                       : colorSpaceType = "HSB"; break; // HSBColorspace
			case HSLColorspace            : colorSpaceType = "HSL"; break;
			case HWBColorspace            : colorSpaceType = "HWB"; break;
			case Rec601LumaColorspace     : colorSpaceType = "Rec601 Luma"; break;
			case 18                       : colorSpaceType = "Rec601 Y Cb Cr"; break; // Rec601YCbCrColorspace
			case Rec709LumaColorspace     : colorSpaceType = "Rec709 Luma"; break;
			case 20                       : colorSpaceType = "Rec709 Y Cb Cr"; break; //  Rec709YCbCrColorspace
			case LogColorspace            : colorSpaceType = "Log"; break;
			case 22                       : colorSpaceType = "CMY"; break; // CMYColorspace
#if MagickLibVersion > 0x677
			case LuvColorspace            : colorSpaceType = "Luv"; break;
			case HCLColorspace            : colorSpaceType = "HCL"; break;
			case LCHColorspace            : colorSpaceType = "LCH"; break;
			case LMSColorspace            : colorSpaceType = "LMS"; break;
#endif
		}

		std::string interlaceType;
		switch( image->interlace )
		{
			case UndefinedInterlace    : interlaceType = "undefined"; break;
			case NoInterlace           : interlaceType = "no interlacing"; break;
			case LineInterlace         : interlaceType = "line interlacing"; break;
			case PlaneInterlace        : interlaceType = "plane interlacing"; break;
			case PartitionInterlace    : interlaceType = "partition interlacing"; break;
			case 5                     : interlaceType = "GIF interlacing"; break; // GIFInterlace
			case 6                     : interlaceType = "Jpeg interlacing"; break; // JPEGInterlace
			case 7                     : interlaceType = "PNG interlacing"; break; // PNGInterlace
		}

		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "width"                 << image->columns                                                                         );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "height"                << image->rows                                                                            );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "bit-depth"             << image->depth  << " bits"                                                               );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "compression quality"   << image->quality                                                                         );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "image type"            << imageType                                                                              );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "" );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "x resolution"          << image->x_resolution  << resolutionType                                                 );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "y resolution"          << image->y_resolution  << resolutionType                                                 );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "interlacing"           << interlaceType                                                                          );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "" );
		//TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "format"                << image->format()                                                                        );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "channels"              << colorSpaceType << ( GetImageAlphaChannel(image)==MagickTrue ? std::string("A") : "" )  );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "color space"           << colorSpaceType                                                                         );
		TUTTLE_LOG_INFO( std::setw(FIRST_COLUMN_WIDTH) << "gamma"                 << image->gamma                                                                           );

		TUTTLE_LOG_INFO( "" );
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( "Caught exception" << "\n" );
	}
	
}

void dumpImageProperties( const sp::File& s )
{
	TUTTLE_LOG_INFO(s);
	printImageProperties( s.getAbsoluteFilename() );
	sam::wasSthgDumped = true;
}

void dumpImageProperties( const sp::Sequence& s )
{
	TUTTLE_LOG_INFO(s);
	printImageProperties( s.getAbsoluteFirstFilename() );
	sam::wasSthgDumped = true;
}


void dumpImageProperties( boost::ptr_vector<sp::FileObject>& listing )
{
	BOOST_FOREACH( const sp::FileObject& sequence, listing )
	{
		switch( sequence.getMaskType () )
		{
			case sp::eMaskTypeSequence	: dumpImageProperties( static_cast<const sp::Sequence&>( sequence ) ); break;
			case sp::eMaskTypeFile		: dumpImageProperties( static_cast<const sp::File&>( sequence ) ); break;
			case sp::eMaskTypeDirectory	: break;
			case sp::eMaskTypeUndefined	: break;
		}
	}
	listing.clear();
}


int main( int argc, char** argv )
{
	signal(SIGINT, signal_callback_handler);

	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );

	sp::EMaskType researchMask       = sp::eMaskTypeSequence; // by default show sequences
	sp::EMaskOptions descriptionMask = sp::eMaskOptionsColor; // by default show nothing
	bool recursiveListing = false;
	std::string availableExtensions;
	std::vector<std::string> paths;
	std::vector<std::string> filters;

	formatter->init_logging();
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		( kAllOptionString, kAllOptionMessage )
		( kExpressionOptionString, bpo::value<std::string>(), kExpressionOptionMessage )
		( kFilesOptionString,      kFilesOptionMessage )
		( kHelpOptionString,       kHelpOptionMessage )
		( kIgnoreOptionString,     kIgnoreOptionMessage )
		( kPathOptionString,       kPathOptionMessage )
		( kRecursiveOptionString,  kRecursiveOptionMessage )
		( kVerboseOptionString,    bpo::value<int>()->default_value( 2 ), kVerboseOptionMessage )
		( kQuietOptionString,      kQuietOptionMessage )
		( kColorOptionString,      kColorOptionMessage )
		( kFirstImageOptionString, bpo::value<unsigned int>(), kFirstImageOptionMessage )
		( kLastImageOptionString,  bpo::value<unsigned int>(), kLastImageOptionMessage )
		( kBriefOptionString,      kBriefOptionMessage );

	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		( kInputDirOptionString, bpo::value< std::vector<std::string> >(), kInputDirOptionMessage )
		( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );

	// define default options
	bpo::positional_options_description pod;
	pod.add(kInputDirOptionLongName, -1);

	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);

	//parse the command line, and put the result in vm
	bpo::variables_map vm;

	try
	{
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

		// get environment options and parse them
		if( const char* env_info_options = std::getenv("SAM_INFO_OPTIONS") )
		{
			const std::vector<std::string> envOptions = bpo::split_unix( env_info_options, " " );
			bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		if( const char* env_info_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> envOptions = bpo::split_unix( env_info_options, " " );
			bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
		}
		bpo::notify(vm);
	}
	catch( const bpo::error& e)
	{
		TUTTLE_LOG_ERROR( "error in command line: " << e.what() );
		exit( 254 );
	}
	catch(...)
	{
		TUTTLE_LOG_ERROR( "unknown error in command line." );
		exit( 254 );
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

	if( vm.count( kHelpOptionLongName ) )
	{
		TUTTLE_LOG_INFO( color->_blue  << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "NAME" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-info - get informations about a sequence" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "SYNOPSIS" << color->_std );
		TUTTLE_LOG_INFO( color->_green << "\tsam-info [options] [sequences]" << color->_std );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "DESCRIPTION\n" << color->_std );
		TUTTLE_LOG_INFO( "Print informations from Sequence (or file) like resolution, colorspace, etc." );
		TUTTLE_LOG_INFO( "" );
		TUTTLE_LOG_INFO( color->_blue  << "OPTIONS" << color->_std);
		TUTTLE_LOG_INFO( mainOptions );
		TUTTLE_LOG_INFO( "" );
		return 0;
	}

	if ( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_LOG_INFO( color->_green << "get informations about a sequence" << color->_std );
		return 0;
	}

	if (vm.count(kExpressionOptionLongName))
	{
		bal::split( filters, vm[kExpressionOptionLongName].as<std::string>(), bal::is_any_of(","));
	}

	if (vm.count(kDirectoriesOptionLongName))
	{
		researchMask |= sp::eMaskTypeDirectory;
	}

	if (vm.count(kFilesOptionLongName))
	{
		researchMask |= sp::eMaskTypeFile;
	}

	if (vm.count(kIgnoreOptionLongName))
	{
		researchMask &= ~sp::eMaskTypeSequence;
	}

	switch( vm[ kVerboseOptionLongName ].as< int >() )
	{
		case 0 :  formatter->setLogLevel( boost::log::trivial::trace   ); break;
		case 1 :  formatter->setLogLevel( boost::log::trivial::debug   ); break;
		case 2 :  formatter->setLogLevel( boost::log::trivial::info    ); break;
		case 3 :  formatter->setLogLevel( boost::log::trivial::warning ); break;
		case 4 :  formatter->setLogLevel( boost::log::trivial::error   ); break;
		case 5 :  formatter->setLogLevel( boost::log::trivial::fatal   ); break;
		default : formatter->setLogLevel( boost::log::trivial::warning ); break;
	}
	if( vm.count(kQuietOptionLongName) )
	{
		formatter->setLogLevel( boost::log::trivial::fatal );
	}

	if (vm.count(kFirstImageOptionLongName))
	{
		firstImage  = vm[kFirstImageOptionLongName].as< unsigned int >();
	}

	if (vm.count(kLastImageOptionLongName))
	{
		lastImage  = vm[kLastImageOptionLongName].as< unsigned int >();
	}

	if (vm.count(kFullRMPathOptionLongName))
	{
		researchMask |= sp::eMaskTypeDirectory;
		researchMask |= sp::eMaskTypeFile;
		researchMask |= sp::eMaskTypeSequence;
	}

	if (vm.count(kAllOptionLongName))
	{
		// add .* files
		descriptionMask |= sp::eMaskOptionsDotFile;
	}

	if (vm.count(kPathOptionLongName))
	{
		 descriptionMask |= sp::eMaskOptionsPath;
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
// 	for(unsigned int i=0; i<filters.size(); i++)
// 	TUTTLE_LOG_TRACE( "filters = " << filters.at(i) );
// 	TUTTLE_LOG_TRACE( "research mask = " << researchMask );
// 	TUTTLE_LOG_TRACE( "options  mask = " << descriptionMask );


	try
	{
		std::vector<boost::filesystem::path> pathsNoRemoved;
		sp::Detector detector;
		BOOST_FOREACH( bfs::path path, paths )
		{
//			TUTTLE_LOG_TRACE( "path: "<< path );
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{
//					TUTTLE_LOG_TRACE( "is a directory" );
					if( recursiveListing )
					{
						for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
						{
							if( bfs::is_directory( *dir ) )
							{
//								TUTTLE_LOG_TRACE *dir );
								boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( ( (bfs::path)*dir ).string() , filters, researchMask, descriptionMask );
								dumpImageProperties( listing );
							}
						}
					}
					boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( path.string(), filters, researchMask, descriptionMask );
					dumpImageProperties( listing );
				}
				else
				{
					if( std::strcmp( path.branch_path().string().c_str(),"" ) == 0 )
						path = "."/path.leaf();
					//TUTTLE_LOG_TRACE( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					boost::ptr_vector<sp::FileObject> listing = detector.fileObjectInDirectory( path.branch_path().string(), filters, researchMask, descriptionMask );
					dumpImageProperties( listing );
					filters.pop_back( );
				}
			}
			else
			{
//				TUTTLE_LOG_TRACE( "not exist ...." );
				try
				{
					sp::Sequence s(path.branch_path(), descriptionMask );
					s.initFromDetection( path.string(), sp::Sequence::ePatternDefault );
					if( s.getNbFiles() )
					{
						//TUTTLE_LOG_TRACE(s);
						dumpImageProperties( s );
					}
				}
				catch(... )
				{
					TUTTLE_LOG_ERROR( "Unrecognized pattern \"" << path << "\"" );
				}
			}
		}
	}
	catch (bfs::filesystem_error &ex)
	{
		TUTTLE_LOG_ERROR( ex.what() );
	}
	catch(... )
	{
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information() );
	}
	
	if( !wasSthgDumped )
	{
		TUTTLE_LOG_ERROR( "No sequence found here." );
	}
	
	return 0;
}

