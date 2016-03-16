#include "ColorSpaceKeyerPlugin.hpp"
#include "ColorSpaceKeyerProcess.hpp"
#include "ColorSpaceKeyerDefinitions.hpp"
#include "CloudPointData.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

ColorSpaceKeyerPlugin::ColorSpaceKeyerPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _clipColor = fetchClip(kClipColorSelection);
    _clipSpill = fetchClip(kClipSpillSelection);

    _paramBoolPointCloudDisplay = fetchBooleanParam(kPointCloudDisplay); // Is CloudPointData displayed (boolean parameter)
    _cloudPointDataCount = 0;

    // Associate intern parameters pointers to GUI components
    _paramBoolDiscretizationActive =
        fetchBooleanParam(kBoolDiscretizationDisplay); // is discretization active on point cloud - check box
    _paramIntDiscretization = fetchIntParam(kIntDiscretizationDisplay); // discretization step - Int parameter
    _paramIntNbOfDivisionsGF =
        fetchIntParam(kIntNumberOfDivisonGeodesicForm); // number of divisions geodesic form - Int parameter
    _paramBoolDisplayGeodesicForm = fetchBooleanParam(kBoolOnlySelection); // display geodesic form - check box
    _paramChoiceAverageMode = fetchChoiceParam(kColorAverageMode); // average color mode - Choice parameter
    _paramRGBAColorSelection = fetchRGBAParam(kColorAverageSelection); // average color selection - RGBA parameter
    _paramPushButtonAverageCompute = fetchPushButtonParam(kColorAverageComputing); // average color computing - Push button
    _paramBoolSeeSelection = fetchBooleanParam(kBoolColorSelectionDisplay); // see selection on overlay - check box
    _paramDoubleScaleGF = fetchDoubleParam(kDoubleScaleGeodesicForm); // scale geodesic form - double parameter
    _paramBoolSeeSpillSelection = fetchBooleanParam(kBoolSpillSelectionDisplay); // see spill selection - check box
    _paramBoolDisplaySpillGF = fetchBooleanParam(kBoolDisplaySpillGF); // see spill geodesic form - check box

    // verify display Discrete enable value
    if(_paramBoolPointCloudDisplay->getValue()) // called default value
    {
        _paramBoolDiscretizationActive->setEnabled(true); // Enable discretization check box
        _paramIntDiscretization->setEnabled(true); // Enable discretization int parameter
        _paramBoolDisplayGeodesicForm->setEnabled(true); // Enable see color geodesic form display
        _paramBoolSeeSelection->setEnabled(true); // Enable see color selection display
        _paramBoolDisplaySpillGF->setEnabled(true); // Enable see spill geodesic form display
        _paramBoolSeeSpillSelection->setEnabled(true); // Enable see spill selection display
    }
    // verify choice average enable value
    if(_paramChoiceAverageMode->getValue() == 1)
    {
        _paramRGBAColorSelection->setEnabled(true); // Enable color average selection (RGBA parameter)
        _paramPushButtonAverageCompute->setEnabled(true); // Enable color average computing (Push button)
    }

    _updateAverage = false; // does display need to update average
    _updateGeodesicForm = false; // does display need to update geodesic form
    _updateVBO = false; // does display need to update VBO
    _updateGeodesicFormAverage = false; // does Geodesic form need to be updated
    _resetViewParameters = false; // do view parameters need to be reseted
    _presetAverageSelection = false; // does average selection need to be set
}

ColorSpaceKeyerProcessParams<ColorSpaceKeyerPlugin::Scalar>
ColorSpaceKeyerPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    ColorSpaceKeyerProcessParams<Scalar> params; // create parameters container object
    return params;                               // pass parameters to process
}

/**
 * A GUI parameter has changed
 * @param args		current time and renderScale
 * @param paramName parameter name (variable name)
 */
void ColorSpaceKeyerPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kPointCloudDisplay) // display point cloud check box value has changed
    {
        if(_paramBoolPointCloudDisplay->getValue() && hasCloudPointData()) // display point cloud is selected
        {
            // enable discretization GUI components
            _paramBoolDiscretizationActive->setEnabled(true); // Enable discretization check box
            _paramIntDiscretization->setEnabled(true); // Enable discretization int parameter
            _paramBoolDisplayGeodesicForm->setEnabled(true); // Enable geodesic form display
            _paramBoolSeeSelection->setEnabled(true); // Enable geodesic form display
            _paramBoolDisplaySpillGF->setEnabled(true); // Enable see spill geodesic form display
            _paramBoolSeeSpillSelection->setEnabled(true); // Enable see spill selection display

            // generate VBO data (VBO object is created in overlay)
            getCloudPointData()._time = args.time; // update time
            getCloudPointData().generateVBOData( // create a VBO
                _clipSrc, // source clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // generate color selection VBO data (object created in overlay)
            getCloudPointData().generateColorSelectionVBO( // create a color selection VBO
                _clipColor, // color clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // generate spill selection VBO data (object created in overlay)
            getCloudPointData().generateSpillSelectionVBO( // create a spill selection VBO
                _clipSpill, // spill clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // update geodesic form
            updateGeodesicForms(args); // update geodesic form
        }
        else // display point cloud is not selected
        {
            // disable discretization GUI components
            _paramBoolDiscretizationActive->setEnabled(false); // Disable discretization treatment
            _paramIntDiscretization->setEnabled(false); // Disable discretization choice
            _paramBoolDisplayGeodesicForm->setEnabled(false); // Disable geodesic form display
            _paramBoolSeeSelection->setEnabled(false); // Disable geodesic form display
            _paramBoolDisplaySpillGF->setEnabled(false); // Disable see spill geodesic form display
            _paramBoolSeeSpillSelection->setEnabled(false); // Disable see spill selection display
        }
    }
    if(paramName == kBoolDiscretizationDisplay) // discretization active check box changed
    {
        if(hasCloudPointData()) // if there is overlay data
        {
            // generate VBO data
            getCloudPointData()._time = args.time; // update time
            getCloudPointData().generateVBOData( // create a VBO data
                _clipSrc, // source clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // generate selection VBO data
            getCloudPointData().generateColorSelectionVBO( // create a selection VBO
                _clipColor, // color clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // generate spill selection VBO data (object created in overlay)
            getCloudPointData().generateSpillSelectionVBO( // create a spill selection VBO
                _clipSpill, // spill clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization mode active
                _paramIntDiscretization->getValue()); // get discretization step
            // update VBOs (create VBO objects in overlay)
            _updateVBO = true; // update VBO on overlay
        }
    }
    if(paramName == kIntDiscretizationDisplay) // discretization value has changed (int range)
    {
        if(hasCloudPointData() && _paramBoolDiscretizationActive->getValue()) // it is not batch mode
        {
            if(_paramBoolDiscretizationActive->getValue()) // discretization is active
            {
                getCloudPointData()._time = args.time; // update time
                getCloudPointData()
                    .generateVBOData( // create a VBO data with discretization (and the new discretization step)
                        _clipSrc, // source clip
                        args.renderScale, // current render scale
                        true, // create cloud point with discretization
                        _paramIntDiscretization->getValue()); // get discretization step
                _updateVBO = true; // update VBO on overlay
            }
        }
    }
    if(paramName == kIntNumberOfDivisonGeodesicForm) // number of divisions geodesic form has changed (int range)
    {
        if(hasCloudPointData()) // if there is overlay data
        {
            updateGeodesicForms(args);
        }
        _renderAttributes.recomputeGeodesicForm = true; // recompute process attributes
    }
    if(paramName == kPushButtonResetTransformationParameters &&
       hasCloudPointData()) // Push button reset transformation has changed
    {
        _resetViewParameters = true; // reset rotation view parameters on overlay
    }
    if(paramName == kColorAverageMode && hasCloudPointData()) // Average mode : choice list
    {
        if(_paramChoiceAverageMode->getValue() == 1) // selection mode is manual
            _paramRGBAColorSelection->setEnabled(true); // active component
        else
            _paramRGBAColorSelection->setEnabled(false); // disable component
        updateGeodesicForms(args); // update geodesic form
    }
    if(paramName == kColorAverageSelection && hasCloudPointData()) // selection average RGBA component
    {
        updateGeodesicForms(args);
    }
    if(paramName == kColorAverageComputing && hasCloudPointData()) // push button : average computing (selection => manual)
    {
        // update automatic average
        getCloudPointData()._averageColor._time = args.time; // change current time
        getCloudPointData()._averageColor.computeAverageSelection(_clipColor, args.renderScale); // compute automatic average
        // set computed average value into GUI component
        double alpha = 1.0; // alpha value of new average (doesn't care)
        _paramRGBAColorSelection->setValue(getCloudPointData()._averageColor._averageValue.x, // red value
                                           getCloudPointData()._averageColor._averageValue.y, // green value
                                           getCloudPointData()._averageColor._averageValue.z, // blue value
                                           alpha // alpha value
                                           );
        // compute geodesic form
        changedParam(args, kColorAverageSelection); // call changedParam function with modification of selectionAverage value
    }
    if(paramName == kDoubleScaleGeodesicForm && hasCloudPointData())
    {
        updateGeodesicForms(args);
    }
}

/*
 * If clip has changed
 */
void ColorSpaceKeyerPlugin::changedClip(const OFX::InstanceChangedArgs& args, const std::string& clipName)
{
    if(clipName == kOfxImageEffectSimpleSourceClipName) // if source clip has changed
    {
        if(this->hasCloudPointData()) // it is not batch mode
        {
            OFX::Clip* test = fetchClip(clipName); // test if source clip is connected
            if(test->isConnected())
            {
                getCloudPointData()._time = args.time; // update time
                getCloudPointData().generateVBOData( // create a VBO data with or without discretization
                    _clipSrc, // source clip
                    args.renderScale, // current render scale
                    _paramBoolDiscretizationActive->getValue(), // is discretization used to generate VBO
                    _paramIntDiscretization->getValue()); // discretization step

                _updateVBO = true; // update VBO on overlay
                this->redrawOverlays(); // redraw scene
            }
            else // source clip is no more connected
                getCloudPointData()._imgVBO.deleteVBO(); // delete VBO
        }
    }
    else if(clipName == kClipColorSelection) // if color clip has changed
    {
        _renderAttributes.recomputeGeodesicForm = true; // recompute process attributes
        if(this->hasCloudPointData()) // it is not batch mode
        {
            OFX::Clip* test = fetchClip(clipName); // test if color clip is connected
            if(test->isConnected())
            {
                // generate selection VBO data
                getCloudPointData()._time = args.time; // update time
                getCloudPointData().generateColorSelectionVBO( // create a selection VBO data with or without discretization
                    _clipColor, // color clip
                    args.renderScale, // current render scale
                    _paramBoolDiscretizationActive->getValue(), // is discretization used to generate selection VBO
                    _paramIntDiscretization->getValue()); // discretization step
                updateGeodesicForms(args);
            }
        }
    }
    else if(clipName == kClipSpillSelection) // if color clip has changed
    {
        _renderAttributes.recomputeGeodesicForm = true; // recompute process attributes
        if(this->hasCloudPointData()) // it is not batch mode
        {
            OFX::Clip* test = fetchClip(clipName); // test if color clip is connected
            if(test->isConnected())
            {

                // generate spill selection VBO data
                getCloudPointData()._time = args.time; // update time
                getCloudPointData()
                    .generateSpillSelectionVBO( // create a spill selection VBO data with or without discretization
                        _clipSpill, // spill clip
                        args.renderScale, // current render scale
                        _paramBoolDiscretizationActive->getValue(), // is discretization used to generate selection VBO
                        _paramIntDiscretization->getValue()); // discretization step
                updateGeodesicForms(args);
            }
        }
    }
    else
    {
    }
}

bool ColorSpaceKeyerPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	ColorSpaceKeyerProcessParams<Scalar> params = getProcessParams();
    //	if( params._in == params._out )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorSpaceKeyerPlugin::render(const OFX::RenderArguments& args)
{
    if(OFX::getImageEffectHostDescription()->hostName ==
           "uk.co.thefoundry.nuke" && /// HACK: Nuke doesn't call changeClip function when time is changed
       hasCloudPointData())           // if there is overlay data
    {
        if(args.time != getCloudPointData()._time) // different time between overlay and VBO data
        {
            // update VBO data
            getCloudPointData()._time = args.time; // change computing time in cloud point data
            getCloudPointData().generateVBOData( // create a VBO data (with discretization or not)
                _clipSrc, // source clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization used
                _paramIntDiscretization->getValue()); // discretization step

            // update selection VBO data
            getCloudPointData().generateColorSelectionVBO( // create a selection VBO data (with or without discretization)
                _clipColor, // color clip
                args.renderScale, // current render scale
                _paramBoolDiscretizationActive->getValue(), // is discretization used
                _paramIntDiscretization->getValue()); // discretization step
            _updateVBO = true; // VBO need to be updated in overlay
        }
        if(args.time != getCloudPointData()._averageColor._time) // different time between overlay and average data
        {
            // update automatic average
            getCloudPointData()._averageColor._time = args.time; // update time
            getCloudPointData()._averageColor.computeAverageSelection(_clipColor, args.renderScale); // update average value
            // color geodesic form
            getCloudPointData()._geodesicFormColor._scale = _paramDoubleScaleGF->getValue(); // set scale value
            // spill geodesic form
            getCloudPointData()._geodesicFormSpill._scale = _paramDoubleScaleGF->getValue(); // set scale value
            // update geodesic form if average mode is automatic
            if(_paramChoiceAverageMode->getValue() == 0) // mode is automatic
            {
                getCloudPointData()._geodesicFormColor.subdiviseFaces(
                    getCloudPointData()._averageColor._averageValue,
                    _paramIntNbOfDivisionsGF->getValue()); // update color geodesic form
                getCloudPointData()._geodesicFormSpill.subdiviseFaces(
                    getCloudPointData()._averageColor._averageValue,
                    _paramIntNbOfDivisionsGF->getValue()); // update geodesic form
            }
            // extends geodesic forms
            getCloudPointData()._averageColor.extendGeodesicForm(
                _clipColor, args.renderScale, getCloudPointData()._geodesicFormColor); // extends geodesic form (color)
            getCloudPointData()._geodesicFormSpill.copyGeodesicForm(
                getCloudPointData()._geodesicFormColor); // extends geodesic form (spill)
            getCloudPointData()._averageColor.extendGeodesicForm(
                _clipSpill, args.renderScale, getCloudPointData()._geodesicFormSpill); // extends geodesic form (spill)
        }
    }
    // update  current parameters for process
    _renderScale = args.renderScale; // change render scale (before rendering)
    _time = args.time; // change time
    // update process attributes
    if(_renderAttributes.recomputeGeodesicForm || _renderAttributes.time != args.time)
        updateProcessGeodesicForms(args);
    // call process functions
    doGilRender<ColorSpaceKeyerProcess>(*this, args); // launch process
}

/*
 *Update geodesic forms for overlay (and call update function for process parameters)
 * @param args
 */
void ColorSpaceKeyerPlugin::updateGeodesicForms(const OFX::InstanceChangedArgs& args)
{
    // update geodesic forms scale values
    getCloudPointData()._geodesicFormColor._scale = _paramDoubleScaleGF->getValue(); // set scale value
    // spill geodesic form
    getCloudPointData()._geodesicFormSpill._scale = _paramDoubleScaleGF->getValue(); // set scale value
    // refresh selection average
    getCloudPointData()._averageColor._time = args.time; // set current time into average
    getCloudPointData()._averageColor.computeAverageSelection(_clipColor, args.renderScale); // update automatic average
    if(_paramChoiceAverageMode->getValue() == 1) // average mode is manual
    {
        // get current average selection
        OfxRGBAColourD selectionColor = _paramRGBAColorSelection->getValue(); // get current average selection
        // transform average color in 3D point average
        Ofx3DPointD selectionAverage; // initialize
        selectionAverage.x = selectionColor.r; // x == red
        selectionAverage.y = selectionColor.g; // y == green
        selectionAverage.z = selectionColor.b; // z == blue
        getCloudPointData()._geodesicFormColor.subdiviseFaces(
            selectionAverage, _paramIntNbOfDivisionsGF->getValue()); // update color geodesic form
        getCloudPointData()._geodesicFormSpill.subdiviseFaces(
            selectionAverage, _paramIntNbOfDivisionsGF->getValue()); // update spill geodesic form
    }
    else // average mode is automatic
    {
        getCloudPointData()._geodesicFormColor.subdiviseFaces(
            getCloudPointData()._averageColor._averageValue,
            _paramIntNbOfDivisionsGF->getValue()); // update color geodesic form
        getCloudPointData()._geodesicFormSpill.subdiviseFaces(
            getCloudPointData()._averageColor._averageValue,
            _paramIntNbOfDivisionsGF->getValue()); // update spill geodesic form
    }
    // extends geodesic forms
    getCloudPointData()._averageColor.extendGeodesicForm(
        _clipColor, args.renderScale, getCloudPointData()._geodesicFormColor); // extends color geodesic form
    getCloudPointData()._geodesicFormSpill.copyGeodesicForm(
        getCloudPointData()._geodesicFormColor); // extends spill geodesic form (color clip)
    getCloudPointData()._averageColor.extendGeodesicForm(
        _clipSpill, args.renderScale, getCloudPointData()._geodesicFormSpill); // extends spill geodesic form (spill clip)
    // scale geodesic form
    getCloudPointData()._geodesicFormColor.scaleGeodesicForm(_paramDoubleScaleGF->getValue());
    getCloudPointData()._geodesicFormSpill.scaleGeodesicForm(_paramDoubleScaleGF->getValue());
    _updateVBO = true; // update VBO in overlay
    _renderAttributes.recomputeGeodesicForm = true; // recompute process attributes
    this->redrawOverlays(); // redraw scene
}

/*
 *Update process parameters (goedesic forms, time and selection average)
 * @param args
 */
void ColorSpaceKeyerPlugin::updateProcessGeodesicForms(const OFX::RenderArguments& args)
{
    // update time
    _renderAttributes.time = args.time; // update time
    // Create geodesic form
    SelectionAverage selectionAverage = SelectionAverage(_time); // create selection
    // update color geodesic form
    _renderAttributes.geodesicFormColor._scale = _paramDoubleScaleGF->getValue(); // set good scale to geodesic form
    // update spill geodesic form
    _renderAttributes.geodesicFormSpill._scale = _paramDoubleScaleGF->getValue(); // set good scale to geodesic forms

    if(_paramChoiceAverageMode->getValue() == 0) // average mode is automatic
    {
        selectionAverage.computeAverageSelection(_clipColor, _renderScale); // compute average selection
        // subdivise geodesic forms
        _renderAttributes.geodesicFormColor.subdiviseFaces(selectionAverage._averageValue,
                                                           _paramIntDiscretization->getValue()); // create geodesic form
        _renderAttributes.geodesicFormSpill.subdiviseFaces(selectionAverage._averageValue,
                                                           _paramIntDiscretization->getValue()); // create geodesic form
    }
    else // average mode is manual
    {
        Ofx3DPointD selectedAverage; // initialize average
        OfxRGBAColourD colorSelected = _paramRGBAColorSelection->getValue(); // get selected color
        selectedAverage.x = colorSelected.r; // x == red
        selectedAverage.y = colorSelected.g; // y == green
        selectedAverage.z = colorSelected.b; // z == blue
        // compute geodesic forms
        _renderAttributes.geodesicFormColor.subdiviseFaces(selectedAverage,
                                                           _paramIntDiscretization->getValue()); // create geodesic form
        _renderAttributes.geodesicFormSpill.subdiviseFaces(selectedAverage,
                                                           _paramIntDiscretization->getValue()); // create geodesic form
    }
    // Extend geodesic forms
    selectionAverage.extendGeodesicForm(_clipColor, _renderScale,
                                        _renderAttributes.geodesicFormColor); // extends geodesic form color
    _renderAttributes.geodesicFormSpill.copyGeodesicForm(
        _renderAttributes.geodesicFormColor); // extends geodesic form spill (color clip)
    selectionAverage.extendGeodesicForm(
        _clipSpill, _renderScale,
        _renderAttributes.geodesicFormSpill); // extends geodesic form spill (spill takes account of spill clip)
    _renderAttributes.recomputeGeodesicForm = false;
}

/// @brief Cloud point data
/// @{
// cloud point data management
// Add a reference to CloudPointData
void ColorSpaceKeyerPlugin::addRefCloudPointData()
{
    if(_cloudPointDataCount == 0) // no reference has been added yet
    {
        const OfxPointI imgSize = this->_clipSrc->getPixelRodSize(0); ///@todo set the correct time !
        _cloudPointData.reset(new CloudPointData(imgSize, 0)); // Create data
    }
    ++_cloudPointDataCount; // increments number of reference
}
// remove a reference to CloudPointData
void ColorSpaceKeyerPlugin::releaseCloudPointData()
{
    --_cloudPointDataCount;
    if(_cloudPointDataCount == 0) // no more reference on CloudPointData
    {
        _cloudPointData.reset(NULL); // reset data
    }
}
// Has CloudPointData already been created (batch mode)
bool ColorSpaceKeyerPlugin::hasCloudPointData() const
{
    return _cloudPointDataCount != 0;
}

CloudPointData& ColorSpaceKeyerPlugin::getCloudPointData()
{
    return *_cloudPointData.get();
}
const CloudPointData& ColorSpaceKeyerPlugin::getCloudPointData() const
{
    return *_cloudPointData.get();
}
/// @}
}
}
}
