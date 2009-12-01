/**
 * @file %CLASSNAME%Process.hpp
 * @brief
 * @author
 * @date    %DATE%
 *
 */
#ifndef % D_CLASSNAME % _PROCESS_HPP
#define % D_CLASSNAME % _PROCESS_HPP

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
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

namespace % NAMESPACE % {
	namespace plugin {

	/**
	 * @brief Base class
	 *
	 */
	template<class View>
	class % CLASSNAME % Process : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
	{
		typedef typename View::value_type value_t;

		protected:
			% CLASSNAME % Plugin &    _plugin;        ///< Rendering plugin
			View _srcView;      ///< Source view

		public:
			% CLASSNAME % Process<View>( % CLASSNAME % Plugin & instance );

			// set up and run a processor
			void setupAndProcess( const OFX::RenderArguments & args );

			// Do some processing
			void multiThreadProcessImages( OfxRectI procWindow );
	};

	}
}

#include "%CLASSNAME%Process.tcc"

#endif  // %D_CLASSNAME%_PROCESS_HPP
