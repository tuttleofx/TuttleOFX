# scons: pluginRamp pluginText pluginPng pluginExr

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testRamp():
	generators.generate( "tuttle.ramp", format="1K-Super35-full-ap" )


def testRampVertical():
	generators.generate( "tuttle.ramp", format="1K-Super35-full-ap", direction="vertical" )


def testColorRamp():
	generators.generate( "tuttle.ramp", format="1K-Super35-full-ap", color=True )


def testColorRampHighDynamic():
	generators.generate( "tuttle.ramp", format="1K-Super35-full-ap", color=True, start=[-3.0, -3.0, -3.0, 1.0], end=[3.0, 3.0, 3.0, 1.0] )
	generators.generateEXR( "tuttle.ramp", format="1K-Super35-full-ap", color=True, start=[-3.0, -3.0, -3.0, 1.0], end=[3.0, 3.0, 3.0, 1.0] )

