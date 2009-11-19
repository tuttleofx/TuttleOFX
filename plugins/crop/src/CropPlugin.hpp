/**
 * @file CropPlugin.hpp
 * @brief
 * @author
 * @date    01/10/09 14:14
 *
 */

#ifndef VOLET_PLUGIN_H
#define VOLET_PLUGIN_H

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

static const std::string kParamUp           = "Up";
static const std::string kParamDown         = "Down";
static const std::string kParamLeft         = "Left";
static const std::string kParamRight        = "Right";
static const std::string kParamFormats      = "Pre-defined formats";
static const std::string kParamDisplayRect  = "Display overlay rectangle";
static const std::string kParamAnamorphic  = "Anamorphic";
static const int k1_1_33 = 0;
static const int k1_1_77 = 1;
static const int k1_1_85 = 2;
static const int k1_2_35 = 3;
static const int k1_2_40 = 4;

/**
 * @brief
 *
 */
class CropPlugin : public OFX::ImageEffect
{
public:
    CropPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;
    OfxRectD getCropRect( );
    bool       displayRect( );
public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

protected:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip*            _srcClip;       ///< Source image clip
    OFX::Clip*            _dstClip;       ///< Destination image clip
    OFX::ChoiceParam*     _formats;       ///< Image formats
    OFX::BooleanParam*    _rect;          ///< Display overlay rect

};

}
}
}

#endif  // VOLET_PLUGIN_H
