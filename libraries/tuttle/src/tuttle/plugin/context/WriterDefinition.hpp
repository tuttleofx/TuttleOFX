#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERDEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_WRITERDEFINITION_HPP_

#include "Definition.hpp"

namespace tuttle {
namespace plugin {

static const std::string kParamWriterExistingFile = "existingFile";
static const std::string kParamWriterExistingFile_overwrite = "overwrite";
static const std::string kParamWriterExistingFile_error = "error";
static const std::string kParamWriterExistingFile_skip = "skip";
static const std::string kParamWriterExistingFile_reader = "reader";

enum EParamWriterExistingFile
{
	eParamWriterExistingFile_overwrite = 0,
	eParamWriterExistingFile_error,
	eParamWriterExistingFile_skip,
	eParamWriterExistingFile_reader
};

static const std::string kParamWriterRender         = "render";
static const std::string kParamWriterRenderAlways   = "renderAlways";
static const std::string kParamWriterCopyToOutput = "copyToOutput";
static const std::string kParamWriterForceNewRender = "forceNewRender";

static const std::string kParamPremultiplied      = "premultiplied";

}
}

#endif

