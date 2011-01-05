#include "DebugImageEffectApiPlugin.hpp"
#include "DebugImageEffectApiProcess.hpp"
#include "DebugImageEffectApiDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

#include <sstream>

namespace tuttle {
namespace plugin {
namespace debugImageEffectApi {

DebugImageEffectApiPlugin::DebugImageEffectApiPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_hostInfos   = fetchStringParam( kHostInfos );
	_currentTime = fetchDoubleParam( kCurrentTime );
	_beginTime   = fetchDoubleParam( kBeginTime );
	_endTime     = fetchDoubleParam( kEndTime );
}

DebugImageEffectApiParams DebugImageEffectApiPlugin::getParams() const
{
	DebugImageEffectApiParams params;

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DebugImageEffectApiPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<DebugImageEffectApiProcess>( *this, args );
}

void DebugImageEffectApiPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	TUTTLE_COUT( "changed param : " << paramName << " time: " << args.time << " render scale : " << args.renderScale.x << ", " << args.renderScale.y << " reason : " << args.reason );
	if( paramName == kGoTo )
	{
		this->timeLineGotoTime( args.time - 2 );
	}
	else if( paramName == kUpdate )
	{
		this->_currentTime->setValue( args.time );

		OFX::ImageEffectHostDescription* desc = OFX::getImageEffectHostDescription();
		std::ostringstream description;
		description << "hostName: " << desc->hostName << std::endl;
		description << "hostIsBackground: " << desc->hostIsBackground << std::endl;
		description << "supportsOverlays: " << desc->supportsOverlays << std::endl;
		description << "supportsMultiResolution: " << desc->supportsMultiResolution << std::endl;
		description << "supportsTiles: " << desc->supportsTiles << std::endl;
		description << "temporalClipAccess: " << desc->temporalClipAccess << std::endl;
		description << "supportsMultipleClipDepths: " << desc->supportsMultipleClipDepths << std::endl;
		description << "supportsMultipleClipPARs: " << desc->supportsMultipleClipPARs << std::endl;
		description << "supportsSetableFrameRate: " << desc->supportsSetableFrameRate << std::endl;
		description << "supportsSetableFielding: " << desc->supportsSetableFielding << std::endl;
		description << "supportsStringAnimation: " << desc->supportsStringAnimation << std::endl;
		description << "supportsCustomInteract: " << desc->supportsCustomInteract << std::endl;
		description << "supportsChoiceAnimation: " << desc->supportsChoiceAnimation << std::endl;
		description << "supportsBooleanAnimation: " << desc->supportsBooleanAnimation << std::endl;
		description << "supportsCustomAnimation: " << desc->supportsCustomAnimation << std::endl;
		description << "maxParameters: " << desc->maxParameters << std::endl;
		description << "maxPages: " << desc->maxPages << std::endl;
		description << "pageRowCount: " << desc->pageRowCount << std::endl;
		description << "pageColumnCount: " << desc->pageColumnCount << std::endl;
		description << "_supportedComponents: ";
		for( OFX::ImageEffectHostDescription::PixelComponentArray::const_iterator it = desc->_supportedComponents.begin(), itEnd = desc->_supportedComponents.end();
		     it != itEnd;
		     ++it )
			description << OFX::mapPixelComponentEnumToString( *it ) << ", ";
		description << std::endl;
		description << "_supportedContexts: ";
		for( OFX::ImageEffectHostDescription::ContextArray::const_iterator it = desc->_supportedContexts.begin(), itEnd = desc->_supportedContexts.end();
		     it != itEnd;
		     ++it )
			description << OFX::mapContextEnumToString( *it ) << ", ";
		description << std::endl;
		description << "_supportedPixelDepths: ";
		for( OFX::ImageEffectHostDescription::BitDepthArray::const_iterator it = desc->_supportedPixelDepths.begin(), itEnd = desc->_supportedPixelDepths.end();
		     it != itEnd;
		     ++it )
			description << OFX::mapBitDepthEnumToString( *it ) << ", ";
		description << std::endl;
		description << "supportsProgressSuite: " << desc->supportsProgressSuite << std::endl;
		description << "supportsTimeLineSuite: " << desc->supportsTimeLineSuite << std::endl;

		description << "this->timeLineGetTime(): " << this->timeLineGetTime() << std::endl;
		double t1 = 0, t2 = 0;
		this->timeLineGetBounds( t1, t2 );
		description << "this->timeLineGetBounds: " << t1 << ", " << t2 << std::endl;
		this->_beginTime->setValue( t1 );
		this->_endTime->setValue( t2 );
		//		description << ": " << desc-> << std::endl;
		/*
		   std::string hostName;
		   bool hostIsBackground;
		   bool supportsOverlays;
		   bool supportsMultiResolution;
		   bool supportsTiles;
		   bool temporalClipAccess;
		   bool supportsMultipleClipDepths;
		   bool supportsMultipleClipPARs;
		   bool supportsSetableFrameRate;
		   bool supportsSetableFielding;
		   bool supportsStringAnimation;
		   bool supportsCustomInteract;
		   bool supportsChoiceAnimation;
		   bool supportsBooleanAnimation;
		   bool supportsCustomAnimation;
		   int maxParameters;
		   int maxPages;
		   int pageRowCount;
		   int pageColumnCount;
		   typedef std::vector<EPixelComponent> PixelComponentArray;
		   PixelComponentArray _supportedComponents;
		   typedef std::vector<EContext> ContextArray;
		   ContextArray _supportedContexts;
		   typedef std::vector<EBitDepth> PixelDepthArray;
		   PixelDepthArray _supportedPixelDepths;
		   bool supportsProgressSuite;
		   bool supportsTimeLineSuite;
		 */
		this->_hostInfos->setValue( description.str() );
	}
}

}
}
}
