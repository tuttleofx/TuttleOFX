#ifndef _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

static const std::string kRawReaderHelpButton = "Help";
static const std::string kRawReaderHelpString = "<b>Raw File reader</b> plugin is used to read raw files.  <br />";

static const std::string kParamFiltering     = "filtering";
static const std::string kParamFilteringAuto = "Auto";
static const std::string kParamFilteringNone = "None";

enum EFiltering
{
	eFilteringAuto = 0,
	eFilteringNone
};

}
}
}
}

#endif
