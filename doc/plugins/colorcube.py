# scons: pluginColorCube pluginText pluginPng

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )


def testColorCube():
	generate( "tuttle.colorcube", format="1K-Super35-full-ap" )

def testColorCube512():
	generate( "tuttle.colorcube", format="1K-Super35-full-ap", cubes="512" )

def testColorCube4():
	generate( "tuttle.colorcube", format="1K-Super35-full-ap", cubes="4" )
