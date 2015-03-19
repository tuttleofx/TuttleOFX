#!/usr/bin/python
from __future__ import print_function

from pyTuttle import tuttle
tuttle.core().preload(False)

for p in tuttle.core().getPlugins():
	print(p.getIdentifier().ljust(30) + " (v" + str( p.getVersionMajor() ) + "." + str( p.getVersionMinor() ) + ")")

