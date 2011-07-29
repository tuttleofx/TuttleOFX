#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_PLUGIN_HPP_

#include "ColorSpaceKeyerDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include "CloudPointData.hpp"

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

template<typename Scalar>
struct ColorSpaceKeyerProcessParams
{
};

/**
 * @brief ColorSpaceKeyer plugin
 */
class ColorSpaceKeyerPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;

	// clips
	OFX::Clip* _clipColor;		//

	//parameters
	OFX::BooleanParam* _paramBoolPointCloudDisplay;		//display cloud point - check box
	OFX::BooleanParam* _paramBoolDiscretizationActive;	//is discretization active on point cloud - check box
	OFX::IntParam* _paramIntDiscretization;				//discretization step - Int param
	
	//Overlay data parameters
	bool _updateVBO;									//VBO data has been changed so update VBO
	boost::scoped_ptr<CloudPointData> _cloudPointData;	//scoped pointer points the overlay data (or NULL)
	std::size_t _cloudPointDataCount;					//count (class calling scoped pointer)
	
public:
    ColorSpaceKeyerPlugin( OfxImageEffectHandle handle );
	
	ColorSpaceKeyerProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	
	/*Input function*/
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );		//a GUI param has changed
    void changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName );			//the source clip has changed

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );	//does plugin do something

    void render( const OFX::RenderArguments &args );	//treatment
	
	//Overlay data functions
	void addRefCloudPointData();					//add reference to cloud point data
	void releaseCloudPointData();					//release reference to cloud point data
	bool hasCloudPointData() const;					//is there cloud point data ?
	CloudPointData& getCloudPointData();				//getter/setter
	const CloudPointData& getCloudPointData() const;	//const getter
};

}
}
}

#endif
