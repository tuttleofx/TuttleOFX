#include "BitDepthPlugin.hpp"
#include "BitDepthDefinitions.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/clamp.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepth {

template<class SView, class DView>
BitDepthProcess<SView, DView>::BitDepthProcess( BitDepthPlugin& instance )
	: ImageGilProcessor<DView>( instance, eImageOrientationIndependant )
	, _plugin( instance )
{
	_clipSrc = _plugin.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template<class SView, class DView>
void BitDepthProcess<SView, DView>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<DView>::setup( args );

	// source view
	_src.reset( _clipSrc->fetchImage( args.time ) );
	if( ! _src.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _src->getRowDistanceBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	_srcView = ImageGilProcessor<DView>::template getCustomView<SView>( _src.get(), _clipSrc->getPixelRod( args.time ) );
	//	this->_srcPixelRod = this->_src->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRod = _clipSrc->getPixelRod( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class SView, class DView>
void BitDepthProcess<SView, DView>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	SView src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
				   procWindowSize.x,
				   procWindowSize.y );
	DView dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
				   procWindowSize.x,
				   procWindowSize.y );

	copy_and_convert_pixels( terry::clamp_view(src), dst ); /// @todo tuttle: the clamp should be directly done by the copy_and_convert_pixels if we need one.
}

}
}
}
