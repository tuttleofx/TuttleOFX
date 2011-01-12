#include "BitDepthPlugin.hpp"
#include "BitDepthDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/clamp.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>

#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace bitDepth {

template<class SView, class DView>
BitDepthProcess<SView, DView>::BitDepthProcess( BitDepthPlugin& instance )
	: ImageGilProcessor<DView>( instance )
	, _plugin( instance )
{
	_clipSrc = _plugin.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template<class SView, class DView>
void BitDepthProcess<SView, DView>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<DView>::setup( args );

	// source view
	this->_src.reset( this->_clipSrc->fetchImage( args.time ) );
	if( !this->_src.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( this->_src->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcView = ::tuttle::plugin::getView<SView>( this->_src.get(), this->_clipSrc->getPixelRod( args.time ) );
	//	this->_srcPixelRod = this->_src->getRegionOfDefinition(); // bug in nuke, returns bounds
	this->_srcPixelRod = this->_clipSrc->getPixelRod( args.time );
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

	copy_and_convert_pixels( clamp_view(src), dst );
}

}
}
}
