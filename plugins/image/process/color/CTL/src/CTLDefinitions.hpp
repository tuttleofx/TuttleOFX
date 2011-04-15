#ifndef _TUTTLE_PLUGIN_CTL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CTL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace ctl {

const static std::string kParamChooseInput( "input" );
const static std::string kParamChooseInputCode( "code" );
const static std::string kParamChooseInputFile( "file" );

enum EParamChooseInput
{
	eParamChooseInputCode = 0,
	eParamChooseInputFile,
};

const static std::string kParamCTLCode( "code" );
const static std::string kParamCTLFile( "file" );

}
}
}

#endif
