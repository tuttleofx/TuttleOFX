#if defined( _MSC_VER )
#pragma warning( disable : 4244 )
#endif

#include "OCIOLutProcess.hpp"
#include "OCIOLutDefinitions.hpp"
//#include "lutEngine/LutReader.hpp"
//#include "lutEngine/Lut.hpp"
//#include "lutEngine/TetraInterpolator.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

namespace tuttle {
namespace plugin {
namespace ocio {
namespace lut {

using namespace boost::filesystem;

template<class View>
OCIOLutProcess<View>::OCIOLutProcess(OCIOLutPlugin& instance) :
	ImageGilFilterProcessor<View> (instance), _plugin(instance) {
	//_lut3D = &_plugin._lut3D;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void OCIOLutProcess<View>::multiThreadProcessImages(
		const OfxRectI& procWindowRoW) {
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(
			procWindowRoW);

	applyLut(this->_dstView, this->_srcView, procWindowOutput);
}

template<class View>
void OCIOLutProcess<View>::applyLut(View& dst, View& src,
		const OfxRectI& procWindow) {
	using namespace boost::gil;
	typedef typename View::x_iterator vIterator;
	typedef typename channel_type<View>::type Pixel;

	const char * inputcolorspace = "RawInput";
	const char * outputcolorspace = "ProcessedOutput";

	int imgwidth = procWindow.x2 - procWindow.x1;//dst.width();
	int imgheight = procWindow.y2 - procWindow.y1;//dst.height();
	int components = dst.num_channels();
	float maxValue =
			channel_traits<typename channel_type<View>::type>::max_value();


	std::vector<float> img;

	img.resize(imgwidth * imgheight * components);
	memset(&img[0], 0, imgwidth * imgheight * components * sizeof(float));

	
	int index = 0;

	
	for (int y = procWindow.y1; y < procWindow.y2; ++y) {
		vIterator sit = src.row_begin(y);
		for (int x = procWindow.x1; x < procWindow.x2; ++x) {
			img[index] = (*sit)[0] / maxValue;
			++index;
			img[index] = (*sit)[1] / maxValue;
			++index;
			img[index] = (*sit)[2] / maxValue;
			++index;
			if (dst.num_channels() > 3) {
				img[index] = (*sit)[3] / 65535.0f;
				++index;
			}
			++sit;

		}
		if (this->progressForward())
			return;
	}

	//////

	try {
		// Load the current config.
		OCIO::ConstConfigRcPtr config = _plugin._config;

		// Get the processor
		OCIO::ConstProcessorRcPtr processor = config->getProcessor(
				inputcolorspace, outputcolorspace);

		// Wrap the image in a light-weight ImageDescription
		OCIO::PackedImageDesc imageDesc(&img[0], imgwidth, imgheight,
				components);

		// Apply the color transformation (in place)
		// Need normalized values
		processor->apply(imageDesc);
	} catch (OCIO::Exception & exception) {
		TUTTLE_COUT( tuttle::common::kColorError  << "OCIO Error: " << exception.what() << tuttle::common::kColorStd );
	} catch (...) {
		TUTTLE_COUT( tuttle::common::kColorError << "Unknown OCIO error encountered." << tuttle::common::kColorStd );
	}

	index = 0;

	for (int y = procWindow.y1; y < procWindow.y2; ++y) {
		vIterator dit = dst.row_begin(y);
		for (int x = procWindow.x1; x < procWindow.x2; ++x) {
			(*dit)[0] = static_cast<Pixel> (img[index] * maxValue);
			++index;
			(*dit)[1] = static_cast<Pixel> (img[index] * maxValue);
			++index;
			(*dit)[2] = static_cast<Pixel> (img[index] * maxValue);
			++index;

			if (dst.num_channels() > 3) {
				(*dit)[3] = static_cast<Pixel> (img[index] * maxValue);
				++index;
			}
			++dit;

		}
		if (this->progressForward())
			return;
	}

}

}
}
}
}
