#ifndef _PNG_READER_PLUGIN_HPP_
#define _PNG_READER_PLUGIN_HPP_

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/FilenameManager.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace boost::gil;

struct PNGReaderParams
{
	std::string _filepath;       ///< filepath
};

/**
 * @brief Png reader
 *
 */
class PNGReaderPlugin : public OFX::ImageEffect
{
public:
	PNGReaderPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getDstClip() const;
	PNGReaderParams getParams(const OfxTime time);

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void         getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool         getTimeDomain( OfxRangeD& range );

public:
	OFX::Clip* _dstClip;            ///< Destination image clip
	point2<ptrdiff_t>    _pngDims;  ///< Png dimensions
	OFX::StringParam*    _filepath;
	FilenameManager      _fPattern; ///< Filename pattern manager
};

}
}
}
}

#endif
