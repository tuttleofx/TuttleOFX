# scons: ColorBars Text Png

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testColorbars():
	generate( "tuttle.colorbars", format="1K-Super35-full-ap" )

def testColorbars100percent():
	generate( "tuttle.colorbars", format="1K-Super35-full-ap", levels="100%" )
