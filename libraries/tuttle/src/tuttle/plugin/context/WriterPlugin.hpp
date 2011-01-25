#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_

#include "WriterDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/common/clip/Sequence.hpp>

#include <ofxsImageEffect.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

class WriterPlugin : public ImageEffectGilPlugin
{
public:
	WriterPlugin( OfxImageEffectHandle handle );
	virtual ~WriterPlugin() = 0;

public:
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	virtual void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	virtual void render( const OFX::RenderArguments& args );

protected:
	inline bool varyOnTime() const { return _isSequence; }

private:
	bool _isSequence;                            ///<
	common::Sequence _filePattern;               ///< Filename pattern manager

	bool _oneRender;                            ///<
	OfxTime _oneRenderAtTime;                         ///<

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const
	{
		if( _isSequence )
			return _filePattern.getAbsoluteFilenameAt( boost::numeric_cast<common::Sequence::Time>(time) );
		else
			return _paramFilepath->getValue();
	}

	std::string getAbsoluteDirectory() const
	{
		namespace bfs = boost::filesystem;
		if( _isSequence )
			return _filePattern.getDirectory().string();
		else
		{
			bfs::path filepath( _paramFilepath->getValue() );
//			return bfs::absolute(filepath).parent_path().string();
			return filepath.parent_path().string();
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
	OFX::Clip* _clipSrc;       ///< Input image clip
	OFX::Clip* _clipDst;       ///< Ouput image clip

	OFX::PushButtonParam* _paramRenderButton;     ///< Render push button
	OFX::StringParam*     _paramFilepath;         ///< Target file path
	OFX::BooleanParam*    _paramRenderAlways;     ///< Render always
	OFX::ChoiceParam*     _paramBitDepth;         ///< Bit depth
	OFX::IntParam*        _paramForceNewRender;   ///< Hack parameter, to force a new rendering
	OFX::BooleanParam*    _paramFlip;             ///< Vertically flip the image
	/// @}
};

}
}

#endif

