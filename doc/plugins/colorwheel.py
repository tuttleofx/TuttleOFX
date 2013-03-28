# scons: ColorWheel Text Png

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testColorWheel():
	generate( "tuttle.colorwheel", format="1K-Super35-full-ap" )

def testColorWheelBlack():
	generate( "tuttle.colorwheel", format="1K-Super35-full-ap", type="black" )

def testRainbowColorWheel():
	generate( "tuttle.colorwheel", format="1K-Super35-full-ap", type="rainbow" )

