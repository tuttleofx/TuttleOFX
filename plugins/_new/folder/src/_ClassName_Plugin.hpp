/**
 * @file %CLASSNAME%Plugin.hpp
 * @brief
 * @author
 * @date    %DATE%
 *
 */

#ifndef % D_CLASSNAME % _PLUGIN_H
#define % D_CLASSNAME % _PLUGIN_H

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace % NAMESPACE % {
	namespace plugin {

	/**
	 * @brief
	 *
	 */
	class % CLASSNAME % Plugin : public OFX::ImageEffect
	{
		public:
			% CLASSNAME % Plugin( OfxImageEffectHandle handle );
			OFX::Clip* getSrcClip() const;
			OFX::Clip* getDstClip() const;

		public:
			virtual void render( const OFX::RenderArguments & args );
			void changedParam( const OFX::InstanceChangedArgs & args, const std::string & paramName );

		protected:
			// do not need to delete these, the ImageEffect is managing them for us
			OFX::Clip* _srcClip;      ///< Source image clip
			OFX::Clip* _dstClip;      ///< Destination image clip
	};

	}
}

#endif  // %D_CLASSNAME%_PLUGIN_H
