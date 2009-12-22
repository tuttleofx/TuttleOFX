#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include "../half/gilHalf.hpp"
#include "EXRReaderProcess.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::gil;
namespace bfs = boost::filesystem;

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

template<class View>
EXRReaderProcess<View>::EXRReaderProcess( EXRReaderPlugin& instance )
	: tuttle::plugin::ImageGilProcessor<View>( instance ),
	tuttle::plugin::Progress( instance ),
	_plugin( instance )
{
	_filepath = instance.fetchStringParam( kInputFilename );
	assert( _filepath != NULL );
}

template<class View>
void EXRReaderProcess<View>::setupAndProcess( const OFX::RenderArguments& args )
{
	try
	{
		std::string sFilepath;
		// Fetch output image
		_filepath->getValue( sFilepath );
		if( bfs::exists( sFilepath ) )
		{
			_exrImage.reset(new Imf::InputFile(sFilepath.c_str()));
			const Imf::Header &h = _exrImage->header();
			typename Imath::V2i imageDims = h.dataWindow().size();
			imageDims.x++;
			imageDims.y++;

			double par       = _plugin.getDstClip()->getPixelAspectRatio();
			OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
			boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
			OfxRectI bounds = dst->getBounds();
			if( !dst.get() )
				throw( tuttle::plugin::ImageNotReadyException() );
			// Build destination view
			this->_dstView = interleaved_view( std::abs( bounds.x2 - bounds.x1 ), std::abs( bounds.y2 - bounds.y1 ),
			                                   static_cast<value_t*>( dst->getPixelData() ),
			                                   dst->getRowBytes() );

			// Set the render window
			this->setRenderWindow( args.renderWindow );
			// Call the base class process member
			this->process();
		}
		else
			throw tuttle::plugin::PluginException( "Unable to open : " + sFilepath );
	}
	catch( tuttle::plugin::PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void EXRReaderProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
	try
	{
		std::string filepath;
		this->_filepath->getValue( filepath );
		readImage( this->_dstView, filepath );
	}
	catch( tuttle::plugin::PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * @brief Function called to apply an anisotropic blur
 *
 * @param[out]  dst     Destination image view
 * @param[in]   amplitude     Amplitude of the anisotropic blur
 * @param dl    spatial discretization.
 * @param da    angular discretization.
 * @param gauss_prec    precision of the gaussian function
 * @param fast_approx   Tell to use the fast approximation or not.
 *
 * @return Result view of the blurring process
 */
template<class View>
View& EXRReaderProcess<View>::readImage( View& dst, std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace std;
	using namespace boost;
	using namespace mpl;
	using namespace boost::gil;
	using namespace Imf;

	Imf::InputFile in( filepath.c_str() );
	Imf::FrameBuffer frameBuffer;
	const Imf::Header &header = in.header();
	const Imath::Box2i & dw = header.dataWindow();
	typename Imath::V2i imageDims = dw.size();
	imageDims.x++;	// Width
	imageDims.y++;	// Height

	// Get number of output components
	switch( _outComponents->getValue() )
	{
		case 0:
		{
			// Copy 1 channel starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 1 );
			break;
		}
		// RGB
		case 1:
		{
			// Copy 3 channels starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 3 );
			break;
		}
		// RGBA
		case 2:
		{
			// Copy 4 channels starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 4 );
			break;
		}
	}
	return dst;
}

template<class View>
void EXRReaderProcess<View>::channelCopy(Imf::InputFile & input,
										 Imf::FrameBuffer & frameBuffer,
										 View & dst, int w, int h, 
										 int n, int left)
{
	const Imf::Header & header = input.header();
	const Imath::Box2i & dw = header.dataWindow();
	if (left) {
		const Imf::ChannelList & cl( header.channels() );
		const Imf::Channel & ch = cl[ _plugin.channelNames()[ _plugin.channelChoice()[n]->getValue() ].c_str() ];
		switch( ch.type )
		{
			case Imf::HALF:
			{
				//@todo: check: this may bug: swap w and h
				Imf::Array2D<half> pixels(w, h);
				frameBuffer.insert (_plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
									Imf::Slice( ch.type,
												(char *) (&pixels[0][0] - // base
												dw.min.x -
												dw.min.y * w),
												sizeof (pixels[0][0]) * 1, // xStride
												sizeof (pixels[0][0]) * w, // yStride
												1, 1, // x/y sampling
												0.0 )
									); // fillValue
				channelCopy(input, frameBuffer, dst, w, h, ++n, --left);
				break;
			}
			case Imf::FLOAT:
			{
				//@todo: check: this may bug: swap w and h
				Imf::Array2D<float> pixels(w, h);
				frameBuffer.insert (_plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
									Imf::Slice( ch.type,
												(char *) (&pixels[0][0] - // base
												dw.min.x -
												dw.min.y * w),
												sizeof (pixels[0][0]) * 1, // xStride
												sizeof (pixels[0][0]) * w, // yStride
												1, 1, // x/y sampling
												0.0 )
									); // fillValue
				channelCopy(input, frameBuffer, dst, w, h, ++n, --left);
				break;
			}
			default:
			{
				//@todo: check: this may bug: swap w and h
				Imf::Array2D<boost::uint32_t> pixels(w, h);
				frameBuffer.insert (_plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
									Imf::Slice( ch.type,
												(char *) (&pixels[0][0] - // base
												dw.min.x -
												dw.min.y * w),
												sizeof (pixels[0][0]) * 1, // xStride
												sizeof (pixels[0][0]) * w, // yStride
												1, 1, // x/y sampling
												0.0 )
									); // fillValue
				channelCopy(input, frameBuffer, dst, w, h, ++n, --left);
				break;
			}
		}
	} else {
		input.setFrameBuffer(frameBuffer);
		input.readPixels(dw.min.y, dw.max.y);
		for(int s = 0; s < n; ++s) {
			const Imf::Slice *slice =
				frameBuffer.findSlice(
					_plugin.channelNames()[_plugin.channelChoice()[s]->getValue()].c_str() );
			sliceCopy(slice, dst, s);
		}
	}
}

template<class View>
void EXRReaderProcess<View>::sliceCopy(const Imf::Slice *slice, View & dst, int n)
{
	int w = slice->xStride + 1;
	int h = slice->yStride + 1;
	typedef typename View::value_type dPix_t;
	switch( slice->type )
	{
		case Imf::HALF:
		{
			gray16h_view_t vw( interleaved_view( w, h, (gray16h_view_t::value_type*)slice->base, w * sizeof(boost::uint16_t) ) );
			copy_and_convert_pixels(vw, nth_channel_view(dst, n));
			break;
		}
		case Imf::FLOAT:
		{
			gray32f_view_t vw( interleaved_view( w, h, (gray32f_view_t::value_type*)slice->base, w * sizeof(float) ) );
			copy_and_convert_pixels(vw, nth_channel_view(dst, n));
			break;
		}
		default:
		{
			gray32_view_t vw( interleaved_view( w, h, (gray32_view_t::value_type*)slice->base, w * sizeof(boost::uint32_t) ) );
			copy_and_convert_pixels(vw, nth_channel_view(dst, n));
			break;
		}
	}
}

}
}
}
}
