# scons: pluginColorGradient pluginText pluginPng

import os

from pyTuttle.tuttle import *
from generators import *

def setUp():
	core().preload( False )

def testColorGradient1dLinearHorizontal():
	generate( "tuttle.colorgradient", format="1K-Super35-full-ap", type="1dLinear", color0=[0,0,0,0.5], point1=[1,0], color1=[1,1,1,1] )

def testColorGradient1dLinearVertical():
	generate( "tuttle.colorgradient", format="1K-Super35-full-ap", type="1dLinear", color0=[0,0,0,0.5], point1=[0,1], color1=[1,1,1,1] )

def testColorGradient1dLinearDiagonal():
	generate( "tuttle.colorgradient", format="1K-Super35-full-ap", type="1dLinear", color0=[0,0,0,0.5], point1=[1,1], color1=[1,1,1,1] )

def testColorGradient1dRadial():
	generate( "tuttle.colorgradient", format="1K-Super35-full-ap", type="1dRadial", color0=[0,0,0,0.5], point1=[0.25,0.5], color1=[1,1,1,1] )

def testColorGradient2d():
	generate( "tuttle.colorgradient", format="1K-Super35-full-ap", type="2d", nbPoints=4, point1=[0.25, 0.65], point2=[0.75, 0.25], point3=[0.75,0.75], color1=[0.41, 0.47, 0.60, 1.0], color2=[0.64, 0.24, 0.14, 1.0], color3=[1,1,1,1] )
