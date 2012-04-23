#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <magick/MagickCore.h>
#include <algorithm>
#include <iostream>
#include <iterator>

#define FIRST_COLUMN_WIDTH 23

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace ttl = tuttle::common;

bool	enableColor	= false;
bool	verbose		= false;

int	firstImage	= 0;
int	lastImage	= 0;

namespace sam
{
	Color _color;
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

		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "width"                 << image->columns                                                                         );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "height"                << image->rows                                                                            );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "bit-depth"             << image->depth  << " bits"                                                               );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "compression quality"   << image->quality                                                                         );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "image type"            << imageType                                                                              );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "" );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "x resolution"          << image->x_resolution  << resolutionType                                                 );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "y resolution"          << image->y_resolution  << resolutionType                                                 );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "interlacing"           << interlaceType                                                                          );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "" );
		//TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "format"                << image->format()                                                                        );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "channels"              << colorSpaceType << ( GetImageAlphaChannel(image)==MagickTrue ? std::string("A") : "" )  );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "color space"           << colorSpaceType                                                                         );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "gamma"                 << image->gamma                                                                           );

		TUTTLE_COUT( "" );
	}
	catch( ... )
	{
		TUTTLE_COUT( "Caught exception" << "\n" );
	}
	
}

void getImageProperties( const ttl::File& s )
{
	TUTTLE_COUT(s);
	printImageProperties( s.getAbsoluteFilename() );
}

void getImageProperties( const ttl::Sequence& s )
{
	TUTTLE_COUT(s);
	printImageProperties( s.getAbsoluteFirstFilename() );
}


void getImageProperties( std::list<boost::shared_ptr<tuttle::common::FileObject> > &listing )
{
	BOOST_FOREACH( boost::shared_ptr<tuttle::common::FileObject> sequence, listing )
	{
		//TUTTLE_COUT( (ttl::Sequence&) *sequence );

		switch( sequence->getMaskType () )
		{
			case ttl::eMaskTypeSequence	: getImageProperties( (ttl::Sequence&) *sequence ); break;
			case ttl::eMaskTypeFile		: getImageProperties( (ttl::File&) *sequence ); break;
			case ttl::eMaskTypeDirectory	: break;
			case ttl::eMaskTypeUndefined	: break;
		}
	}
	listing.clear();
}


int main( int argc, char** argv )
{
	using namespace tuttle::common;
	using namespace sam;

	EMaskType researchMask       = eMaskTypeSequence;	// by default show sequences
	EMaskOptions descriptionMask = eMaskOptionsColor;	// by default show nothing
	bool recursiveListing	     = false;
	std::string availableExtensions;
	std::vector<std::string> paths;
	std::vector<std::string> filters;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		(kAllOptionString            , kAllOptionMessage)
		(kExpressionOptionString     , bpo::value<std::string>(), kExpressionOptionMessage)
		(kFilesOptionString          , kFilesOptionMessage)
		(kHelpOptionString           , kHelpOptionMessage)
		(kIgnoreOptionString          , kIgnoreOptionMessage)
		(kPathOptionString     , kPathOptionMessage)
		(kRecursiveOptionString      , kRecursiveOptionMessage)
		(kColorOptionString           , kColorOptionMessage)
		(kFirstImageOptionString     , bpo::value<unsigned int>(), kFirstImageOptionMessage)
		(kLastImageOptionString      , bpo::value<unsigned int>(), kLastImageOptionMessage)
		(kBriefOptionString            , kBriefOptionMessage)
	;

	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		(kInputDirOptionString        , bpo::value< std::vector<std::string> >(), kInputDirOptionMessage)
		(kEnableColorOptionString     , bpo::value<std::string>(), kEnableColorOptionMessage)
	;

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
		TUTTLE_COUT("error in command line: " << e.what() );
		exit( -2 );
	}
	catch(...)
	{
		TUTTLE_COUT("unknown error in command line.");
		exit( -2 );
	}

	if ( vm.count(kColorOptionLongName) )
	{
		enableColor = true;
	}
	if ( vm.count(kEnableColorOptionLongName) )
	{
		const std::string str = vm[kEnableColorOptionLongName].as<std::string>();
		enableColor = string_to_boolean( str );
	}

	if( enableColor )
	{
		descriptionMask |= eMaskOptionsColor;
		_color.enable();
	}

	if (vm.count(kHelpOptionLongName))
	{
	    TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
	    TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
	    TUTTLE_COUT( _color._green << "\tsam-info - get informations about a sequence" << _color._std << std::endl );
	    TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
	    TUTTLE_COUT( _color._green << "\tsam-info [options] [sequences]" << _color._std << std::endl );
	    TUTTLE_COUT( _color._blue  << "DESCRIPTION\n" << _color._std );
	    TUTTLE_COUT( "Print informations from Sequence (or file) like resolution, colorspace, etc." << std::endl );
	    TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std);
	    TUTTLE_COUT( mainOptions );
	    return 0;
	}

	if ( vm.count(kBriefOptionLongName) )
	{
		TUTTLE_COUT( _color._green << "get informations about a sequence" << _color._std);
		return 0;
	}

	if (vm.count(kExpressionOptionLongName))
	{
	    bal::split( filters, vm[kExpressionOptionLongName].as<std::string>(), bal::is_any_of(","));
	}

	if (vm.count(kDirectoriesOptionLongName))
	{
	    researchMask |= eMaskTypeDirectory;
	}

	if (vm.count(kFilesOptionLongName))
	{
	    researchMask |= eMaskTypeFile;
	}

	if (vm.count(kIgnoreOptionLongName))
	{
		researchMask &= ~eMaskTypeSequence;
	}

	if (vm.count(kVerboseOptionLongName))
	{
		verbose = true;
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
		researchMask |= eMaskTypeDirectory;
		researchMask |= eMaskTypeFile;
		researchMask |= eMaskTypeSequence;
	}

	if (vm.count(kAllOptionLongName))
	{
		// add .* files
		descriptionMask |= eMaskOptionsDotFile;
	}

	if (vm.count(kPathOptionLongName))
	{
		 descriptionMask |= eMaskOptionsPath;
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
// 	  TUTTLE_COUT("filters = " << filters.at(i));
// 	TUTTLE_COUT("research mask = " << researchMask);
// 	TUTTLE_COUT("options  mask = " << descriptionMask);

	try
	{
		std::vector<boost::filesystem::path> pathsNoRemoved;
		BOOST_FOREACH( bfs::path path, paths )
		{
//			TUTTLE_COUT( "path: "<< path );
			if( bfs::exists( path ) )
			{
				if( bfs::is_directory( path ) )
				{
//					TUTTLE_COUT( "is a directory" );
					if( recursiveListing )
					{
						for ( bfs::recursive_directory_iterator end, dir(path); dir != end; ++dir )
						{
							if( bfs::is_directory( *dir ) )
							{
//								TUTTLE_COUT( *dir );
								std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)*dir, filters, researchMask, descriptionMask );
								getImageProperties( listing );
							}
						}
					}
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path, filters, researchMask, descriptionMask );
					getImageProperties( listing );
				}
				else
				{
					if( std::strcmp( path.branch_path().string().c_str(),"" ) == 0 )
						path = "."/path.leaf();
					//TUTTLE_COUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path.branch_path(), filters, researchMask, descriptionMask );
					getImageProperties( listing );
					filters.pop_back( );
				}
			}
			else
			{
//				TUTTLE_COUT( "not exist ...." );
				try
				{
					Sequence s(path.branch_path(), descriptionMask );
					s.initFromDetection( path.string(), Sequence::ePatternDefault );
					if( s.getNbFiles() )
					{
						//TUTTLE_COUT(s);
						getImageProperties( s );
					}
				}
				catch(... )
				{
					TUTTLE_CERR ( _color._red << "Unrecognized pattern \"" << path << "\"" << _color._std );
				}
			}
		}
	}
	catch (bfs::filesystem_error &ex)
	{
		TUTTLE_CERR( _color._error << ex.what() << _color._std );
	}
	catch(... )
	{
		TUTTLE_CERR ( _color._error << boost::current_exception_diagnostic_information() << _color._std );
	}
	return 0;
}

