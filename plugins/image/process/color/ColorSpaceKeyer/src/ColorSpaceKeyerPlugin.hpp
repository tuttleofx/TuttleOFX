#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_PLUGIN_HPP_

#include "ColorSpaceKeyerDefinitions.hpp"
#include "CloudPointData.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

template <typename Scalar>
struct ColorSpaceKeyerProcessParams
{
    OFX::Clip* _clipColor; // clip color
    ColorSpaceKeyerProcessParams()
        : _clipColor(NULL)
    {
    }
};

// Structure used to evite to recompute geodesic form on each render
struct CSProcessParams
{
    OfxTime time;               // current time
    bool recomputeGeodesicForm; // does data need to recomputed
    // Create geodesic form
    GeodesicForm geodesicFormColor; // color form
    GeodesicForm geodesicFormSpill; // spill form
};

/**
 * @brief ColorSpaceKeyer plugin
 */
class ColorSpaceKeyerPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

    // clips
    OFX::Clip* _clipColor; // clip color
    OFX::Clip* _clipSpill; // clip spill

    // parameters
    OFX::BooleanParam* _paramBoolPointCloudDisplay;       // display cloud point - check box
    OFX::BooleanParam* _paramBoolDiscretizationActive;    // is discretization active on point cloud - check box
    OFX::IntParam* _paramIntDiscretization;               // discretization step - Int parameter
    OFX::IntParam* _paramIntNbOfDivisionsGF;              // number of divisions geodesic form - Int parameter
    OFX::BooleanParam* _paramBoolDisplayGeodesicForm;     // see color geodesic form on screen - check box
    OFX::ChoiceParam* _paramChoiceAverageMode;            // choice average mode - Choice parameter
    OFX::RGBAParam* _paramRGBAColorSelection;             // color average selection - RGBA parameter
    OFX::PushButtonParam* _paramPushButtonAverageCompute; // color average computing - Push button
    OFX::BooleanParam* _paramBoolSeeSelection;            // see color selection on overlay - check box
    OFX::DoubleParam* _paramDoubleScaleGF;                // scale geodesic form - Double parameters
    OFX::BooleanParam* _paramBoolSeeSpillSelection;       // see spill selection on overlay - check box
    OFX::BooleanParam* _paramBoolDisplaySpillGF;          // see spill geodesic form on screen - check box

    // Overlay data parameters
    bool _updateVBO;                 // VBO data has been changed so update VBO
    bool _updateAverage;             // Color clip source has changed so update average
    bool _updateGeodesicForm;        // Average of Geodesic parameters has changed so update
    bool _updateGeodesicFormAverage; // Change Geodesic average parameters
    bool _resetViewParameters;       // Push button - reset transform parameters has changed
    bool _presetAverageSelection;    // Compute average selection in overlay

    // Render attributes
    OfxPointD _renderScale; // current render scale
    OfxTime _time;          // current time

    // Process attributes
    CSProcessParams _renderAttributes; // contain current time and if data need to be recomputed

    // Scoped pointer attributes
    boost::scoped_ptr<CloudPointData> _cloudPointData; // scoped pointer points the overlay data (or NULL)
    std::size_t _cloudPointDataCount;                  // count (class calling scoped pointer)

public:
    ColorSpaceKeyerPlugin(OfxImageEffectHandle handle);

    ColorSpaceKeyerProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    /*Input function*/
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName); // a GUI param has changed
    void changedClip(const OFX::InstanceChangedArgs& args, const std::string& clipName);   // the source clip has changed

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip,
                    double& identityTime); // does plugin do something

    void render(const OFX::RenderArguments& args); // treatment

    /** @brief client end sequence render function, this is one of the few that must be set */
    void endSequenceRender(const OFX::EndSequenceRenderArguments& args)
    {
        // clear maps of process datas
    }

    // Overlay data functions
    void addRefCloudPointData();                     // add reference to cloud point data
    void releaseCloudPointData();                    // release reference to cloud point data
    bool hasCloudPointData() const;                  // is there cloud point data ?
    CloudPointData& getCloudPointData();             // getter/setter
    const CloudPointData& getCloudPointData() const; // const getter
private:
    void updateGeodesicForms(const OFX::InstanceChangedArgs& args); // update color & spill geodesic forms
    void
    updateProcessGeodesicForms(const OFX::RenderArguments& args); // update color & spill geodesic forms (used in process)
};
}
}
}

#endif
