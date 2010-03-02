#ifndef _PNG_READER_PLUGIN_HPP_
#define _PNG_READER_PLUGIN_HPP_

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace boost::gil;

/**
 * @brief Png reader
 *
 */
class PNGReaderPlugin : public OFX::ImageEffect
{
public:
	PNGReaderPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getDstClip() const;

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void         getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void         getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

public:
	point2<ptrdiff_t>    _pngDims;       ///< Png dimensions
	OFX::StringParam* _filepath;
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _dstClip;      ///< Destination image clip
};

}
}
}
}

#endif
