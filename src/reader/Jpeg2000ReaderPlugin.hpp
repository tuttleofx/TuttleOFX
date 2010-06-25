#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>
#include "openjpeg/J2KReader.hpp"

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {

struct Jpeg2000ReaderProcessParams
{
	std::string _paramFilepath;      ///< Ffmpeg filepath
};

/**
 * @brief Jpeg2000 plugin
 */
class Jpeg2000ReaderPlugin : public ReaderPlugin
{
public:
    Jpeg2000ReaderPlugin( OfxImageEffectHandle handle );
	tuttle::io::J2KReader & getReader();
	~Jpeg2000ReaderPlugin();

public:
	Jpeg2000ReaderProcessParams getProcessParams(const OfxTime time);
    void render( const OFX::RenderArguments &args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	OFX::BitDepthEnum getParamExplicitConversion() const
	{
		switch( this->_paramExplicitConv->getValue() )
		{
			case 0:
				return OFX::eBitDepthNone;
			case 1:
				return OFX::eBitDepthUByte;
			case 2:
				return OFX::eBitDepthUShort;
			case 3:
				return OFX::eBitDepthFloat;
			default:
				throw(OFX::Exception::Suite( kOfxStatErrImageFormat ));
		}
	}

	struct FileInfo
	{
		OfxTime _time;
		bool _failed;
		std::size_t _width;
		std::size_t _height;
		std::size_t _components;
		std::size_t _precision;
		OFX::BitDepthEnum _precisionType;
	};

	FileInfo retrieveFileInfo( const OfxTime time );
public:
	tuttle::io::J2KReader          _reader;
private:
	mutable FileInfo _fileInfos;
};


}
}
}
}

#endif
