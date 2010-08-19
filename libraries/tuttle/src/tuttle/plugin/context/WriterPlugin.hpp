#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_
#define	_TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_

#include "WriterDefinition.hpp"

#include <tuttle/common/clip/Sequence.hpp>

#include <ofxsImageEffect.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

class WriterPlugin : public OFX::ImageEffect {
public:
	WriterPlugin( OfxImageEffectHandle handle );
	virtual ~WriterPlugin();

public:
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	inline bool varyOnTime() const { return _isSequence; }

private:
	bool                   _isSequence;          ///<
	common::Sequence       _filePattern;         ///< Filename pattern manager

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const
	{
		if( _isSequence )
			return _filePattern.getAbsoluteFilenameAt( time );
		else
			return _paramFilepath->getValue();
	}
	std::string getAbsoluteFirstFilename() const
	{
		if( _isSequence )
			return _filePattern.getAbsoluteFirstFilename();
		else
			return _paramFilepath->getValue();
	}
	OfxTime getFirstTime() const
	{
		if( _isSequence )
			return _filePattern.getFirstTime();
		else
			return kOfxFlagInfiniteMin;
	}
	OfxTime getLastTime() const
	{
		if( _isSequence )
			return _filePattern.getLastTime();
		else
			return kOfxFlagInfiniteMax;
	}
	
public:
	/// @group Attributes
	/// @{
	OFX::PushButtonParam* _paramRenderButton;     ///< Render push button
	OFX::StringParam*     _paramFilepath;         ///< Target file path
	OFX::BooleanParam*    _paramRenderAlways;     ///< Render always
	OFX::ChoiceParam*     _paramBitDepth;         ///< Bit depth

	OFX::Clip* _clipSrc;       ///< Input image clip
	OFX::Clip* _clipDst;       ///< Ouput image clip
	/// @}
};


}
}

#endif

