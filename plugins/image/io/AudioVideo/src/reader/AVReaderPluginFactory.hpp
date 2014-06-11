#ifndef _TUTTLE_PLUGIN_AV_READER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

static const bool kSupportTiles = false;

mDeclarePluginFactory( AVReaderPluginFactory, {}, {} );

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, void* av_class, int req_flags, int rej_flags );

}
}
}
}

#endif
