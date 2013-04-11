#ifndef _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONS_HPP_
#define _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONS_HPP_

#include "LibAV.hpp"
#include "LibAVPresetDefinitions.hpp"
#include "LibAVVideoWriter.hpp"

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <ofxsImageEffect.h>

#include <vector>

namespace tuttle {
namespace plugin {
namespace av {

class AVOptionPlugin : public WriterPlugin
{
public:
	AVOptionPlugin( OfxImageEffectHandle handle );

protected:
	int convertIntWithOptionalUnit( const std::string& param, const std::string& stringValue );
	
	void setParameters( LibAVVideoWriter& writer, const EAVParamType& type, void* av_class, int req_flags, int rej_flags );
	void setParameters( LibAVVideoWriter& writer, const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec );
	void setParameters( const PresetParameters& parameters );
	
private:
	
};



}
}
}
#endif
