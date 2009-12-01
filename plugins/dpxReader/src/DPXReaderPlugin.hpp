#ifndef DPX_READER_PLUGIN_H
#define DPX_READER_PLUGIN_H

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include "dpxImage.hpp"

namespace bgil = boost::gil;

namespace tuttle {
namespace plugin {
namespace dpx {

/**
 * @brief Class used to denoise with partial derivated equations
 *
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
	tuttle::io::DpxImage _dpxImg;          ///< Dpx image reader
	OFX::StringParam* _filepath;
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _dstClip;      ///< Destination image clip
};

}
}
}

#endif

