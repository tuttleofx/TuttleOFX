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
			pixFmt,
			this->_dst->getBoundsSize().x, this->_dst->getBoundsSize().y,
			pixFmt,
			_params._sws_filter,
			NULL, NULL, NULL );
	if ( !_context )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user( "swscale: Could not get swscale context." ) );
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
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user( "swscale: Pixel data pointer invalid." ) );
	}

	// "this->_src->getRowDistanceBytes()" is not the same than "this->_src->getBoundsSize().x * pixelBytes"
	// The buffer could contain a padding between lines.

	int srcStride = this->_src->getRowDistanceBytes();
	int dstStride = this->_dst->getRowDistanceBytes();
	int ret = sws_scale( _context, &srcPtr, &srcStride, 0,
			this->_src->getBoundsSize().y, &dstPtr, &dstStride );
	if ( ret < 0 )
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user( ( "swscale: Scaling failed (" + ret + ")" ) ) );
}

}
}
}
