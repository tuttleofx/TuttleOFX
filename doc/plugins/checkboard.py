# scons: pluginCheckerboard pluginText pluginPng

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testCheckerboard():
	generate( "tuttle.checkerboard", format="1K-Super35-full-ap" )

def testCheckerboardMultiboxes():
	generate( "tuttle.checkerboard", format="1K-Super35-full-ap", boxes=[50, 20] )

def testCheckerboard2Colors():
	generate( "tuttle.checkerboard", format="1K-Super35-full-ap", boxes=[10, 5], color1=[0.41, 0.47, 0.60, 1.0], color2=[0.64, 0.24, 0.14, 1.0] )
