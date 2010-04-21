/**
 * @file EXRWriterPlugin.cpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#include "EXRWriterPlugin.hpp"
#include "EXRWriterProcess.hpp"
#include "EXRWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

using namespace boost::gil;

EXRWriterPlugin::EXRWriterPlugin( OfxImageEffectHandle handle )
: WriterPlugin( handle )
{
	_componentsType = fetchChoiceParam( kParamComponentsType );
}

EXRWriterProcessParams EXRWriterPlugin::getParams(const OfxTime time)
{
	EXRWriterProcessParams params;
	params._bitDepth = (EParamBitDepth)this->_bitDepth->getValue();
	params._componentsType = (ECompType)_componentsType->getValue();
	params._filepath = _fPattern.getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _renderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _dstClip->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					EXRWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					EXRWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					EXRWriterProcess<rgba32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone:
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom:
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			}
		}
		else
		{
			COUT_FATALERROR( "Pixel component unrecognize ! (" << mapPixelComponentEnumToStr( dstComponents ) << ")" );
		}
	}
}

}
}
}
}
