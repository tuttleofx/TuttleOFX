# scons: pluginCheckerboard pluginText pluginPng

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testCheckerboard():
	generators.generate( "tuttle.checkerboard", format="1K-Super35-full-ap" )


def testCheckerboardMultiboxes():
	generators.generate( "tuttle.checkerboard", format="1K-Super35-full-ap", boxes=[50, 20] )


def testCheckerboard2Colors():
	generators.generate( "tuttle.checkerboard", format="1K-Super35-full-ap", boxes=[10, 5], color1=[0.41, 0.47, 0.60, 1.0], color2=[0.64, 0.24, 0.14, 1.0] )

