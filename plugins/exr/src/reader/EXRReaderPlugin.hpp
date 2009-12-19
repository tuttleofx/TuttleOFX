#ifndef EXR_READER_PLUGIN_H
#define EXR_READER_PLUGIN_H

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <ImfInputFile.h>

namespace bgil = boost::gil;

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

/**
 * @brief Class used to denoise with partial derivated equations
 *
 */
class EXRReaderPlugin : public OFX::ImageEffect
{
public:
	EXRReaderPlugin( OfxImageEffectHandle handle );
	OFX::Clip*            getDstClip() const;

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void         getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void         getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

protected:
	bgil::point2<ptrdiff_t>    _imageDims;       ///< Image file dimensions
	OFX::StringParam*          _filepath;        ///< File path
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _dstClip;      ///< Destination image clip
};

}
}
}
}

#endif

