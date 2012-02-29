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
	_componentsType = fetchChoiceParam( kTuttlePluginComponents );
	_compressed     = fetchBooleanParam( kParamCompressed );
}

DPXWriterProcessParams DPXWriterPlugin::getProcessParams( const OfxTime time )
{
	DPXWriterProcessParams params;

	params._filepath       = getAbsoluteFilenameAt( time );
	params._componentsType = static_cast<ETuttlePluginComponents>( _componentsType->getValue() );
	params._bitDepth       = static_cast<ETuttlePluginBitDepth>( this->_paramBitDepth->getValue() );
	params._compressed     = _compressed->getValue();

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
		{
			doGilRender<DPXWriterProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
			return;
		}
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
