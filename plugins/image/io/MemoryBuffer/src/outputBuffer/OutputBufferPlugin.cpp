#include "OutputBufferPlugin.hpp"
#include "OutputBufferDefinitions.hpp"

#include <pointerParam.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <ofxImageEffect.h>

#include <boost/assert.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace outputBuffer {

using namespace memoryBuffer;

OutputBufferPlugin::OutputBufferPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
, _tempStoreCustomDataPtr(NULL)
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );

	_paramCallbackOutputPointer = fetchStringParam( kParamOutputCallbackPointer );
	_paramCustomData = fetchStringParam( kParamOutputCustomData );
	_paramCallbackDestroyCustomData = fetchStringParam( kParamOutputCallbackDestroyCustomData );
	_paramMaxImageSize = fetchInt2DParam( kParamMaxImageSize );
}

OutputBufferPlugin::~OutputBufferPlugin()
{
	OutputBufferProcessParams params = getProcessParams();
	if( params._callbackDestroyPtr != NULL )
		params._callbackDestroyPtr( params._customDataPtr );
}

void OutputBufferPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamOutputCustomData )
	{
		OutputBufferProcessParams params = getProcessParams();
		if( params._callbackDestroyPtr != NULL && _tempStoreCustomDataPtr != NULL )
			params._callbackDestroyPtr( _tempStoreCustomDataPtr );
		_tempStoreCustomDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramCustomData->getValue() ) );
	}
}

void OutputBufferPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois ) {
  OfxPointI s = _paramMaxImageSize->getValue();

  if (s.x != 0 && s.y != 0) {
    OfxRectD srcRoi = {0,0,0,0};
    srcRoi.x2 = s.x;
    srcRoi.y2 = s.y;
    rois.setRegionOfInterest( *_clipSrc, srcRoi );
  }
}

bool OutputBufferPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod ) {
  const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
  OfxPointI s = _paramMaxImageSize->getValue();

  if (s.x != 0 && s.y != 0) {
    rod.x1 = 0;
    rod.y1 = 0;
    if (s.x > srcRod.x2)
      rod.x2 = srcRod.x2;
    else
      rod.x2 = s.x;
    if (s.y > srcRod.y2)
      rod.y2 = srcRod.y2;
    else
      rod.y2 = s.y;
    return true;
  } else
    return false;
}

OutputBufferProcessParams OutputBufferPlugin::getProcessParams() const
{
	OutputBufferProcessParams params;
	params._callbackPtr = reinterpret_cast<CallbackOutputImagePtr>( stringToPointer( _paramCallbackOutputPointer->getValue() ) );
	params._customDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramCustomData->getValue() ) );
	params._callbackDestroyPtr = reinterpret_cast<CallbackDestroyCustomDataPtr>( stringToPointer( _paramCallbackDestroyCustomData->getValue() ) );
	
	return params;
}

void OutputBufferPlugin::render( const OFX::RenderArguments& args )
{
  //typedef std::vector<char, OfxAllocator<char> > DataVector;
  //DataVector rawImage;
	char* rawImagePtrLink;


	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );

	// Get Image info
	const OfxRectI bounds = src->getBounds();
	const OFX::EBitDepth depth = src->getPixelDepth();
	const OFX::EPixelComponent components = src->getPixelComponents();
	const OFX::EField field = src->getField();	
	const CallbackOutputImagePtr callbackPtr = reinterpret_cast<CallbackOutputImagePtr>( stringToPointer( _paramCallbackOutputPointer->getValue() ) );
	const CustomDataPtr customDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramCustomData->getValue() ) );
	const OfxPointI s = _paramMaxImageSize->getValue();
	const int rowDistance = src->getRowDistanceBytes();

	if (s.x > 0 && s.y > 0) {
	  rawImagePtrLink = (char *)src->getPixelAddress( 0, 0 );
	  if( callbackPtr != NULL )
	    {
	      callbackPtr(
			  args.time, customDataPtr, rawImagePtrLink,
			  s.x, s.y, rowDistance,
			  depth, components, field );
	    }
	} else {
	  rawImagePtrLink = (char *)src->getPixelAddress( bounds.x1, bounds.y1 );
	  if( callbackPtr != NULL )
	    {
	      callbackPtr(
			  args.time, customDataPtr, rawImagePtrLink,
			  bounds.x2-bounds.x1, bounds.y2-bounds.y1, rowDistance,
			  depth, components, field );
	    }
	}


}
}
}
}
