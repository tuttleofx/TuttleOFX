from __future__ import print_function
import sys
import os
import argparse

from pyTuttle import tuttle
import getBestPlugin

tuttle.core().preload()

# parse command line
parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('input', help='input file or sequence')

args = parser.parse_args()
inputSequence = args.input

# get reader plugin
fileName, fileExtension = os.path.splitext(inputSequence)
readerPlugin = getBestPlugin.getBestReader(fileExtension)

if readerPlugin == "":
	print("ERROR: unsuported file format")
	exit()

# create graph
graph = tuttle.Graph()
read = graph.createNode(readerPlugin, filename=inputSequence).asImageEffectNode()

try:
	graph.setup()
	td = read.getTimeDomain()

	graph.setupAtTime( td.min, [read] )

except Exception as e:
	print("ERROR: ", str(e))
	exit(1)

# print info of sequence
rod = read.getRegionOfDefinition( td.min )
width  = rod.x2 - rod.x1
height = rod.y2 - rod.y1

print("Input                    : %s" % inputSequence)
print("Width                    : %d" % int(width))
print("Height                   : %d" % int(height))
print("RoD                      : %d, %d, %d, %d" % (int(rod.x1), int(rod.x2), int(rod.y1), int(rod.y2)))

def getComponents( componentString ):
	if componentString == "OfxImageComponentNone":
		return "None"
	if componentString == "OfxImageComponentAlpha":
		return "Alpha"
	if componentString == "OfxImageComponentRGB":
		return "RGB"
	if componentString == "OfxImageComponentRGBA":
		return "RGBA"
	if componentString == "OfxImageComponentYUVA":
		return "YUVA (deprecated)"


def getBitDetph( bitDepthString ):
	if bitDepthString == "kOfxBitDepthNone":
		return "None"
	if bitDepthString == "kOfxBitDepthByte":
		return "8  bits"
	if bitDepthString == "kOfxBitDepthShort":
		return "16 bits"
	if bitDepthString == "kOfxBitDepthFloat":
		return "32 bits floating point"

clip = read.getOutputClip()

print("Bit depth                :", getBitDetph(clip.getBitDepthString()))
print("Pixel Aspect Ratio (par) :", clip.getPixelAspectRatio())
print("Components               :", getComponents(clip.getComponentsString()))
print("Number of components     :", clip.getNbComponents())
print("frame rate (fps)         :", clip.getFrameRate())
print("frame range              : %d, %d" % (int(clip.getFrameRange().min), int(clip.getFrameRange().max)))
