// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/EffectInstance.hpp>
#include <tuttle/host/core/ClipInstance.hpp>
#include <tuttle/host/core/ParamInstance.hpp>
#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/Core.hpp>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

// boost
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace boost::gil;
using namespace boost;
int main( int argc, char** argv )
{
	try
	{
		// @TODO_OFX: Members in a class
		//std::string _pixelDepth = kOfxBitDepthByte;
		//std::string _components = kOfxImageComponentRGBA;
		const int numFramesToRender = SOFXCLIPLENGTH;

		tuttle::host::core::Core::instance().preload();

		// get some plugins examples
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* pluginR = tuttle::host::core::Core::instance().getImageEffectPluginById( "fr.hd3d.tuttle.pngreader" );
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* pluginI = tuttle::host::core::Core::instance().getImageEffectPluginById( "fr.hd3d.tuttle.invert" );
		tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin* pluginW = tuttle::host::core::Core::instance().getImageEffectPluginById( "fr.hd3d.tuttle.pngwriter" );

		//tuttle::host::core::Core::instance().getImageEffectPluginCache().dumpToStdOut( );

		if( pluginR == NULL || pluginI == NULL || pluginW == NULL )
		{
			COUT( "Plugin loading error" );
			COUT_VAR( pluginR );
			COUT_VAR( pluginI );
			COUT_VAR( pluginW );
			return 1;
		}
		typedef boost::ptr_vector< tuttle::host::core::EffectInstance > EffectInstVector;
		EffectInstVector vPluginsInst;
		tuttle::host::core::EffectInstance* ofxinstR = dynamic_cast< tuttle::host::core::EffectInstance* >( pluginR->createInstance( kOfxImageEffectContextGenerator, NULL ) );
		tuttle::host::core::EffectInstance* ofxinstI = dynamic_cast< tuttle::host::core::EffectInstance* >( pluginI->createInstance( kOfxImageEffectContextFilter, NULL ) );
		tuttle::host::core::EffectInstance* ofxinstW = dynamic_cast< tuttle::host::core::EffectInstance* >( pluginW->createInstance( kOfxImageEffectContextGeneral, NULL ) );
		ofxinstR->setName("pluginR");
		ofxinstI->setName("pluginI");
		ofxinstW->setName("pluginW");
		vPluginsInst.push_back( ofxinstR );
		vPluginsInst.push_back( ofxinstI );
		vPluginsInst.push_back( ofxinstW );


		// Initialize variables
		if( vPluginsInst.size() > 0 )
		{
			// current render scale of 1
			OfxPointD renderScale = { 1.0, 1.0 };
			for( EffectInstVector::iterator it = vPluginsInst.begin(), itEnd = vPluginsInst.end();
			     it != itEnd;
			     ++it )
			{
				// now we need to call the create instance action. Only call this once you have initialised all the params
				// and clips to their correct values. So if you are loading a saved plugin state, set up your params from
				// that state, _then_ call create instance.
				it->createInstanceAction();

				// now we need to to call getClipPreferences on the instance so that it does the clip component/depth
				// logic and caches away the components and depth on each clip.
				it->getClipPreferences();

				it->dumpToStdOut();
			}
			tuttle::host::core::ClipImgInstance* outputClip, * inputClip;
			tuttle::host::core::Image* inImg;
			tuttle::host::core::Image* outImg;

			// Setup parameters
			tuttle::host::core::StringInstance* srcFileParam = dynamic_cast<tuttle::host::core::StringInstance*>( vPluginsInst[0].getParams()["Input filename"] );
			tuttle::host::core::StringInstance* dstFileParam = dynamic_cast<tuttle::host::core::StringInstance*>( vPluginsInst[2].getParams()["Output filename"] );
			if( srcFileParam && dstFileParam )
			{
				srcFileParam->set( "input.png" );
				dstFileParam->set( "output.png" );
				vPluginsInst[0].paramInstanceChangedAction( srcFileParam->getName(), kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
				vPluginsInst[2].paramInstanceChangedAction( dstFileParam->getName(), kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
			}

			// Setup clips
			tuttle::host::core::ClipImgInstance& inputI = dynamic_cast<tuttle::host::core::ClipImgInstance&>( ofxinstI->getClip(kOfxImageEffectSimpleSourceClipName) );
			tuttle::host::core::ClipImgInstance& inputW = dynamic_cast<tuttle::host::core::ClipImgInstance&>( ofxinstW->getClip(kOfxImageEffectSimpleSourceClipName) );
			inputI.setHackFullName( "pluginR.Output" );
			inputW.setHackFullName( "pluginI.Output" );

			// say we are about to render a bunch of frames
			for( EffectInstVector::iterator it = vPluginsInst.begin(), itEnd = vPluginsInst.end();
			     it != itEnd;
			     ++it )
			{
				it->beginRenderAction( 0, numFramesToRender, 1.0, false, renderScale );
			}

			for( int t = 0; t < numFramesToRender; ++t )
			{
				OfxTime frame = t;

				// get the RoI for each input clip
				// the regions of interest for each input clip are returned in a std::map
				// on a real host, these will be the regions of each input clip that the
				// effect needs to render a given frame (clipped to the RoD).
				std::map<tuttle::host::ofx::attribute::OfxhClipImage*, OfxRectD> rois;
				OfxRectD defaultRoi;
				memset( &defaultRoi, 0, sizeof( OfxRectD ) );
				vPluginsInst[0].getRegionOfInterestAction( frame, renderScale, defaultRoi, rois );

				// get the output clip
				outputClip = dynamic_cast<tuttle::host::core::ClipImgInstance*>( &vPluginsInst[0].getClip( kOfxImageEffectOutputClipName ) );
				/// RoI is in canonical coords,
				OfxRectD regionOfInterest = rois[outputClip];

				double par = vPluginsInst[0].getProjectPixelAspectRatio();

				// The render window is in pixel coordinates and is expected to to be the generator output roi.
				// ie: render scale and a PAR of not 1
				OfxRectI renderWindow = {
					int(regionOfInterest.x1 / par),
					int(regionOfInterest.y1),
					int(regionOfInterest.x2 / par),
					int(regionOfInterest.y2)
				};

				// Generates & propagates
				EffectInstVector::iterator it = vPluginsInst.begin(), itEnd = vPluginsInst.end();
				for( ; it != itEnd-1;
					 ++it )
				{
					( it + 1 )->getRegionOfInterestAction( frame, renderScale, regionOfInterest, rois );
					// get the output clip
					outputClip = dynamic_cast<tuttle::host::core::ClipImgInstance*>( &it->getClip( kOfxImageEffectOutputClipName ) );
					// get next input
					inputClip = dynamic_cast<tuttle::host::core::ClipImgInstance*>( &(it+1)->getClip( kOfxImageEffectSimpleSourceClipName ) );
					it->renderAction( t, kOfxImageFieldBoth, renderWindow, renderScale );
					if( inputClip && outputClip )
					{
						regionOfInterest = tuttle::intersection( regionOfInterest, tuttle::intersection( outputClip->fetchRegionOfDefinition( frame ), rois[inputClip] ) );

						outImg = dynamic_cast<tuttle::host::core::Image*>( outputClip->getImage( frame, &regionOfInterest ) );
						OfxRectD reqRegion = {
							( regionOfInterest.x1 / outputClip->getPixelAspectRatio() ) * inputClip->getPixelAspectRatio(), regionOfInterest.y1,
							( regionOfInterest.x2 / outputClip->getPixelAspectRatio() ) * inputClip->getPixelAspectRatio(), regionOfInterest.y2
						};
						inImg = dynamic_cast<tuttle::host::core::Image*>( inputClip->getImage( frame, &reqRegion ) );

						OfxRectI bounds   = inImg->getBounds();
						OfxPointI sCorner = { 0, 0 };
						OfxPointI dCorner = { 0, 0 };
						OfxPointI count   = { bounds.x2 - bounds.x1, bounds.y2 - bounds.y1 };
						//tuttle::host::core::Image::copy( inImg, outImg, dCorner, sCorner, count );
					}

					// recompute render window
					renderWindow.x1 = (int) std::ceil( regionOfInterest.x1 / par );
					renderWindow.x2 = (int) std::ceil( regionOfInterest.x2 / par );
					renderWindow.y1 = int(regionOfInterest.y1);
					renderWindow.y2 = int(regionOfInterest.y2);
				}
				it->renderAction( t, kOfxImageFieldBoth, renderWindow, renderScale );
			}

			// say we are about to render a bunch of frames
			for( EffectInstVector::iterator it = vPluginsInst.begin(), itEnd = vPluginsInst.end();
					 it != itEnd-1;
					 ++it )
			{
				it->endRenderAction( 0, numFramesToRender, 1.0, false, renderScale );
			}
		}

	}
	catch( std::exception& e )
	{
		std::cout << "Exception : main de tuttle..." << std::endl;
		std::cout << e.what() << std::endl;
	}
	catch( ... )
	{
		std::cout << "Exception..." << std::endl;
	}

	//    COUT_INFOS;
	return 0;
}

