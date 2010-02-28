#ifndef MERGE_PROCESS_HPP
#define MERGE_PROCESS_HPP

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

/**
 * @brief Base class
 *
 */
template<class View, class Functor>
class MergeProcess : public ImageGilProcessor<View>
{
protected:
	MergePlugin& _plugin; ///< Rendering plugin
	View _srcViewA; ///< Source view A
	View _srcViewB; ///< Source view B

public:
	MergeProcess( MergePlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "MergeProcess.tcc"

#endif  // MERGE_PROCESS_HPP
