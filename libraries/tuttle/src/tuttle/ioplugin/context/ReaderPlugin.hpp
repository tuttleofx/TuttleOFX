#ifndef _TUTTLE_IOPLUGIN_CONTEXT_READERPLUGIN_HPP_
#define _TUTTLE_IOPLUGIN_CONTEXT_READERPLUGIN_HPP_

#include <boost/gil/channel_algorithm.hpp> // force to use the boostHack version first

#include "ReaderDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <Sequence.hpp> // sequenceParser

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>


namespace tuttle {
namespace plugin {

namespace bfs = boost::filesystem;

class ReaderPlugin : public OFX::ImageEffect
{
public:
	ReaderPlugin( OfxImageEffectHandle handle );
	virtual ~ReaderPlugin() = 0;

public:
	virtual void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod ) = 0;
	virtual void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	virtual bool getTimeDomain( OfxRangeD& range );

	virtual void render( const OFX::RenderArguments& args );

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const;
	std::string getAbsoluteFirstFilename() const;
	std::string getAbsoluteDirectory() const;

	OfxTime getFirstTime() const;
	OfxTime getLastTime() const;

	EParamReaderBitDepth getExplicitBitDepthConversion() const;
	EParamReaderChannel getExplicitChannelConversion() const;
	
	OFX::EBitDepth getOfxExplicitConversion() const;

protected:
	virtual inline bool varyOnTime() const { return _isSequence; }

public:
	OFX::Clip*           _clipDst;        ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::StringParam*    _paramFilepath;  ///< File path
	OFX::ChoiceParam*    _paramBitDepth;  ///< Explicit bit depth conversion
	OFX::ChoiceParam*    _paramChannel;   ///< Explicit component conversion
	/// @}

private:
	bool _isSequence;
	sequenceParser::Sequence _filePattern;            ///< Filename pattern manager
};

}
}

#endif

