#include "DPXWriterDefinitions.hpp"
#include "DPXWriterPlugin.hpp"

#include <boost/cstdint.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

template<class View>
DPXWriterProcess<View>::DPXWriterProcess( DPXWriterPlugin& instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
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
	try
	{
		DPXWriterParams params = _plugin.getParams(this->_renderArgs.time);
		int packing = params._compressed == false;

		switch( params._bitDepth )
		{
			case 3: {
				switch( params._componentsType )
				{
					case 0: {
						writeImage<rgb16_image_t>( this->_srcView, params._filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16, packing );
						break;
					}
					case 1: {
						writeImage<rgba16_image_t>( this->_srcView, params._filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16A16, packing );
						break;
					}
					case 2: {
						writeImage<abgr16_image_t>( this->_srcView, params._filepath, 16, tuttle::io::DpxImage::eCompTypeA16B16G16R16, packing );
						break;
					}
				}
				break;
			}
			case 2: {
				switch( params._componentsType )
				{
					case 0:
						///@todo to get it working:
						// swap half byte then swap shorts (16 bits)
						// ex: 0405 -> 4050 -> 5040
						// or: roll short by 4 bits right.
						//						writeImage<rgb12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12, packing );
						throw( PluginException( "DPX Writer: Unsupported 12 bits rgb image..." ) );
						break;
					case 1:
						//						writeImage<rgba12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12A12, packing );
						throw( PluginException( "DPX Writer: Unsupported 12 bits rgba image..." ) );
						break;
					case 2:
						//						writeImage<abgr12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeA12B12G12R12, packing );
						throw( PluginException( "DPX Writer: Unsupported 12 bits abgr image..." ) );
						break;
				}
				break;
			}
			case 1: {
				switch( params._componentsType )
				{
					case 0:
						//						writeImage<rgb10_packed_image_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10, packing );
						throw( PluginException( "DPX Writer: Unsupported 10 bits rgb image..." ) );
						break;
					case 1:
						// Unsupported
						//						writeImage<rgba10_view_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, packing );
						throw( PluginException( "DPX Writer: Unsupported 10 bits rgba image..." ) );
						break;
					case 2:
						//						writeImage<abgr10_view_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeA10B10G10R10, packing );
						throw( PluginException( "DPX Writer: Unsupported 10 bits abgr image..." ) );
						break;
				}
				break;
			}
			case 0: {
				switch( params._componentsType )
				{
					case 0:
						writeImage<rgb8_image_t>( this->_srcView, params._filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8, packing );
						break;
					case 1:
						writeImage<rgba8_image_t>( this->_srcView, params._filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8A8, packing );
						break;
					case 2:
						writeImage<abgr8_image_t>( this->_srcView, params._filepath, 8, tuttle::io::DpxImage::eCompTypeA8B8G8R8, packing );
						break;
				}
				break;
			}
		}
	}
	catch( PluginException& err )
	{
		COUT_EXCEPTION( err );
	}
}

/**
 */
template<class View>
template<class CONV_IMAGE>
void DPXWriterProcess<View>::writeImage( View& src, std::string& filepath, int bitDepth, tuttle::io::DpxImage::EDPX_CompType eCompType, int packing ) throw( tuttle::plugin::PluginException )
{
	View flippedView = flipped_up_down_view( src );
	CONV_IMAGE img( src.width(), src.height() );

	typename CONV_IMAGE::view_t vw( view( img ) );
	copy_and_convert_pixels( flippedView, vw );
	boost::uint8_t* pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );
	// Little endian
	_dpxHeader.setBigEndian( false );
	// bitDepth
	_dpxHeader.setBitSize( bitDepth );
	// 1 rgb? element
	_dpxHeader.setElementNumber( 1 );
	_dpxHeader.setElementOffset( sizeof( tuttle::io::DpxHeader ), 0 );
	_dpxHeader.setWidth( src.width() );
	_dpxHeader.setHeight( src.height() );
	// Uncompressed by default
	_dpxHeader.setPacking( packing );
	_dpxHeader.setDescriptor( _dpxImg.compTypeToDescriptor( eCompType ) );
	_dpxImg.setHeader( _dpxHeader );
	_dpxImg.setData( pData, false );
	_dpxImg.write( filepath );
}

}
}
}
}
