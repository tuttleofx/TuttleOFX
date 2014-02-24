# scons: pluginConstant pluginText pluginPng

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testConstant():
	generators.generate( "tuttle.constant", format="1K-Super35-full-ap" )


def testConstantColor1():
	generators.generate( "tuttle.constant", format="1K-Super35-full-ap", color=[0.64, 0.24, 0.14, 0.75] )


def testConstantcolor2():
	generators.generate( "tuttle.constant", format="1K-Super35-full-ap", color=[0.41, 0.47, 0.60, 1.0] )
