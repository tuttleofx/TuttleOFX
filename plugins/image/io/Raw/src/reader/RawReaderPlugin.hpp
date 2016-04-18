#ifndef _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_

#include "RawReaderDefinitions.hpp"
#include <tuttle/ioplugin/context/ReaderPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace raw
{
namespace reader
{

template <typename Scalar>
struct RawReaderProcessParams
{
    std::string _filepath; ///< filepath
    EFiltering _filtering;
    EInterpolation _interpolation;
    EOutputColor _outputColor;
    float _gammaPower;
    float _gammaToe;
    double _redAbber;
    double _greenAbber;

    double _bright;
    bool _autoBright;
    double _threshold;
    bool _fourColorRgb;

    EHighlight _hightlight;

    double _exposure;
    double _exposurePreserve;

    EWhiteBalance _whiteBalance;
    double _manualWBKelvin;

    EFBDDNoiseRd _fbddNoiseRd;

    boost::gil::point2<Scalar> _greyboxPoint;
    boost::gil::point2<Scalar> _greyboxSize;
};

/**
 * @brief Raw reader
 *
 */
class RawReaderPlugin : public ReaderPlugin
{
public:
    typedef float Scalar;
    typedef boost::gil::point2<double> Point2;

public:
    RawReaderPlugin(OfxImageEffectHandle handle);

public:
    RawReaderProcessParams<Scalar> getProcessParams(const OfxTime time);

    void updateInfos(const OfxTime time);

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);
    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);

    void render(const OFX::RenderArguments& args);

public:
    /// @name user parameters
    /// @{
    OFX::ChoiceParam* _paramFiltering; ///< Filtering mode
    OFX::ChoiceParam* _paramInterpolation;
    OFX::ChoiceParam* _paramOutputColor;

    OFX::DoubleParam* _paramGammaPower;
    OFX::DoubleParam* _paramGammaToe;
    OFX::DoubleParam* _paramRedAbber;
    OFX::DoubleParam* _paramGreenAbber;

    OFX::DoubleParam* _paramBright;
    OFX::BooleanParam* _paramAutoBright;

    OFX::DoubleParam* _paramThreshold;
    OFX::BooleanParam* _paramFourColorRgb;

    OFX::ChoiceParam* _paramHighlight;

    OFX::DoubleParam* _paramExposure;
    OFX::DoubleParam* _paramExposurePreserve;

    OFX::ChoiceParam* _paramWhiteBalance;
    OFX::DoubleParam* _paramManualWBKelvin;

    OFX::Double2DParam* _paramGreyboxPoint;
    OFX::Double2DParam* _paramGreyboxSize;

    OFX::ChoiceParam* _paramFbddNoiseRd;

    /// metadata
    OFX::StringParam* _paramManufacturer;
    OFX::StringParam* _paramModel;
    OFX::IntParam* _paramIso;
    OFX::IntParam* _paramShutter;
    OFX::DoubleParam* _paramAperture;
    OFX::IntParam* _paramFocal;
    OFX::DoubleParam* _paramWBR;
    OFX::DoubleParam* _paramWBG;
    OFX::DoubleParam* _paramWBB;
    OFX::StringParam* _paramDateOfShooting;
    OFX::StringParam* _paramGPS;
    OFX::StringParam* _paramDesc;
    OFX::StringParam* _paramArtist;
    /// @}
};
}
}
}
}

#endif
