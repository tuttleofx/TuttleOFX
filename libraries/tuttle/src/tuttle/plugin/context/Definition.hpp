#ifndef _TUTTLE_PLUGIN_CONTEXT_DEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_DEFINITION_HPP_

#include <string>

namespace tuttle
{
namespace plugin
{

static const std::string kTuttlePluginFilename = "filename";
static const std::string kTuttlePluginFilenameLabel = "Filename";

static const std::string kTuttlePluginBitDepth = "bitDepth";
static const std::string kTuttlePluginBitDepthLabel = "Bit depth";

static const std::string kTuttlePluginBitDepthAuto = "auto";
static const std::string kTuttlePluginBitDepthNone = "none";
static const std::string kTuttlePluginBitDepth8 = "8i    8  bits integer";
static const std::string kTuttlePluginBitDepth10 = "10i   10 bits integer";
static const std::string kTuttlePluginBitDepth12 = "12i   12 bits integer";
static const std::string kTuttlePluginBitDepth16 = "16i   16 bits integer";
static const std::string kTuttlePluginBitDepth16f = "16f   16 bits floating point";
static const std::string kTuttlePluginBitDepth32 = "32i   32 bits integer";
static const std::string kTuttlePluginBitDepth32f = "32f   32 bits floating point";
static const std::string kTuttlePluginBitDepth64 = "64i   64 bits integer";
static const std::string kTuttlePluginBitDepth64f = "64f   64 bits floating point";

static const std::string kTuttlePluginChannel = "channel";
static const std::string kTuttlePluginChannelLabel = "Channel";
static const std::string kTuttlePluginChannelHint = "Select channel";
static const std::string kTuttlePluginChannelAuto = "auto";
static const std::string kTuttlePluginChannelGray = "gray";
static const std::string kTuttlePluginChannelRGB = "rgb";
static const std::string kTuttlePluginChannelRGBA = "rgba";
static const std::string kTuttlePluginChannelABGR = "abgr";
}
}

#endif
