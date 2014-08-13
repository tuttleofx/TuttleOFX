
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_subdirectory(image/generator/Checkerboard)
add_subdirectory(image/generator/ColorBars)
add_subdirectory(image/generator/ColorCube)
add_subdirectory(image/generator/ColorGradient)
add_subdirectory(image/io/Dpx)
add_subdirectory(image/io/Exr)
add_subdirectory(image/io/Jpeg)
add_subdirectory(image/io/Jpeg2000)
add_subdirectory(image/io/ImageMagick)
add_subdirectory(image/io/MemoryBuffer)
add_subdirectory(image/io/OpenImageIO)

# FIXME: Check with Clement
#add_subdirectory(image/io/AudioVideo)


# Current directory structure
#
#── image
#   ├── display
#   │   ├── ColorCubeViewer
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── Histogram
#   │   │   ├── Resources
#   │   │   └── src
#   │   └── Viewer
#   │       ├── Resources
#   │       └── src
#   ├── Dummy
#   │   ├── Resources
#   │   └── src
#   ├── generator
#   │   ├── Checkerboard
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── ColorBars
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── ColorCube
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── ColorGradient
#   │   │   ├── Resources
#   │   │   └── src
#   │   │       └── algorithm
#   │   ├── ColorWheel
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── Constant
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── Ramp
#   │   │   ├── Resources
#   │   │   └── src
#   │   ├── SeExpr
#   │   │   ├── Resources
#   │   │   └── src
#   │   └── Text
#   │       ├── Resources
#   │       └── src
#   ├── io
#   │   ├── AudioVideo
#   │   │   ├── Resources
#   │   │   └── src
#   │   │       ├── libav
#   │   │       ├── reader
#   │   │       ├── swscale
#   │   │       └── writer
#   │   ├── Dpx
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── dpx-google-code
#   │   │   │   │   └── libdpx
#   │   │   │   ├── reader
#   │   │   │   │   └── gil_fixes
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── Exr
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── reader
#   │   │   │   │   └── gil_fixes
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── ImageMagick
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── reader
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── Jpeg
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── reader
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── Jpeg2000
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── openjpeg
#   │   │   │   ├── reader
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── MemoryBuffer
#   │   │   ├── Resources
#   │   │   └── src
#   │   │       ├── inputBuffer
#   │   │       └── outputBuffer
#   │   ├── OpenImageIO
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── reader
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── Png
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   ├── PngEngine
#   │   │   │   ├── reader
#   │   │   │   └── writer
#   │   │   └── tests
#   │   ├── Raw
#   │   │   ├── Resources
#   │   │   ├── src
#   │   │   │   └── reader
#   │   │   └── tests
#   │   ├── test
#   │   └── TurboJpeg
#   │       ├── Resources
#   │       ├── src
#   │       │   ├── reader
#   │       │   └── writer
#   │       └── tests
#   └── process
#       ├── channel
#       │   ├── ChannelShuffle
#       │   │   ├── Resources
#       │   │   └── src
#       │   └── Component
#       │       ├── Resources
#       │       └── src
#       ├── color
#       │   ├── BasicKeyer
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── BitDepth
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── ColorGradation
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Colorspace
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── ColorSpaceKeyer
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── ColorSuppress
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── ColorTransfer
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── ColorTransform
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── CTL
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Gamma
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── HistogramKeyer
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── IdKeyer
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Invert
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Lut
#       │   │   ├── Resources
#       │   │   ├── src
#       │   │   │   └── lutEngine
#       │   │   └── tests
#       │   │       └── test1
#       │   ├── Normalize
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── OCIO
#       │   │   ├── Resources
#       │   │   └── src
#       │   │       ├── OCIOColorSpace
#       │   │       └── OCIOLut
#       │   └── Print
#       │       ├── Resources
#       │       └── src
#       ├── filter
#       │   ├── AnisotropicDiffusion
#       │   │   ├── Resources
#       │   │   └── src
#       │   │       ├── anisotropicDiffusion
#       │   │       ├── anisotropicTensors
#       │   │       └── imageUtils
#       │   │           └── filters
#       │   ├── Blur
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Convolution
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── FloodFill
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── LocalMaxima
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── NlmDenoiser
#       │   │   ├── Resources
#       │   │   └── src
#       │   │       └── imageUtils
#       │   ├── Sobel
#       │   │   ├── Resources
#       │   │   └── src
#       │   └── Thinning
#       │       ├── Resources
#       │       └── src
#       ├── geometry
#       │   ├── Crop
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Flip
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── LensDistort
#       │   │   ├── Resources
#       │   │   ├── src
#       │   │   └── tests
#       │   │       └── src
#       │   ├── Move2D
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Pinning
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── PushPixel
#       │   │   ├── Resources
#       │   │   └── src
#       │   ├── Resize
#       │   │   ├── Resources
#       │   │   └── src
#       │   └── Warp
#       │       ├── Resources
#       │       └── src
#       │           ├── Bezier
#       │           └── TPS
#       ├── math
#       │   └── MathOperator
#       │       ├── Resources
#       │       └── src
#       ├── time
#       │   └── TimeShift
#       │       ├── Resources
#       │       └── src
#       └── transition
#           ├── Fade
#           │   ├── Resources
#           │   └── src
#           └── Merge
#               ├── Resources
#               └── src
#── param
#   ├── analysis
#   │   ├── Diff
#   │   │   ├── Resources
#   │   │   └── src
#   │   └── ImageStatistics
#   │       ├── Resources
#   │       └── src
#   ├── debug
#   │   └── DebugImageEffectApi
#   │       ├── Resources
#   │       └── src
#   └── Math
#       └── src
#── private
#── _scripts
#   └── ImageEffectApi
#       ├── Resources
#       └── src

#74 directories
