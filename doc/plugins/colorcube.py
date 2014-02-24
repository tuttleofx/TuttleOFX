# scons: pluginColorCube pluginText pluginPng

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testColorCube():
	generators.generate( "tuttle.colorcube", format="1K-Super35-full-ap" )


def testColorCube512():
	generators.generate( "tuttle.colorcube", format="1K-Super35-full-ap", cubes="512" )


def testColorCube4():
	generators.generate( "tuttle.colorcube", format="1K-Super35-full-ap", cubes="4" )
