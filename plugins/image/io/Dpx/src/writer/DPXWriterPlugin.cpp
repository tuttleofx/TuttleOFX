#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <dpxEngine/dpxImage.hpp>

#include <boost/filesystem.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_componentsType = fetchChoiceParam( kParamComponentsType );
	_compressed     = fetchBooleanParam( kParamCompressed );
}

DPXWriterProcessParams DPXWriterPlugin::getProcessParams( const OfxTime time )
{
	DPXWriterProcessParams params;

	params._componentsType = _componentsType->getValue();
	params._compressed     = _compressed->getValue();
	params._filepath       = getAbsoluteFilenameAt( time );
	switch( static_cast<EParamBitDepth>( this->_paramBitDepth->getValue() ) )
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth10:
			params._bitDepth = 10;
			break;
		case eParamBitDepth12:
			params._bitDepth = 12;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		default:
			BOOST_THROW_EXCEPTION( exception::Value()
				<< exception::user() + "Incorrect bit depth." );
			break;
	}
	params._flip = _paramFlip->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<DPXWriterProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
}

}
}
}
}
