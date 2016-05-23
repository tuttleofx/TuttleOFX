#ifndef _TUTTLE_PLUGIN_IMAGE_HPP_
#define _TUTTLE_PLUGIN_IMAGE_HPP_

namespace tuttle
{
namespace plugin
{

enum EImageOrientation
{
    eImageOrientationIndependant,     //< Use memory order if your process is idenpendant image order
    eImageOrientationFromTopToBottom, //< Use image orientation from top to bottom
    eImageOrientationFromBottomToTop  //< Use image orientation from bottom to top
};
}
}

#endif
