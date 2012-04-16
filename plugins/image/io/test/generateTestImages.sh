#!/bin/sh

rm process.txt

red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m'  # No Color


export TUTTLE_ROOT=`pwd`/../../../../dist/linux/release
export OFX_PLUGIN_PATH=$TUTTLE_ROOT/plugin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TUTTLE_ROOT/lib
export PATH=$PATH:$TUTTLE_ROOT/bin
export SAM_PATH=$TUTTLE_ROOT/bin

if [ ! -d "$TUTTLE_ROOT/.tuttleofxTest" ]; then
   mkdir $TUTTLE_ROOT/.tuttleofxTest
fi
export TUTTLE_HOME=$TUTTLE_ROOT/.tuttleofxTest

echo "***********************************************"
echo "***************  GENERATE IMAGES **************"
echo "***********************************************"

SRC_IMAGE="checkerboard width=1000 color1=0.2,0.3,0.5,1.0"

SRC_IMAGE="colorgradient type=2d nbPoints=5 point0=0,0 color0=0,0,0,1 point1=1,1 color1=1,1,1,1 point2=0,1 color2=0,1,0,1 point3=1,0.1 color3=0,0,1,1 point4=0.5,0.5 color4=1,0,0,1 width=512"

BITDEPTH8="// bitdepth byte "
BITDEPTH10="// bitdepth short "
BITDEPTH12="// bitdepth short "
BITDEPTH16="// bitdepth short "
BITDEPTH16f="// bitdepth float "
BITDEPTH32="// bitdepth float "
BITDEPTH32f="// bitdepth float "

#BITDEPTH8=" "
#BITDEPTH10=" "
#BITDEPTH12=" "
#BITDEPTH16=" "
#BITDEPTH16f=" "
#BITDEPTH32=" "
#BITDEPTH32f=" "

echo "- png"
sam-do $SRC_IMAGE $BITDEPTH8  // pngwriter png.gray.8bits.001.png components=gray bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8  // pngwriter png.gray.8bits.003.png components=gray bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.gray.16bits.001.png components=gray bitDepth=16i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.gray.16bits.003.png components=gray bitDepth=16i >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // pngwriter png.rgb.8bits.001.png components=rgb bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // pngwriter png.rgb.8bits.003.png components=rgb bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.rgb.16bits.001.png components=rgb bitDepth=16i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.rgb.16bits.003.png components=rgb bitDepth=16i >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // pngwriter png.rgba.8bits.001.png components=rgba bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // pngwriter png.rgba.8bits.003.png components=rgba bitDepth=8i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.rgba.16bits.001.png components=rgba bitDepth=16i >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // pngwriter png.rgba.16bits.003.png components=rgba bitDepth=16i >> process.txt

echo "- dpx"
sam-do $SRC_IMAGE components=gray $BITDEPTH8 // dpxwriter dpx.gray.8bits.001.dpx bitDepth=8i components=luma >> process.txt
sam-do $SRC_IMAGE components=gray $BITDEPTH8 // dpxwriter dpx.gray.8bits.003.dpx bitDepth=8i components=luma >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH8 // dpxwriter dpx.rgb.8bits.001.dpx bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH8 // dpxwriter dpx.rgb.8bits.003.dpx bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH8 // dpxwriter dpx.rgba.8bits.001.dpx bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH8 // dpxwriter dpx.rgba.8bits.003.dpx bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH8 // dpxwriter dpx.abgr.8bits.001.dpx bitDepth=8i components=abgr >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH8 // dpxwriter dpx.abgr.8bits.003.dpx bitDepth=8i components=abgr >> process.txt

sam-do $SRC_IMAGE components=gray $BITDEPTH10 // dpxwriter dpx.gray.10bits.001.dpx bitDepth=10i components=luma >> process.txt
sam-do $SRC_IMAGE components=gray $BITDEPTH10 // dpxwriter dpx.gray.10bits.003.dpx bitDepth=10i components=luma >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH10 // dpxwriter dpx.rgb.10bits.001.dpx bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH10 // dpxwriter dpx.rgb.10bits.003.dpx bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH10 // dpxwriter dpx.rgba.10bits.001.dpx bitDepth=10i components=rgba packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH10 // dpxwriter dpx.rgba.10bits.003.dpx bitDepth=10i components=rgba packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH10 // dpxwriter dpx.abgr.10bits.001.dpx bitDepth=10i components=abgr packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH10 // dpxwriter dpx.abgr.10bits.003.dpx bitDepth=10i components=abgr packed=packed >> process.txt

sam-do $SRC_IMAGE components=gray $BITDEPTH12 // dpxwriter dpx.gray.12bits.001.dpx bitDepth=12i components=luma packed=packed >> process.txt
sam-do $SRC_IMAGE components=gray $BITDEPTH12 // dpxwriter dpx.gray.12bits.003.dpx bitDepth=12i components=luma packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH12 // dpxwriter dpx.rgb.12bits.001.dpx bitDepth=12i components=rgb packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH12 // dpxwriter dpx.rgb.12bits.003.dpx bitDepth=12i components=rgb packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH12 // dpxwriter dpx.rgba.12bits.001.dpx bitDepth=12i components=rgba packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH12 // dpxwriter dpx.rgba.12bits.003.dpx bitDepth=12i components=rgba packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH12 // dpxwriter dpx.abgr.12bits.001.dpx bitDepth=12i components=abgr packed=packed >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH12 // dpxwriter dpx.abgr.12bits.003.dpx bitDepth=12i components=abgr packed=packed >> process.txt

sam-do $SRC_IMAGE components=gray $BITDEPTH16 // dpxwriter dpx.gray.16bits.001.dpx bitDepth=16i >> process.txt
sam-do $SRC_IMAGE components=gray $BITDEPTH16 // dpxwriter dpx.gray.16bits.003.dpx bitDepth=16i >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH16 // dpxwriter dpx.rgb.16bits.001.dpx bitDepth=16i >> process.txt
sam-do $SRC_IMAGE components=rgb  $BITDEPTH16 // dpxwriter dpx.rgb.16bits.003.dpx bitDepth=16i >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH16 // dpxwriter dpx.rgba.16bits.001.dpx bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH16 // dpxwriter dpx.rgba.16bits.003.dpx bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH16 // dpxwriter dpx.abgr.16bits.001.dpx bitDepth=16i components=abgr >> process.txt
sam-do $SRC_IMAGE components=rgba $BITDEPTH16 // dpxwriter dpx.abgr.16bits.003.dpx bitDepth=16i components=abgr >> process.txt

echo "- exr"
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.gray.16float.001.exr bitDepth=16f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.gray.16float.003.exr bitDepth=16f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.rgb.16float.001.exr bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.rgb.16float.003.exr bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.rgba.16float.001.exr bitDepth=16f components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16f // exrwriter exr.rgba.16float.003.exr bitDepth=16f components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.gray.32bits.001.exr bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.gray.32bits.003.exr bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.rgb.32bits.001.exr bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.rgb.32bits.003.exr bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.rgba.32bits.001.exr bitDepth=32i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32 // exrwriter exr.rgba.32bits.003.exr bitDepth=32i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.gray.32float.001.exr bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.gray.32float.003.exr bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.rgb.32float.001.exr bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.rgb.32float.003.exr bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.rgba.32float.001.exr bitDepth=32f components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH32f // exrwriter exr.rgba.32float.003.exr bitDepth=32f components=rgba >> process.txt

echo "- jpeg"
sam-do $SRC_IMAGE $BITDEPTH8 // jpegwriter jpeg.rgb.8bits.001.jpg >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // jpegwriter jpeg.rgb.8bits.003.jpg >> process.txt

echo "- jpeg 2000"
sam-do $SRC_IMAGE $BITDEPTH8 // jpeg2000writer jpeg2000.rgb.8bits.001.j2k lossless=1 >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // jpeg2000writer jpeg2000.rgb.8bits.003.j2k lossless=1 >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // jpeg2000writer jpeg2000.rgb.12bits.001.j2k bitDepth=12i lossless=1 >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // jpeg2000writer jpeg2000.rgb.12bits.003.j2k bitDepth=12i lossless=1 >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // jpeg2000writer jpeg2000.rgb.16bits.001.j2k bitDepth=16i lossless=1 >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // jpeg2000writer jpeg2000.rgb.16bits.003.j2k bitDepth=16i lossless=1 >> process.txt

#sam-do $SRC_IMAGE $BITDEPTH32 // jpeg2000writer jpeg2000.rgb.32bits.001.j2k bitDepth=32i lossless=1 >> process.txt
#sam-do $SRC_IMAGE $BITDEPTH32 // jpeg2000writer jpeg2000.rgb.32bits.003.j2k bitDepth=32i lossless=1 >> process.txt


echo "- OIIO png"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.001.png bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.003.png bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.png bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.png bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.png bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.png bitDepth=8i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.001.png bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.003.png bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.png bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.png bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.png bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.png bitDepth=16i components=rgba >> process.txt

echo "- OIIO ppm"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.001.ppm bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.003.ppm bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.ppm bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.ppm bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.ppm bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.ppm bitDepth=8i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.001.ppm bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.003.ppm bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.ppm bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.ppm bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.ppm bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.ppm bitDepth=16i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32bits.001.ppm bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32bits.003.ppm bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.001.ppm bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.003.ppm bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32bits.001.ppm bitDepth=32i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32bits.003.ppm bitDepth=32i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32float.001.ppm bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32float.003.ppm bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.001.ppm bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.003.ppm bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32float.001.ppm bitDepth=32f components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32float.003.ppm bitDepth=32f components=rgba >> process.txt

echo "- OIIO dpx"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.dpx bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.dpx bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.dpx bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.dpx bitDepth=8i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgb.10bits.001.dpx bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgb.10bits.003.dpx bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgba.10bits.001.dpx bitDepth=10i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgba.10bits.003.dpx bitDepth=10i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgb.12bits.001.dpx bitDepth=12i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgb.12bits.003.dpx bitDepth=12i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgba.12bits.001.dpx bitDepth=12i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgba.12bits.003.dpx bitDepth=12i components=rgba >> process.txt


sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.dpx bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.dpx bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.dpx bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.dpx bitDepth=16i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.dpx bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.dpx bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.dpx bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.dpx bitDepth=16i components=rgba >> process.txt

echo "- OIIO cin"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.cin bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.cin bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.cin bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.cin bitDepth=8i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgb.10bits.001.cin bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgb.10bits.003.cin bitDepth=10i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgba.10bits.001.cin bitDepth=10i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH10 // oiiowriter oiio.rgba.10bits.003.cin bitDepth=10i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgb.12bits.001.cin bitDepth=12i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgb.12bits.003.cin bitDepth=12i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgba.12bits.001.cin bitDepth=12i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH12 // oiiowriter oiio.rgba.12bits.003.cin bitDepth=12i components=rgba >> process.txt


sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.cin bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.cin bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.cin bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.cin bitDepth=16i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.cin bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.cin bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.cin bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.cin bitDepth=16i components=rgba >> process.txt

echo "- OIIO jpeg"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.jpg >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.jpg >> process.txt

#echo "- OIIO jpeg 2000"
#sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.j2k components=rgb >> process.txt
#sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.j2k components=rgb >> process.txt

#sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.j2k bitDepth=16i components=rgb >> process.txt
#sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.j2k bitDepth=16i components=rgb >> process.txt

#sam-do $SRC_IMAGE $BITDEPTH32 // oiiowriter oiio.rgb.32bits.001.j2k bitDepth=32f components=rgb >> process.txt
#sam-do $SRC_IMAGE $BITDEPTH32 // oiiowriter oiio.rgb.32bits.003.j2k bitDepth=32f components=rgb >> process.txt

echo "- OIIO tga"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.001.tga components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.003.tga components=gray >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.tga components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.tga components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.tga components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.tga components=rgba >> process.txt

echo "- OIIO tif"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.001.tif components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.003.tif components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.001.tif components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.003.tif components=gray >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.tif components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.tif components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.tif components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.tif components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.tif components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.tif components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.tif components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.tif components=rgba >> process.txt

echo "- OIIO sgi"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.001.sgi bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.gray.8bits.003.sgi bitDepth=8i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.sgi bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.sgi bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.001.sgi bitDepth=8i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgba.8bits.003.sgi bitDepth=8i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.001.sgi bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16bits.003.sgi bitDepth=16i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.sgi bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.sgi bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.001.sgi bitDepth=16i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16bits.003.sgi bitDepth=16i components=rgba >> process.txt

echo "- OIIO exr"

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16float.001.exr bitDepth=16f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.16float.003.exr bitDepth=16f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.001.exr bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.003.exr bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16float.001.exr bitDepth=16f components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.16float.003.exr bitDepth=16f components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32bits.001.exr bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32bits.003.exr bitDepth=32i components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.001.exr bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.003.exr bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32bits.001.exr bitDepth=32i components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32bits.003.exr bitDepth=32i components=rgba >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32float.001.exr bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.gray.32float.003.exr bitDepth=32f components=gray >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.001.exr bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.003.exr bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32float.001.exr bitDepth=32f components=rgba >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgba.32float.003.exr bitDepth=32f components=rgba >> process.txt

echo "- OIIO hdr"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.hdr bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.hdr bitDepth=8i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.hdr bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.hdr bitDepth=16i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.001.hdr bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.003.hdr bitDepth=16f components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.001.hdr bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.003.hdr bitDepth=32i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.001.hdr bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.003.hdr bitDepth=32f components=rgb >> process.txt

echo "- OIIO rgbe"
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.001.rgbe bitDepth=8i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH8 // oiiowriter oiio.rgb.8bits.003.rgbe bitDepth=8i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.001.rgbe bitDepth=16i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16bits.003.rgbe bitDepth=16i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.001.rgbe bitDepth=16f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.16float.003.rgbe bitDepth=16f components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.001.rgbe bitDepth=32i components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32bits.003.rgbe bitDepth=32i components=rgb >> process.txt

sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.001.rgbe bitDepth=32f components=rgb >> process.txt
sam-do $SRC_IMAGE $BITDEPTH16 // oiiowriter oiio.rgb.32float.003.rgbe bitDepth=32f components=rgb >> process.txt
