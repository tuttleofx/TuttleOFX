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

/**
 * @brief
 *
 */
class CropPlugin : public OFX::ImageEffect
{
public:
	CropPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;
	OfxRectD   getCropRect(OfxRectD *clipROD = NULL);
	bool       displayRect();

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool		 getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

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
