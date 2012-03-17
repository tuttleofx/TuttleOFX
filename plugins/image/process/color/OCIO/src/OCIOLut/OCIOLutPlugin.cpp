#include "OCIOLutPlugin.hpp"
#include "OCIOLutProcess.hpp"

#include <tuttle/common/utils/color.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace OCIO = OCIO_NAMESPACE;

namespace bfs = boost::filesystem;

namespace tuttle {
namespace plugin {
namespace ocio {
namespace lut {

using namespace boost::gil;

OCIOLutPlugin::OCIOLutPlugin(OfxImageEffectHandle handle) :
	ImageEffectGilPlugin(handle) {
	_paramFilename = fetchStringParam(kTuttlePluginFilename);
	_paramInterpolationType = fetchChoiceParam(kParamInterpolationType);

}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OCIOLutPlugin::render(const OFX::RenderArguments& args) {
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	// do the rendering
	switch (components) {
	case OFX::ePixelComponentRGBA: {
		switch (bitDepth) {
		case OFX::eBitDepthFloat: {
			doGilRender<OCIOLutProcess, false, rgba_layout_t, bits32f> (*this,
					args);
			return;
		}
		case OFX::eBitDepthUByte:
		case OFX::eBitDepthUShort:
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone: {
			BOOST_THROW_EXCEPTION( exception::Unsupported()
					<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
		}
		}
		break;
	}
	case OFX::ePixelComponentRGB: {
		switch (bitDepth) {
		case OFX::eBitDepthFloat: {
			doGilRender<OCIOLutProcess, false, rgb_layout_t, bits32f> (*this,
					args);
			return;
		}
		case OFX::eBitDepthUByte:
		case OFX::eBitDepthUShort:
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone: {
			BOOST_THROW_EXCEPTION( exception::Unsupported()
					<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
		}
		}
		break;
	}
	case OFX::ePixelComponentAlpha: //@todo support Alpha (mfe)
	case OFX::ePixelComponentCustom:
	case OFX::ePixelComponentNone: {
		BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
	}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

OCIOLutProcessParams OCIOLutPlugin::getProcessParams(
		const OfxPointD& renderScale) const {
	using namespace boost::filesystem;

	OCIOLutProcessParams params;
	EInterpolationType
			interpolationType =
					static_cast<EInterpolationType> (_paramInterpolationType->getValue());

	std::string str;
	_paramFilename->getValue(str);
	if (!bfs::exists(str)) {
		BOOST_THROW_EXCEPTION( exception::FileNotExist( )
				<< exception::filename( str ) );

		//				 ::boost::throw_exception( exception::Failed( ));
		////		throw new std::string("File doesn't exist.");
	}
	//Init the OCIO file transform
	params._fileTransform = OCIO::FileTransform::Create();
	params._fileTransform->setSrc(str.c_str());
	params._fileTransform->setInterpolation(
			getOCIOInterpolationType(interpolationType));

	//Add the file transform to the group, required by the transform process
	params._groupTransform = OCIO::GroupTransform::Create();
	params._groupTransform->push_back(params._fileTransform);

	// Create the OCIO processor for the specified transform.
	params._config = OCIO::Config::Create();

	OCIO::ColorSpaceRcPtr inputColorSpace = OCIO::ColorSpace::Create();
	inputColorSpace->setName(kOCIOInputspace.c_str());
	params._config->addColorSpace(inputColorSpace);

	OCIO::ColorSpaceRcPtr outputColorSpace = OCIO::ColorSpace::Create();
	outputColorSpace->setName(kOCIOOutputspace.c_str());

	outputColorSpace->setTransform(params._groupTransform,
			OCIO::COLORSPACE_DIR_FROM_REFERENCE);

	TUTTLE_COUT_DEBUG( tuttle::common::kColorMagenta << "Specified Transform:" << *(params._groupTransform) << tuttle::common::kColorStd <<std::endl);

	params._config->addColorSpace(outputColorSpace);

	return params;
}

}
}
}
}
