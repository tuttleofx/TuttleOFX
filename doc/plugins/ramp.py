# scons: Ramp Text Png Exr

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testRamp():
	generate( "tuttle.ramp", format="1K-Super35-full-ap" )

def testRampVertical():
	generate( "tuttle.ramp", format="1K-Super35-full-ap", direction="vertical" )

def testColorRamp():
	generate( "tuttle.ramp", format="1K-Super35-full-ap", color=True )

def testColorRampHighDynamic():
	generate( "tuttle.ramp", format="1K-Super35-full-ap", color=True, start=[-3.0, -3.0, -3.0, 1.0], end=[3.0, 3.0, 3.0, 1.0] )
	generateEXR( "tuttle.ramp", format="1K-Super35-full-ap", color=True, start=[-3.0, -3.0, -3.0, 1.0], end=[3.0, 3.0, 3.0, 1.0] )

