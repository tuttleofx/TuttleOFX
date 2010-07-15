#include "Jpeg2000ReaderPlugin.hpp"
#include <tuttle/plugin/image/gil/typedefs.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {

template<class View>
Jpeg2000ReaderProcess<View>::Jpeg2000ReaderProcess( Jpeg2000ReaderPlugin &instance )
: ImageGilProcessor<View>( instance )
, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
Jpeg2000ReaderProcess<View>::~Jpeg2000ReaderProcess()
{
	_plugin.getReader().close();
}

template<class View>
void Jpeg2000ReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	// Fetch output image
//	Jpeg2000ReaderProcessParams params = _plugin.getProcessParams(args.time);

	boost::gil::point2<ptrdiff_t> imageDims( _plugin.getReader().width(),
                                             _plugin.getReader().height() );

	double par       = _plugin._clipDst->getPixelAspectRatio();
	OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
	this->_dst.reset( _plugin._clipDst->fetchImage( args.time, reqRect ) );
	if( !this->_dst.get() )
	{
		BOOST_THROW_EXCEPTION( tuttle::plugin::ImageNotReadyException() );
	}
	if( this->_dst->getRowBytes( ) <= 0 )
	{
		BOOST_THROW_EXCEPTION( WrongRowBytesException( ) );
	}

	// Build destination view
	this->_dstView = this->getView( this->_dst.get(), _plugin._clipDst->getPixelRod(args.time) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void Jpeg2000ReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	switch(_plugin.getReader().components())
	{
		case 1:
		{
			switchLayoutCopy<gray_layout_t>();
			break;
		}
		case 3:
		{
			switchLayoutCopy<rgb_layout_t>();
			break;
		}
		case 4:
		{
			switchLayoutCopy<rgba_layout_t>();
			break;
		}
		default:
		{
			break;
		}
	}
}

template<class View>
template<class Layout>
void Jpeg2000ReaderProcess<View>::switchLayoutCopy()
{
	using namespace boost::gil;

	switch(_plugin.getReader().precision())
	{
		case 8:
		{
			typedef pixel<bits8, Layout > PixelT;
			switchPrecisionCopy<PixelT>( flipped_up_down_view( this->_dstView ) );
			break;
		}
		case 12:
		{
			typedef pixel<bits12, Layout > PixelT;
			switchPrecisionCopy<PixelT>(flipped_up_down_view( this->_dstView ));
			break;
		}
		case 16:
		{
			typedef pixel<bits16, Layout > PixelT;
			switchPrecisionCopy<PixelT>(flipped_up_down_view( this->_dstView ));
			break;
		}
		case 32:
		{
			typedef pixel<bits32, Layout > PixelT;
			switchPrecisionCopy<PixelT>(flipped_up_down_view( this->_dstView ));
			break;
		}
		default:
			BOOST_THROW_EXCEPTION( tuttle::plugin::PluginException("Unhandled image precision!") );
			break;
	}
}

template<class View>
template<class WorkingPixel>
void Jpeg2000ReaderProcess<View>::switchPrecisionCopy(const View & wv)
{
	using namespace boost::gil;
	tuttle::io::J2KReader & reader = _plugin.getReader();
	int w = reader.width();
	int h = reader.height();

	unsigned int *data[num_channels<WorkingPixel>::type::value];
	for(int i = 0; i < num_channels<WorkingPixel>::type::value; ++i)
	{
		data[i] = (unsigned int*)reader.compData(i);
	}
	WorkingPixel pix;

	for( typename View::y_coord_t y = 0; y < h; ++y )
	{
		typename View::x_iterator it = wv.row_begin( y );

		for( typename View::x_coord_t x = 0; x < w; ++x )
		{
			for(int i = 0; i < num_channels<WorkingPixel>::type::value; ++i)
			{
				pix[i] = *(data[i]);
				++data[i];
			}
			color_convert(pix, *it);
			++it;
		}
	}
}

}
}
}
}
