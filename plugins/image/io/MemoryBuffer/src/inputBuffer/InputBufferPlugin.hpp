#ifndef _TUTTLE_PLUGIN_INPUTBUFFER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_INPUTBUFFER_PLUGIN_HPP_

#include "InputBufferDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>


namespace tuttle {
namespace plugin {
namespace inputBuffer {


struct InputBufferProcessParams
{
	EParamInputMode _mode;
	
	unsigned char* _inputBuffer;
	CallbackInputImagePtr _callbackPtr;
	CustomDataPtr _customDataPtr;
	CallbackDestroyCustomDataPtr _callbackDestroyPtr;
	
	int _width;
	int _height;
	int _rowByteSize;
	double _pixelAspectRatio;
	double _framerate;
	OFX::EPixelComponent _pixelComponents;
	OFX::EBitDepth _bitDepth;
	OFX::EField _field;
};

/**
 * @brief InputBuffer plugin
 */
class InputBufferPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    InputBufferPlugin( OfxImageEffectHandle handle );
    ~InputBufferPlugin();

public:
	InputBufferProcessParams getProcessParams( const OfxTime time ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getTimeDomain( OfxRangeD& range );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

    void render( const OFX::RenderArguments &args );
	
public:
	OFX::Clip *_clipDst; ///< Destination image clip

	OFX::ChoiceParam* _paramInputMode;
	OFX::StringParam* _paramInputBufferPointer;
	OFX::StringParam* _paramInputCallbackPointer;
	OFX::StringParam* _paramInputCallbackCustomData;
	OFX::StringParam* _paramInputCallbackDestroyCustomData;
	
	OFX::Int2DParam* _paramSize;
	OFX::IntParam* _paramRowByteSize;
	OFX::DoubleParam* _paramPixelAspectRatio;
	OFX::DoubleParam* _paramFramerate;
	OFX::ChoiceParam* _paramPixelComponents;
	OFX::ChoiceParam* _paramBitDepth;
	OFX::ChoiceParam* _paramField;
	
	OFX::Double2DParam* _paramTimeDomain;
	
	CustomDataPtr _tempStoreCustomDataPtr; //< keep track of the previous value
};

}
}
}

#endif
