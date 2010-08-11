#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

struct Jpeg2000ProcessParams
{
	std::string _filepath;      ///< Ffmpeg filepath
	int _bitDepth;				///< Precision (in bits)
	int _cineProfil;			///< Cinema Profile
	bool _lossless;				///< Lossless compression
};

/**
 * @brief FFMpeg plugin
 */
class Jpeg2000WriterPlugin : public WriterPlugin
{
public:
    Jpeg2000WriterPlugin( OfxImageEffectHandle handle );

public:
	Jpeg2000ProcessParams getProcessParams(const OfxTime time);
	
	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );

protected:
	OFX::ChoiceParam    *_cineProfil;
	OFX::BooleanParam   *_lossless;
};

}
}
}
}

#endif
