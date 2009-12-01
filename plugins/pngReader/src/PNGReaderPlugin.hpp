#ifndef PNG_READER_PLUGIN_H
#define PNG_READER_PLUGIN_H

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {

using namespace boost::gil;

/**
 * @brief Class used to denoise with partial derivated equations
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

protected:
	point2<ptrdiff_t>    _pngDims;       ///< Png dimensions
	OFX::StringParam* _filepath;
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _dstClip;      ///< Destination image clip
};

}
}
}

#endif
