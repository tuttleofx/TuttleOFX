#ifndef _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_
#define _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_

#include "OCIOLutDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <OpenColorIO/OpenColorIO.h>

namespace tuttle {
namespace plugin {
namespace ocio {
namespace lut {

/**
 * @brief
 *
 */
class OCIOLutPlugin: public ImageEffectGilPlugin {

public:
	OCIOLutPlugin(OfxImageEffectHandle handle);

public:
	void render(const OFX::RenderArguments& args);
	void changedParam(const OFX::InstanceChangedArgs& args,
			const std::string& paramName);

public:
	OFX::StringParam* _sFilename;
	OFX::ChoiceParam* _interpolationType;
	OCIO_NAMESPACE::FileTransformRcPtr _fileTransform;
	OCIO_NAMESPACE::GroupTransformRcPtr _groupTransform;
	OCIO_NAMESPACE::ConfigRcPtr _config;

	EInterpolationType getInterpolationType() const { return static_cast<EInterpolationType>( _interpolationType->getValue() ); }

	OCIO_NAMESPACE::Interpolation getOCIOInterpolationType(EInterpolationType tuttleInterpolationType) const{
		switch (tuttleInterpolationType) {
			case eInterpolationTypeNearest:
				return OCIO_NAMESPACE::INTERP_NEAREST;
			case eInterpolationTypeTetrahedral:
				return OCIO_NAMESPACE::INTERP_TETRAHEDRAL;
			case eInterpolationTypeLinear:
			default:
				return OCIO_NAMESPACE::INTERP_LINEAR;
		}

	}
};

}
}
}
}
#endif
