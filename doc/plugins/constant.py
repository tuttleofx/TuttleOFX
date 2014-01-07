# scons: pluginConstant pluginText pluginPng

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testConstant():
	generate( "tuttle.constant", format="1K-Super35-full-ap" )

def testConstantColor1():
	generate( "tuttle.constant", format="1K-Super35-full-ap", color=[0.64, 0.24, 0.14, 0.75] )

def testConstantcolor2():
	generate( "tuttle.constant", format="1K-Super35-full-ap", color=[0.41, 0.47, 0.60, 1.0] )
