#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_

#include <boost/gil/channel_algorithm.hpp> // force to use the boostHack version first

#include "WriterDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <ofxsImageEffect.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <boost/gil/gil_all.hpp>

#include <Sequence.hpp>

namespace tuttle {
namespace plugin {


class WriterPlugin : public ImageEffectGilPlugin
{
public:
	WriterPlugin( OfxImageEffectHandle handle );
	virtual ~WriterPlugin() = 0;

public:
	virtual void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	virtual bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	virtual void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	virtual void render( const OFX::RenderArguments& args );

protected:
	inline bool varyOnTime() const { return _isSequence; }

private:
	bool _isSequence;                            ///<
	sequenceParser::Sequence _filePattern;               ///< Filename pattern manager

	bool _oneRender;                            ///<
	OfxTime _oneRenderAtTime;                         ///<

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const
	{
		if( _isSequence )
			return _filePattern.getAbsoluteFilenameAt( boost::numeric_cast<sequenceParser::Time>(time) );
		else
			return _paramFilepath->getValue();
	}

	std::string getAbsoluteDirectory() const
	{
		namespace bfs = boost::filesystem;
		if( _isSequence )
		{
			return _filePattern.getAbsoluteDirectory().string();
		}
		else
		{
			bfs::path filepath( _paramFilepath->getValue() );
			return bfs::absolute(filepath).parent_path().string();
		}
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
	OFX::Clip* _clipSrc; ///< Input image clip
	OFX::Clip* _clipDst; ///< Ouput image clip

	OFX::PushButtonParam* _paramRenderButton; ///< Render push button
	OFX::StringParam*     _paramFilepath; ///< Target file path
	OFX::BooleanParam*    _paramRenderAlways;
	OFX::BooleanParam*    _paramCopyToOutput; ///< Copy the image buffer to the output clip
	OFX::ChoiceParam*     _paramBitDepth;
	OFX::BooleanParam*    _paramPremult;
	OFX::ChoiceParam*     _paramExistingFile;
	OFX::IntParam*        _paramForceNewRender; ///< Hack parameter, to force a new rendering
	/// @}
};

}
}

#endif

