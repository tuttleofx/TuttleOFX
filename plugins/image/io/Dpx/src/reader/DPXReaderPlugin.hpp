#ifndef _DPX_READER_PLUGIN_HPP_
#define _DPX_READER_PLUGIN_HPP_

#include <dpxEngine/dpxImage.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

/**
 * @brief Dpx reader
 */
class DPXReaderPlugin : public OFX::ImageEffect
{
public:
	DPXReaderPlugin( OfxImageEffectHandle handle );
	OFX::Clip*            getDstClip() const;
	tuttle::io::DpxImage& getDpxImg() { return _dpxImg; }

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void         getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void         getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

protected:
	bgil::point2<ptrdiff_t>    _imageDims;       ///< Image file dimensions
	tuttle::io::DpxImage       _dpxImg;          ///< Dpx image reader
	OFX::StringParam*          _filepath;        ///< Dpx filepath
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip*                 _dstClip;      ///< Destination image clip
};

}
}
}
}

#endif

