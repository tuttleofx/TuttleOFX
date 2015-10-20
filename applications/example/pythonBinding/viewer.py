#!/usr/bin/python
from __future__ import print_function
import sys
import os

from pyTuttle import tuttle
import getBestPlugin

tuttle.core().preload(False)
tuttle.core().getFormatter().setLogLevel(tuttle.eVerboseLevelError)

if len(sys.argv) < 2 :
	print("ERROR: no such input file")
	exit(-1)

print(sys.argv[1])
(base, ext) = os.path.splitext(sys.argv[1])
plugin = getBestPlugin.getBestReader(ext)

if not plugin:
	print("ERROR: unsupported input file")
	exit(-1)

graph = tuttle.Graph()
reader = graph.createNode(plugin, filename=sys.argv[1])
viewer = graph.createNode("tuttle.viewer")

options = tuttle.ComputeOptions()

graph.connect([reader, viewer])
graph.compute(viewer, options)
