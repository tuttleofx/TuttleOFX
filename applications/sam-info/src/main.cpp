#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Magick++.h>
#include <algorithm>
#include <iostream>
#include <iterator>

#define FIRST_COLUMN_WIDTH 23

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
namespace bal = boost::algorithm;
namespace ttl = tuttle::common;

bool	colorOutput	= false;
bool	verbose		= false;

int	firstImage	= 0;
int	lastImage	= 0;


void printImageProperties( std::string path )
{
	try
	{
		Magick::Image image;
		// Read a file into image object
		image.read( path );
		bool alphaChannel = false;
		std::string imageType;
		switch( image.type() )
		{
			case Magick::UndefinedType			: imageType = "Undefined type of image"; break;
			case Magick::BilevelType			: imageType = "Bilevel image"; break;
			case Magick::GrayscaleType			: imageType = "Grayscale image"; break;
			case Magick::GrayscaleMatteType			: imageType = "Grayscale image with opacity"; alphaChannel=true; break;
			case Magick::PaletteType			: imageType = "Indexed color image"; break;
			case Magick::PaletteMatteType			: imageType = "Indexed color image with opacity"; alphaChannel=true; break;
			case Magick::TrueColorType			: imageType = "Truecolor image"; break;
			case Magick::TrueColorMatteType			: imageType = "Truecolor image with opacity"; alphaChannel=true; break;
			case Magick::ColorSeparationType		: imageType = "Cyan/Yellow/Magenta/Black (CYMK) image"; break;
			case Magick::ColorSeparationMatteType		: imageType = "Cyan/Yellow/Magenta/Black (CYMK) image with opacity"; alphaChannel=true; break;
			case Magick::OptimizeType			: imageType = "Optimize image"; break;
			case 11/*Magick::PaletteBilevelMatteType*/	: imageType = "Indexed bilevel image with opacity"; alphaChannel=true; break;
		}

		std::string resolutionType;
		switch( image.resolutionUnits() )
		{
			case Magick::UndefinedResolution		: resolutionType = " [unknown units]"; break;
			case Magick::PixelsPerInchResolution		: resolutionType = " [dpi]"; break;
			case Magick::PixelsPerCentimeterResolution	: resolutionType = " [pixels/cm}"; break;
		}

		std::string colorSpaceType;
		switch( image.colorSpace() )
		{
			case Magick::UndefinedColorspace		: colorSpaceType = "unknown color space"; break;
			case Magick::RGBColorspace			: colorSpaceType = "RGB"; break;
			case Magick::GRAYColorspace			: colorSpaceType = "Gray"; break;
			case Magick::TransparentColorspace		: colorSpaceType = "Transparent"; break;
			case Magick::OHTAColorspace			: colorSpaceType = "OHTA"; break;
			case Magick::XYZColorspace			: colorSpaceType = "XYZ"; break;
			case Magick::YCbCrColorspace			: colorSpaceType = "Y Cb Cr"; break;
			case Magick::YCCColorspace			: colorSpaceType = "YCC"; break;
			case Magick::YIQColorspace			: colorSpaceType = "YIQ"; break;
			case Magick::YPbPrColorspace			: colorSpaceType = "Y Pb Pr"; break;
			case Magick::YUVColorspace			: colorSpaceType = "YUV"; break;
			case Magick::CMYKColorspace			: colorSpaceType = "CMYK"; break;
			case Magick::sRGBColorspace			: colorSpaceType = "sRGB"; break;
			case Magick::LabColorspace			: colorSpaceType = "Lab"; break;
			case 14/*Magick::HSBColorspace*/		: colorSpaceType = "HSB"; break;
			case Magick::HSLColorspace			: colorSpaceType = "HSL"; break;
			case Magick::HWBColorspace			: colorSpaceType = "HWB"; break;
			case Magick::Rec601LumaColorspace		: colorSpaceType = "Rec601 Luma"; break;
			case 18/*Magick::Rec601YCbCrColorspace*/	: colorSpaceType = "Rec601 Y Cb Cr"; break;
			case Magick::Rec709LumaColorspace		: colorSpaceType = "Rec709 Luma"; break;
			case 20/* Magick::Rec709YCbCrColorspace*/	: colorSpaceType = "Rec709 Y Cb Cr"; break;
			case Magick::LogColorspace			: colorSpaceType = "Log"; break;
			case 22/*Magick::CMYColorspace*/		: colorSpaceType = "CMY"; break;
		}

		std::string interlaceType;
		switch( image.interlaceType() )
		{
			case Magick::UndefinedInterlace		: interlaceType = "undefined"; break;
			case Magick::NoInterlace		: interlaceType = "no interlacing"; break;
			case Magick::LineInterlace		: interlaceType = "line interlacing"; break;
			case Magick::PlaneInterlace		: interlaceType = "plane interlacing"; break;
			case Magick::PartitionInterlace		: interlaceType = "partition interlacing"; break;
			case 5/*Magick::GIFInterlace*/		: interlaceType = "GIF interlacing"; break;
			case 6/*Magick::JPEGInterlace*/		: interlaceType = "Jpeg interlacing"; break;
			case 7/*Magick::PNGInterlace*/		: interlaceType = "PNG interlacing"; break;
		}

		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "width"			<< image.size().width()				);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "height"			<< image.size().height()			);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "bit-depth"		<< image.depth()	<< " bits"		);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "compression quality"	<< image.quality()				);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "image type"		<< imageType					);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "" );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "x resolution"		<< image.xResolution()	<< resolutionType	);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "y resolution"		<< image.yResolution()	<< resolutionType	);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "interlacing"		<< interlaceType				);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "" );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "format"			<< image.format()				);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "channels"		<< colorSpaceType << ( alphaChannel ? std::string("A") : "" ) );
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "color space"		<< colorSpaceType				);
		TUTTLE_COUT( std::setw(FIRST_COLUMN_WIDTH) << "gamma"			<< image.gamma()				);

		TUTTLE_COUT( "" );
	}
	catch( Magick::Exception &error_ )
	{
		//TUTTLE_COUT( "Caught exception: " << error_.what() );
		TUTTLE_COUT( kColorError << "Unrecognized file like an image" << kColorStd << "\n" );
	}
	catch( ... )
	{
		TUTTLE_COUT( "Caught exception" << "\n" );;
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

	int					researchMask		= eMaskTypeSequence;	// by default show sequences
	EMaskOptions				descriptionMask		= eMaskOptionsColor;	// by default show nothing
	bool					recursiveListing	= false;
	std::string				availableExtensions;
	std::vector<std::string>		paths;
	std::vector<std::string>		filters;

	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("all,a"		, "do not ignore entries starting with .")
		("expression,e"		, bpo::value<std::string>(), "remove with a specific pattern, ex: *.jpg,*.png")
		("files,f"		, "informations about files in path(s)")
		("help,h"		, "show this help")
		("mask,m"		, "not remove sequences in path(s)")
		("path-root,p"		, "show the root path for each objects")
		("recursive,R"		, "remove subdirectories recursively")
		("color"		, "color the outup")
		("first-image"		, bpo::value<unsigned int>(), "specify the first image")
		("last-image"		, bpo::value<unsigned int>(), "specify the last image")
	;

	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		("input-dir", bpo::value< std::vector<std::string> >(), "input directories")
	;

	// define default options
	bpo::positional_options_description pod;
	pod.add("input-dir", -1);

	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);

	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

	// get environnement options and parse them
	if( std::getenv("SAM_INFO_OPTIONS") != NULL)
	{
	    std::vector<std::string> envOptions;
	    std::string env = std::getenv("SAM_INFO_OPTIONS");
	    envOptions.push_back( env );
	    bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
	}

	bpo::notify(vm);

	if (vm.count("help"))
	{
	    TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
	    TUTTLE_COUT( "NAME");
	    TUTTLE_COUT( "\tsam-info - remove directory contents\n" );
	    TUTTLE_COUT( "SYNOPSIS" );
	    TUTTLE_COUT( "\tsam-info [options] [sequences]\n" );
	    TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
	    return 1;
	}

	if (vm.count("expression"))
	{
	    bal::split( filters, vm["expression"].as<std::string>(), bal::is_any_of(","));
	}

	if (vm.count("directories"))
	{
	    researchMask |= eMaskTypeDirectory;
	}

	if (vm.count("files"))
	{
	    researchMask |= eMaskTypeFile;
	}

	if (vm.count("mask"))
	{
		researchMask &= ~eMaskTypeSequence;
	}

	if (vm.count("verbose"))
	{
		verbose = true;
	}

	if (vm.count("first-image"))
	{
		firstImage  = vm["first-image"].as< unsigned int >();
	}

	if (vm.count("last-image"))
	{
		lastImage  = vm["last-image"].as< unsigned int >();
	}

	if (vm.count("full-rm"))
	{
		researchMask |= eMaskTypeDirectory;
		researchMask |= eMaskTypeFile;
		researchMask |= eMaskTypeSequence;
	}

	if (vm.count("all"))
	{
		// add .* files
		descriptionMask |= eMaskOptionsDotFile;
	}

	if (vm.count("path-root"))
	{
		 descriptionMask |= eMaskOptionsPath;
	}

	if (vm.count("color") )
	{
		colorOutput = true;
		descriptionMask |=  eMaskOptionsColor;
	}

	// defines paths, but if no directory specify in command line, we add the current path
	if (vm.count("input-dir"))
	{
		paths = vm["input-dir"].as< std::vector<std::string> >();
	}
	else
	{
		paths.push_back( "./" );
	}

	if (vm.count("recursive"))
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
								std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)*dir, researchMask, descriptionMask, filters );
								getImageProperties( listing );
							}
						}
					}
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path, researchMask, descriptionMask, filters );
					getImageProperties( listing );
				}
				else
				{
					if( std::strcmp( path.branch_path().string().c_str(),"" ) == 0 )
						path = "."/path.leaf();
					//TUTTLE_COUT( "is NOT a directory "<< path.branch_path() << " | "<< path.leaf() );
					filters.push_back( path.leaf().string() );
					std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir( (bfs::path)path.branch_path(), researchMask, descriptionMask, filters );
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
					TUTTLE_CERR ( "Unrecognized pattern \"" << path << "\"" );
				}
			}
		}
	}
	catch (bfs::filesystem_error &ex)
	{
		TUTTLE_COUT( ex.what() );
	}
	catch(... )
	{
		TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}
	return 0;
}

