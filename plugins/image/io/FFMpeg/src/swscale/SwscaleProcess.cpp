#include "SwscaleProcess.hpp"

namespace tuttle {
namespace plugin {
namespace swscale {

SwscaleProcess::SwscaleProcess( SwscalePlugin &effect )
: ImageFilterProcessor( effect, eImageOrientationFromTopToBottom )
, _plugin( effect )
, _context( NULL )
{
	this->setNoMultiThreading();
}


void SwscaleProcess::setup( const OFX::RenderArguments& args )
{
	ImageFilterProcessor::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

PixelFormat ofxPixelComponentToSwsPixelFormat( const OFX::EPixelComponent component )
{
	switch( component )
	{
		case OFX::ePixelComponentRGBA:
			return PIX_FMT_RGBA;
		case OFX::ePixelComponentRGB:
			return PIX_FMT_RGB24;
		case OFX::ePixelComponentAlpha:
			return PIX_FMT_GRAY8;
		case OFX::ePixelComponentNone:
		case OFX::ePixelComponentCustom:
			break;
	}
	BOOST_ASSERT( false );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
void SwscaleProcess::multiThreadProcessImages( const OfxRectI& procWindow )
{
	OFX::EPixelComponent component = this->_src->getPixelComponents();
	PixelFormat pixFmt = ofxPixelComponentToSwsPixelFormat( component );
	OFX::EBitDepth bitDepth = this->_src->getPixelDepth();
	
	_context = sws_getCachedContext( _context,
			this->_src->getBoundsSize().x, this->_src->getBoundsSize().y, 
			pixFmt, ///@todo depending on the data
			this->_dst->getBoundsSize().x, this->_dst->getBoundsSize().y,
			pixFmt, ///@todo depending on the data
			_params._sws_filter, 
			NULL, NULL, NULL );
	if ( !_context )
	{
		std::cerr << "swscale: Could not get swscale context" << std::endl;
		return;
	}

	// set filtering mode
	// @todo: sws set filtermode( _params._sws_filter )
	
	// for tile support... (currently disabled)
//	void* srcPtr = this->_src->getPixelAddress( procWindow.x1, procWindow.y1 );
//	void* dstPtr = this->_dst->getPixelAddress( procWindow.x1, procWindow.y1 );
	
	// for simple buffer access
	uint8_t* srcPtr = (uint8_t*)this->_src->getPixelData();
	uint8_t* dstPtr = (uint8_t*)this->_dst->getPixelData();
	if ( !srcPtr || !dstPtr )
	{
		std::cerr << "swscale: Pixel data pointer invalid" << std::endl;
		return;
	}
	
	// "this->_src->getRowDistanceBytes()" is not the same than "this->_src->getBoundsSize().x * pixelBytes"
	// The buffer could contain a padding between lines.
	
//	uint8_t *rgb_src[4] = { rgb_data, NULL, NULL, NULL };
//	int rgb_stride[4]   = { 4 * W, 0, 0, 0 };
//	uint8_t *src[4]     = { data, data + W * H, data + W * H * 2, data + W * H * 3 };
//	int stride[4]       = { W, W, W, W };
	
	int srcStride = this->_src->getRowDistanceBytes();
	int dstStride = this->_dst->getRowDistanceBytes();
	int ret = sws_scale( _context, &srcPtr, &srcStride, 0,
			this->_src->getBoundsSize().y, &dstPtr, &dstStride );
	if ( ret < 0 )
		std::cerr << "swscale: Scaling failed (" << ret << ")" << std::endl;
}

}
}
}
