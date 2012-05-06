#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
#import urllib
import subprocess

processFilename = 'process.txt'

if(os.path.exists( processFilename )):
  os.remove( processFilename )

osname		= os.name.lower()
sysplatform	= sys.platform.lower()
windows		= osname == "nt" and sysplatform.startswith("win")
macos		= sysplatform.startswith("darwin")
linux		= not windows and not macos
unix		= not windows
is_64bits	= sys.maxsize > 2**32

currentDir	= os.getcwd()

TUTTLE_ROOT	= currentDir+'/../../../../dist/linux/release'
if( not os.path.exists( TUTTLE_ROOT+'/.tuttleofxTest' )):
  os.mkdir( TUTTLE_ROOT+'/.tuttleofxTest' )

os.environ['TUTTLE_ROOT']= TUTTLE_ROOT 
os.environ['TUTTLE_HOME']=TUTTLE_ROOT+'/.tuttleofxTest'
os.environ['OFX_PLUGIN_PATH']= TUTTLE_ROOT+'/plugin'
os.environ['LD_LIBRARY_PATH']= TUTTLE_ROOT+'/lib'
os.environ['PATH']= TUTTLE_ROOT+'/bin'
os.environ['SAM_PATH']=TUTTLE_ROOT+'/bin'

SRC_IMAGE=" checkerboard width=1000 color1=0.2,0.3,0.5,1.0 "

SRC_IMAGE=" colorgradient type=2d nbPoints=5 point0=0,0 color0=0,0,0,0 point1=1,1 color1=1,1,1,1 point2=0,1 color2=0,1,0,1 point3=1,0.1 color3=0,0,1,1 point4=0.5,0.5 color4=1,0,0,1 width=512 "

BITDEPTH8=  "// bitdepth byte "
BITDEPTH10= "// bitdepth short "
BITDEPTH12= "// bitdepth short "
BITDEPTH16= "// bitdepth short "
BITDEPTH16f="// bitdepth float "
BITDEPTH32= "// bitdepth float "
BITDEPTH32f="// bitdepth float "

BITDEPTH8=  ""
BITDEPTH10= ""
BITDEPTH12= ""
BITDEPTH16= ""
BITDEPTH16f=""
BITDEPTH32= ""
BITDEPTH32f=""

COMPONENT_GRAY=" components=gray "
COMPONENT_RGB=" components=rgb "
COMPONENT_RGBA=" components=rgba "

TT_EXE='sam-do'
if( windows ):
	TT_EXE='sam-do.exe'

TT_OUT_FILE=' >> generate.txt'
gF = open("test.txt", 'w')

print( "***********************************************" )
print( "***************  GENERATE IMAGES **************" )
print( "***********************************************" )

print( "- png" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.gray.8bits.001.png components=gray bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.gray.8bits.003.png components=gray bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.gray.16bits.001.png components=gray bitDepth=16i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.gray.16bits.003.png components=gray bitDepth=16i ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.rgb.8bits.001.png components=rgb bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.rgb.8bits.003.png components=rgb bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.rgb.16bits.001.png components=rgb bitDepth=16i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.rgb.16bits.003.png components=rgb bitDepth=16i ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.rgba.8bits.001.png components=rgba bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// pngwriter png.rgba.8bits.003.png components=rgba bitDepth=8i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.rgba.16bits.001.png components=rgba bitDepth=16i ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// pngwriter png.rgba.16bits.003.png components=rgba bitDepth=16i ' + TT_OUT_FILE, shell=True )

print( "- dpx" )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH8  + '// dpxwriter dpx.gray.8bits.001.dpx bitDepth=8i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH8  + '// dpxwriter dpx.gray.8bits.003.dpx bitDepth=8i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH8  + '// dpxwriter dpx.rgb.8bits.001.dpx bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH8  + '// dpxwriter dpx.rgb.8bits.003.dpx bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH8  + '// dpxwriter dpx.rgba.8bits.001.dpx bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH8  + '// dpxwriter dpx.rgba.8bits.003.dpx bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH8  + '// dpxwriter dpx.abgr.8bits.001.dpx bitDepth=8i components=abgr ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH8  + '// dpxwriter dpx.abgr.8bits.003.dpx bitDepth=8i components=abgr ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH10  + '// dpxwriter dpx.gray.10bits.001.dpx bitDepth=10i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH10  + '// dpxwriter dpx.gray.10bits.003.dpx bitDepth=10i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH10  + '// dpxwriter dpx.rgb.10bits.001.dpx bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH10  + '// dpxwriter dpx.rgb.10bits.003.dpx bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH10  + '// dpxwriter dpx.rgba.10bits.001.dpx bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH10  + '// dpxwriter dpx.rgba.10bits.003.dpx bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH10  + '// dpxwriter dpx.abgr.10bits.001.dpx bitDepth=10i components=abgr ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH10  + '// dpxwriter dpx.abgr.10bits.003.dpx bitDepth=10i components=abgr ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH12  + '// dpxwriter dpx.gray.12bits.001.dpx bitDepth=12i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH12  + '// dpxwriter dpx.gray.12bits.003.dpx bitDepth=12i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH12  + '// dpxwriter dpx.rgb.12bits.001.dpx bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH12  + '// dpxwriter dpx.rgb.12bits.003.dpx bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH12  + '// dpxwriter dpx.rgba.12bits.001.dpx bitDepth=12i components=rgba packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH12  + '// dpxwriter dpx.rgba.12bits.003.dpx bitDepth=12i components=rgba packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH12  + '// dpxwriter dpx.abgr.12bits.001.dpx bitDepth=12i components=abgr packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH12  + '// dpxwriter dpx.abgr.12bits.003.dpx bitDepth=12i components=abgr packed=packed ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH16  + '// dpxwriter dpx.gray.16bits.001.dpx bitDepth=16i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH16  + '// dpxwriter dpx.gray.16bits.003.dpx bitDepth=16i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH16  + '// dpxwriter dpx.rgb.16bits.001.dpx bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH16  + '// dpxwriter dpx.rgb.16bits.003.dpx bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH16  + '// dpxwriter dpx.rgba.16bits.001.dpx bitDepth=16i components=rgba packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH16  + '// dpxwriter dpx.rgba.16bits.003.dpx bitDepth=16i components=rgba packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH16  + '// dpxwriter dpx.abgr.16bits.001.dpx bitDepth=16i components=abgr packed=packed ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH16  + '// dpxwriter dpx.abgr.16bits.003.dpx bitDepth=16i components=abgr packed=packed ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH32  + '// dpxwriter dpx.gray.32bits.001.dpx bitDepth=32i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_GRAY + BITDEPTH32  + '// dpxwriter dpx.gray.32bits.003.dpx bitDepth=32i components=luma ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH32  + '// dpxwriter dpx.rgb.32bits.001.dpx bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGB  + BITDEPTH32  + '// dpxwriter dpx.rgb.32bits.003.dpx bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH32  + '// dpxwriter dpx.rgba.32bits.001.dpx bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH32  + '// dpxwriter dpx.rgba.32bits.003.dpx bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH32  + '// dpxwriter dpx.abgr.32bits.001.dpx bitDepth=32i components=abgr ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + COMPONENT_RGBA + BITDEPTH32  + '// dpxwriter dpx.abgr.32bits.003.dpx bitDepth=32i components=abgr ' + TT_OUT_FILE, shell=True )

print( "- exr" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.gray.16float.001.exr bitDepth=16f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.gray.16float.003.exr bitDepth=16f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.rgb.16float.001.exr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.rgb.16float.003.exr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.rgba.16float.001.exr bitDepth=16f components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + '// exrwriter exr.rgba.16float.003.exr bitDepth=16f components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.gray.32bits.001.exr bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.gray.32bits.003.exr bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.rgb.32bits.001.exr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.rgb.32bits.003.exr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.rgba.32bits.001.exr bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + '// exrwriter exr.rgba.32bits.003.exr bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.gray.32float.001.exr bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.gray.32float.003.exr bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.rgb.32float.001.exr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.rgb.32float.003.exr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.rgba.32float.001.exr bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + '// exrwriter exr.rgba.32float.003.exr bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )

print( "- jpeg" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// jpegwriter jpeg.rgb.8bits.001.jpg ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// jpegwriter jpeg.rgb.8bits.003.jpg ' + TT_OUT_FILE, shell=True )

print( "- jpeg 2000" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// jpeg2000writer jpeg2000.rgb.8bits.001.j2k lossless=1 ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// jpeg2000writer jpeg2000.rgb.8bits.003.j2k lossless=1 ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// jpeg2000writer jpeg2000.rgb.12bits.001.j2k bitDepth=12i lossless=1 ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// jpeg2000writer jpeg2000.rgb.12bits.003.j2k bitDepth=12i lossless=1 ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// jpeg2000writer jpeg2000.rgb.16bits.001.j2k bitDepth=16i lossless=1 ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// jpeg2000writer jpeg2000.rgb.16bits.003.j2k bitDepth=16i lossless=1 ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // jpeg2000writer jpeg2000.rgb.32bits.001.j2k bitDepth=32i lossless=1 ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // jpeg2000writer jpeg2000.rgb.32bits.003.j2k bitDepth=32i lossless=1 ' + TT_OUT_FILE, shell=True )


print( "- OIIO png" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.001.png bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.003.png bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.png bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.png bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.png bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.png bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.001.png bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.003.png bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.png bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.png bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.png bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.png bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO ppm" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.001.ppm bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.003.ppm bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.ppm bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.ppm bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.ppm bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.ppm bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.001.ppm bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.003.ppm bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.ppm bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.ppm bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.ppm bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.ppm bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.32bits.001.ppm bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.32bits.003.ppm bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.001.ppm bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.003.ppm bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.32bits.001.ppm bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.32bits.003.ppm bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.32float.001.ppm bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.32float.003.ppm bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.001.ppm bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.003.ppm bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.32float.001.ppm bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.32float.003.ppm bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO dpx" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.dpx bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.dpx bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.dpx bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.dpx bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgb.10bits.001.dpx bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgb.10bits.003.dpx bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgba.10bits.001.dpx bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgba.10bits.003.dpx bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgb.12bits.001.dpx bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgb.12bits.003.dpx bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgba.12bits.001.dpx bitDepth=12i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgba.12bits.003.dpx bitDepth=12i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.dpx bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.dpx bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.dpx bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.dpx bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )


#print( "- OIIO cin" )
#OIIO Writer: Cineon writer is not implemented yet, please poke Leszek in the mailing list
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.cin bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.cin bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.cin bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.cin bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgb.10bits.001.cin bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgb.10bits.003.cin bitDepth=10i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgba.10bits.001.cin bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH10 + ' // oiiowriter oiio.rgba.10bits.003.cin bitDepth=10i components=rgba ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgb.12bits.001.cin bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgb.12bits.003.cin bitDepth=12i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgba.12bits.001.cin bitDepth=12i components=rgba ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH12 + ' // oiiowriter oiio.rgba.12bits.003.cin bitDepth=12i components=rgba ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.cin bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.cin bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.cin bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.cin bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.cin bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.cin bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.cin bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.cin bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO jpeg" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.jpg ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.jpg ' + TT_OUT_FILE, shell=True )

#print( "- OIIO jpeg 2000" )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.j2k components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.j2k components=rgb ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.j2k bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.j2k bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )

#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgb.32bits.001.j2k bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
#subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgb.32bits.003.j2k bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )

print( "- OIIO tga" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.001.tga components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.003.tga components=gray ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.tga components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.tga components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.tga components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.tga components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO tif" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.001.tif components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.003.tif components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.001.tif components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.003.tif components=gray ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.tif components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.tif components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.tif components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.tif components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.tif components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.tif components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.tif components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.tif components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO sgi" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.001.sgi bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.gray.8bits.003.sgi bitDepth=8i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.sgi bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.sgi bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.001.sgi bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgba.8bits.003.sgi bitDepth=8i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.001.sgi bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.gray.16bits.003.sgi bitDepth=16i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.sgi bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.sgi bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.001.sgi bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgba.16bits.003.sgi bitDepth=16i components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO exr" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.gray.16float.001.exr bitDepth=16f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.gray.16float.003.exr bitDepth=16f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.rgb.16float.001.exr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.rgb.16float.003.exr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.rgba.16float.001.exr bitDepth=16f components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16f + ' // oiiowriter oiio.rgba.16float.003.exr bitDepth=16f components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.gray.32bits.001.exr bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.gray.32bits.003.exr bitDepth=32i components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgb.32bits.001.exr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgb.32bits.003.exr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgba.32bits.001.exr bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32 + ' // oiiowriter oiio.rgba.32bits.003.exr bitDepth=32i components=rgba ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.gray.32float.001.exr bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.gray.32float.003.exr bitDepth=32f components=gray ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.rgb.32float.001.exr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.rgb.32float.003.exr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.rgba.32float.001.exr bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH32f + ' // oiiowriter oiio.rgba.32float.003.exr bitDepth=32f components=rgba ' + TT_OUT_FILE, shell=True )

print( "- OIIO hdr" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.hdr bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.hdr bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.hdr bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.hdr bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16float.001.hdr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16float.003.hdr bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.001.hdr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.003.hdr bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.001.hdr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.003.hdr bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )

print( "- OIIO rgbe" )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.001.rgbe bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH8  + '// oiiowriter oiio.rgb.8bits.003.rgbe bitDepth=8i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.001.rgbe bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16bits.003.rgbe bitDepth=16i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16float.001.rgbe bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.16float.003.rgbe bitDepth=16f components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.001.rgbe bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32bits.003.rgbe bitDepth=32i components=rgb ' + TT_OUT_FILE, shell=True )

subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.001.rgbe bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )
subprocess.call( TT_EXE + SRC_IMAGE + BITDEPTH16 + '// oiiowriter oiio.rgb.32float.003.rgbe bitDepth=32f components=rgb ' + TT_OUT_FILE, shell=True )

gF.close()
