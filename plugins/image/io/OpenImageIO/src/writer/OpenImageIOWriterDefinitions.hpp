#ifndef _OPENIMAGEIOWRITER_DEFINITIONS_HPP_
#define _OPENIMAGEIOWRITER_DEFINITIONS_HPP_

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

enum ETuttlePluginBitDepth
{
    eTuttlePluginBitDepthAuto = 0,
    eTuttlePluginBitDepth8,
    eTuttlePluginBitDepth10,
    eTuttlePluginBitDepth12,
    eTuttlePluginBitDepth16,
    eTuttlePluginBitDepth16f,
    eTuttlePluginBitDepth32,
    eTuttlePluginBitDepth32f
};

enum ETuttlePluginComponents
{
    eTuttlePluginComponentsAuto = 0,
    eTuttlePluginComponentsGray,
    eTuttlePluginComponentsRGB,
    eTuttlePluginComponentsRGBA
};

static const std::string kParamOutputQuality = "quality";
static const std::string kParamOutputQualityLabel = "Quality";

static const std::string kParamOutputStorageType = "storage";
static const std::string kParamOutputStorageTypeLabel = "Storage type";
static const std::string kParamOutputStorageTypeHint = "Set the type of storage of the output file.\n"
                                                       "It could be ignored depending on the format.\n";
static const std::string kParamOutputStorageScanLine = "scanLine";
static const std::string kParamOutputStorageTiles = "tiles (64x64)";

enum ETuttlePluginStorage
{
    eTuttlePluginStorageScanLine = 0,
    eTuttlePluginStorageTiles
};

enum ETuttlePluginSubsampling
{
    eETuttlePluginSubsampling420 = 0,
    eETuttlePluginSubsampling422,
    eETuttlePluginSubsampling411,
    eETuttlePluginSubsampling444
};

static const std::string kParamOutputSubsampling = "subsampling";
static const std::string kParamOutputSubsamplingLabel = "Subsampling";
static const std::string kParamOutputSubsamplingHint = "Controlling chroma-subsampling of output JPEG files:\n"
                                                 "4:2:0 : one chrominance component for every 2x2 block of pixels.\n"
                                                 "4:2:2 : one chrominance component for every 2x1 block of pixels.\n"
                                                 "4:1:1 : one chrominance component for every 4x1 block of pixels.\n"
                                                 "4:4:4 : one chrominance component for every pixel (no subsampling)\n";

static const std::string kParamOutputSubsampling420 = "420";
static const std::string kParamOutputSubsampling422 = "422";
static const std::string kParamOutputSubsampling411 = "411";
static const std::string kParamOutputSubsampling444 = "444";

static const std::string kParamOutputOrientation = "orientation";
static const std::string kParamOutputOrientationLabel = "Orientation";

static const std::string kParamOutputOrientationNormal = "normal";
static const std::string kParamOutputOrientationFlop = "flop";
static const std::string kParamOutputOrientationR180 = "180";
static const std::string kParamOutputOrientationFlip = "flip";
static const std::string kParamOutputOrientationTransposed = "transposed";
static const std::string kParamOutputOrientationR90Clockwise = "90clockwise";
static const std::string kParamOutputOrientationTransverse = "transverse";
static const std::string kParamOutputOrientationR90CounterClockwise = "90counter-clockwise";

static const std::string kParamCompression = "compression";
static const std::string kParamCompressionLabel = "Compression";
static const std::string kParamCompressionNone = "None";
static const std::string kParamCompressionRLE = "RLE";
static const std::string kParamCompressionZIPS = "ZIPS";
static const std::string kParamCompressionZIP = "ZIP";
static const std::string kParamCompressionPIZ = "PIZ";
static const std::string kParamCompressionPXR24 = "PXR24";
static const std::string kParamCompressionB44 = "B44";
static const std::string kParamCompressionB44A = "B44A";

enum ETuttlePluginCompression
{
    eTuttlePluginCompressionNone = 0,
    eTuttlePluginCompressionRLE,
    eTuttlePluginCompressionZIP,
    eTuttlePluginCompressionZIPS,
    eTuttlePluginCompressionPIZ,
    eTuttlePluginCompressionPXR24,
    eTuttlePluginCompressionB44,
    eTuttlePluginCompressionB44A
};

static const std::string kParamCompressionHintNone = "No compression.";
static const std::string kParamCompressionHintRLE =
    "Lossless, Run length encoding.\n"
    "Differences between horizontally adjacent pixels are run-length encoded. This "
    "method is fast, and works well for images with large flat areas, but for photographic "
    "images, the compressed file size is usually between 60 and 75 percent of the "
    "uncompressed size.";
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
static const std::string kParamCompressionHintZIPS =
    "Lossless, zlib compression, one scan line at a time.\n"
    "Uses the open-source zlib library for compression. Like ZIP compression, but "
    "operates on one scan line at a time.";
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
}

#endif
