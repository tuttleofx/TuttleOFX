#ifndef EXR_DEFINITIONS_HPP
#define EXR_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{

static const std::string kParamFileBitDepth = "fileBitDepth";

enum ETuttlePluginFileBitDepth
{
    eTuttlePluginFileBitDepth16f = 0,
    eTuttlePluginFileBitDepth32,
    eTuttlePluginFileBitDepth32f,
    eTuttlePluginFileBitDepthNone
};

enum ETuttlePluginComponents
{
    eTuttlePluginComponentsAuto = 0,
    eTuttlePluginComponentsGray,
    eTuttlePluginComponentsRGB,
    eTuttlePluginComponentsRGBA,
    eTuttlePluginComponentsABGR
};

static const std::string kParamCompression = "compression";
static const std::string kParamCompressionNone = "None";
static const std::string kParamCompressionRLE = "RLE";
static const std::string kParamCompressionZIPS = "ZIPS";
static const std::string kParamCompressionZIP = "ZIP";
static const std::string kParamCompressionPIZ = "PIZ";
static const std::string kParamCompressionPXR24 = "PXR24";
static const std::string kParamCompressionB44 = "B44";
static const std::string kParamCompressionB44A = "B44A";

enum EParamCompression
{
    eParamCompression_None = 0,
    eParamCompression_RLE,
    eParamCompression_ZIPS,
    eParamCompression_ZIP,
    eParamCompression_PIZ,
    eParamCompression_PXR24,
    eParamCompression_B44,
    eParamCompression_B44A
};

static const std::string kParamCompressionHintNone = "No compression.";
static const std::string kParamCompressionHintRLE =
    "Lossless, Run length encoding.\n"
    "Differences between horizontally adjacent pixels are run-length encoded. This "
    "method is fast, and works well for images with large flat areas, but for photographic "
    "images, the compressed file size is usually between 60 and 75 percent of the "
    "uncompressed size.";
static const std::string kParamCompressionHintZIPS =
    "Lossless, zlib compression, one scan line at a time.\n"
    "Uses the open-source zlib library for compression. Like ZIP compression, but "
    "operates on one scan line at a time.";
static const std::string kParamCompressionHintZIP =
    "Lossless zlib compression, in blocks of 16 scan lines.\n"
    "Differences between horizontally adjacent pixels are compressed using the open-"
    "source zlib library. ZIP decompression is faster than PIZ decompression, but ZIP "
    "compression is significantly slower. Photographic images tend to shrink to between "
    "45 and 55 percent of their uncompressed size.\n"
    "Multi-resolution files are often used as texture maps for 3D renderers. For this "
    "application, fast read accesses are usually more important than fast writes, or "
    "maximum compression. For texture maps, ZIP is probably the best compression "
    "method.\n"
    "In scan-line based files, 16 rows of pixels are accumulated and compressed "
    "together as a single block.";
static const std::string kParamCompressionHintPIZ =
    "Lossless, piz-based wavelet compression.\n"
    "A wavelet transform is applied to the pixel data, and the result is Huffman-"
    "encoded. This scheme tends to provide the best compression ratio for the types of "
    "images that are typically processed at Industrial Light & Magic. Files are "
    "compressed and decompressed at roughly the same speed. For photographic "
    "images with film grain, the files are reduced to between 35 and 55 percent of their "
    "uncompressed size.\n"
    "PIZ compression works well for scan-line based files, and also for tiled files with "
    "large tiles, but small tiles do not shrink much. (PIZ-compressed data start with a "
    "relatively long header; if the input to the compressor is short, adding the header "
    "tends to offset any size reduction of the input.)\n"
    "PIZ compression is only supported for flat images.";
static const std::string kParamCompressionHintPXR24 =
    "Lossy, 24-bit float compression.\n"
    "After reducing 32-bit floating-point data to 24 bits by rounding (while leaving 16-bit "
    "floating-point data unchanged), differences between horizontally adjacent pixels "
    "are compressed with zlib, similar to ZIP. PXR24 compression preserves image "
    "channels of type HALF and UINT exactly, but the relative error of FLOAT data "
    "increases to about 3x10^-5. This compression method works well for depth "
    "buffers and similar images, where the possible range of values is very large, but "
    "where full 32-bit floating-point accuracy is not necessary. Rounding improves "
    "compression significantly by eliminating the pixels' 8 least significant bits, which "
    "tend to be very noisy, and therefore difficult to compress.\n"
    "PXR24 compression is only supported for flat images.";
static const std::string kParamCompressionHintB44 =
    "Lossy, 4-by-4 pixel block compression fixed compression rate.\n"
    "Channels of type HALF are split into blocks of four by four pixels or 32 bytes. Each "
    "block is then packed into 14 bytes, reducing the data to 44 percent of their "
    "uncompressed size. When B44 compression is applied to RGB images in "
    "combination with luminance/chroma encoding (see below), the size of the "
    "compressed pixels is about 22 percent of the size of the original RGB data. "
    "Channels of type UINT or FLOAT are not compressed. "
    "Decoding is fast enough to allow real-time playback of B44-compressed OpenEXR "
    "image sequences on commodity hardware. "
    "The size of a B44-compressed file depends on the number of pixels in the image, "
    "but not on the data in the pixels. All images with the same resolution and the same "
    "set of channels have the same size. This can be advantageous for systems that "
    "support real-time playback of image sequences; the predictable file size makes it "
    "easier to allocate space on storage media efficiently. "
    "B44 compression is only supported for flat images.";
static const std::string kParamCompressionHintB44A =
    "Lossy, 4-by-4 pixel block compression flat fields are compressed more.\n"
    "Like B44, except for blocks of four by four pixels where all pixels have the same "
    "value, which are packed into 3 instead of 14 bytes. For images with large uniform "
    "areas, B44A produces smaller files than B44 compression. "
    "B44A compression is only supported for flat images.";
}
}
}

#endif
