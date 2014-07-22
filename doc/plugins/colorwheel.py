# scons: pluginColorWheel pluginText pluginPng

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testColorWheel():
	generators.generate( "tuttle.colorwheel", format="1K-Super35-full-ap" )


def testColorWheelBlack():
	generators.generate( "tuttle.colorwheel", format="1K-Super35-full-ap", type="black" )


def testRainbowColorWheel():
	generators.generate( "tuttle.colorwheel", format="1K-Super35-full-ap", type="rainbow" )

