#include "DiffPlotPlugin.hpp"

#include <boost/gil/gil_all.hpp>

#include <boost/smart_ptr/scoped_array.hpp>

namespace tuttle {
namespace plugin {
namespace diffPlot {

/*
 * fecth plugin IHM parameters with plugin parameters pointer
 */
DiffPlotPlugin::DiffPlotPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_clipSrcB = fetchClip( kClipSourceB );

	_paramGlobalDisplaySelection = fetchBooleanParam( kGlobalDisplay );
	_paramColorMapping = fetchChoiceParam( kParamColorMapping );

	_paramGroupRGB = fetchGroupParam( kGroupRGB );
	_paramOverlayRSelection = fetchBooleanParam( kBoolRed );
	_paramMutliplierR = fetchDoubleParam(kMultiplierRed);
	_paramOverlayGSelection = fetchBooleanParam( kBoolGreen );
	_paramMutliplierG = fetchDoubleParam(kMultiplierGreen);
	
	_paramOverlayBSelection = fetchBooleanParam( kBoolBlue );
	_paramMutliplierB = fetchDoubleParam(kMultiplierBlue);
	
	_paramGroupHSL = fetchGroupParam( kGroupHSL );
	_paramOverlayHSelection = fetchBooleanParam( kBoolHue );
	_paramMutliplierH = fetchDoubleParam(kMultiplierHue);
	_paramOverlaySSelection = fetchBooleanParam( kBoolSaturation );
	_paramMutliplierS = fetchDoubleParam(kMultiplierSaturation);
	
	_paramOverlayLSelection = fetchBooleanParam( kBoolLightness );
	_paramMutliplierL = fetchDoubleParam(kMultiplierLightness);
	
	_paramDisplayTypeSelection = fetchChoiceParam( kHistoDisplayListParamLabel );

	_paramDisplaySelection = fetchBooleanParam( kBoolSelection );
	_paramSelectionMode = fetchChoiceParam( kSelectionModeListParamLabel );
	
	_paramSelectionMultiplierSelection = fetchDoubleParam( kselectionMultiplier );
	_paramRefreshOverlaySelection = fetchPushButtonParam( kButtonRefreshOverlay );
	_paramNbStepSelection = fetchIntParam( knbStepRange );

	//Reset param booleans
	_isCleaned = false;
	_isNbStepChanged = false;
	_isDiffPlotRefresh = false;

	//Initialize scoped pointer
	_overlayDataCount = 0;

	/// @todo: HACK to display curves default position
	OFX::InstanceChangedArgs changed( this->timeLineGetTime( ) );
	changedClip( changed, kOfxImageEffectSimpleSourceClipName );
}

/*
 * create and return DiffPlotProcessParams structure for process
 */
DiffPlotProcessParams<DiffPlotPlugin::Scalar> DiffPlotPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	DiffPlotProcessParams<Scalar> params;

	params._time = time;
	params._boolRGB[0] = _paramOverlayRSelection; //R (is channel selected?)
	params._boolRGB[1] = _paramOverlayGSelection; //G (is channel selected?)
	params._boolRGB[2] = _paramOverlayBSelection; //B (is channel selected?)

	params._boolHSL[0] = _paramOverlayHSelection; //H (is channel selected?)
	params._boolHSL[1] = _paramOverlaySSelection; //S (is channel selected?)
	params._boolHSL[2] = _paramOverlayLSelection; //L (is channel selected?)

	return params;
}

/*
 * a plugin parameter as been changed
 */
void DiffPlotPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamColorMapping )
	{
		int colorMappingMode = _paramColorMapping->getValue();
		bool isNotChannelMapping = colorMappingMode != 0;
		_paramGroupRGB->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlayRSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierR->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlayGSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierG->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlayBSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierB->setIsSecretAndDisabled( isNotChannelMapping );
		_paramGroupHSL->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlayHSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierH->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlaySSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierS->setIsSecretAndDisabled( isNotChannelMapping );
		_paramOverlayLSelection->setIsSecretAndDisabled( isNotChannelMapping );
		_paramMutliplierL->setIsSecretAndDisabled( isNotChannelMapping );

	}
	// refresh diffPlot overlay
	else if( paramName == kButtonRefreshOverlay )
	{
		//Draw forced
		OFX::InstanceChangedArgs changed( args.time, args.renderScale );
		this->changedClip( changed, this->_clipSrc->name( ) );
	}
	// nbStep changed
	else if( paramName == knbStepRange )
	{
		if( this->hasOverlayData( ) ) //if there is overlay value
		{
			getOverlayData().setNbStep( _paramNbStepSelection->getValue( ) ); //change nbStep value
			// getOverlayData( ).computeFullData( this->_clipSrc, args.time, args.renderScale ); //reset buffer and compute them
			getOverlayData()._isDataInvalid = true;
		}
	}
	// Clear user selection
	else if( paramName == kButtonResetSelection )
	{
		if( this->hasOverlayData( ) )//if there is overlay value
		{
			this->getOverlayData( ).clearSelection( ); //clear selection
		}
	}
}

/*
 * the source clip has been changed
 */
void DiffPlotPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		if( this->hasOverlayData( ) )
		{
			this->getOverlayData()._isDataInvalid = true;
			this->redrawOverlays();
		}
	}
}

bool DiffPlotPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	/// @todo HACK: nuke doesn't call changedClip when the time is modified.
	//OFX::InstanceChangedArgs changed( args.time, args.renderScale );
	//this->changedClip(changed,this->_clipSrc->name());
	//	DiffPlotProcessParams<Scalar> params = getProcessParams();
	
//	identityClip = this->_clipSrc;
//	identityTime = args.time;
//	return true;
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DiffPlotPlugin::render( const OFX::RenderArguments &args )
{
	if( ! _clipSrc->isConnected() )
		BOOST_THROW_EXCEPTION( exception::ImageNotConnected() );
	
	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	if( ! src.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady()
				<< exception::dev() + "Error on clip " + quotes(_clipSrc->name())
				<< exception::time( args.time ) );
	if( src->getRowDistanceBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
				<< exception::dev() + "Error on clip " + quotes(_clipSrc->name())
				<< exception::time( args.time ) );

	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );
	if( ! dst.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name())
				<< exception::time( args.time ) );
	if( dst->getRowDistanceBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name())
				<< exception::time( args.time ) );

	setIsRendering(true);  //plugin is rendering
	
	// Copy buffer
	const OfxRectI bounds = dst->getBounds();
	if( src->isLinearBuffer() && dst->isLinearBuffer() )
	{
		const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
		if( imageDataBytes )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, bounds.y1 );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, bounds.y1 );
			memcpy( dataDstPtr, dataSrcPtr, imageDataBytes );
		}
	}
	else
	{
		const std::size_t rowBytesToCopy = dst->getBoundsRowDataBytes();
		for( int y = bounds.y1; y < bounds.y2; ++y )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, y );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, y );
			memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
		}
	}

	setIsRendering(false);  // plugin is not rendering anymore

	if( this->hasOverlayData( ) )
	{
		this->getOverlayData()._isDataInvalid = true;
		this->redrawOverlays();
	}

	if( OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke" )	/// @todo: HACK Nuke doesn't call changeClip function when time is changed
	{
		if( getOverlayData().isCurrentTimeModified(args.time) ) //if time is changed
		{
			this->redrawOverlays();		//redraw scene
		}
	}
}

/// @brief Overlay data
/// @{

/*Overlay data management*/
void DiffPlotPlugin::addRefOverlayData( )
{
	if( _overlayDataCount == 0 )
	{
		const OfxPointI imgSize = this->_clipSrc->getPixelRodSize( 0 ); ///@todo set the correct time !
		_overlayData.reset( new OverlayData( imgSize, this->_paramNbStepSelection->getValue() ) );
	}
	++_overlayDataCount;
}

void DiffPlotPlugin::releaseOverlayData( )
{
	--_overlayDataCount;
	if( _overlayDataCount == 0 )
	{
		_overlayData.reset( NULL );
	}
}

bool DiffPlotPlugin::hasOverlayData( ) const
{
	return _overlayDataCount != 0;
}

OverlayData& DiffPlotPlugin::getOverlayData( )
{
	return *_overlayData.get( );
}

const OverlayData& DiffPlotPlugin::getOverlayData( ) const
{
	return *_overlayData.get( );
}
/// @}
}
}
}
