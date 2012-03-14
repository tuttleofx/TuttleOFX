#include "DPXWriterDefinitions.hpp"
#include "DPXWriterPlugin.hpp"

#include <terry/clamp.hpp>
#include <terry/typedefs.hpp>

#include <boost/exception/errinfo_file_name.hpp>
#include <boost/assert.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

template<class View>
DPXWriterProcess<View>::DPXWriterProcess( DPXWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void DPXWriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void DPXWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	BOOST_ASSERT( this->_srcPixelRod == this->_dstPixelRod );

	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );

	View src = this->_srcView;

	copy_pixels( src, this->_dstView );
/*
	::dpx::Writer writer;
	OutStream stream;

	if( ! stream.Open( _params._filepath.c_str() ) )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "Dpx: Unable to open output file" ) );
	}

	writer.SetOutStream( &stream );

	writer.Start();

	writer.SetFileInfo( _params._filepath.c_str(), 0, "TuttleOFX DPX Writer", "project", "copyright", ~0, true );

	writer.SetImageInfo( src.width(), src.height() );

	int bitDepth = 0;

	switch ( _params._bitDepth )
	{
		case eTuttlePluginBitDepth8: bitDepth = 8; break;
		case eTuttlePluginBitDepth10: bitDepth = 10; break;
		case eTuttlePluginBitDepth12: bitDepth = 12; break;
		case eTuttlePluginBitDepth16: bitDepth = 16; break;
		case eTuttlePluginBitDepth32: bitDepth = 32; break;
		case eTuttlePluginBitDepth64: bitDepth = 64; break;
	}

	writer.SetElement( 0,
			_params._descriptor,
			bitDepth,
			_params._transfer,
			_params._colorimetric,
			_params._packed,
			_params._encoding );

	if( ! writer.WriteHeader() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX Header)" ) );
	}

	switch ( _params._bitDepth )
	{
		case eTuttlePluginBitDepth8:  writeImage<rgb8_pixel_t>( src, writer ); break;
		case eTuttlePluginBitDepth10: writeImage<rgb16_pixel_t>( src, writer ); break;
		case eTuttlePluginBitDepth12: writeImage<rgb16_pixel_t>( src, writer ); break;
		case eTuttlePluginBitDepth16: writeImage<rgb16_pixel_t>( src, writer ); break;
		case eTuttlePluginBitDepth32: writeImage<rgb32_pixel_t>( src, writer );; break;
		case eTuttlePluginBitDepth64: writeImage<rgb64_pixel_t>( src, writer ); break;
	}

	//writer.header.SetImageOffset( 0x00000800 );

	if( ! writer.Finish() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX finish)" ) );
	}
*/
}

/*

template<class View>
template<class WPixel>
void DPXWriterProcess<View>::writeImage( View& src, ::dpx::Writer& writer )
{
	using namespace terry;

	typedef image<WPixel, true> image_t;
	typedef typename image_t::view_t view_t;
	image_t img( src.width(), src.height() );
	view_t  dvw( view( img ) );
	copy_and_convert_pixels( clamp_view( src ), dvw );


	long count = 0;
	int channels = num_channels<view_t>::value;

	switch( _params._bitDepth )
	{
		case eTuttlePluginBitDepth8:
			count = channels * src.width() * src.height();
			break;
		case eTuttlePluginBitDepth16:
			count = 2.0 * channels * src.width() * src.height();
			break;
		case eTuttlePluginBitDepth32:
			count = 4.0 * channels * src.width() * src.height();
			break;
		case eTuttlePluginBitDepth64:
			count = 8.0 * channels * src.width() * src.height();
			break;
		case eTuttlePluginBitDepth10:
		case eTuttlePluginBitDepth12:
			count = 4 * src.width();// * src.height();
			break;
	}
	TUTTLE_COUT( count << "  " <<  channels << "  " <<  src.width());


	char* buffer = new char[count];

	for( int y = 0; y < src.height(); ++y )
	{
		View srcLineV = subimage_view( this->_srcView, 0, y, src.width(), 1 );

		std::memcpy(  );

		if( ! writer.WriteElement( 0, interleaved_view_get_raw_data( srcLineV ), ::dpx::kByte ) )
		{
			BOOST_THROW_EXCEPTION( exception::Data()
				<< exception::user( "Dpx: Unable to write data (DPX User Data)" ) );
		}
	}

	if( ! writer.WriteElement( 0, interleaved_view_get_raw_data( src ), ::dpx::kByte ) )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX User Data)" ) );
	}

}
*/


}
}
}
}
