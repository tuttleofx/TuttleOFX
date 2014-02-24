# scons: pluginColorBars pluginText pluginPng

from pyTuttle import tuttle
import generators

import os


def setUp():
	tuttle.core().preload( False )


def testColorbars():
	generators.generate( "tuttle.colorbars", format="1K-Super35-full-ap" )


def testColorbars100percent():
	generators.generate( "tuttle.colorbars", format="1K-Super35-full-ap", levels="100%" )
