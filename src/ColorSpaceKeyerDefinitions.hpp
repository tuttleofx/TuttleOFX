#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include "tuttle/plugin/ImageGilProcessor.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/algorithm.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/color/hsl.hpp>

#include <vector>


namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {


const static std::string kClipColorSelection = "color";
const static std::string kClipSpillSelection = "spill";


//Drawing constant
const static float kOpenGLSceneMaxSize = 200.0;

//Global display (check box)
const static std::string kPointCloudDisplay = "pointCloudDisplay";
const static std::string kPointCloudDisplayLabel = "Display point cloud";

//Discretization display (check box)
const static std::string kBoolDiscretizationDisplay = "discretizationDisplayBool";
const static std::string kBoolDiscretizationDisplayLabel = "Discretize point cloud";

//Discretization display
const static std::string kIntDiscretizationDisplay = "discretizationDisplay";
const static std::string kIntDiscretizationDisplayLabel = "Discretization step";
const std::size_t discretizationDisplayInt = 10;


//Rotation constants
const static int KMaxDegres = 360;		//360° max for a rotation
const static int kRotationSpeed = 5;	//mouse rotation scale
}
}
}

#endif
