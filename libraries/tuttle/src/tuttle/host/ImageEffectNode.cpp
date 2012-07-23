#include "ImageEffectNode.hpp"
#include "HostDescriptor.hpp"

// ofx host
#include <tuttle/host/Core.hpp> // for Core::instance().getMemoryCache()
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/allParams.hpp>
#include <tuttle/host/graph/ProcessEdgeAtTime.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

#include <tuttle/host/ofx/OfxhUtilities.hpp>
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/attribute/OfxhClip.hpp>
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>

#ifndef TUTTLE_PRODUCTION
// to output all nodes as png for debug
//#define TUTTLE_DEBUG_OUTPUT_ALL_NODES
#endif

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>

namespace tuttle {
namespace host {

ImageEffectNode::ImageEffectNode( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin&         plugin,
				  tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
				  const std::string&                                             context )
	: tuttle::host::ofx::imageEffect::OfxhImageEffectNode( plugin, desc, context, false )
{
	populate();
	//	createInstanceAction();
}

ImageEffectNode::ImageEffectNode( const ImageEffectNode& other )
	: INode( other )
	, tuttle::host::ofx::imageEffect::OfxhImageEffectNode( other )
{
	populate();
	copyAttributesValues( other ); // values need to be setted before the createInstanceAction !
	createInstanceAction();
}

ImageEffectNode::~ImageEffectNode()
{}

void ImageEffectNode::connect( const INode& sourceEffect, attribute::Attribute& attr )
{
	const attribute::ClipImage& outputClip = dynamic_cast<const attribute::ClipImage&>( sourceEffect.getClip( kOfxImageEffectOutputClipName ) );
	attribute::ClipImage& inputClip        = dynamic_cast<attribute::ClipImage&>( attr ); // throw an exception if not a ClipImage attribute

	inputClip.setConnectedClip( outputClip );
}

bool ImageEffectNode::operator==( const INode& other ) const
{
	const ImageEffectNode* other_ptr = dynamic_cast<const ImageEffectNode*>( &other );
	if( other_ptr == NULL )
		return false;
	return operator==( *other_ptr );
}

/**
 * @warning do a deep comparison
 */
bool ImageEffectNode::operator==( const ImageEffectNode& other ) const
{
	return ofx::imageEffect::OfxhImageEffectNode::operator==( other );
}

// get a new clip instance
tuttle::host::ofx::attribute::OfxhClipImage* ImageEffectNode::newClipImage( const tuttle::host::ofx::attribute::OfxhClipImageDescriptor& descriptor )
{
	return new attribute::ClipImage( *this, descriptor );
}

/// get default output fielding. This is passed into the clip prefs action
/// and  might be mapped (if the host allows such a thing)
const std::string& ImageEffectNode::getDefaultOutputFielding() const
{
	/// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
	static const std::string v( kOfxImageFieldNone );

	return v;
}

/**
 * @return 1 to abort processing
 */
int ImageEffectNode::abort()
{
	return 0;
}

ofx::OfxhMemory* ImageEffectNode::newMemoryInstance( size_t nBytes )
{
	ofx::OfxhMemory* instance = new ofx::OfxhMemory();

	instance->alloc( nBytes );
	return instance;
}

// vmessage
void ImageEffectNode::vmessage( const char* type,
				const char* id,
				const char* format,
				va_list     args ) const OFX_EXCEPTION_SPEC
{
	vprintf( format, args );
}

// get the project size in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectSize( double& xSize, double& ySize ) const
{
	xSize = 720;
	ySize = 576;
}

// get the project offset in CANONICAL pixels, we are at 0,0
void ImageEffectNode::getProjectOffset( double& xOffset, double& yOffset ) const
{
	xOffset = 0;
	yOffset = 0;
}

// get the project extent in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectExtent( double& xSize, double& ySize ) const
{
	xSize = 720.0;
	ySize = 576.0;
}

// get the PAR, SD PAL is 768/720=1.0666
double ImageEffectNode::getProjectPixelAspectRatio() const
{
	return 1.0;
}

// we are only 25 frames
double ImageEffectNode::getEffectDuration() const
{
	return 25.0;
}

double ImageEffectNode::getFrameRate() const
{
	return this->_outputFrameRate;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct frame
double ImageEffectNode::getFrameRecursive() const
{
	return 0.0;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct
/// renderScale
void ImageEffectNode::getRenderScaleRecursive( double& x, double& y ) const
{
	x = y = 1.0;
}

/**
 * The pixel components type of the current project
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectPixelComponentsType() const
{
	return kOfxImageComponentRGBA;
}

/**
 * The pixel bit depth of the current project (host work in float)
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectBitDepth() const
{
	//return kOfxBitDepthByte;
	return kOfxBitDepthFloat;
}

// make a parameter instance
ofx::attribute::OfxhParam* ImageEffectNode::newParam( const ofx::attribute::OfxhParamDescriptor& descriptor ) OFX_EXCEPTION_SPEC
{
	const std::string name = descriptor.getName();
	ofx::attribute::OfxhParam* param = NULL;

	try
	{
		if( descriptor.getParamType() == kOfxParamTypeString )
			param = new attribute::ParamString( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger )
			param = new attribute::ParamInteger( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble )
			param = new attribute::ParamDouble( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeBoolean )
			param = new attribute::ParamBoolean( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeChoice )
			param = new attribute::ParamChoice( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeRGBA )
			param = new attribute::ParamRGBA( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeRGB )
			param = new attribute::ParamRGB( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble2D )
			param = new attribute::ParamDouble2D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble3D )
			param = new attribute::ParamDouble3D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger2D )
			param = new attribute::ParamInteger2D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger3D )
			param = new attribute::ParamInteger3D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePushButton )
			param = new attribute::ParamPushButton( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeGroup )
			param = new attribute::ParamGroup( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePage )
			param = new attribute::ParamPage( *this, name,  descriptor );
		else
		{
			BOOST_THROW_EXCEPTION( exception::Failed()
			    << exception::user() + "Can't create param " + quotes( name ) + " instance from param descriptor, type not recognized." );
		}
		this->addParam( param );
	}
	catch( exception::Common& e )
	{
		BOOST_THROW_EXCEPTION( ofx::OfxhException( *boost::get_error_info<exception::ofxStatus>( e ),
							   boost::diagnostic_information( e ) ) );
	}
	catch(... )
	{
		BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrUnknown,
							   boost::current_exception_diagnostic_information() ) );
	}
	return param;
}

void ImageEffectNode::editBegin( const std::string& name ) OFX_EXCEPTION_SPEC
{
	//BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ImageEffectNode::editEnd() OFX_EXCEPTION_SPEC
{
	//BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

/// Start doing progress.
void ImageEffectNode::progressStart( const std::string& message )
{
	//TUTTLE_COUT( message );
	if( !( getContext() == kOfxImageEffectContextReader ) && !( getContext() == kOfxImageEffectContextWriter ) )
		std::cout << std::left << "       " << tuttle::common::kColorGreen << std::setw( TUTTLE_COUT_PLUGIN_NAME_WIDTH ) << getName() << tuttle::common::kColorStd << std::flush;
}

/// finish yer progress
void ImageEffectNode::progressEnd()
{
	std::cout << std::endl;
}

/// set the progress to some level of completion,
/// returns true if you should abandon processing, false to continue
bool ImageEffectNode::progressUpdate( const double progress )
{
	if( ( getContext() == kOfxImageEffectContextReader ) || ( getContext() == kOfxImageEffectContextWriter ) )
		std::cout << "\r" << tuttle::common::kColorStd << "[" << std::right << std::setw(3) << int(progress * 100) << "%] " << " " << std::left << std::flush;
	else
		std::cout << "\r" << tuttle::common::kColorStd << "[" << std::right << std::setw(3) << int(progress * 100) << "%] " << std::left << tuttle::common::kColorGreen << getName() << tuttle::common::kColorStd << std::flush;
	return false;
}

/// get the current time on the timeline. This is not necessarily the same
/// time as being passed to an action (eg render)
double ImageEffectNode::timelineGetTime()
{
	return 0;
}

/// set the timeline to a specific time
void ImageEffectNode::timelineGotoTime( double t )
{}

/// get the first and last times available on the effect's timeline
void ImageEffectNode::timelineGetBounds( double& t1, double& t2 )
{
	t1 = 0;
	t2 = 25;
}

/// override to get frame range of the effect
void ImageEffectNode::beginSequenceRenderAction( OfxTime   startFrame,
					 OfxTime   endFrame,
					 OfxTime   step,
					 bool      interactive,
					 OfxPointD renderScale ) OFX_EXCEPTION_SPEC
{
	OfxhImageEffectNode::beginSequenceRenderAction( startFrame, endFrame, step, interactive, renderScale );
}

void ImageEffectNode::checkClipsConnections() const
{
	for( ClipImageMap::const_iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		const attribute::ClipImage& clip = dynamic_cast<const attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && !clip.isConnected() && !clip.isOptional() ) // one non optional input clip is unconnected
		{
			BOOST_THROW_EXCEPTION( exception::Logic()
			    << exception::user( "A non optional clip is unconnected ! (" + clip.getFullName() + ")" ) );
		}
	}
}

void ImageEffectNode::initComponents()
{
	attribute::ClipImage& outputClip    = dynamic_cast<attribute::ClipImage&>( getOutputClip() );
	//bool inputClipsFound                = false;
	std::string mostChromaticComponents = kOfxImageComponentNone;

	for( ClipImageMap::iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && clip.isConnected() )
		{
			//inputClipsFound = true;
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			mostChromaticComponents = findMostChromaticComponents( linkClip.getComponentsString(), mostChromaticComponents );
		}
	}
	// components
	for( ClipImageMap::iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && clip.isConnected() )
		{
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			if( clip.isSupportedComponent( mostChromaticComponents ) )
				clip.setComponentsStringIfNotModifiedByPlugin( linkClip.getComponentsString() );
		}
	}
	if( outputClip.isSupportedComponent( mostChromaticComponents ) )
		outputClip.setComponentsStringIfNotModifiedByPlugin( mostChromaticComponents );
}

/// @todo multiple PAR
void ImageEffectNode::initPixelAspectRatio()
{
	if( supportsMultipleClipPARs() )
	{}
	else
	{}
}

void ImageEffectNode::maximizeBitDepthFromReadsToWrites()
{
	std::string biggestBitDepth      = kOfxBitDepthNone;
	attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>( getOutputClip() );
	bool inputClipsFound             = false;

	// init variables
	for( ClipImageMap::iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && clip.isConnected() ) // filter for clip.isMask() and clip.isOptional() ?
		{
			inputClipsFound = true;
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			biggestBitDepth = ofx::imageEffect::findDeepestBitDepth( linkClip.getBitDepthString(), biggestBitDepth );
		}
	}
	const std::string validBitDepth = this->bestSupportedBitDepth( biggestBitDepth );

	// bit depth
	if( supportsMultipleClipDepths() )
	{
		// check if we support the bit depth of each input
		// and fill input clip with connected clips bit depth
		for( ClipImageMap::iterator it = _clips.begin();
		     it != _clips.end();
		     ++it )
		{
			attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
			if( !clip.isOutput() && clip.isConnected() )
			{
				const attribute::ClipImage& linkClip = clip.getConnectedClip();
				const std::string& linkClipBitDepth  = linkClip.getBitDepthString();
				if( this->isSupportedBitDepth( linkClipBitDepth ) )
				{
					clip.setBitDepthStringIfUpperAndNotModifiedByPlugin( linkClipBitDepth );
				}
			}
		}
	}
	else // multiple clip depth not supported (standard case)
	{
		if( inputClipsFound && // if we have an input clip
		    validBitDepth == kOfxBitDepthNone ) // if we didn't found a valid bit depth value
		{
			BOOST_THROW_EXCEPTION( exception::Logic()
			    << exception::user( "Pixel depth " + biggestBitDepth + " not supported on plugin : " + getName() ) );
		}
		for( ClipImageMap::iterator it = _clips.begin();
		     it != _clips.end();
		     ++it )
		{
			attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
			if( !clip.isOutput() && clip.isConnected() )
			{
				const attribute::ClipImage& linkClip = clip.getConnectedClip();
				if( ( linkClip.getNode().getNodeType() == INode::eNodeTypeImageEffect &&
				      linkClip.getNode().asImageEffectNode().isSupportedBitDepth( validBitDepth )
				    ) ||
				      linkClip.getNode().getNodeType() == INode::eNodeTypeBuffer
				  )
				{
					clip.setBitDepthStringIfUpperAndNotModifiedByPlugin( validBitDepth );
				}
			}
		}
	}
	outputClip.setBitDepthStringIfUpperAndNotModifiedByPlugin( validBitDepth );
}

void ImageEffectNode::maximizeBitDepthFromWritesToReads()
{
	//TUTTLE_TCOUT( "maximizeBitDepthFromWritesToReads: " << getName() );
	if( !supportsMultipleClipDepths() )
	{
		attribute::ClipImage& outputClip         = dynamic_cast<attribute::ClipImage&>( getOutputClip() );
		const std::string& outputClipBitDepthStr = outputClip.getBitDepthString();
		for( ClipImageMap::iterator it = _clips.begin();
		     it != _clips.end();
		     ++it )
		{
			attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
			if( !clip.isOutput() && clip.isConnected() )
			{
				/// @todo tuttle: what is the best way to access another node ?
				/// through the graph ? through a graph inside ProcessOptions ?
				/*const */ attribute::ClipImage& linkClip = clip.getConnectedClip();

				//TUTTLE_TCOUT_X( 20, "-" );
				//TUTTLE_TCOUT( clip.getFullName() << "(" << clip.getBitDepth() << ")" << "-->" << linkClip.getFullName() << "(" << linkClip.getBitDepth() << ")" );
				if( linkClip.getNode().getNodeType() == INode::eNodeTypeImageEffect &&
				    linkClip.getNode().asImageEffectNode().isSupportedBitDepth( outputClipBitDepthStr ) ) // need to be supported by the other node
				{
					if( linkClip.getNode().asImageEffectNode().supportsMultipleClipDepths() ) /// @todo tuttle: is this test correct in all cases?
					{
						linkClip.setBitDepthStringIfUpper( outputClipBitDepthStr );
					}
					else
					{
						linkClip.setBitDepthStringIfUpperAndNotModifiedByPlugin( outputClipBitDepthStr );
					}
				}
				//TUTTLE_TCOUT( clip.getFullName() << "(" << clip.getBitDepth() << ")" << "-->" << linkClip.getFullName() << "(" << linkClip.getBitDepth() << ")" );
			}
			//else
			//{
			//	TUTTLE_TCOUT( clip.getFullName() << "(" << clip.getBitDepth() << ")" << ", unconnected ? " << clip.isConnected() << ", output ? " << clip.isOutput() );
			//}
		}
	}
}

void ImageEffectNode::coutBitDepthConnections() const
{
#ifndef TUTTLE_PRODUCTION
	// validation
	for( ClipImageMap::const_iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );

		const ofx::property::String& propPixelDepth       = clip.getProperties().fetchStringProperty( kOfxImageEffectPropPixelDepth );
		const ofx::property::String& propComponent        = clip.getProperties().fetchStringProperty( kOfxImageEffectPropComponents );
		const ofx::property::Double& propPixelAspectRatio = clip.getProperties().fetchDoubleProperty( kOfxImagePropPixelAspectRatio );
		TUTTLE_TCOUT( "-- " << "clip: " << " = " << clip.getFullName() );
		TUTTLE_TCOUT( "-- " << kOfxImageEffectPropPixelDepth << " = " << propPixelDepth.getValue()
			     << " : " << ( propPixelDepth.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );
		TUTTLE_TCOUT( "-- " << kOfxImageEffectPropComponents << " = " << propComponent.getValue()
			     << " : " << ( propComponent.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );
		TUTTLE_TCOUT( "-- " << kOfxImagePropPixelAspectRatio << " = " << propPixelAspectRatio.getValue()
			     << " : " << ( propPixelAspectRatio.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );

		if( !clip.isOutput() && clip.isConnected() )
		{
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			TUTTLE_TCOUT( "  Connection between " << clip.getFullName() << " (" << clip.getBitDepth() << " bytes)" << " => " << linkClip.getFullName() << " (" << linkClip.getBitDepth() << " bytes)." );
		}
	}
#endif
}

void ImageEffectNode::validBitDepthConnections() const
{
	// validation
	for( ClipImageMap::const_iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && clip.isConnected() )
		{
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			if( clip.getBitDepth() != linkClip.getBitDepth() )
			{
				BOOST_THROW_EXCEPTION( exception::Logic()
					<< exception::dev() + "Error in graph bit depth propagation."
							      "Connection between " + clip.getFullName() + " (" + clip.getBitDepth() + " bytes)" + " => " + linkClip.getFullName() + " (" + linkClip.getBitDepth() + " bytes)."
					<< exception::pluginName( getName() )
					<< exception::pluginIdentifier( getPlugin().getIdentifier() ) );
			}
		}
	}
}

void ImageEffectNode::getTimeDomain( OfxRangeD& range ) const
{
	// ask to the plugin
	if( getTimeDomainAction( range ) )
	{
		TUTTLE_TCOUT( "getTimeDomain " << quotes(getName()) << " computed by the plugin." );
		return;
	}

//	TUTTLE_TCOUT( "getTimeDomain " << quotes(getName()) << " computed by the host." );
	// if no answer, compute it from input clips
	bool first = true;
	OfxRangeD mergeRange;
	mergeRange.min = kOfxFlagInfiniteMin;
	mergeRange.max = kOfxFlagInfiniteMax;
	for( ClipImageMap::const_iterator it = _clips.begin();
	     it != _clips.end();
	     ++it )
	{
		const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( it->second ) );
		if( !clip.isOutput() && clip.isConnected() )
		{
			const attribute::ClipImage& linkClip = clip.getConnectedClip();
			OfxRangeD clipRange;
			linkClip.getNode().getTimeDomain( clipRange ); /// @todo tuttle: how to not recompute all timeDomain of all previousNodes? Need vData?
			if( first )
			{
				first = false;
				mergeRange = clipRange;
			}
			else
			{
				mergeRange.min = std::min( mergeRange.min, clipRange.min );
				mergeRange.max = std::max( mergeRange.max, clipRange.max );
			}
		}
	}
	range = mergeRange;
//	TUTTLE_TCOUT_VAR( range );
}


void ImageEffectNode::beginSequence( graph::ProcessVertexData& vData )
{
//	TUTTLE_TCOUT( "begin: " << getName() );
	beginSequenceRenderAction(
			vData._renderTimeRange.min,
			vData._renderTimeRange.max,
			vData._step,
			vData._interactive,
			vData._renderScale
		);
}

void ImageEffectNode::preProcess1( graph::ProcessVertexAtTimeData& vData )
{
	TUTTLE_TCOUT( "preProcess1_finish: " << getName() << " at time: " << vData._time );
//	setCurrentTime( vData._time );

	checkClipsConnections();

	getClipPreferencesAction();
	initComponents();
	initPixelAspectRatio();
	maximizeBitDepthFromReadsToWrites();

	OfxRectD rod;
	getRegionOfDefinitionAction(
			vData._time,
			vData._nodeData->_renderScale,
			rod );
//	TUTTLE_TCOUT_VAR3( this->getName(), vData._time, rod );
//	TUTTLE_TCOUT_VAR( &getData(vData._time) );
//	TUTTLE_TCOUT_VAR( &vData );
	vData._apiImageEffect._renderRoD = rod;
	vData._apiImageEffect._renderRoI = rod; ///< @todo tuttle: tile supports

	TUTTLE_TCOUT_VAR( rod );
}


void ImageEffectNode::preProcess2_reverse( graph::ProcessVertexAtTimeData& vData )
{
//	TUTTLE_TCOUT( "preProcess2_finish: " << getName() << " at time: " << vData._time );

	maximizeBitDepthFromWritesToReads();
	
	getRegionOfInterestAction( vData._time,
				   vData._nodeData->_renderScale,
				   vData._apiImageEffect._renderRoI,
				   vData._apiImageEffect._inputsRoI );
//	TUTTLE_TCOUT_VAR( vData._renderRoD );
//	TUTTLE_TCOUT_VAR( vData._renderRoI );
}


void ImageEffectNode::preProcess3( graph::ProcessVertexAtTimeData& vData )
{
//	TUTTLE_TCOUT( "preProcess3_finish: " << getName() << " at time: " << vData._time );
	maximizeBitDepthFromReadsToWrites();
//	coutBitDepthConnections();
	validBitDepthConnections();
}


bool ImageEffectNode::isIdentity( const graph::ProcessVertexAtTimeData& vData, std::string& clip, OfxTime& time ) const
{
	time = vData._time;
	OfxRectI roi;
	roi.x1 = std::floor( vData._apiImageEffect._renderRoI.x1 );
	roi.x2 = std::ceil( vData._apiImageEffect._renderRoI.x2 );
	roi.y1 = std::floor( vData._apiImageEffect._renderRoI.y1 );
	roi.y2 = std::ceil( vData._apiImageEffect._renderRoI.y2 );
	return isIdentityAction( time, vData._apiImageEffect._field, roi, vData._nodeData->_renderScale, clip );
}


void ImageEffectNode::preProcess_infos( const graph::ProcessVertexAtTimeData& vData, const OfxTime time, graph::ProcessVertexAtTimeInfo& nodeInfos ) const
{
//	TUTTLE_TCOUT( "preProcess_infos: " << getName() );
	const OfxRectD rod             = vData._apiImageEffect._renderRoD;
	const std::size_t bitDepth     = this->getOutputClip().getBitDepth(); // value in bytes
	const std::size_t nbComponents = getOutputClip().getNbComponents();
	nodeInfos._memory = std::ceil( ( rod.x2 - rod.x1 ) * ( rod.y2 - rod.y1 ) * nbComponents * bitDepth );
}


void ImageEffectNode::process( graph::ProcessVertexAtTimeData& vData )
{
//	TUTTLE_TCOUT( "process: " << getName() );
	memory::IMemoryCache& memoryCache( Core::instance().getMemoryCache() );
	// keep the hand on all needed datas during the process function
	std::list<memory::CACHE_ELEMENT> allNeededDatas;

	const OfxRectI roi = {
		boost::numeric_cast<int>( floor( vData._apiImageEffect._renderRoI.x1 ) ),
		boost::numeric_cast<int>( floor( vData._apiImageEffect._renderRoI.y1 ) ),
		boost::numeric_cast<int>( ceil( vData._apiImageEffect._renderRoI.x2 ) ),
		boost::numeric_cast<int>( ceil( vData._apiImageEffect._renderRoI.y2 ) )
	};
//	TUTTLE_TCOUT_VAR( roi );

//	INode::ClipTimesSetMap timesSetMap = this->getFramesNeeded( vData._time );
	
	// acquire needed clip images
	/*
	TUTTLE_TCOUT_X( 80, "_" );
	TUTTLE_TCOUT( "acquire needed input clip images" );
	TUTTLE_TCOUT_VAR( vData._inEdges.size() );
	TUTTLE_TCOUT_VAR( vData._outEdges.size() );
	BOOST_FOREACH( const graph::ProcessEdgeAtTime* o, vData._outEdges )
	{
		TUTTLE_TCOUT_VAR( o );
		TUTTLE_TCOUT_VAR( o->getInTime() );
		TUTTLE_TCOUT_VAR( o->getInAttrName() );
	}
	BOOST_FOREACH( const graph::ProcessEdgeAtTime* i, vData._inEdges )
	{
		TUTTLE_TCOUT_VAR( i );
		TUTTLE_TCOUT_VAR( i->getInTime() );
		TUTTLE_TCOUT_VAR( i->getInAttrName() );
	}
	TUTTLE_TCOUT_X( 40, "-" );
	*/
	BOOST_FOREACH( const graph::ProcessEdgeAtTime* inEdge, vData._inEdges )
	{
		//TUTTLE_TCOUT_VAR( i );
		//TUTTLE_TCOUT_VAR( i->getInTime() );
		//TUTTLE_TCOUT_VAR( i->getInAttrName() );
		attribute::ClipImage& clip = getClip( inEdge->getInAttrName() );
		const OfxTime outTime = inEdge->getOutTime();
		
		TUTTLE_TCOUT_X( 20, "*" );
		TUTTLE_TCOUT_VAR2( clip.getClipIdentifier(), outTime );
		TUTTLE_TCOUT_VAR2( inEdge->getOut(), inEdge->getIn() );
		memory::CACHE_ELEMENT imageCache( memoryCache.get( clip.getClipIdentifier(), outTime ) );
		if( imageCache.get() == NULL )
		{
			BOOST_THROW_EXCEPTION( exception::Memory()
				<< exception::dev() + "Input attribute " + quotes( clip.getFullName() ) + " at time " + vData._time + " not in memory cache (identifier:" + quotes( clip.getClipIdentifier() ) + ")." );
		}
		allNeededDatas.push_back( imageCache );
	}
	
	TUTTLE_TCOUT( "acquire needed output clip images" );
	BOOST_FOREACH( ClipImageMap::value_type& i, _clips )
	{
		attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( i.second ) );
		if( clip.isOutput() )
		{
			TUTTLE_TCOUT_VAR( vData._apiImageEffect._renderRoI );
			memory::CACHE_ELEMENT imageCache( new attribute::Image(
					clip,
					vData._time,
					vData._apiImageEffect._renderRoI,
					attribute::Image::eImageOrientationFromBottomToTop,
					0 )
				);
			imageCache->setPoolData( Core::instance().getMemoryPool().allocate( imageCache->getMemorySize() ) );
			memoryCache.put( clip.getClipIdentifier(), vData._time, imageCache );
			
			allNeededDatas.push_back( imageCache );
		}
//		else
//		{
//			if( ! clip.isConnected() )
//				continue;
//			
//			// for each framesNeeded
//			const INode::TimesSet& timesSet = timesSetMap[clip.getName()]; /// @todo tuttle: for each edge use edge._outTime
//			if( timesSet.size() == 0 )
//				continue; // the plugin don't use this input (is it allowed by the standard?)
//			BOOST_FOREACH( const INode::TimesSet::value_type& inTime, timesSet )
//			{
//				memory::CACHE_ELEMENT imageCache( memoryCache.get( clip.getClipIdentifier(), inTime ) );
//				if( imageCache.get() == NULL )
//				{
//					BOOST_THROW_EXCEPTION( exception::Memory()
//						<< exception::dev() + "Input attribute " + quotes( clip.getFullName() ) + " at time " + vData._time + " not in memory cache (identifier:" + quotes( clip.getClipIdentifier() ) + ")." );
//				}
//				allNeededDatas.push_back( imageCache );
//			}
//		}
	}
	TUTTLE_TCOUT_X( 40, "-" );
	TUTTLE_TCOUT( "Plugin Render Action" );

	renderAction( vData._time,
		      vData._apiImageEffect._field,
		      roi,
		      vData._nodeData->_renderScale );

	TUTTLE_TCOUT_X( 40, "-" );
	
	debugOutputImage( vData._time );

	// release input images
	BOOST_FOREACH( const graph::ProcessEdgeAtTime* inEdge, vData._inEdges )
	{
		attribute::ClipImage& clip = getClip( inEdge->getInAttrName() );
		const OfxTime outTime = inEdge->getOutTime();
		
		TUTTLE_TCOUT_X( 20, "!" );
		TUTTLE_TCOUT_VAR2( clip.getClipIdentifier(), outTime );
		TUTTLE_TCOUT_VAR2( inEdge->getOut(), inEdge->getIn() );
		
		//TUTTLE_TCOUT_VAR2( clip.getIdentifier(), clip.getFullName() );
		memory::CACHE_ELEMENT imageCache = memoryCache.get( clip.getClipIdentifier(), outTime );
		if( imageCache.get() == NULL )
		{
			BOOST_THROW_EXCEPTION( exception::Memory()
				<< exception::dev() + "Clip " + quotes( clip.getFullName() ) + " not in memory cache (identifier: " + quotes( clip.getClipIdentifier() ) + ", time: " + outTime + ")." );
		}
		imageCache->releaseReference();
	}
	
	// declare future usages of the output
	BOOST_FOREACH( ClipImageMap::value_type& item, _clips )
	{
		attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>( *( item.second ) );
		if( ! clip.isOutput() && ! clip.isConnected() )
			continue;
		
		if( clip.isOutput() )
		{
			memory::CACHE_ELEMENT imageCache = memoryCache.get( clip.getClipIdentifier(), vData._time );
			if( imageCache.get() == NULL )
			{
				BOOST_THROW_EXCEPTION( exception::Memory()
					<< exception::dev() + "Clip " + quotes( clip.getFullName() ) + " not in memory cache (identifier:" + quotes( clip.getClipIdentifier() ) + ")." );
			}
			TUTTLE_TCOUT( "== Declare future usages ==" );
			TUTTLE_TCOUT_VAR2( clip.getClipIdentifier(), clip.getFullName() );
			TUTTLE_TCOUT( "++ image->addReference: " << vData._outDegree );
			if( vData._outDegree > 0 )
			{
				imageCache->addReference( vData._outDegree ); // add a reference on this node for each future usages
			}
		}
//		else
//		{
//			const INode::TimesSet& timesSet = timesSetMap[clip.getName()]; /// @todo tuttle: use edge._timesNeeded
//			if( timesSet.size() == 0 )
//				continue; // the plugin don't use this input (is it allowed by the standard?)
//			BOOST_FOREACH( const INode::TimesSet::value_type& inTime, timesSet )
//			{
//				//TUTTLE_TCOUT_VAR2( clip.getIdentifier(), clip.getFullName() );
//				memory::CACHE_ELEMENT imageCache = memoryCache.get( clip.getClipIdentifier(), inTime );
//				if( imageCache.get() == NULL )
//				{
//					BOOST_THROW_EXCEPTION( exception::Memory()
//						<< exception::dev() + "Clip " + quotes( clip.getFullName() ) + " not in memory cache (identifier:" + quotes( clip.getClipIdentifier() ) + ")." );
//				}
//				imageCache->releaseReference();
//			}
//		}
	}
}


void ImageEffectNode::postProcess( graph::ProcessVertexAtTimeData& vData )
{
//	TUTTLE_TCOUT( "postProcess: " << getName() );
}


void ImageEffectNode::endSequence( graph::ProcessVertexData& vData )
{
//	TUTTLE_TCOUT( "end: " << getName() );
	endSequenceRenderAction( vData._renderTimeRange.min,
			 vData._renderTimeRange.max,
			 vData._step,
			 vData._interactive,
			 vData._renderScale );
}


std::ostream& ImageEffectNode::print( std::ostream& os ) const
{
	const ImageEffectNode& v = *this;
	os << "________________________________________________________________________________" << std::endl;
	os << "Plug-in:" << v.getLabel() << std::endl;
	os << "Description:" << v.getLongLabel() << std::endl;
	os << "Context:" << v._context << std::endl;
	os << "Clips:" << std::endl;
	for( ImageEffectNode::ClipImageMap::const_iterator it = v._clips.begin(), itEnd = v._clips.end();
	     it != itEnd;
	     ++it )
	{
		os << "  * " << it->second->getName() << std::endl;
	}
	os << "Params:" << std::endl;
	for( ImageEffectNode::ParamVector::const_iterator it = v._paramVector.begin(), itEnd = v._paramVector.end();
	     it != itEnd;
	     ++it )
	{
		os << "  * " << it->getName() << " (" << it->getLabel() << "): " <<	it->displayValues(os) << std::endl;
	}
	os << "________________________________________________________________________________" << std::endl;
	return os;
}

std::ostream& operator<<( std::ostream& os, const ImageEffectNode& v )
{
	return v.print(os);
}

void ImageEffectNode::debugOutputImage( const OfxTime time ) const
{
	#ifdef TUTTLE_DEBUG_OUTPUT_ALL_NODES
	IMemoryCache& memoryCache( Core::instance().getMemoryCache() );

	boost::shared_ptr<Image> image = memoryCache.get( this->getName() + "." kOfxOutputAttributeName, time );

	// big hack, for debug...
	image->debugSaveAsPng( "data/debug/" + boost::lexical_cast<std::string>( time ) + "_" + this->getName() + ".png" );
	#endif
}

}
}
