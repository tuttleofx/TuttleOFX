#ifndef _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONS_HPP_
#define _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONS_HPP_

#include "LibAV.hpp"
#include "LibAVPresetDefinitions.hpp"
#include "LibAVVideoWriter.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>
#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <ofxsImageEffect.h>

#include <vector>

namespace tuttle {
namespace plugin {
namespace av {

template< typename IOPlugin >
class AVOptionPlugin : public IOPlugin
{
public:
	AVOptionPlugin( OfxImageEffectHandle handle );

protected:
	int convertIntWithOptionalUnit( const std::string& param, const std::string& stringValue );
	
	template< typename LibAVVideoRW >
	void setParameters( LibAVVideoRW& writer, const EAVParamType& type, void* av_class, int req_flags, int rej_flags );
	
	template< typename LibAVVideoRW >
	void setParameters( LibAVVideoRW& writer, const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec );
	void setParameters( const PresetParameters& parameters );
	
private:
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, bool &value );
	
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption& opt, bool& value, std::string& valueToSetFlag );
	
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, int &value );
	
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, double &value );
	
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, int &valueNum, int& valueDen );
	
	template< typename LibAVVideoRW >
	void optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, std::string &value );
};

}
}
}

#include "LibAVOptions.tcc"

#endif
