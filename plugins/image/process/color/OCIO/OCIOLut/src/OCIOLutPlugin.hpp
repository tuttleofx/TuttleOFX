#ifndef _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_
#define _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_

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
	OFX::StringParam* _sFilename; ///< Filename
	OCIO_NAMESPACE::FileTransformRcPtr _fileTransform;
	OCIO_NAMESPACE::GroupTransformRcPtr _groupTransform;
	OCIO_NAMESPACE::ConfigRcPtr _config;
};

}
}
}
}
#endif
